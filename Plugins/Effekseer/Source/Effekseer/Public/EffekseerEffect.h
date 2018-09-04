
#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "EffekseerModel.h"
#include "EffekseerEffect.generated.h"

UENUM()
enum class EEffekseerAlphaBlendType : uint8
{
	/// <summary>
	/// ïsìßñæ
	/// </summary>
	Opacity = 0,
	/// <summary>
	/// ìßñæ
	/// </summary>
	Blend = 1,
	/// <summary>
	/// â¡éZ
	/// </summary>
	Add = 2,
	/// <summary>
	/// å∏éZ
	/// </summary>
	Sub = 3,
	/// <summary>
	/// èÊéZ
	/// </summary>
	Mul = 4,
};


UCLASS()
class EFFEKSEER_API UEffekseerMaterial
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

	bool operator == (const UEffekseerMaterial* Other)
	{
		return
			Texture == Other->Texture &&
			AlphaBlend == Other->AlphaBlend &&
			IsDepthTestDisabled == Other->IsDepthTestDisabled &&
			IsLighting == Other->IsLighting &&
			IsDistorted == Other->IsDistorted;
	}

	friend uint32 GetTypeHash(const UEffekseerMaterial* Other)
	{
		return GetTypeHash(Other->Texture);
	}
};

struct EffekseerMaterial
{
	UTexture2D*		Texture = nullptr;

	EEffekseerAlphaBlendType	AlphaBlend;

	bool			IsDepthTestDisabled;

	bool			IsLighting = false;

	bool			IsDistorted = false;

	bool operator < (const EffekseerMaterial& rhs) const
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
	float Scale = 1.0f;

	UPROPERTY(VisibleAnywhere)
	FString	Name = TEXT("Effect");

	UPROPERTY(VisibleAnywhere)
	TArray<UTexture2D*>	ColorTextures;

	UPROPERTY(VisibleAnywhere)
	TArray<UTexture2D*>	DistortionTextures;

	UPROPERTY(VisibleAnywhere)
	TArray<UEffekseerModel*>	Models;

	UPROPERTY(Transient)
	TArray<UEffekseerMaterial*>	Materials;
	
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