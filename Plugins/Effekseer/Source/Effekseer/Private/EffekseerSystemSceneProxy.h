
#pragma once

#include "Components/PrimitiveComponent.h"
#include "CoreMinimal.h"
#include "EffekseerSystem.h"

class UEffekseerSystemComponent;

enum class EffekseerUpdateData_CommandType
{
	Play,
	SetP,
	SetR,
	SetS,
	SetPRS,
	Stop,
	StopRoot,
	StopAll,
	SetSpeed,
	SetAllColor,
	SetDynamicInput,
	StartNetwork,
	StopNetwork,
	SendTrigger,
};

class EffekseerUpdateData_Command
{
public:
	//! ID or port
	int32_t ID = -1;
	void* Effect = nullptr;
	FVector Position;
	FQuat Rotation;
	FVector Scale;
	FColor AllColor;

	union
	{
		float Speed;
		int TriggerIndex;
		struct
		{
			float Value;
			int Index;
		} DynamicInput;
	};

	EffekseerUpdateData_CommandType Type;
};

class EffekseerUpdateData
{
public:
	TArray<EffekseerUpdateData_Command> Commands;
	TMap<UEffekseerEffectMaterialParameterHolder*, UMaterialInstanceDynamic*> Materials;
	std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*> NMaterials;

	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> LightingDynamicMaterials;

	float DeltaTime = 0;

	EffekseerUpdateData() = default;
	virtual ~EffekseerUpdateData();
};

class FEffekseerSystemSceneProxy : public FPrimitiveSceneProxy
{
private:
	int32_t maxSprite_ = 10000;
	::Effekseer::ManagerRef effekseerManager = nullptr;
	::Effekseer::RefPtr<::EffekseerRendererUE::RendererImplemented> effekseerRenderer = nullptr;

	::Effekseer::ServerRef server_ = nullptr;
	std::map<std::u16string, ::Effekseer::RefPtr<::Effekseer::Effect>> registeredEffects;

	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> LightingDynamicMaterials;

	TMap<UEffekseerEffectMaterialParameterHolder*, UMaterialInstanceDynamic*> Materials;
	std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*> NMaterials;

	float Time = 0;

	TMap<int, int> internalHandle2EfkHandle_;

	std::mutex mutex_removedHandles_;
	TArray<int32_t> removedHandles_;

	std::shared_ptr<EffekseerRendererUE::EffekseerSystem> system_;

	void CalculateCameraDirectionAndPosition(const Effekseer::Matrix44& matrix, Effekseer::Vector3D& direction, Effekseer::Vector3D& position) const;

public:
	FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent, int32_t maxSprite, int32_t threadCount, EEffekseerColorSpaceType colorSpace);
	virtual ~FEffekseerSystemSceneProxy();

	std::shared_ptr<EffekseerRendererUE::EffekseerSystem> GetSystem()
	{
		return system_;
	}

	virtual SIZE_T GetTypeHash() const;

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize() const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}

	void ConvertInternalHandleToEffekseerHandles(const std::vector<int32_t>& internalHandles, std::vector<int32_t>& efkHandles);

	void UpdateData_RenderThread(EffekseerUpdateData* updateData);

	// This function can be called out of renderThread.
	void UpdateData(EffekseerUpdateData* updateData);

	TArray<int32_t> PopRemovedHandles();
};