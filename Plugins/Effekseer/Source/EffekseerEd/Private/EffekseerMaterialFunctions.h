
#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstance.h"
#include "EffekseerMaterial.h"
#include <memory>

struct NativeEffekseerMaterialContext;

struct UE4MaterialFromEffekseerMaterialOption
{
	EEffekseerAlphaBlendType AlphaBlend = EEffekseerAlphaBlendType::Blend;
};

UMaterial* CreateUE4MaterialFromEffekseerMaterial(const std::shared_ptr<NativeEffekseerMaterialContext>& context, const FString& packageRoot, const FString& filename, const UE4MaterialFromEffekseerMaterialOption& option);

UMaterialInstance* CreateUE4MaterialInstanceFromUE4Material(UMaterial* parent, const FString& packageRoot, const FString& filename, const UE4MaterialFromEffekseerMaterialOption& option);

TArray<UObject*> AssignElementMaterials(UEffekseerMaterial* material, bool isInstance);