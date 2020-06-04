
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EffekseerModel.h"
#include "EffekseerMaterial.h"
#include "EffekseerAlphaBlendType.h"
#include "EffekseerEffect.generated.h"

#ifdef __EFFEKSEER_BUILD_VERSION16__
namespace Effekseer
{
class EffectNode;
};
#endif

UCLASS()
class EFFEKSEER_API UEffekseerEffectMaterialParameterHolder
	: public UObject
{
public:
	GENERATED_BODY()

	UPROPERTY()
	UTexture2D*		Texture = nullptr;

//#ifdef __EFFEKSEER_BUILD_VERSION16__
	UPROPERTY()
	UTexture2D*		AlphaTexture = nullptr;

	UPROPERTY()
	int32			TextureAddressType = 0;

	UPROPERTY()
	int32			AlphaTextureAddressType = 0;
//#endif

	UPROPERTY()
	EEffekseerAlphaBlendType	AlphaBlend;

	UPROPERTY()
	bool			IsDepthTestDisabled;

	UPROPERTY()
	bool			IsLighting = false;

	UPROPERTY()
	bool			IsDistorted = false;

	bool operator == (const UEffekseerEffectMaterialParameterHolder* Other)
	{
		return
			Texture == Other->Texture &&
#ifdef __EFFEKSEER_BUILD_VERSION16__
			AlphaTexture == Other->AlphaTexture &&
			TextureAddressType == Other->TextureAddressType &&
			AlphaTextureAddressType == Other->AlphaTextureAddressType &&
#endif
			AlphaBlend == Other->AlphaBlend &&
			IsDepthTestDisabled == Other->IsDepthTestDisabled &&
			IsLighting == Other->IsLighting &&
			IsDistorted == Other->IsDistorted;
	}

	friend uint32 GetTypeHash(const UEffekseerEffectMaterialParameterHolder* Other)
	{
		return GetTypeHash(Other->Texture);
	}
};

struct EffekseerEffectMaterial
{
	UTexture2D*		Texture = nullptr;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	UTexture2D*		AlphaTexture = nullptr;
#endif

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

#ifdef __EFFEKSEER_BUILD_VERSION16__
		if (AlphaTexture != rhs.AlphaTexture)
		{
			return AlphaTexture < rhs.AlphaTexture;
		}
#endif

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

//#ifdef __EFFEKSEER_BUILD_VERSION16__
	void SetTextureAddressMode(::Effekseer::EffectNode* node);
//#endif

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