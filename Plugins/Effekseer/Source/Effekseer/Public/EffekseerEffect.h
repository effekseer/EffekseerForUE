
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EffekseerModel.h"
#include "EffekseerMaterial.h"
#include "EffekseerCurve.h"
#include "EffekseerAlphaBlendType.h"
#include "EffekseerEffect.generated.h"

namespace Effekseer
{
class EffectNode;
};

inline bool CompareColor(const FLinearColor& lhs, const FLinearColor& rhs)
{
	if (lhs.R != rhs.R)
	{
		return lhs.R < rhs.R;
	}

	if (lhs.G != rhs.G)
	{
		return lhs.G < rhs.G;
	}

	if (lhs.B != rhs.B)
	{
		return lhs.B < rhs.B;
	}

	if (lhs.A != rhs.A)
	{
		return lhs.A < rhs.A;
	}

	return false;

}

USTRUCT()
struct FFlipbookParameters
{
	GENERATED_USTRUCT_BODY()
	bool Enable = false;
	int32 LoopType = 0;
	int32 DivideX = 1;
	int32 DivideY = 1;

	bool operator==(const FFlipbookParameters& rhs) const
	{
		if (Enable   != rhs.Enable) return false;
		if (LoopType != rhs.LoopType) return false;
		if (DivideX  != rhs.DivideX) return false;
		if (DivideY  != rhs.DivideY) return false;

		return true;
	}

	bool operator!=(const FFlipbookParameters& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator < (const FFlipbookParameters& rhs) const
	{
		if (Enable != rhs.Enable) return Enable < rhs.Enable;
		if (LoopType != rhs.LoopType) return LoopType < rhs.LoopType;
		if (DivideX != rhs.DivideX) return DivideX < rhs.DivideX;
		if (DivideY != rhs.DivideY) return DivideY < rhs.DivideY;
		return false;
	}
};

USTRUCT()
struct FEdgeParameters
{
	GENERATED_USTRUCT_BODY()
	FLinearColor Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	float Threshold = 0;
	float ColorScaling = 1;

	bool operator==(const FEdgeParameters& rhs) const
	{
		if (Color != rhs.Color) return false;
		if (Threshold  != rhs.Threshold) return false;
		if (ColorScaling  != rhs.ColorScaling) return false;

		return true;
	}

	bool operator!=(const FEdgeParameters& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator < (const FEdgeParameters& rhs) const
	{
		if (Color != rhs.Color)
		{
			return CompareColor(Color, rhs.Color);
		}

		if (Threshold != rhs.Threshold)
		{
			return Threshold < rhs.Threshold;
		}

		if (ColorScaling != rhs.ColorScaling)
		{
			return ColorScaling < rhs.ColorScaling;
		}

		return false;
	}
};

USTRUCT()
struct FFalloffParameter
{
	GENERATED_USTRUCT_BODY()

	int32			ColorBlendType = 0;
	FLinearColor	BeginColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	FLinearColor	EndColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	float			Pow = 1.0f;

	bool operator==(const FFalloffParameter& rhs) const
	{
		if (ColorBlendType != rhs.ColorBlendType) return false;
		if (BeginColor != rhs.BeginColor) return false;
		if (EndColor != rhs.EndColor) return false;
		if (Pow != rhs.Pow) return false;

		return true;
	}

	bool operator!=(const FFalloffParameter& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator < (const FFalloffParameter& rhs) const
	{
		if (ColorBlendType != rhs.ColorBlendType) return ColorBlendType < rhs.ColorBlendType;
		if (BeginColor != rhs.BeginColor) return CompareColor(BeginColor,rhs.BeginColor);
		if (EndColor != rhs.EndColor) return CompareColor(EndColor,rhs.EndColor);
		if (Pow != rhs.Pow) return Pow < rhs.Pow;

		return false;
	}
};

USTRUCT()
struct FSoftParticleParameter
{
	GENERATED_USTRUCT_BODY()

