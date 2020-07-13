
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EffekseerModel.h"
#include "EffekseerMaterial.h"
#ifdef __EFFEKSEER_BUILD_VERSION16__
#include "EffekseerCurve.h"
#endif
#include "EffekseerAlphaBlendType.h"
#include "EffekseerEffect.generated.h"

#ifdef __EFFEKSEER_BUILD_VERSION16__
namespace Effekseer
{
class EffectNode;
};
#endif

USTRUCT()
struct FFlipbookParameters
{
	GENERATED_USTRUCT_BODY()
	bool Enable = false;
	int32 LoopType = 0;
	int32 DivideX = 1;
	int32 DivideY = 1;

	bool operator==(const FFlipbookParameters& Params)
	{
		if (Enable   != Params.Enable) return false;
		if (LoopType != Params.LoopType) return false;
		if (DivideX  != Params.DivideX) return false;
		if (DivideY  != Params.DivideY) return false;

		return true;
	}
};

USTRUCT()
struct FEdgeParameters
{
	GENERATED_USTRUCT_BODY()
	FLinearColor Color;
	float Threshold = 0;
	int32_t ColorScaling = 1;

	bool operator==(const FEdgeParameters& Params)
	{
		if (Color != Params.Color) return false;
		if (Threshold  != Params.Threshold) return false;
		if (ColorScaling  != Params.ColorScaling) return false;

		return true;
	}
};

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
	int32			TextureAddressType = 0;

	UPROPERTY()
	UTexture2D*		AlphaTexture = nullptr;

	UPROPERTY()
	int32			AlphaTextureAddressType = 0;

	UPROPERTY()
	UTexture2D*		UVDistortionTexture = nullptr;

	UPROPERTY()
	int32			UVDistortionTextureAddressType = 0;

	UPROPERTY()
	UTexture2D*		BlendTexture = nullptr;

	UPROPERTY()
	int32			BlendTextureAddress = 0;

	UPROPERTY()
	UTexture2D*		BlendAlphaTexture = nullptr;

	UPROPERTY()
	int32			BlendAlphaTextureAddress = 0;

	UPROPERTY()
	UTexture2D*		BlendUVDistortionTexture = nullptr;

	UPROPERTY()
	int32			BlendUVDistortionTextureAddress = 0;

	UPROPERTY()
	FFlipbookParameters FlipbookParams;

	UPROPERTY()
	float			UVDistortionIntensity = 1.0f;

	UPROPERTY()
	int32			TextureBlendType = -1;

	UPROPERTY()
	float			BlendUVDistortionIntensity = 1.0f;

	UPROPERTY()
	bool			EnableFalloff = false;

	struct
	{
		UPROPERTY()
		int32			ColorBlendType = 0;

		UPROPERTY()
		FLinearColor	BeginColor;

		UPROPERTY()
		FLinearColor	EndColor;

		UPROPERTY()
		int32			Pow = 1;
	} FalloffParam;

	UPROPERTY()
	int32			EmissiveScaling = 1;

	UPROPERTY()
	FEdgeParameters	EdgeParams;
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
			TextureAddressType == Other->TextureAddressType &&
			AlphaTexture == Other->AlphaTexture &&
			AlphaTextureAddressType == Other->AlphaTextureAddressType &&
			UVDistortionTexture == Other->UVDistortionTexture &&
			UVDistortionTextureAddressType == Other->UVDistortionTextureAddressType &&
			BlendTexture == Other->BlendTexture &&
			BlendTextureAddress == Other->BlendTextureAddress &&
			BlendAlphaTexture == Other->BlendAlphaTexture &&
			BlendAlphaTextureAddress == Other->BlendAlphaTextureAddress &&
			FlipbookParams == Other->FlipbookParams &&
			UVDistortionIntensity == Other->UVDistortionIntensity && 
			TextureBlendType == Other->TextureBlendType &&
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

	UTexture2D*		UVDistortionTexture = nullptr;

	UTexture2D*		BlendTexture = nullptr;

	UTexture2D*		BlendAlphaTexture = nullptr;

	UTexture2D*		BlendUVDistortionTexture = nullptr;
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

		if (UVDistortionTexture != rhs.UVDistortionTexture)
		{
			return UVDistortionTexture < rhs.UVDistortionTexture;
		}

		if (BlendTexture != rhs.BlendTexture)
		{
			return BlendTexture < rhs.BlendTexture;
		}

		if (BlendAlphaTexture != rhs.BlendAlphaTexture)
		{
			return BlendAlphaTexture < rhs.BlendAlphaTexture;
		}

		if (BlendUVDistortionTexture != rhs.BlendUVDistortionTexture)
		{
			return BlendUVDistortionTexture < rhs.BlendUVDistortionTexture;
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

#ifdef __EFFEKSEER_BUILD_VERSION16__
	void SetTextureAddressMode(::Effekseer::EffectNode* node);
#endif

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

	UPROPERTY(VisibleAnywhere)
	TArray<UEffekseerCurve*> Curves;

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