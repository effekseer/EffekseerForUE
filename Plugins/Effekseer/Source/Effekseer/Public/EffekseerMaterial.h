
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

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif

	void StoreData(const uint8_t* data, uint32_t size);
	void LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path);

	Effekseer::Material* GetNativePtr() { return internal_; }

	virtual void Serialize(FArchive& Ar) override;
};