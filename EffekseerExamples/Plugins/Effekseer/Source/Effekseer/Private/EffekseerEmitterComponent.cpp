#include "EffekseerPrivatePCH.h"
#include "EffekseerEmitterComponent.h"

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