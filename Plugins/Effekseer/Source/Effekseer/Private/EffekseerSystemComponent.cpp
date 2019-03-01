#include "EffekseerSystemComponent.h"

#include "DynamicMeshBuilder.h"

#include "EffekseerNative.h"
#include "EffekseerRendererImplemented.h"

#include <mutex>

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
	int32_t	maxSprite_ = 10000;
	::Effekseer::Manager*	effekseerManager = nullptr;
	::EffekseerRendererUE4::RendererImplemented*	effekseerRenderer = nullptr;
	
#ifdef _WIN32
	::Effekseer::Server*	server = nullptr;
	std::map<std::u16string, ::Effekseer::Effect*> registeredEffects;
#endif

	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> LightingDynamicMaterials;

	TMap<UEffekseerMaterial*, UMaterialInstanceDynamic*> Materials;
	std::map<EffekseerMaterial, UMaterialInstanceDynamic*> NMaterials;

	float	Time = 0;

	TMap<int, int>	internalHandle2EfkHandle;

	// is it safe?
	std::mutex criticalSection;
	TArray<int32_t> removedHandles;

public:
	FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent, int32_t maxSprite)
		: FPrimitiveSceneProxy(InComponent)
		, maxSprite_(maxSprite)
	{
		effekseerManager = ::Effekseer::Manager::Create(maxSprite_);
		effekseerRenderer = ::EffekseerRendererUE4::RendererImplemented::Create();
		effekseerRenderer->Initialize(maxSprite_);

		effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
		effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
		effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
		effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
		effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

		// To avoid verify error (I don't know why caused error)
		bVerifyUsedMaterials = false;
	}

	virtual ~FEffekseerSystemSceneProxy()
	{
#ifdef _WIN32
		if (server != nullptr)
		{
			ES_SAFE_DELETE(server);
			server = nullptr;
		}
#endif

		if (effekseerManager != nullptr)
		{
			effekseerManager->Destroy();
			effekseerManager = nullptr;
		}

		if (effekseerRenderer != nullptr)
		{
			effekseerRenderer->Destroy();
			effekseerRenderer = nullptr;
		}
	}

#if ENGINE_MINOR_VERSION >= 19
	virtual SIZE_T GetTypeHash() const
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}
#endif

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
			effekseerRenderer->SetMaterials(&LightingDynamicMaterials, 5);

			effekseerRenderer->SetNMaterials(NMaterials);

			effekseerRenderer->SetMeshElementCollector(&Collector);
			effekseerRenderer->SetViewIndex(ViewIndex);

			{
				auto vmat = Views[ViewIndex]->ViewMatrices.GetViewMatrix();
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
		Result.bDistortionRelevance = true;

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
		LightingDynamicMaterials = updateData->LightingDynamicMaterials;

		Materials = updateData->Materials;
		NMaterials = updateData->NMaterials;

		// TODO become fast
		
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

#ifdef _WIN32
				if (server != nullptr)
				{
					if (registeredEffects.count(effect->GetName()) == 0)
					{
						registeredEffects[effect->GetName()] = effect;
						server->Register(effect->GetName(), effect);
					}
				}
#endif
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

			if (cmd.Type == EffekseerUpdateData_CommandType::SetR)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto rotation = updateData->Commands[i].Rotation;
					effekseerManager->SetRotation(
						eid, 
						rotation.Roll/ 180.0f * 3.1415f, 
						-rotation.Yaw / 180.0f * 3.1415f,
						rotation.Pitch / 180.0f * 3.1415f);
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::SetS)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto scale = updateData->Commands[i].Scale;
					effekseerManager->SetScale(eid, scale.X, scale.Z, scale.Y);
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::StopRoot)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->StopRoot(eid);
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::Stop)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->StopEffect(eid);
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::SetAllColor)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SetAllColor(eid, Effekseer::Color(cmd.AllColor.R, cmd.AllColor.G, cmd.AllColor.B, cmd.AllColor.A));
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::SetSpeed)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SetSpeed(eid, cmd.Speed);
				}
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::StartNetwork)
			{
#ifdef _WIN32
				if (server == nullptr)
				{
					server = Effekseer::Server::Create();
					if (server->Start(cmd.ID))
					{
					}
					else
					{
						ES_SAFE_DELETE(server);
					}
				}
#endif
			}

			if (cmd.Type == EffekseerUpdateData_CommandType::StopNetwork)
			{
#ifdef _WIN32
				if (server != nullptr)
				{
					server->Stop();
					ES_SAFE_DELETE(server);
					registeredEffects.clear();
				}
#endif
			}
		}

		// Update effects.
		Time += updateData->DeltaTime;

		auto frame = (int32_t)(Time * 60);
		Time -= frame * (1.0f / 60.0f);

		{
#ifdef _WIN32
			if (server != nullptr)
			{
				server->Update(&effekseerManager, 1, Effekseer::ReloadingThreadType::Render);
			}
#endif

			if (effekseerManager != nullptr)
			{
				effekseerManager->Update(frame);
			}
		}
		
		// Check existence.
		{
			criticalSection.lock();

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

			criticalSection.unlock();
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
		criticalSection.lock();
		auto ret = removedHandles;
		removedHandles.Reset();
		criticalSection.unlock();
		return ret;
	}
};

