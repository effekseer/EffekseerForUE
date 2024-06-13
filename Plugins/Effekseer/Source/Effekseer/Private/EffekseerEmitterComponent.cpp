#include "EffekseerEmitterComponent.h"

#include "EffekseerSystemComponent.h"
#include "EffekseerSystemSceneProxy.h"

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

enum class EffekseerEmitterUpdateData_CommandType
{
	Add,
	Remove,
};

struct EffekseerEmitterUpdateData_Command
{
	EffekseerEmitterUpdateData_CommandType Type;
	FEffekseerHandle Handle;
};

class FEffekseerEmitterSceneProxy : public FPrimitiveSceneProxy
{
private:
	const UEffekseerEmitterComponent* component_ = nullptr;
	TArray<FEffekseerHandle> handles_;

public:
	FEffekseerEmitterSceneProxy(const UEffekseerEmitterComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
	{
		component_ = InComponent;
	}

	virtual ~FEffekseerEmitterSceneProxy() override
	{
	}

	virtual SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
#if WITH_EDITOR
		if (GShaderCompilingManager->IsCompiling())
		{
		//	return;
		}
#endif

		auto systemComponent = component_->system_;
		if (systemComponent != nullptr)
		{
			auto systemComponentSceneProxy = static_cast<FEffekseerSystemSceneProxy*>(systemComponent->SceneProxy);
			auto system = systemComponentSceneProxy->GetSystem();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (!(VisibilityMap & (1 << ViewIndex)))
					continue;

				const auto& view = Views[ViewIndex];

				{
					std::lock_guard<std::mutex> lock(system->Mutex);

					// TODO : fix memory allocation
					std::vector<int> internalHandlesTemp_;
					std::vector<int> efkHandlesTemp_;

					internalHandlesTemp_.clear();
					for (const auto h : handles_)
					{
						internalHandlesTemp_.push_back(h.ID);
					}
					systemComponentSceneProxy->ConvertInternalHandleToEffekseerHandles(internalHandlesTemp_, efkHandlesTemp_);
					system->Render(efkHandlesTemp_, systemComponentSceneProxy->GetLocalToWorld(), view->ViewMatrices.GetViewMatrix(), ViewIndex, Collector);
				}
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		bool bVisible = true;
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bOpaque = false;
		Result.bMasked = false;
		Result.bSeparateTranslucency = true;
		Result.bNormalTranslucency = true;
		Result.bDistortion = true;

		return Result;
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize() const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}

	void UpdateData_RenderThread(EffekseerEmitterUpdateData_Command* updateData)
	{
		check(IsInRenderingThread());

		if (updateData->Type == EffekseerEmitterUpdateData_CommandType::Add)
		{
			handles_.Add(updateData->Handle);
		}
		else if (updateData->Type == EffekseerEmitterUpdateData_CommandType::Remove)
		{
			for (int32_t i = 0; i < handles_.Num(); i++)
			{
				if (handles_[i].ID == updateData->Handle.ID)
				{
					handles_.RemoveAt(i);
					break;
				}
			}
		}
		delete updateData;
	}

	// This function can be called out of renderThread.
	void UpdateData(EffekseerEmitterUpdateData_Command* updateData)
	{
		ENQUEUE_RENDER_COMMAND(
			EffekseerEmitterUpdateData_Command)
		([this, updateData](FRHICommandListImmediate& RHICmdList)
		 { this->UpdateData_RenderThread(updateData); });
	}

	virtual void UpdatedReferencedMaterials()
	{
		// copied from FPrimitiveSceneProxy::FPrimitiveSceneProxy()
#if WITH_EDITOR
		TArray<UMaterialInterface*> Materials;
		component_->GetUsedMaterials(Materials, true);
		ENQUEUE_RENDER_COMMAND(FMeshRenderBufferSetDestroy)
		(
			[this, Materials](FRHICommandListImmediate& RHICmdList)
			{
				this->SetUsedMaterialForVerification(Materials);
			});
#endif
	}

	virtual void UpdatedReferencedMaterialsWithHandles(TArray<FEffekseerHandle> handles)
	{
		// copied from FPrimitiveSceneProxy::FPrimitiveSceneProxy()
#if WITH_EDITOR
		TArray<UMaterialInterface*> Materials;
		component_->GetUsedMaterials(Materials, true);
		ENQUEUE_RENDER_COMMAND(FMeshRenderBufferSetDestroy)
		(
			[this, Materials, handles](FRHICommandListImmediate& RHICmdList)
			{
				handles_ = handles;
				this->SetUsedMaterialForVerification(Materials);
			});
#endif
	}

	const UEffekseerEmitterComponent* GetInComponent() const
	{
		return component_;
	}
};

void UEffekseerEmitterComponent::ApplyParameters(const FEffekseerHandle& handle, bool forced)
{
	if (AllColor != AllColor_ || forced)
	{
		system_->SetEffectAllColor(handle, AllColor);
		AllColor_ = AllColor;
	}

	if (Speed != Speed_ || forced)
	{
		system_->SetEffectSpeed(handle, Speed);
		Speed_ = Speed;
	}

	bool isDynamicInputChanged = false;
	for (int32_t i = 0; i < DynamicInput.Num(); i++)
	{
		if (DynamicInput[i] != DynamicInput_[i] || forced)
		{
			system_->SetEffectDynamicInput(handle, i, DynamicInput[i]);
			isDynamicInputChanged = true;
		}
	}

	if (isDynamicInputChanged || forced)
	{
		DynamicInput_ = DynamicInput;
	}
}

FEffekseerHandle UEffekseerEmitterComponent::PlayInternal()
{
	if (System != nullptr)
	{
		for (auto it : System->GetComponents())
		{
			auto cas = Cast<UEffekseerSystemComponent>(it);
			if (cas != nullptr)
			{
				system_ = cas;
				break;
			}
		}
	}

	if (system_ == nullptr)
	{
		System = nullptr;
	}

	// Find a System
	if (System == nullptr)
	{
		for (TActorIterator<AActor> ait(GetWorld()); ait; ++ait)
		{
			for (auto it : ait->GetComponents())
			{
				auto cas = Cast<UEffekseerSystemComponent>(it);
				if (cas != nullptr)
				{
					system_ = cas;
					System = *ait;
					goto LoopExit;
				}
			}
		}
	}
LoopExit:;

	if (system_ == nullptr)
		return FEffekseerHandle();

	if (lastPlayingEffect != Effect)
	{
		Stop();
		lastPlayingEffect = Effect;
	}

	// TODO : Optimize
	system_->AssignMaterials(lastPlayingEffect, &materials_);

	auto transform = GetComponentToWorld();

	auto loc = transform.GetTranslation();

	auto ret = system_->Play(Effect, loc);

	sceneProxy_->UpdatedReferencedMaterials();
	{
		auto cmd = new EffekseerEmitterUpdateData_Command();
		cmd->Type = EffekseerEmitterUpdateData_CommandType::Add;
		cmd->Handle = ret;
		sceneProxy_->UpdateData(cmd);
	}

	return ret;
}

UEffekseerEmitterComponent::UEffekseerEmitterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickInEditor = true;
	bAutoDestroy = false;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostPhysics;

