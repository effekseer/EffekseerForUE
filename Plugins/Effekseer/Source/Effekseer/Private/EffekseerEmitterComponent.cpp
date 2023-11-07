#include "EffekseerEmitterComponent.h"

#include "EffekseerSystemComponent.h"
#include "EffekseerSystemSceneProxy.h"

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
	std::vector<FEffekseerHandle> handles_;

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
			handles_.push_back(updateData->Handle);
		}
		else if (updateData->Type == EffekseerEmitterUpdateData_CommandType::Remove)
		{
			for (size_t i = 0; i < handles_.size(); i++)
			{
				if (handles_[i].ID == updateData->Handle.ID)
				{
					handles_.erase(handles_.begin() + i);
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
};

void UEffekseerEmitterComponent::ApplyParameters(bool forced)
{
	if (AllColor != AllColor_ || forced)
	{
		system_->SetEffectAllColor(handle_, AllColor);
		AllColor_ = AllColor;
	}

	if (Speed != Speed_ || forced)
	{
		system_->SetEffectSpeed(handle_, Speed);
		Speed_ = Speed;
	}

	bool isDynamicInputChanged = false;
	for (int32_t i = 0; i < DynamicInput.Num(); i++)
	{
		if (DynamicInput[i] != DynamicInput_[i] || forced)
		{
			system_->SetEffectDynamicInput(handle_, i, DynamicInput[i]);
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
		system_->AssignMaterials(lastPlayingEffect, &materials_);
	}

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
	auto sp = new FEffekseerEmitterSceneProxy(this);
	sceneProxy_ = sp;
	return sp;
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
			handle_ = handle;
			isPlaying = true;
			shouldActivate = false;
			ApplyParameters(true);
		}
	}

	if (system_ != nullptr && isPlaying)
	{
		if (system_->Exists(handle_))
		{
			auto transform = GetComponentToWorld();

			system_->SetEffectPosition(handle_, transform.GetTranslation());
			system_->SetEffectRotation(handle_, transform.GetRotation());
			system_->SetEffectScaling(handle_, transform.GetScale3D());

			ApplyParameters(false);
		}
		else
		{
			{
				auto cmd = new EffekseerEmitterUpdateData_Command();
				cmd->Type = EffekseerEmitterUpdateData_CommandType::Remove;
				cmd->Handle = handle_;
				sceneProxy_->UpdateData(cmd);
				handle_ = FEffekseerHandle();
			}

			if (IsLooping)
			{
				AllColor_ = FColor(255, 255, 255, 255);
				Speed_ = 1.0f;
				handle_ = PlayInternal();
			}
			else
			{
				isPlaying = false;

				if (bAutoDestroy)
				{
					DestroyComponent();
					return;
				}
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
		system_->Stop(handle_);

		{
			auto cmd = new EffekseerEmitterUpdateData_Command();
			cmd->Type = EffekseerEmitterUpdateData_CommandType::Remove;
			cmd->Handle = handle_;
			sceneProxy_->UpdateData(cmd);
		}

		isPlaying = false;
	}
}

void UEffekseerEmitterComponent::StopRoot()
{
	if (system_ != nullptr)
	{
		system_->StopRoot(handle_);
	}
}

bool UEffekseerEmitterComponent::Exists() const
{
	if (system_ != nullptr)
	{
		return system_->Exists(handle_);
	}
	return false;
}

void UEffekseerEmitterComponent::SendTrigger(int index)
{
	if (system_ != nullptr)
	{
		return system_->SendTrigger(handle_, index);
	}
}

#if WITH_EDITOR
void UEffekseerEmitterComponent::Preview()
{
	Stop();
	shouldActivate = true;
}
#endif