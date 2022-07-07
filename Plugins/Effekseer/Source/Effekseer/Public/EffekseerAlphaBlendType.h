#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EffekseerAlphaBlendType.generated.h"

UENUM(BlueprintType)
enum class EEffekseerAlphaBlendType : uint8
{
	Opacity = 0,
	Blend = 1,
	Add = 2,
	Sub = 3,
	Mul = 4,
};