UEffekseerSystemComponent::UEffekseerSystemComponent()
{
	//bWantsBeginPlay = true;
	bTickInEditor = true;
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

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopAll;
	currentUpdateData->Commands.Add(cmd);

	if (DoStartNetworkAutomatically)
	{
		StartNetwork();
	}
}

void UEffekseerSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopAll;
	currentUpdateData->Commands.Add(cmd);

	StopNetwork();

	Super::EndPlay(EndPlayReason);
}

void UEffekseerSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	auto sp = (FEffekseerSystemSceneProxy*)sceneProxy;
	if (sp != nullptr)
	{
		sp->UpdateData(currentUpdateData);
		currentUpdateData = new EffekseerUpdateData();

		currentUpdateData->OpaqueDynamicMaterials = OpaqueDynamicMaterials;
		currentUpdateData->TranslucentDynamicMaterials = TranslucentDynamicMaterials;
		currentUpdateData->AdditiveDynamicMaterials = AdditiveDynamicMaterials;
		currentUpdateData->SubtractiveDynamicMaterials = SubtractiveDynamicMaterials;
		currentUpdateData->ModulateDynamicMaterials = ModulateDynamicMaterials;
		currentUpdateData->LightingDynamicMaterials = LightingDynamicMaterials;

		currentUpdateData->Materials = Materials;
		currentUpdateData->NMaterials = NMaterials;

		currentUpdateData->DeltaTime = DeltaTime;

		auto removedHandles = sp->PopRemovedHandles();
		for (auto& h : removedHandles)
		{
			internalHandle2EfkHandle.Remove(h);
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UEffekseerSystemComponent::CreateSceneProxy()
{
	auto sp = new FEffekseerSystemSceneProxy(this, MaxSprite);
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
	if (ElementIndex == 5) 	return LightingMaterial;

	if (ElementIndex == 6) 	return DistortionTranslucentMaterial;
	if (ElementIndex == 7) 	return DistortionAdditiveMaterial;

	int32_t offset = 8;

	if (ElementIndex - offset < NMaterials.size())
	{
		for (auto& m : NMaterials)
		{
			if (ElementIndex == offset) return m.second;
			offset++;
		}
	}
	else
	{
		offset += NMaterials.size();
	}

	return nullptr;
}

#if ENGINE_MINOR_VERSION == 14
void UEffekseerSystemComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials) const
#else
void UEffekseerSystemComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
#endif
{
	if (OpaqueMaterial) OutMaterials.Add(OpaqueMaterial);
	if (TranslucentMaterial) OutMaterials.Add(TranslucentMaterial);
	if (AdditiveMaterial) OutMaterials.Add(AdditiveMaterial);
	if (SubtractiveMaterial) OutMaterials.Add(SubtractiveMaterial);
	if (ModulateMaterial) OutMaterials.Add(ModulateMaterial);
	if (LightingMaterial) OutMaterials.Add(LightingMaterial);
	if (DistortionTranslucentMaterial) OutMaterials.Add(DistortionTranslucentMaterial);
	if (DistortionAdditiveMaterial) OutMaterials.Add(DistortionAdditiveMaterial);

	for (auto& m : NMaterials)
	{
		OutMaterials.Add(m.second);
	}
}

int32 UEffekseerSystemComponent::GetNumMaterials()const
{
	auto ret = 0;
	if (OpaqueMaterial) ret++;
	if (TranslucentMaterial) ret++;
	if (AdditiveMaterial) ret++;
	if (SubtractiveMaterial) ret++;
	if (ModulateMaterial) ret++;
	if (LightingMaterial) ret++;
	if (DistortionTranslucentMaterial) ret++;
	if (DistortionAdditiveMaterial) ret++;

	ret += NMaterials.size();
	return ret;
}


FBoxSphereBounds UEffekseerSystemComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// TODO Optimize a size of bounding box.
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
	UMaterialInstanceConstant* _mats[16];
	TMap<UTexture2D*, UMaterialInstanceDynamic*>* _matss[8];

	_mats[0] = OpaqueMaterial;
	_mats[1] = TranslucentMaterial;
	_mats[2] = AdditiveMaterial;
	_mats[3] = SubtractiveMaterial;
	_mats[4] = ModulateMaterial;
	_mats[5] = LightingMaterial;
	_mats[6] = DistortionTranslucentMaterial;
	_mats[7] = DistortionAdditiveMaterial;

	_mats[0 + 8] = Opaque_DD_Material;
	_mats[1 + 8] = Translucent_DD_Material;
	_mats[2 + 8] = Additive_DD_Material;
	_mats[3 + 8] = Subtractive_DD_Material;
	_mats[4 + 8] = Modulate_DD_Material;
	_mats[5 + 8] = LightingMaterial;
	_mats[6 + 8] = DistortionTranslucent_DD_Material;
	_mats[7 + 8] = DistortionAdditive_DD_Material;

	_matss[0] = &OpaqueDynamicMaterials;
	_matss[1] = &TranslucentDynamicMaterials;
	_matss[2] = &AdditiveDynamicMaterials;
	_matss[3] = &SubtractiveDynamicMaterials;
	_matss[4] = &ModulateDynamicMaterials;
	_matss[5] = &LightingDynamicMaterials;
	_matss[6] = &DistortionTranslucentDynamicMaterials;
	_matss[7] = &DistortionAdditiveDynamicMaterials;

	for(int i = 0; i < 8; i++)
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

	for (auto m : effect->Materials)
	{
		if (Materials.Contains(m)) continue;

		auto blendInd = (int32_t)m->AlphaBlend;
		if (m->IsLighting) blendInd = 5;
		if (blendInd == 1 && m->IsDistorted) blendInd = 6;
		if (blendInd == 2 && m->IsDistorted) blendInd = 7;

		if (m->IsDepthTestDisabled) blendInd += 8;
		auto mat = _mats[blendInd];

		if (mat != nullptr)
		{
			auto dynamicMaterial = UMaterialInstanceDynamic::Create(mat, this);
			dynamicMaterial->SetTextureParameterValue(TEXT("ColorTexture"), m->Texture);
			Materials.Add(m, dynamicMaterial);

			EffekseerMaterial mkey;
			mkey.Texture = m->Texture;
			mkey.AlphaBlend = m->AlphaBlend;
			mkey.IsDepthTestDisabled = m->IsDepthTestDisabled;
			mkey.IsLighting = m->IsLighting;
			mkey.IsDistorted = m->IsDistorted;
			NMaterials[mkey] = dynamicMaterial;
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

void UEffekseerSystemComponent::SetEffectRotation(FEffekseerHandle handle, FRotator rotation)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetR;
	cmd.ID = handle.ID;
	cmd.Rotation = rotation;

	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectScaling(FEffekseerHandle handle, FVector scaling)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetS;
	cmd.ID = handle.ID;
	cmd.Scale = scaling;

	currentUpdateData->Commands.Add(cmd);
}

bool UEffekseerSystemComponent::Exists(FEffekseerHandle handle) const
{
	if (handle.Effect == nullptr) return false;
	return internalHandle2EfkHandle.Contains(handle.ID);
}

void UEffekseerSystemComponent::Stop(FEffekseerHandle handle)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::Stop;
	cmd.ID = handle.ID;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::StopRoot(FEffekseerHandle handle)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopRoot;
	cmd.ID = handle.ID;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectSpeed(FEffekseerHandle handle, float speed)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetSpeed;
	cmd.ID = handle.ID;
	cmd.Speed = speed;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectAllColor(FEffekseerHandle handle, FColor color)
{
	if (handle.Effect == nullptr) return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetAllColor;
	cmd.ID = handle.ID;
	cmd.AllColor = color;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::StartNetwork()
{
	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StartNetwork;
	cmd.ID = NetworkPort;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::StopNetwork()
{
	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopNetwork;
	currentUpdateData->Commands.Add(cmd);
}