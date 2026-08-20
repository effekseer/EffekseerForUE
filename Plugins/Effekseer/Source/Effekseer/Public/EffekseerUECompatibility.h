
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

#define EFFEKSEER_UE_VERSION_AT_LEAST(Major, Minor) \
	((ENGINE_MAJOR_VERSION > (Major)) || (ENGINE_MAJOR_VERSION == (Major) && ENGINE_MINOR_VERSION >= (Minor)))

#define EFFEKSEER_UE_HAS_ANIM_NOTIFY_EVENT_REFERENCE EFFEKSEER_UE_VERSION_AT_LEAST(5, 0)
#define EFFEKSEER_UE_HAS_ASSET_REGISTRY_TAGS_CONTEXT EFFEKSEER_UE_VERSION_AT_LEAST(5, 4)
#define EFFEKSEER_UE_HAS_STATIC_MESH_IMPORT_DATA_ACCESSORS EFFEKSEER_UE_VERSION_AT_LEAST(5, 7)
#define EFFEKSEER_UE_HAS_WORLD_TO_VIEW_MATRIX EFFEKSEER_UE_VERSION_AT_LEAST(5, 8)

namespace EffekseerUE
{
// Maximum distance where a float retains at least 1/16 cm precision. This is
// UE4's HALF_WORLD_MAX and UE5's UE_FLOAT_HUGE_DISTANCE.
constexpr float FloatSafeBoundsExtent = 1048576.0f;

#if ENGINE_MAJOR_VERSION == 5
using UEFVector2f = FVector2f;
using UEFVector3f = FVector3f;
using UEFSoftObjectPath = FSoftObjectPath;
#else
using UEFVector2f = FVector2D;
using UEFVector3f = FVector;
using UEFSoftObjectPath = FStringAssetReference;
#endif
} // namespace EffekseerUE
