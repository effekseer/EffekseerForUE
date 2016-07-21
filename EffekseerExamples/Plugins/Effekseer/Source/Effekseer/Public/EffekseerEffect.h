
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

	void LoadEffect(const uint8_t* data, int32_t size);
	void ReleaseEffect();
public:
	void Load(const uint8_t* data, int32_t size);

	void BeginDestroy() override;

	UPROPERTY(EditAnywhere, Transient)
	int32 Version = -1;

	UPROPERTY(EditAnywhere)
	float Scale = 1.0f;

	// TODO Reimport
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Instanced, Category = Reimport)
	class UAssetImportData* AssetImportData;
#endif

	void* GetNativePtr() { return effectPtr; }

	virtual void Serialize(FArchive& Ar) override;
};