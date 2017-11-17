
#include "EffekseerPrivatePCH.h"

#include "AnimNotify_PlayEffekseerEffect.h"
#include "Components/SkeletalMeshComponent.h"

#include "EffekseerEmitterComponent.h"

#include "EffekseerEffect.h"

static UEffekseerEmitterComponent* CreateEffekseerEmitter(UEffekseerEffect* EffekseerEffect, UWorld* World, AActor* Actor, bool bAutoDestroy)
{
	UEffekseerEmitterComponent* PSC = NewObject<UEffekseerEmitterComponent>((Actor ? Actor : (UObject*)World));
	PSC->bAutoDestroy = bAutoDestroy;
	PSC->bAllowAnyoneToDestroyMe = true;
	PSC->bAutoActivate = false;
	PSC->Effect = EffekseerEffect;
	
	return PSC;
}

static void SpawnEmitterAtLocation(UWorld* World, UEffekseerEffect* effectTemplate, const FTransform& SpawnTransform, bool bAutoDestroy = true)
{
	UEffekseerEmitterComponent* eec = nullptr;
	if (World != nullptr && effectTemplate != nullptr)
	{
		eec = CreateEffekseerEmitter(effectTemplate, World, World->GetWorldSettings(), bAutoDestroy);

		eec->bAbsoluteLocation = true;
		eec->bAbsoluteRotation = true;
		eec->bAbsoluteScale = true;
		eec->RelativeLocation = SpawnTransform.GetLocation();
		eec->RelativeRotation = SpawnTransform.GetRotation().Rotator();
		eec->RelativeScale3D = SpawnTransform.GetScale3D();

		eec->RegisterComponentWithWorld(World);

		eec->Activate();
	}
}

static void SpawnEmitterAttached(UEffekseerEffect* effectTemplate, class USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bAutoDestroy = true)
{
	UEffekseerEmitterComponent* eec = nullptr;
	if (effectTemplate != nullptr)
	{
		if (AttachToComponent == nullptr)
		{
			UE_LOG(LogScript, Warning, TEXT("UGameplayStatics::SpawnEmitterAttached: NULL AttachComponent specified!"));
		}
		else
		{
			UWorld* const World = AttachToComponent->GetWorld();
			if (World && World->GetNetMode() != NM_DedicatedServer)
			{
				eec = CreateEffekseerEmitter(effectTemplate, World, AttachToComponent->GetOwner(), bAutoDestroy);

				eec->SetupAttachment(AttachToComponent, AttachPointName);

				if (LocationType == EAttachLocation::KeepWorldPosition)
				{
					const FTransform ParentToWorld = AttachToComponent->GetSocketTransform(AttachPointName);
					const FTransform ComponentToWorld(Rotation, Location);
					const FTransform RelativeTM = ComponentToWorld.GetRelativeTransform(ParentToWorld);
					eec->RelativeLocation = RelativeTM.GetLocation();
					eec->RelativeRotation = RelativeTM.GetRotation().Rotator();
					eec->RelativeScale3D = FVector(1.f);
				}
				else
				{
					eec->RelativeLocation = Location;
					eec->RelativeRotation = Rotation;

					if (LocationType == EAttachLocation::SnapToTarget)
					{
						const FTransform ParentToWorld = AttachToComponent->GetSocketTransform(AttachPointName);
						eec->RelativeScale3D = ParentToWorld.GetSafeScaleReciprocal(ParentToWorld.GetScale3D());
					}
					else
					{
						eec->RelativeScale3D = FVector(1.f);
					}
				}

				eec->RegisterComponentWithWorld(World);
				eec->Activate();
			}
		}
	}
}

UAnimNotify_PlayEffekseerEffect::UAnimNotify_PlayEffekseerEffect()
	: Super()
{
	Attached = true;

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 192, 128, 255);
#endif
}

void UAnimNotify_PlayEffekseerEffect::PostLoad()
{
	Super::PostLoad();

	RotationOffsetQuat = FQuat(RotationOffset);
}

#if WITH_EDITOR
void UAnimNotify_PlayEffekseerEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty && 
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotify_PlayEffekseerEffect, RotationOffset))
	{
		RotationOffsetQuat = FQuat(RotationOffset);
	}
}
#endif

void UAnimNotify_PlayEffekseerEffect::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	if (EffekseerEffect)
	{
		if (Attached)
		{
			SpawnEmitterAttached(EffekseerEffect, MeshComp, SocketName, LocationOffset, RotationOffset);
		}
		else
		{
			const FTransform MeshTransform = MeshComp->GetSocketTransform(SocketName);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(MeshTransform.TransformPosition(LocationOffset));
			SpawnTransform.SetRotation(MeshTransform.GetRotation() * RotationOffsetQuat);
			SpawnEmitterAtLocation(MeshComp->GetWorld(), EffekseerEffect, SpawnTransform);
		}
	}
	else
	{
		UE_LOG(LogScript, Warning, TEXT("Effekseer Notify: Null EffekseerEffect for effekseer notify in anim: %s"), *GetNameSafe(Animation));
	}
}

FString UAnimNotify_PlayEffekseerEffect::GetNotifyName_Implementation() const
{
	if (EffekseerEffect)
	{
		return EffekseerEffect->Name;
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}
