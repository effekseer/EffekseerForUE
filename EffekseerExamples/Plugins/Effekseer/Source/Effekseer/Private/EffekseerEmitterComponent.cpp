#include "EffekseerPrivatePCH.h"
#include "EffekseerEmitterComponent.h"
#include "EffekseerSystemComponent.h"

UEffekseerEmitterComponent::UEffekseerEmitterComponent()
{
}

UEffekseerEmitterComponent::~UEffekseerEmitterComponent()
{

}

void UEffekseerEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffekseerEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEffekseerEmitterComponent::Play()
{
	UEffekseerSystemComponent* system_ = nullptr;

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

	if (system_ == nullptr) return;

	auto loc = GetComponentLocation();
	
	system_->Play(Effect, loc);
}