	if (DynamicInput.Num() < 4)
	{
		for (size_t i = DynamicInput.Num(); i < 4; i++)
		{
			DynamicInput.Add(0.0f);
		}
	}

	DynamicInput_.Empty();
	for (size_t i = 0; i < 4; i++)
	{
		DynamicInput_.Add(0.0f);
	}
}

UEffekseerEmitterComponent::~UEffekseerEmitterComponent()
{
}

FPrimitiveSceneProxy* UEffekseerEmitterComponent::CreateSceneProxy()
{
	if (sceneProxy_ != nullptr)
	{
		sceneProxy_ = new FEffekseerEmitterSceneProxy(this);

		if (system_ != nullptr && lastPlayingEffect != nullptr)
		{
			system_->AssignMaterials(lastPlayingEffect, &materials_);
		}

		sceneProxy_->UpdatedReferencedMaterialsWithHandles(handles_);
	}
	else
	{
		sceneProxy_ = new FEffekseerEmitterSceneProxy(this);
	}

	return sceneProxy_;
}

UMaterialInterface* UEffekseerEmitterComponent::GetMaterial(int32 ElementIndex) const
{
	return materials_[ElementIndex];
}

void UEffekseerEmitterComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	for (const auto& e : materials_)
	{
		OutMaterials.Add(e);
	}
}

