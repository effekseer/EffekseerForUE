#include "EffekseerPrivatePCH.h"
#include "EffekseerSystemComponent.h"

#include "DynamicMeshBuilder.h"

#include "EffekseerNative.h"
#include "EffekseerRendererImplemented.h"

EffekseerUpdateData::EffekseerUpdateData()
{

}

EffekseerUpdateData::~EffekseerUpdateData()
{
	for (auto c : Commands)
	{
		if (c.Effect == nullptr) continue;

		auto p_ = (::Effekseer::Effect*)c.Effect;
		p_->Release();
	}
}

class FEffekseerSystemSceneProxy : public FPrimitiveSceneProxy
{
private:
	static const int32_t	particleMax = 10000;
	::Effekseer::Manager*	effekseerManager = nullptr;
	::EffekseerRendererUE4::RendererImplemented*	effekseerRenderer = nullptr;
	
	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;

	float	Time = 0;

	TMap<int, int>	internalHandle2EfkHandle;

	// この実装でいいのだろうか？
	::Effekseer::CriticalSection criticalSection;
	TArray<int32_t> removedHandles;

public:
	FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
	{
		effekseerManager = ::Effekseer::Manager::Create(particleMax);
		effekseerRenderer = ::EffekseerRendererUE4::RendererImplemented::Create();
		effekseerRenderer->Initialize();

		effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
		effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
		effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
		effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	}

	virtual ~FEffekseerSystemSceneProxy()
	{
		if (effekseerManager != nullptr)
		{
			effekseerManager->Destroy();
			effekseerManager = nullptr;
		}

		if (effekseerRenderer != nullptr)
		{
			effekseerRenderer->Destory();
			effekseerRenderer = nullptr;
		}
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (!(VisibilityMap & (1 << ViewIndex))) continue;

			effekseerRenderer->SetLocalToWorld(GetLocalToWorld());
			effekseerRenderer->SetMaterials(&OpaqueDynamicMaterials, 0);
			effekseerRenderer->SetMaterials(&TranslucentDynamicMaterials, 1);
			effekseerRenderer->SetMaterials(&AdditiveDynamicMaterials, 2);
			effekseerRenderer->SetMaterials(&SubtractiveDynamicMaterials, 3);
			effekseerRenderer->SetMaterials(&ModulateDynamicMaterials, 4);

			effekseerRenderer->SetMeshElementCollector(&Collector);
			effekseerRenderer->SetViewIndex(ViewIndex);

			{
				auto vmat = Views[ViewIndex]->ViewMatrices.ViewMatrix;
				::Effekseer::Matrix44 evmat = *((::Effekseer::Matrix44*)(&vmat));

				std::swap(evmat.Values[1][0], evmat.Values[2][0]);
				std::swap(evmat.Values[1][1], evmat.Values[2][1]);
				std::swap(evmat.Values[1][2], evmat.Values[2][2]);

				effekseerRenderer->SetCameraMatrix(evmat);
				effekseerRenderer->BeginRendering();
				effekseerManager->Draw();
				effekseerRenderer->EndRendering();
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
		Result.bOpaqueRelevance = false;
		Result.bMaskedRelevance = false;
		Result.bSeparateTranslucencyRelevance = true;
		Result.bNormalTranslucencyRelevance = true;
		
		return Result;
	}
	virtual uint32 GetMemoryFootprint() const { return sizeof(*this) + GetAllocatedSize(); }
	uint32 GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

	void UpdateData_RenderThread(EffekseerUpdateData* updateData)
	{
		OpaqueDynamicMaterials = updateData->OpaqueDynamicMaterials;
		TranslucentDynamicMaterials = updateData->TranslucentDynamicMaterials;
		AdditiveDynamicMaterials = updateData->AdditiveDynamicMaterials;
		SubtractiveDynamicMaterials = updateData->SubtractiveDynamicMaterials;
		ModulateDynamicMaterials = updateData->ModulateDynamicMaterials;

		// TODO いずれ高速に処理できる方法を考える。
		
		// Execute commands.
		for (auto i = 0; i < updateData->Commands.Num(); i++)
		{
			auto& cmd = updateData->Commands[i];
			if (cmd.Type == EffekseerUpdateData_CommandType::Play)
			{
				auto effect = (::Effekseer::Effect*)updateData->Commands[i].Effect;
				auto position = updateData->Commands[i].Position;

				auto eid = effekseerManager->Play(effect, position.X, position.Z, position.Y);
				internalHandle2EfkHandle.Add(cmd.ID, eid);
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::SetP)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto position = updateData->Commands[i].Position;
					effekseerManager->SetLocation(eid, position.X, position.Z, position.Y);
				}
			}
		}

