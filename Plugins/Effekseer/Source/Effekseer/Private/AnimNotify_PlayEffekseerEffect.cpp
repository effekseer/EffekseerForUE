
#include "AnimNotify_PlayEffekseerEffect.h"

#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"

#include "EffekseerEmitterComponent.h"
#include "EffekseerEffect.h"


static UEffekseerEmitterComponent* CreateEffekseerEmitter(UEffekseerEffect* effekseerEffect, UWorld* World, AActor* Actor, bool bAutoDestroy)
{
	auto eec = NewObject<UEffekseerEmitterComponent>((Actor != nullptr ? Actor : (UObject*)World));
	eec->bAutoDestroy = bAutoDestroy;
	eec->bAllowAnyoneToDestroyMe = true;
	eec->bAutoActivate = false;
	eec->Effect = effekseerEffect;
	return eec;
}

static void SpawnEmitterAtLocation(UWorld* world, UEffekseerEffect* effekseerEffect, const FTransform& transform, bool bAutoDestroy = true)
{
	if (effekseerEffect == nullptr) return;
	if (world == nullptr) return;

	auto eec = CreateEffekseerEmitter(effekseerEffect, world, world->GetWorldSettings(), bAutoDestroy);
	eec->SetUsingAbsoluteLocation(true);
	eec->SetUsingAbsoluteRotation(true);
	eec->SetUsingAbsoluteScale(true);
	eec->SetRelativeLocation(transform.GetLocation());
	eec->SetRelativeRotation(transform.GetRotation().Rotator());
	eec->SetRelativeScale3D(transform.GetScale3D());
	eec->RegisterComponentWithWorld(world);
	eec->Activate();
}

static void SpawnEmitterAttached(UEffekseerEffect* effekseerEffect, class USceneComponent* attachToComponent, FName attachPointName, FVector location, FRotator rotation, EAttachLocation::Type locationType = EAttachLocation::KeepRelativeOffset, bool bAutoDestroy = true)
{
	if (effekseerEffect == nullptr) return;

	if (attachToComponent == nullptr)
	{
		UE_LOG(LogScript, Warning, TEXT("Effekseer::SpawnEmitterAttached : attachToComponent is null."));
		return;
	}

	auto world = attachToComponent->GetWorld();
	if (world == nullptr) return;
	if (world->GetNetMode() == NM_DedicatedServer) return;

	auto eec = CreateEffekseerEmitter(effekseerEffect, world, attachToComponent->GetOwner(), bAutoDestroy);
	eec->SetupAttachment(attachToComponent, attachPointName);

	if (locationType == EAttachLocation::KeepWorldPosition)
	{
		auto componentToWorld = FTransform(rotation, location);
		auto parentToWorld = attachToComponent->GetSocketTransform(attachPointName);
		auto relative = componentToWorld.GetRelativeTransform(parentToWorld);

#if ENGINE_MINOR_VERSION >= 24
		eec->SetRelativeLocation(location);
		eec->SetRelativeRotation(relative.GetRotation().Rotator());
		eec->SetRelativeScale3D(FVector(1.0f));
#else
		eec->RelativeLocation = relative.GetLocation();
		eec->RelativeRotation = relative.GetRotation().Rotator();
		eec->RelativeScale3D = FVector(1.0f);
#endif
	}
	else
	{
		eec->SetRelativeLocation(location);
		eec->SetRelativeRotation(rotation);

		if (locationType == EAttachLocation::SnapToTarget)
		{
			auto parentToWorld = attachToComponent->GetSocketTransform(attachPointName);

#if ENGINE_MINOR_VERSION >= 24
			eec->SetRelativeScale3D(parentToWorld.GetSafeScaleReciprocal(parentToWorld.GetScale3D()));
#else
			eec->RelativeScale3D = parentToWorld.GetSafeScaleReciprocal(parentToWorld.GetScale3D());
#endif
		}
		else
		{
#if ENGINE_MINOR_VERSION >= 24
			eec->SetRelativeScale3D(FVector(1.0f));
#else
			eec->RelativeScale3D = FVector(1.0f);
#endif
		}
	}

	eec->RegisterComponentWithWorld(world);
	eec->Activate();
}

UAnimNotify_PlayEffekseerEffect::UAnimNotify_PlayEffekseerEffect()
	: Super()
{
	Attached = true;

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 192, 128, 255);
#endif
}

void UAnimNotify_PlayEffekseerEffect::Notify(class USkeletalMeshComponent* smc, class UAnimSequenceBase* animSequence)
{
	if (smc == nullptr) return;
	if (animSequence == nullptr) return;

	if (EffekseerEffect == nullptr)
	{
		UE_LOG(LogScript, Warning, TEXT("Effekseer::Notify : EffekseerEffect is null in anim: %s"), *GetNameSafe(animSequence));
		return;
	}

	if (Attached)
	{
		SpawnEmitterAttached(EffekseerEffect, smc, SocketName, LocationOffset, RotationOffset);
	}
	else
	{
		auto quat = FQuat(RotationOffset);
		auto tr = smc->GetSocketTransform(SocketName);
		auto spawnTransform = FTransform(
			tr.GetRotation() * quat,
			tr.TransformPosition(LocationOffset)
		);
		SpawnEmitterAtLocation(smc->GetWorld(), EffekseerEffect, spawnTransform);
	}
}

FString UAnimNotify_PlayEffekseerEffect::GetNotifyName_Implementation() const
{
	if (EffekseerEffect != nullptr)
	{
		return EffekseerEffect->Name;
	}
	
	return Super::GetNotifyName_Implementation();
}
