
#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "EffekseerUECompatibility.h"

#include "AnimNotify_PlayEffekseerEffect.generated.h"

class UAnimSequenceBase;
class UEffekseerEffect;
class USkeletalMeshComponent;

/**
	@brief	Animation notify for Effekseer
	@note
	This class is based on Cascade
*/
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play Effekseer Effect"))
class EFFEKSEER_API UAnimNotify_PlayEffekseerEffect
	: public UAnimNotify
{
	GENERATED_BODY()
public:
	UAnimNotify_PlayEffekseerEffect();

	virtual FString GetNotifyName_Implementation() const override;

#if EFFEKSEER_UE_HAS_ANIM_NOTIFY_EVENT_REFERENCE
	virtual void Notify(USkeletalMeshComponent* smc, UAnimSequenceBase* animSequence, const FAnimNotifyEventReference&) override;
#else
	virtual void Notify(USkeletalMeshComponent* smc, UAnimSequenceBase* animSequence) override;
#endif

	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (DisplayName = "Effekseer Effect"))
	UEffekseerEffect* EffekseerEffect;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	uint32 Attached : 1;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FName SocketName;
};