		// Update effects.
		Time += updateData->DeltaTime;

		auto frame = (int32_t)(Time * 60);
		Time -= frame * (1.0f / 60.0f);

		{
			if (effekseerManager != nullptr)
			{
				effekseerManager->Update(frame);
			}
		}
		
		// Check existence.
		{
			criticalSection.Enter();

			TArray<int32_t> removingHandle;
			for (auto& kv : internalHandle2EfkHandle)
			{
				if (!effekseerManager->Exists(kv.Value))
				{
					removingHandle.Add(kv.Key);
					removedHandles.Add(kv.Key);
				}
			}

			for (auto& k : removingHandle)
			{
				internalHandle2EfkHandle.Remove(k);
			}

			criticalSection.Leave();
		}

		delete updateData;
	}

	// This function can be called out of renderThread.
	void UpdateData(EffekseerUpdateData* updateData)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			ParticleUpdateDataCommand,
			FEffekseerSystemSceneProxy*, Proxy, this,
			EffekseerUpdateData*, Data, updateData,
			{
				Proxy->UpdateData_RenderThread(Data);
			}
		);
	}

	TArray<int32_t> PopRemovedHandles()
	{
		criticalSection.Enter();
		auto ret = removedHandles;
		removedHandles.Reset();
		criticalSection.Leave();
		return ret;
	}
};

UEffekseerSystemComponent::UEffekseerSystemComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	currentUpdateData = new EffekseerUpdateData();
}

UEffekseerSystemComponent::~UEffekseerSystemComponent()
{
	ES_SAFE_DELETE(currentUpdateData);
}

void UEffekseerSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffekseerSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	auto sp = (FEffekseerSystemSceneProxy*)sceneProxy;

	sp->UpdateData(currentUpdateData);
	currentUpdateData = new EffekseerUpdateData();

	currentUpdateData->OpaqueDynamicMaterials = OpaqueDynamicMaterials;
	currentUpdateData->TranslucentDynamicMaterials = TranslucentDynamicMaterials;
	currentUpdateData->AdditiveDynamicMaterials = AdditiveDynamicMaterials;
	currentUpdateData->SubtractiveDynamicMaterials = SubtractiveDynamicMaterials;
	currentUpdateData->ModulateDynamicMaterials = ModulateDynamicMaterials;

	currentUpdateData->DeltaTime = DeltaTime;

	auto removedHandles = sp->PopRemovedHandles();
	for (auto& h : removedHandles)
	{
		internalHandle2EfkHandle.Remove(h);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UEffekseerSystemComponent::CreateSceneProxy()
{
	auto sp = new FEffekseerSystemSceneProxy(this);
	sceneProxy = sp;
	return sp;
}

UMaterialInterface* UEffekseerSystemComponent::GetMaterial(int32 ElementIndex) const
{
	if (ElementIndex == 0) 	return OpaqueMaterial;
	if (ElementIndex == 1) 	return TranslucentMaterial;
	if (ElementIndex == 2) 	return AdditiveMaterial;
	if (ElementIndex == 3) 	return SubtractiveMaterial;
	if (ElementIndex == 4) 	return ModulateMaterial;

	
	return nullptr;
}

void UEffekseerSystemComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials) const
{
	if (OpaqueMaterial) OutMaterials.Add(OpaqueMaterial);
	if (TranslucentMaterial) OutMaterials.Add(TranslucentMaterial);
	if (AdditiveMaterial) OutMaterials.Add(AdditiveMaterial);
	if (SubtractiveMaterial) OutMaterials.Add(SubtractiveMaterial);
	if (ModulateMaterial) OutMaterials.Add(ModulateMaterial);
}

