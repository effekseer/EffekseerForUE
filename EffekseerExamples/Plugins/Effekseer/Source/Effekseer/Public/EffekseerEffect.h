
#pragma once

#include "Object.h"
#include "EffekseerEffect.generated.h"

UCLASS()
class EFFEKSEER_API UEffekseerEffect : public UObject
{
	GENERATED_BODY()

private:
	void*			effectPtr = nullptr;
	TArray<uint8>	buffer;

	// ロード時に設定されていたスケール
	float			loadedScale = 1.0f;

	void LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path);
	void ReleaseEffect();
public:
	void Load(const uint8_t* data, int32_t size, const TCHAR* path);

	void BeginDestroy() override;

	UPROPERTY(VisibleAnywhere, Transient)
	int32 Version = -1;

	UPROPERTY(EditAnywhere)
	float Scale = 1.0f;

	UPROPERTY(VisibleAnywhere, Transient)
	TArray<UTexture2D*>	ColorTextures;

	// TODO Reimport
#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	class UAssetImportData* AssetImportData;
#endif

	void ReloadIfRequired();
	void AssignResources();

	void* GetNativePtr() { return effectPtr; }

	virtual void Serialize(FArchive& Ar) override;
};