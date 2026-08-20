
#pragma once

#include "CoreMinimal.h"
#include "EditorFramework/AssetImportData.h"
#include "EffekseerUECompatibility.h"
#include "Engine/StaticMesh.h"
#include "UObject/Object.h"

#include "EffekseerCurve.generated.h"

namespace Effekseer
{
class Curve;
};

struct FEffekseerCurve
{
	int mControllPointCount;
	TArray<FVector4> mControllPoint;

	int mKnotCount;
	TArray<double> mKnotValue;

	int mOrder;
	int mStep;
	int mType;
	int mDimension;

	float mLength;
};

UCLASS()
class EFFEKSEER_API UEffekseerCurve : public UObject
{
	GENERATED_BODY()
private:
	Effekseer::Curve* curvePtr = nullptr;
	TArray<uint8> buffer;

	void LoadModel(const uint8_t* data, int32_t size, const TCHAR* path);
	void ReleaseModel();

public:
	void BeginDestroy() override;

	void Load(const uint8_t* data, int32_t size, const TCHAR* path);

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif
#if EFFEKSEER_UE_HAS_ASSET_REGISTRY_TAGS_CONTEXT
	void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
#else
	void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif

	void AssignInternalPtr();

	FEffekseerCurve GetCurve();

	Effekseer::Curve* GetNativePtr() const
	{
		return curvePtr;
	}

	virtual void Serialize(FArchive& Ar) override;
};
