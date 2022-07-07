
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

namespace EffekseerUE
{
#if ENGINE_MAJOR_VERSION == 5
using UEFVector2f = FVector2f;
using UEFVector3f = FVector3f;
#else
using UEFVector2f = FVector2D;
using UEFVector3f = FVector;
#endif
} // namespace EffekseerUE
