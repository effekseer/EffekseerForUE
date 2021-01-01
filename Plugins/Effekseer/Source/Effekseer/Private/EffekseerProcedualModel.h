
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include <Effekseer.h>

#include "EffekseerProcedualModel.generated.h"

class FProcedualModelMeshRenderData;
class FMeshElementCollector;
class materialRenderProxy;

UCLASS()
class UEFfekseerProcedualModel
	: public UObject
{
private:
	Effekseer::ModelRef modelPtr_;
	FProcedualModelMeshRenderData* renderData_ = nullptr;

public:
	GENERATED_BODY()

	UEFfekseerProcedualModel();

	~UEFfekseerProcedualModel() override;
	
	void Init(Effekseer::ModelRef modelPtr);

	bool GenerateRenderDataIfRequired(ERHIFeatureLevel::Type inFeatureLevel);

	void Render(int32_t viewIndex, FMeshElementCollector& collector, const FMatrix& localToWorld, FMaterialRenderProxy* materialRenderProxy);

	Effekseer::ModelRef GetNativePtr() const
	{
		return modelPtr_;
	}
};