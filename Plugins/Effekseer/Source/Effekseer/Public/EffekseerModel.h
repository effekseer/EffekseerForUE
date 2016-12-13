
#pragma once

#include "Object.h"
#include "EffekseerModel.generated.h"

UCLASS()
class EFFEKSEER_API UEffekseerModel : public UObject
{
	GENERATED_BODY()
private:
	void*			modelPtr = nullptr;
	TArray<uint8>	buffer;

	void LoadModel(const uint8_t* data, int32_t size, const TCHAR* path);
	void ReleaseModel();
public:
	void Load(const uint8_t* data, int32_t size, const TCHAR* path);

	UPROPERTY(EditAnywhere)
	UStaticMesh*	Mesh = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif

	void* GetNativePtr() { return modelPtr; }

	virtual void Serialize(FArchive& Ar) override;
};