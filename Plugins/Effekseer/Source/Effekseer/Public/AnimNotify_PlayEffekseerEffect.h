
#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayEffekseerEffect.generated.h"

class UAnimSequenceBase;
class UEffekseerEffect;
class USkeletalMeshComponent;

/**
	@brief	Animation notify for Effekseer
	@note
	This class is based on Cascade
*/
UCLASS(const, hidecategories=Object, collapsecategories, meta=(DisplayName="Play Effekseer Effect"))
class EFFEKSEER_API UAnimNotify_PlayEffekseerEffect
	: public UAnimNotify
{
	GENERATED_BODY()
public:

	UAnimNotify_PlayEffekseerEffect();

	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual FString GetNotifyName_Implementation() const override;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, Category="AnimNotify", meta=(DisplayName="Effekseer Effect"))
	UEffekseerEffect* EffekseerEffect;

	UPROPERTY(EditAnywhere, Category="AnimNotify")
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, Category="AnimNotify")
	FRotator RotationOffset;

private:
	FQuat RotationOffsetQuat;

public:

	UPROPERTY(EditAnywhere, Category="AnimNotify")
	uint32 Attached:1;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FName SocketName;
};



