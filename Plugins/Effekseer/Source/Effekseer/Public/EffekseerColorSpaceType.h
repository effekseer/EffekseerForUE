
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EffekseerColorSpaceType.generated.h"

UENUM(BlueprintType)
enum class EEffekseerColorSpaceType : uint8
{
	Gamma,
	Linear,
};