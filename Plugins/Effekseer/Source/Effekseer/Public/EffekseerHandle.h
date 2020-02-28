
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EffekseerEffect.h"
#include "EffekseerHandle.generated.h"

class UEffekseerSystemComponent;

USTRUCT(BlueprintType)
struct EFFEKSEER_API FEffekseerHandle
{
	GENERATED_BODY()

	UPROPERTY()
	UEffekseerEffect* Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEffekseerSystemComponent* System = nullptr;

	UPROPERTY()
	int ID = -1;
};