	float DepthFadeFar = 0.0f;
	float DepthFadeNear = 0.0f;
	float DepthFadeNearOffset = 0.0f;

	bool operator==(const FSoftParticleParameter& rhs) const
	{
		if (DepthFadeFar != rhs.DepthFadeFar)
			return false;
		if (DepthFadeNear != rhs.DepthFadeNear)
			return false;
		if (DepthFadeNearOffset != rhs.DepthFadeNearOffset)
			return false;

		return true;
	}

	bool operator!=(const FSoftParticleParameter& rhs) const
	{
		return !(*this == rhs);
	}

	bool operator<(const FSoftParticleParameter& rhs) const
	{
		if (DepthFadeFar != rhs.DepthFadeFar)
			return DepthFadeFar < rhs.DepthFadeFar;
		if (DepthFadeNear != rhs.DepthFadeNear)
			return DepthFadeNear < rhs.DepthFadeNear;
		if (DepthFadeNearOffset != rhs.DepthFadeNearOffset)
			return DepthFadeNearOffset < rhs.DepthFadeNearOffset;
		return false;
	}
};

struct EffekseerEffectMaterialKey
{
	UTexture2D* Texture = nullptr;
	int32			TextureAddressType = 0;
	UTexture2D* AlphaTexture = nullptr;
	int32			AlphaTextureAddressType = 0;
	UTexture2D* UVDistortionTexture = nullptr;
	int32			UVDistortionTextureAddressType = 0;
	UTexture2D* BlendTexture = nullptr;
	int32			BlendTextureAddress = 0;
	UTexture2D* BlendAlphaTexture = nullptr;
	int32			BlendAlphaTextureAddress = 0;
	UTexture2D* BlendUVDistortionTexture = nullptr;
	int32			BlendUVDistortionTextureAddress = 0;
	FFlipbookParameters FlipbookParams;
	float			UVDistortionIntensity = 1.0f;
	int32			TextureBlendType = -1;
	float			BlendUVDistortionIntensity = 1.0f;
	bool			EnableFalloff = false;
	FFalloffParameter FalloffParam;
	float			EmissiveScaling = 1;
	FEdgeParameters	EdgeParams;
	FSoftParticleParameter SoftParticleParam;

	EEffekseerAlphaBlendType	AlphaBlend = EEffekseerAlphaBlendType::Opacity;
	bool			IsDepthTestDisabled = false;
	bool			IsLighting = false;
	bool			IsDistorted = false;
	UEffekseerMaterial* Material = nullptr;

	bool operator == (const EffekseerEffectMaterialKey& rhs) const
	{
		return !((*this) < rhs) && !(rhs < (*this));
	}

	bool operator != (const EffekseerEffectMaterialKey& rhs) const
	{
		return !((*this) == rhs);
	}

