
#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "Materials/Material.h"
#include "EffekseerAlphaBlendType.h"
#include "EditorFramework/AssetImportData.h"
#include "EffekseerMaterial.generated.h"

namespace Effekseer
{

class Material;

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
	Effekseer::Material* internal_ = nullptr;
	TArray<uint8> buffer_;

	void ReleaseMaterial();
public:

	UPROPERTY(EditInstanceOnly)
	UMaterial* Material = nullptr;

	UPROPERTY(EditInstanceOnly)
	TArray<FEffekseerMaterialElement> MaterialElements;

	UPROPERTY(VisibleAnywhere)
	TArray<FEffekseerUniformProperty> Uniforms;

	UPROPERTY(Transient)
	TMap<FString, int> UniformNameToIndex;

	UPROPERTY(VisibleAnywhere)
	TArray<FEffekseerTextureProperty> Textures;

	UPROPERTY(Transient)
	TMap<FString, int> TextureNameToIndex;


#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif

	const TArray<uint8>& GetData() const;
	void StoreData(const uint8_t* data, uint32_t size);
	void LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path);

	void ReassignSearchingMaps();

	UMaterialInterface* FindMatrial(EEffekseerAlphaBlendType alphaBlend) const;

	Effekseer::Material* GetNativePtr() { return internal_; }

	virtual void Serialize(FArchive& Ar) override;


};