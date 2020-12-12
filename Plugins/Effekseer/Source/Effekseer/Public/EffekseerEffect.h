
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EffekseerModel.h"
#include "EffekseerMaterial.h"
#include "EffekseerAlphaBlendType.h"
#include "EffekseerEffect.generated.h"

UCLASS()
class EFFEKSEER_API UEffekseerEffectMaterialParameterHolder
	: public UObject
{
public:
	GENERATED_BODY()

	UPROPERTY()
	UTexture2D*		Texture = nullptr;

	UPROPERTY()
	EEffekseerAlphaBlendType	AlphaBlend;

	UPROPERTY()
	bool			IsDepthTestDisabled;

	UPROPERTY()
	bool			IsLighting = false;

	UPROPERTY()
	bool			IsDistorted = false;
};

struct EffekseerEffectMaterial
{
	UTexture2D*		Texture = nullptr;

	EEffekseerAlphaBlendType	AlphaBlend;

	bool			IsDepthTestDisabled;

	bool			IsLighting = false;

	bool			IsDistorted = false;

	bool operator < (const EffekseerEffectMaterial& rhs) const
	{
		if (Texture != rhs.Texture)
		{
			return Texture < rhs.Texture;
		}

		if (AlphaBlend != rhs.AlphaBlend)
		{
			return AlphaBlend < rhs.AlphaBlend;
		}

		if (IsDepthTestDisabled != rhs.IsDepthTestDisabled)
		{
			return IsDepthTestDisabled < rhs.IsDepthTestDisabled;
		}

		if (IsLighting != rhs.IsLighting)
		{
			return IsLighting < rhs.IsLighting;
		}

		if (IsDistorted != rhs.IsDistorted)
		{
			return IsDistorted < rhs.IsDistorted;
		}

		return false;
	}
};

UCLASS()
class EFFEKSEER_API UEffekseerEffect : public UObject
{
	GENERATED_BODY()

private:
	void*			effectPtr = nullptr;
	TArray<uint8>	buffer;

	// scale when loaded
	float			loadedScale = 1.0f;

	bool			isDirty = true;

	void LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path, bool isResourceReset);
	void ReleaseEffect();
public:
	void Load(const uint8_t* data, int32_t size, const TCHAR* path);

	void BeginDestroy() override;

	UPROPERTY(Transient)
	int32 Version = -1;

	UPROPERTY(EditAnywhere)
	float Scale = 20.0f;

	UPROPERTY(VisibleAnywhere)
	FString	Name = TEXT("Effect");

	UPROPERTY(VisibleAnywhere)
	TArray<UTexture2D*>	ColorTextures;

	UPROPERTY(VisibleAnywhere)
	TArray<UTexture2D*>	DistortionTextures;

	UPROPERTY(VisibleAnywhere)
	TArray<UEffekseerModel*>	Models;

	UPROPERTY(VisibleAnywhere)
	TArray<UEffekseerMaterial*>	Materials;

	UPROPERTY(Transient)
	TArray<UEffekseerEffectMaterialParameterHolder*>	EffekseerMaterials;
	
	/*
	UPROPERTY(VisibleAnywhere, Transient)
	TArray<UTexture2D*>	ColorTextures;

	UPROPERTY(Transient)
	TArray<UEffekseerMaterial*>	Materials;

	UPROPERTY(VisibleAnywhere, Transient)
	TArray<UEffekseerModel*>	Models;
	*/

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	class UAssetImportData* AssetImportData;
#endif

	void ReloadIfRequired();
	void AssignResources();

	void* GetNativePtr() { return effectPtr; }

	virtual void PostLoad() override;

	virtual void Serialize(FArchive& Ar) override;
};