	bool operator < (const EffekseerEffectMaterialKey& rhs) const
	{
		if (Texture != rhs.Texture)
		{
			return Texture < rhs.Texture;
		}

		if (TextureAddressType != rhs.TextureAddressType)
		{
			return TextureAddressType < rhs.TextureAddressType;
		}

		if (AlphaTexture != rhs.AlphaTexture)
		{
			return AlphaTexture < rhs.AlphaTexture;
		}

		if (AlphaTextureAddressType != rhs.AlphaTextureAddressType)
		{
			return AlphaTextureAddressType < rhs.AlphaTextureAddressType;
		}

		if (UVDistortionTexture != rhs.UVDistortionTexture)
		{
			return UVDistortionTexture < rhs.UVDistortionTexture;
		}

		if (UVDistortionTextureAddressType != rhs.UVDistortionTextureAddressType)
		{
			return UVDistortionTextureAddressType < rhs.UVDistortionTextureAddressType;
		}

		if (BlendTexture != rhs.BlendTexture)
		{
			return BlendTexture < rhs.BlendTexture;
		}

		if (BlendTextureAddress != rhs.BlendTextureAddress)
		{
			return BlendTextureAddress < rhs.BlendTextureAddress;
		}

		if (BlendAlphaTexture != rhs.BlendAlphaTexture)
		{
			return BlendAlphaTexture < rhs.BlendAlphaTexture;
		}

		if (BlendAlphaTextureAddress != rhs.BlendAlphaTextureAddress)
		{
			return BlendAlphaTextureAddress < rhs.BlendAlphaTextureAddress;
		}

		if (BlendUVDistortionTexture != rhs.BlendUVDistortionTexture)
		{
			return BlendUVDistortionTexture < rhs.BlendUVDistortionTexture;
		}

		if (BlendUVDistortionTextureAddress != rhs.BlendUVDistortionTextureAddress)
		{
			return BlendUVDistortionTextureAddress < rhs.BlendUVDistortionTextureAddress;
		}

		if (FlipbookParams != rhs.FlipbookParams)
		{
			return FlipbookParams < rhs.FlipbookParams;
		}

		if (UVDistortionIntensity != rhs.UVDistortionIntensity)
		{
			return UVDistortionIntensity < rhs.UVDistortionIntensity;
		}

		if (TextureBlendType != rhs.TextureBlendType)
		{
			return TextureBlendType < rhs.TextureBlendType;
		}

		if (BlendUVDistortionIntensity != rhs.BlendUVDistortionIntensity)
		{
			return BlendUVDistortionIntensity < rhs.BlendUVDistortionIntensity;
		}

		if (EnableFalloff != rhs.EnableFalloff)
		{
			return EnableFalloff < rhs.EnableFalloff;
		}

		if (FalloffParam != rhs.FalloffParam)
		{
			return FalloffParam < rhs.FalloffParam;
		}

		if (EmissiveScaling != rhs.EmissiveScaling)
		{
			return EmissiveScaling < rhs.EmissiveScaling;
		}

		if (EdgeParams != rhs.EdgeParams)
		{
			return EdgeParams < rhs.EdgeParams;
		}

		if (SoftParticleParam != rhs.SoftParticleParam)
		{
			return SoftParticleParam < rhs.SoftParticleParam;
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

		if (Material != rhs.Material)
		{
			return Material < rhs.Material;
		}


		return false;
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

	UPROPERTY()
	FFalloffParameter FalloffParam;

	UPROPERTY()
	float			EmissiveScaling = 1;

	UPROPERTY()
	FEdgeParameters	EdgeParams;

	UPROPERTY()
	FSoftParticleParameter SoftParticleParam;

	UPROPERTY()
	EEffekseerAlphaBlendType	AlphaBlend = EEffekseerAlphaBlendType::Opacity;

	UPROPERTY()
	bool			IsDepthTestDisabled;

	UPROPERTY()
	bool			IsLighting = false;

	UPROPERTY()
	bool			IsDistorted = false;

	UPROPERTY()
	UEffekseerMaterial* Material = nullptr;

	EffekseerEffectMaterialKey Key;
};

class UEFfekseerProceduralModel;
class ProceduralModelGenerator;

UCLASS()
class EFFEKSEER_API UEffekseerEffect : public UObject
{
	GENERATED_BODY()

	friend class ProceduralModelGenerator;

private:
	void*			effectPtr = nullptr;
	TArray<uint8>	buffer;

	// scale when loaded
	float			loadedScale = 1.0f;

	bool			isDirty = true;

	void LoadEffect(const uint8_t* data, int32_t size, const TCHAR* path, bool isResourceReset);
	void ReleaseEffect();

	void SetTextureAddressMode(::Effekseer::EffectNode* node);

	UPROPERTY(Transient)
	TArray<UEFfekseerProceduralModel*> ProceduralModels;

	void GenerateRenderingData();

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

	void GenerateRenderingDataIfRequired();
	void ReloadIfRequired();
	void AssignResources();

	void* GetNativePtr() { return effectPtr; }

	virtual void PostLoad() override;

	virtual void Serialize(FArchive& Ar) override;
};