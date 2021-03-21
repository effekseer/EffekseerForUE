
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "EffekseerAlphaBlendType.h"
#include "EditorFramework/AssetImportData.h"
#include "Runtime/Launch/Resources/Version.h"
#include "EffekseerColorSpaceType.h"
#include "EffekseerMaterial.generated.h"

namespace Effekseer
{

class MaterialFile;

}

USTRUCT(BlueprintType)
struct FEffekseerTextureProperty
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(VisibleAnywhere)
	FString Name;
};

USTRUCT(BlueprintType)
struct FEffekseerUniformProperty
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(VisibleAnywhere)
	FString Name;

	UPROPERTY(VisibleAnywhere)
	int Count;
};

USTRUCT()
struct EFFEKSEER_API FEffekseerMaterialElement
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly)
	UMaterialInterface* Material = nullptr;

	UPROPERTY(EditInstanceOnly)
	EEffekseerAlphaBlendType AlphaBlend = EEffekseerAlphaBlendType::Blend;
};

UCLASS()
class EFFEKSEER_API UEffekseerMaterial 
	: public UObject
{
	GENERATED_BODY()
private:
	Effekseer::MaterialFile* internal_ = nullptr;
	TArray<uint8> buffer_;

#if WITH_EDITOR
	bool isMaterialCompilationRegistered_ = false;
#endif

	void ReleaseMaterial();

	UMaterial* materialPrevious_ = nullptr;

public:

	UPROPERTY(EditInstanceOnly)
	UMaterial* Material = nullptr;

	UPROPERTY(EditInstanceOnly)
	TArray<FEffekseerMaterialElement> MaterialElements;

	UPROPERTY(VisibleAnywhere)
	TArray<FEffekseerUniformProperty> Uniforms;

#if ENGINE_MINOR_VERSION >= 25
	TMap<FHashedName, int> UniformHashedNameToIndex;
#endif
	UPROPERTY(Transient)
	TMap<FString, int> UniformNameToIndex;

	UPROPERTY(VisibleAnywhere)
	TArray<FEffekseerTextureProperty> Textures;

#if ENGINE_MINOR_VERSION >= 25
	TMap<FHashedName, int> TextureHashedNameToIndex;
#endif
	UPROPERTY(Transient)
	TMap<FString, int> TextureNameToIndex;

	UPROPERTY(Transient)
	TMap<EEffekseerAlphaBlendType, UMaterialInstanceDynamic*> ColorSpaceMaterials;

	UPROPERTY()
	bool IsEffectScaleRequired = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif

#if WITH_EDITOR
	//! Reupdate elements to apply parent shaders (UE4 requires to update after a parent material finishes to be compiled)
	void ReupdateElements();

	void OnMaterialCompilationFinished(UMaterialInterface* MaterialInterface);

	void RegisterMaterialCompicationFinished();
#endif

	const TArray<uint8>& GetData() const;
	void StoreData(const uint8_t* data, uint32_t size);
	void LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path);

	void ReassignSearchingMaps();

	UMaterialInterface* FindMatrial(EEffekseerAlphaBlendType alphaBlend);

	bool GenerateColorSpaceMaterial(EEffekseerAlphaBlendType alphaBlend, EEffekseerColorSpaceType colorSpaceType);

	Effekseer::MaterialFile* GetNativePtr() { return internal_; }

	virtual void BeginDestroy() override;

	virtual void Serialize(FArchive& Ar) override;


};