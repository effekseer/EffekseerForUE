
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <Effekseer.h>

#include "EffekseerProceduralModel.generated.h"

class FProceduralModelMeshRenderData;
class FMeshElementCollector;
class materialRenderProxy;

UCLASS()
class UEFfekseerProceduralModel
	: public UObject
{
private:
	Effekseer::ModelRef modelPtr_;
	FProceduralModelMeshRenderData* renderData_ = nullptr;

public:
	GENERATED_BODY()

	UEFfekseerProceduralModel();

	~UEFfekseerProceduralModel() override;

	void BeginDestroy() override;

	void Init(Effekseer::ModelRef modelPtr);

	bool GenerateRenderDataIfRequired(ERHIFeatureLevel::Type inFeatureLevel);

	void Render(int32_t viewIndex, FMeshElementCollector& collector, const FMatrix& localToWorld, FMaterialRenderProxy* materialRenderProxy);

	Effekseer::ModelRef GetNativePtr() const
	{
		return modelPtr_;
	}
};