int32 UEffekseerSystemComponent::GetNumMaterials()const
{
	auto ret = 0;
	if (OpaqueMaterial) ret++;
	if (TranslucentMaterial) ret++;
	if (AdditiveMaterial) ret++;
	if (SubtractiveMaterial) ret++;
	if (ModulateMaterial) ret++;

	return ret;
}


FBoxSphereBounds UEffekseerSystemComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// TODO 最適な数値にする
	float infinity = FLT_MAX / 100.0f;

	return FBoxSphereBounds(LocalToWorld.GetLocation(), FVector(infinity, infinity, infinity), infinity);
}

FEffekseerHandle UEffekseerSystemComponent::Play(UEffekseerEffect* effect, FVector position)
{
	if (effect == nullptr) return FEffekseerHandle();
	if (effect->GetNativePtr() == nullptr) return FEffekseerHandle();

	// システムからの相対位置に変換する。
	position -= this->RelativeLocation;

	// 動的にマテリアルを生成する。
	UMaterialInstanceConstant* _mats[5];
	TMap<UTexture2D*, UMaterialInstanceDynamic*>* _matss[5];

	_mats[0] = OpaqueMaterial;
	_mats[1] = TranslucentMaterial;
	_mats[2] = AdditiveMaterial;
	_mats[3] = SubtractiveMaterial;
	_mats[4] = ModulateMaterial;
	
	_matss[0] = &OpaqueDynamicMaterials;
	_matss[1] = &TranslucentDynamicMaterials;
	_matss[2] = &AdditiveDynamicMaterials;
	_matss[3] = &SubtractiveDynamicMaterials;
	_matss[4] = &ModulateDynamicMaterials;

	for(int i = 0; i < 5; i++)
	{
		auto& mat = _mats[i];
		auto& mats = *_matss[i];

		if (!mats.Contains(nullptr))
		{
			if (mat != nullptr)
			{
				auto dynamicMaterial = UMaterialInstanceDynamic::Create(mat, this);
				mats.Add(nullptr, dynamicMaterial);
			}
		}

		for (auto& tex : effect->ColorTextures)
		{
			if (!mats.Contains(tex))
			{
				if (mat != nullptr)
				{
					auto dynamicMaterial = UMaterialInstanceDynamic::Create(mat, this);
					dynamicMaterial->SetTextureParameterValue(TEXT("ColorTexture"), tex);
					mats.Add(tex, dynamicMaterial);
				}
			}
		}
	}


	effect->ReloadIfRequired();

	auto p = (::Effekseer::Effect*)effect->GetNativePtr();

	p->AddRef();
	
	auto handle = nextInternalHandle;
	
	internalHandle2EfkHandle.Add(handle, -1);

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::Play;
	cmd.Effect = p;
	cmd.ID = handle;
	cmd.Position = position;
	
	currentUpdateData->Commands.Add(cmd);

	nextInternalHandle++;

	FEffekseerHandle ehandle;
	ehandle.Effect = effect;
	ehandle.ID = handle;
	ehandle.System = this;

	return ehandle;
}

void UEffekseerSystemComponent::SetEffectPosition(FEffekseerHandle handle, FVector position)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetP;
	cmd.ID = handle.ID;
	cmd.Position = position;

	currentUpdateData->Commands.Add(cmd);
}

bool UEffekseerSystemComponent::Exists(FEffekseerHandle handle)
{
	if (handle.Effect == nullptr) return false;
	return internalHandle2EfkHandle.Contains(handle.ID);
}