#include "EffekseerEmitterComponent.h"
#include "EffekseerSystemComponent.h"


void UEffekseerEmitterComponent::ApplyParameters(bool forced)
{
	if (AllColor != AllColor_ || forced)
	{
		system_->SetEffectAllColor(handle, AllColor);
		AllColor_ = AllColor;
	}

	if (Speed != Speed_ || forced)
	{
		system_->SetEffectSpeed(handle, Speed);
		Speed_ = Speed;
	}

	bool isDynamicInputChanged = false;
	for (int32_t i = 0; i < DynamicInput.Num(); i++)
	{
		if (DynamicInput[i] != DynamicInput_[i] || forced)
		{
			system_->SetEffectDynamicInput(handle, i, DynamicInput[i]);
			isDynamicInputChanged = true;
		}
	}

	if (isDynamicInputChanged || forced)
	{
		DynamicInput_ = DynamicInput;
	}
}

UEffekseerEmitterComponent::UEffekseerEmitterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickInEditor = true;
	bAutoDestroy = false;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostPhysics;

	if (DynamicInput.Num() < 4)
	{
		for (size_t i = DynamicInput.Num(); i < 4; i++)
		{
			DynamicInput.Add(0.0f);
		}
	}

	DynamicInput_.Empty();
	for (size_t i = 0; i < 4; i++)
	{
		DynamicInput_.Add(0.0f);
	}
}

UEffekseerEmitterComponent::~UEffekseerEmitterComponent()
{

}

#if WITH_EDITOR
void UEffekseerEmitterComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Stop();
	if (bAutoActivate)
	{
		shouldActivate = true;
		autoActivateOnActivate_ = bAutoActivate;
	}
}
#endif


void UEffekseerEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffekseerEmitterComponent::BeginDestroy()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::BeginDestroy();
}

void UEffekseerEmitterComponent::OnUnregister()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::OnUnregister();
}

void UEffekseerEmitterComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void UEffekseerEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// HACK for activate
	if (shouldActivate && autoActivateOnActivate_ != bAutoActivate)
	{
		shouldActivate = false;
	}

	if (shouldActivate)
	{
		handle = Play();

		if (handle.Effect != nullptr)
		{
			isPlaying = true;

			shouldActivate = false;

			ApplyParameters(true);
		}
	}

	if (system_ != nullptr && isPlaying)
	{
		if (system_->Exists(handle))
		{
			auto transform = GetComponentToWorld();

			system_->SetEffectPosition(handle, transform.GetTranslation());
			system_->SetEffectRotation(handle, transform.GetRotation());
			system_->SetEffectScaling(handle, transform.GetScale3D());

			ApplyParameters(false);
		}
		else
		{
			if (IsLooping)
			{
				AllColor_ = FColor(255, 255, 255, 255);
				Speed_ = 1.0f;
				handle = Play();
			}
			else
			{
				isPlaying = false;

				if (bAutoDestroy)
				{
					DestroyComponent();
					return;
				}
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEffekseerEmitterComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (isPlaying && bReset)
	{
		return;
	}

	if (isPlaying)
	{
		Stop();
	}

	shouldActivate = true;
	autoActivateOnActivate_ = bAutoActivate;
}

void UEffekseerEmitterComponent::Deactivate()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::Deactivate();
}

void UEffekseerEmitterComponent::Stop()
{
	if (system_ != nullptr)
	{
		system_->Stop(handle);
		isPlaying = false;
	}
}

void UEffekseerEmitterComponent::StopRoot()
{
	if (system_ != nullptr)
	{
		system_->StopRoot(handle);
	}
}

bool UEffekseerEmitterComponent::Exists() const
{
	if (system_ != nullptr)
	{
		return system_->Exists(handle);
	}
	return false;
}

FEffekseerHandle UEffekseerEmitterComponent::Play()
{
	if (System != nullptr)
	{
		for (auto it : System->GetComponents())
		{
			auto cas = Cast<UEffekseerSystemComponent>(it);
			if (cas != nullptr)
			{
				system_ = cas;
				break;
			}
		}
	}

	if (system_ == nullptr)
	{
		System = nullptr;
	}

	// Find a System
	if (System == nullptr)
	{
		for (TActorIterator<AActor> ait(GetWorld()); ait; ++ait)
		{
			for (auto it : ait->GetComponents())
			{
				auto cas = Cast<UEffekseerSystemComponent>(it);
				if (cas != nullptr)
				{
					system_ = cas;
					System = *ait;
					goto LoopExit;
				}
			}
		}
	}
LoopExit:;

	if (system_ == nullptr) return FEffekseerHandle();

	auto transform = GetComponentToWorld();
	
	auto loc = transform .GetTranslation();
	
	return system_->Play(Effect, loc);
}

#if WITH_EDITOR
void UEffekseerEmitterComponent::Preview()
{
	Stop();
	shouldActivate = true;
}
#endif