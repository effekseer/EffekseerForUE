
#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "Materials/Material.h"
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

	UPROPERTY(EditAnywhere)
	UMaterial* Material = nullptr;

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

	void StoreData(const uint8_t* data, uint32_t size);
	void LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path);

	void ReassignSearchingMaps();

	Effekseer::Material* GetNativePtr() { return internal_; }

	virtual void Serialize(FArchive& Ar) override;


};