int32 UEffekseerEmitterComponent::GetNumMaterials() const
{
	return materials_.Num();
}

FBoxSphereBounds UEffekseerEmitterComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// TODO Optimize a size of bounding box.
	float infinity = FLT_MAX / 100.0f;

	return FBoxSphereBounds(LocalToWorld.GetLocation(), FVector(infinity, infinity, infinity), infinity);
}

#if WITH_EDITOR
void UEffekseerEmitterComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Stop();
	if (bAutoActivate)
	{
		shouldActivate = true;
		autoActivateOnActivate_ = bAutoActivate;
	}
}
#endif

void UEffekseerEmitterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffekseerEmitterComponent::BeginDestroy()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::BeginDestroy();
}

void UEffekseerEmitterComponent::OnUnregister()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::OnUnregister();
}

void UEffekseerEmitterComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void UEffekseerEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// HACK for activate
	if (shouldActivate && autoActivateOnActivate_ != bAutoActivate)
	{
		shouldActivate = false;
	}

	if (shouldActivate)
	{
		auto handle = PlayInternal();

		if (handle.Effect != nullptr)
		{
			handles_.Add(handle);
			isPlaying = true;
			shouldActivate = false;
			ApplyParameters(handle, true);
		}
	}

	if (system_ != nullptr && isPlaying)
	{
		for (int i = 0; i < handles_.Num();)
		{
			auto handle = handles_[i];
			if (system_->Exists(handle))
			{
				auto transform = GetComponentToWorld();

				system_->SetEffectPosition(handle, transform.GetTranslation());
				system_->SetEffectRotation(handle, transform.GetRotation());
				system_->SetEffectScaling(handle, transform.GetScale3D());

				ApplyParameters(handle, false);
				i++;
			}
			else
			{
				{
					auto cmd = new EffekseerEmitterUpdateData_Command();
					cmd->Type = EffekseerEmitterUpdateData_CommandType::Remove;
					cmd->Handle = handle;
					sceneProxy_->UpdateData(cmd);
				}

				if (IsLooping)
				{
					AllColor_ = FColor(255, 255, 255, 255);
					Speed_ = 1.0f;
					auto new_handle = PlayInternal();

					if (new_handle.Effect != nullptr)
					{
						handles_[i] = new_handle;
						ApplyParameters(new_handle, true);
						i++;
					}
					else
					{
						handles_.RemoveAt(i);
					}
				}
				else
				{
					handles_.RemoveAt(i);
				}
			}
		}

		if (handles_.Num() == 0)
		{
			isPlaying = false;

			if (bAutoDestroy)
			{
				DestroyComponent();
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEffekseerEmitterComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (isPlaying && bReset)
	{
		return;
	}

	if (isPlaying)
	{
		Stop();
	}

	shouldActivate = true;
	autoActivateOnActivate_ = bAutoActivate;
}

void UEffekseerEmitterComponent::Deactivate()
{
	if (isPlaying)
	{
		Stop();
	}
	system_ = nullptr;

	Super::Deactivate();
}

void UEffekseerEmitterComponent::Stop()
{
	if (system_ != nullptr)
	{
		for (auto handle : handles_)
		{
			system_->Stop(handle);

			{
				auto cmd = new EffekseerEmitterUpdateData_Command();
				cmd->Type = EffekseerEmitterUpdateData_CommandType::Remove;
				cmd->Handle = handle;
				sceneProxy_->UpdateData(cmd);
			}
		}

		handles_.Empty();

		isPlaying = false;
	}
}

void UEffekseerEmitterComponent::StopRoot()
{
	if (system_ != nullptr)
	{
		for (auto handle : handles_)
		{
			system_->StopRoot(handle);
		}
	}
}

bool UEffekseerEmitterComponent::Exists() const
{
	if (system_ != nullptr)
	{
		bool exists = false;
		for (auto handle : handles_)
		{
			exists |= system_->Exists(handle);
		}
		return exists;
	}
	return false;
}

void UEffekseerEmitterComponent::SendTrigger(int index)
{
	if (system_ != nullptr)
	{
		for (auto handle : handles_)
		{
			system_->SendTrigger(handle, index);
		}
	}
}

#if WITH_EDITOR
void UEffekseerEmitterComponent::Preview()
{
	Stop();
	shouldActivate = true;
}
#endif