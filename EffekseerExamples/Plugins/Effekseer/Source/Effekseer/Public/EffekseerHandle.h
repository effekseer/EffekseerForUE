
#pragma once

#include "Object.h"
#include "EffekseerEffect.h"
#include "EffekseerHandle.generated.h"

class UEffekseerSystemComponent;

USTRUCT()
struct EFFEKSEER_API FEffekseerHandle
{
	GENERATED_BODY()

	UPROPERTY()
	UEffekseerEffect* Effect = nullptr;

	UPROPERTY()
	UEffekseerSystemComponent* System = nullptr;

	UPROPERTY()
	int ID = -1;
};