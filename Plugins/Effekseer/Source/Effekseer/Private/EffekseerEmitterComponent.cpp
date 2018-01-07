#include "EffekseerPrivatePCH.h"
#include "EffekseerEmitterComponent.h"
#include "EffekseerSystemComponent.h"

UEffekseerEmitterComponent::UEffekseerEmitterComponent()
{
	bTickInEditor = true;
	bAutoDestroy = false;
	PrimaryComponentTick.bCanEverTick = true;
}

UEffekseerEmitterComponent::~UEffekseerEmitterComponent()
{

}

#if WITH_EDITOR
void UEffekseerEmitterComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (bAutoActivate)
	{
		Stop();
		shouldActivate = true;
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

void  UEffekseerEmitterComponent::OnUnregister()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::OnUnregister();
}

void UEffekseerEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (shouldActivate)
	{
		handle = Play();
		isPlaying = true;

		shouldActivate = false;

		if (AllColor != AllColor_)
		{
			system_->SetEffectAllColor(handle, AllColor);
			AllColor_ = AllColor;
		}

		if (Speed != Speed_)
		{
			system_->SetEffectSpeed(handle, Speed);
			Speed_ = Speed;
		}
	}

	if (system_ != nullptr && isPlaying)
	{
		if (system_->Exists(handle))
		{
			auto transform = GetComponentToWorld();

			system_->SetEffectPosition(handle, transform.GetTranslation());
			system_->SetEffectRotation(handle, transform.GetRotation().Rotator());
			system_->SetEffectScaling(handle, transform.GetScale3D());

			if (AllColor != AllColor_)
			{
				system_->SetEffectAllColor(handle, AllColor);
				AllColor_ = AllColor;
			}

			if (Speed != Speed_)
			{
				system_->SetEffectSpeed(handle, Speed);
				Speed_ = Speed;
			}
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
	if (isPlaying)
	{
		Stop();
	}

	shouldActivate = true;
	Super::Activate(bReset);
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

	// SystemÇÃíTçı
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