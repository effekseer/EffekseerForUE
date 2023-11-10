#include "EffekseerSystemComponent.h"

#include "DynamicMeshBuilder.h"
#include "EffekseerRendererImplemented.h"
#include <Effekseer.h>
#include <mutex>

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

EffekseerUpdateData::EffekseerUpdateData()
{
}

EffekseerUpdateData::~EffekseerUpdateData()
{
	for (auto c : Commands)
	{
		if (c.Effect == nullptr)
			continue;
		::Effekseer::RefPtr<::Effekseer::Effect>::Unpin(c.Effect);
	}
}

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

	TMap<int, int> internalHandle2EfkHandle;

	// is it safe?
	std::mutex criticalSection;
	TArray<int32_t> removedHandles;

	void CalculateCameraDirectionAndPosition(const Effekseer::Matrix44& matrix, Effekseer::Vector3D& direction, Effekseer::Vector3D& position) const
	{
		const auto& mat = matrix;

		direction = -::Effekseer::Vector3D(matrix.Values[0][2], matrix.Values[1][2], matrix.Values[2][2]);

		{
			auto localPos = ::Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
			auto f = ::Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);
			auto r = ::Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
			auto u = ::Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

			position = r * localPos.X + u * localPos.Y + f * localPos.Z;
		}
	}

public:
	FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent, int32_t maxSprite, int32_t threadCount, EEffekseerColorSpaceType colorSpace)
		: FPrimitiveSceneProxy(InComponent)
		, maxSprite_(maxSprite)
	{
		effekseerManager = ::Effekseer::Manager::Create(maxSprite_);
#ifndef __EMSCRIPTEN__
		if (threadCount >= 2)
		{
			effekseerManager->LaunchWorkerThreads(threadCount);
		}
#endif
		effekseerRenderer = ::EffekseerRendererUE::RendererImplemented::Create();
		effekseerRenderer->Initialize(maxSprite_, colorSpace);

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
		if (server_ != nullptr)
		{
			server_.Reset();
		}

		effekseerManager.Reset();
		effekseerRenderer.Reset();
	}

	virtual SIZE_T GetTypeHash() const
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
#if WITH_EDITOR
		if (GShaderCompilingManager->IsCompiling())
		{
			return;
		}
#endif

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (!(VisibilityMap & (1 << ViewIndex)))
				continue;

			const auto& view = Views[ViewIndex];

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
				const auto vmat = view->ViewMatrices.GetViewMatrix();
				::Effekseer::Matrix44 evmat;

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						evmat.Values[i][j] = static_cast<float>(vmat.M[i][j]);
					}
				}

				std::swap(evmat.Values[1][0], evmat.Values[2][0]);
				std::swap(evmat.Values[1][1], evmat.Values[2][1]);
				std::swap(evmat.Values[1][2], evmat.Values[2][2]);

				evmat.Values[0][2] = -evmat.Values[0][2];
				evmat.Values[1][2] = -evmat.Values[1][2];
				evmat.Values[2][2] = -evmat.Values[2][2];
				evmat.Values[3][2] = -evmat.Values[3][2];

				effekseerRenderer->SetCameraMatrix(evmat);
				effekseerRenderer->BeginRendering();

				Effekseer::Vector3D direction;
				Effekseer::Vector3D position;
				CalculateCameraDirectionAndPosition(evmat, direction, position);

				Effekseer::Manager::DrawParameter drawParam;
				drawParam.CameraFrontDirection = direction;
				drawParam.CameraPosition = position;
				drawParam.IsSortingEffectsEnabled = true;

				effekseerManager->Draw(drawParam);
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
		Result.bOpaque = false;
		Result.bMasked = false;
		Result.bSeparateTranslucency = true;
		Result.bNormalTranslucency = true;
		Result.bDistortion = true;

		return Result;
	}
	virtual uint32 GetMemoryFootprint() const
	{
		return sizeof(*this) + GetAllocatedSize();
	}
	uint32 GetAllocatedSize() const
	{
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}

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
				auto effect = ::Effekseer::RefPtr<::Effekseer::Effect>::FromPinned(updateData->Commands[i].Effect);
				auto position = updateData->Commands[i].Position;

				auto eid = effekseerManager->Play(effect, position.X, position.Z, position.Y);
				internalHandle2EfkHandle.Add(cmd.ID, eid);

				if (server_ != nullptr)
				{
					if (registeredEffects.count(effect->GetName()) == 0)
					{
						registeredEffects[effect->GetName()] = effect;
						server_->Register(effect->GetName(), effect);
					}
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetP)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto position = updateData->Commands[i].Position;
					effekseerManager->SetLocation(eid, position.X, position.Z, position.Y);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetR)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto rotation = updateData->Commands[i].Rotation;
					rotation = FQuat(rotation.X, rotation.Z, rotation.Y, -rotation.W);
					FVector axis = rotation.GetRotationAxis();
					effekseerManager->SetRotation(eid, Effekseer::Vector3D(axis.X, axis.Y, axis.Z), rotation.GetAngle());
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetS)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					auto scale = updateData->Commands[i].Scale;
					effekseerManager->SetScale(eid, scale.X, scale.Z, scale.Y);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StopRoot)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->StopRoot(eid);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::Stop)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->StopEffect(eid);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetAllColor)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SetAllColor(eid, Effekseer::Color(cmd.AllColor.R, cmd.AllColor.G, cmd.AllColor.B, cmd.AllColor.A));
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetDynamicInput)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SetDynamicInput(eid, cmd.DynamicInput.Index, cmd.DynamicInput.Value);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetSpeed)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SetSpeed(eid, cmd.Speed);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StartNetwork)
			{
				if (server_ == nullptr)
				{
					server_ = Effekseer::Server::Create();
					if (server_ == nullptr)
					{
						if (server_->Start(cmd.ID))
						{
						}
						else
						{
							server_.Reset();
						}
					}
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StopNetwork)
			{
				if (server_ != nullptr)
				{
					server_->Stop();
					server_.Reset();
					registeredEffects.clear();
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SendTrigger)
			{
				if (internalHandle2EfkHandle.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle[cmd.ID];
					effekseerManager->SendTrigger(eid, cmd.TriggerIndex);
				}
			}
		}

		// Update effects.
		Time += updateData->DeltaTime;

		auto frame = (int32_t)(Time * 60);
		Time -= frame * (1.0f / 60.0f);

		{
			if (server_ != nullptr)
			{
				server_->Update(&effekseerManager, 1, Effekseer::ReloadingThreadType::Render);
			}

			if (effekseerManager != nullptr)
			{
				const auto evmat = effekseerRenderer->GetCameraMatrix();

				Effekseer::Vector3D direction;
				Effekseer::Vector3D position;
				CalculateCameraDirectionAndPosition(evmat, direction, position);

				Effekseer::Manager::LayerParameter layerParams;
				layerParams.ViewerPosition = position;
				effekseerManager->SetLayerParameter(0, layerParams);

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
		ENQUEUE_RENDER_COMMAND(
			EffekseerUpdateDataCommand)
		([this, updateData](FRHICommandListImmediate& RHICmdList)
		 { this->UpdateData_RenderThread(updateData); });
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
	// bWantsBeginPlay = true;
	bTickInEditor = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostUpdateWork;
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
	auto sp = new FEffekseerSystemSceneProxy(this, MaxSprite, ThreadCount, ColorSpace);
	sceneProxy = sp;
	return sp;
}

UMaterialInterface* UEffekseerSystemComponent::GetMaterial(int32 ElementIndex) const
{
	if (ElementIndex == 0)
		return OpaqueMaterial;
	if (ElementIndex == 1)
		return TranslucentMaterial;
	if (ElementIndex == 2)
		return AdditiveMaterial;
	if (ElementIndex == 3)
		return SubtractiveMaterial;
	if (ElementIndex == 4)
		return ModulateMaterial;
	if (ElementIndex == 5)
		return LightingMaterial;

	if (ElementIndex == 6)
		return DistortionTranslucentMaterial;
	if (ElementIndex == 7)
		return DistortionAdditiveMaterial;

	int32_t offset = 8;

	if (ElementIndex - offset < NMaterials.size())
	{
		for (auto& m : NMaterials)
		{
			if (ElementIndex == offset)
				return m.second;
			offset++;
		}
	}
	else
	{
		offset += NMaterials.size();
	}

	return nullptr;
}

void UEffekseerSystemComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	if (OpaqueMaterial)
		OutMaterials.Add(OpaqueMaterial);
	if (TranslucentMaterial)
		OutMaterials.Add(TranslucentMaterial);
	if (AdditiveMaterial)
		OutMaterials.Add(AdditiveMaterial);
	if (SubtractiveMaterial)
		OutMaterials.Add(SubtractiveMaterial);
	if (ModulateMaterial)
		OutMaterials.Add(ModulateMaterial);
	if (LightingMaterial)
		OutMaterials.Add(LightingMaterial);
	if (DistortionTranslucentMaterial)
		OutMaterials.Add(DistortionTranslucentMaterial);
	if (DistortionAdditiveMaterial)
		OutMaterials.Add(DistortionAdditiveMaterial);

	for (auto& m : NMaterials)
	{
		OutMaterials.Add(m.second);
	}
}

int32 UEffekseerSystemComponent::GetNumMaterials() const
{
	auto ret = 0;
	if (OpaqueMaterial)
		ret++;
	if (TranslucentMaterial)
		ret++;
	if (AdditiveMaterial)
		ret++;
	if (SubtractiveMaterial)
		ret++;
	if (ModulateMaterial)
		ret++;
	if (LightingMaterial)
		ret++;
	if (DistortionTranslucentMaterial)
		ret++;
	if (DistortionAdditiveMaterial)
		ret++;

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
	if (effect == nullptr)
		return FEffekseerHandle();
	if (effect->GetNativePtr() == nullptr)
		return FEffekseerHandle();

	if (isNetworkRunning_)
	{
		effect->GenerateRenderingDataIfRequired();
	}

	// Convert to a position relative from the system.
	position -= this->GetRelativeLocation();

	// it generates a material dynamically.
	std::array<UMaterialInstanceConstant*, 16> baseMats;

	baseMats[0] = OpaqueMaterial;
	baseMats[1] = TranslucentMaterial;
	baseMats[2] = AdditiveMaterial;
	baseMats[3] = SubtractiveMaterial;
	baseMats[4] = ModulateMaterial;
	baseMats[5] = LightingMaterial;
	baseMats[6] = DistortionTranslucentMaterial;
	baseMats[7] = DistortionAdditiveMaterial;

	baseMats[0 + 8] = Opaque_DD_Material;
	baseMats[1 + 8] = Translucent_DD_Material;
	baseMats[2 + 8] = Additive_DD_Material;
	baseMats[3 + 8] = Subtractive_DD_Material;
	baseMats[4 + 8] = Modulate_DD_Material;
	baseMats[5 + 8] = LightingMaterial;
	baseMats[6 + 8] = DistortionTranslucent_DD_Material;
	baseMats[7 + 8] = DistortionAdditive_DD_Material;

	for (auto paramHolder : effect->EffekseerMaterials)
	{
		if (Materials.Contains(paramHolder))
			continue;

		if (paramHolder->Material != nullptr)
		{
			paramHolder->Material->GenerateColorSpaceMaterial(paramHolder->AlphaBlend, ColorSpace);
		}
		else
		{
			auto blendInd = static_cast<int32_t>(paramHolder->AlphaBlend);
			if (paramHolder->IsLighting)
				blendInd = 5;
			if (blendInd == 1 && paramHolder->IsDistorted)
				blendInd = 6;
			if (blendInd == 2 && paramHolder->IsDistorted)
				blendInd = 7;

			if (paramHolder->IsDepthTestDisabled)
				blendInd += 8;
			auto baseMat = baseMats[blendInd];

			if (baseMat != nullptr)
			{
				UMaterialInstanceDynamic* created = nullptr;

				auto found = NMaterials.find(paramHolder->Key);

				if (found != NMaterials.end())
				{
					created = found->second;
				}
				else
				{
					auto createdMaterial = UMaterialInstanceDynamic::Create(baseMat, this);
					createdMaterial->SetTextureParameterValue(TEXT("ColorTexture"), paramHolder->Texture);

					createdMaterial->SetTextureParameterValue(TEXT("AlphaTexture"), paramHolder->AlphaTexture);
					createdMaterial->SetTextureParameterValue(TEXT("UVDistortionTexture"), paramHolder->UVDistortionTexture);
					createdMaterial->SetTextureParameterValue(TEXT("BlendTexture"), paramHolder->BlendTexture);
					createdMaterial->SetTextureParameterValue(TEXT("BlendAlphaTexture"), paramHolder->BlendAlphaTexture);
					createdMaterial->SetTextureParameterValue(TEXT("BlendUVDistortionTexture"), paramHolder->BlendUVDistortionTexture);

					createdMaterial->SetScalarParameterValue(TEXT("TextureTilingType"), paramHolder->TextureAddressType);
					createdMaterial->SetScalarParameterValue(TEXT("AlphaTextureTilingType"), paramHolder->AlphaTextureAddressType);
					createdMaterial->SetScalarParameterValue(TEXT("UVDistortionTextureTilingType"), paramHolder->UVDistortionTextureAddressType);
					createdMaterial->SetScalarParameterValue(TEXT("BlendTextureTilingType"), paramHolder->BlendTextureAddress);
					createdMaterial->SetScalarParameterValue(TEXT("BlendAlphaTextureTilingType"), paramHolder->BlendAlphaTextureAddress);
					createdMaterial->SetScalarParameterValue(TEXT("BlendUVDistortionTextureTilingType"), paramHolder->BlendUVDistortionTextureAddress);

					createdMaterial->SetVectorParameterValue(TEXT("FlipbookParameters1"), FLinearColor(paramHolder->FlipbookParams.Enable ? 1.0f : 0.0f, paramHolder->FlipbookParams.LoopType, paramHolder->FlipbookParams.DivideX, paramHolder->FlipbookParams.DivideY));
					createdMaterial->SetVectorParameterValue(TEXT("FlipbookParameters2"), FLinearColor(paramHolder->FlipbookParams.OneSizeX, paramHolder->FlipbookParams.OneSizeY, paramHolder->FlipbookParams.OffsetX, paramHolder->FlipbookParams.OffsetY));

					createdMaterial->SetScalarParameterValue(TEXT("UVDistortionIntensity"), paramHolder->UVDistortionIntensity);

					createdMaterial->SetScalarParameterValue(TEXT("TextureBlendType"), paramHolder->TextureBlendType);

					createdMaterial->SetScalarParameterValue(TEXT("BlendUVDistortionIntensity"), paramHolder->BlendUVDistortionIntensity);

					createdMaterial->SetScalarParameterValue(TEXT("EnableFalloff"), static_cast<float>(paramHolder->EnableFalloff));
					createdMaterial->SetScalarParameterValue(TEXT("FalloffBlendType"), static_cast<float>(paramHolder->FalloffParam.ColorBlendType));
					createdMaterial->SetVectorParameterValue(TEXT("BeginColor"), paramHolder->FalloffParam.BeginColor);
					createdMaterial->SetVectorParameterValue(TEXT("EndColor"), paramHolder->FalloffParam.EndColor);
					createdMaterial->SetScalarParameterValue(TEXT("FalloffPow"), static_cast<float>(paramHolder->FalloffParam.Pow));

					createdMaterial->SetScalarParameterValue(TEXT("EmissiveScaling"), paramHolder->EmissiveScaling);

					createdMaterial->SetVectorParameterValue(TEXT("EdgeColor"), paramHolder->EdgeParams.Color);
					createdMaterial->SetScalarParameterValue(TEXT("EdgeThreshold"), paramHolder->EdgeParams.Threshold);
					createdMaterial->SetScalarParameterValue(TEXT("EdgeColorScaling"), static_cast<float>(paramHolder->EdgeParams.ColorScaling));

					createdMaterial->SetVectorParameterValue(TEXT("SoftParticleParams"), FLinearColor(float(paramHolder->SoftParticleParam.DepthFadeFar), paramHolder->SoftParticleParam.DepthFadeNear, paramHolder->SoftParticleParam.DepthFadeNearOffset, 0.0f));

					if (ColorSpace == EEffekseerColorSpaceType::Gamma)
					{
						createdMaterial->SetScalarParameterValue(TEXT("GammaScale"), 2.2f);
						createdMaterial->SetScalarParameterValue(TEXT("InvGammaScale"), 1.0f / 2.2f);
						createdMaterial->SetScalarParameterValue(TEXT("GammaScaleEnabled"), 1.0f);
					}
					else
					{
						createdMaterial->SetScalarParameterValue(TEXT("GammaScale"), 1.0f);
						createdMaterial->SetScalarParameterValue(TEXT("InvGammaScale"), 1.0f);
						createdMaterial->SetScalarParameterValue(TEXT("GammaScaleEnabled"), 0.0f);
					}

					NMaterials[paramHolder->Key] = createdMaterial;
					created = createdMaterial;
				}

				Materials.Add(paramHolder, created);
			}
		}
	}

	effect->ReloadIfRequired();

	auto efk = ::Effekseer::RefPtr<::Effekseer::Effect>::FromPinned(effect->GetNativePtr());
	auto p = efk.Pin();

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
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetP;
	cmd.ID = handle.ID;
	cmd.Position = position;

	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectRotation(FEffekseerHandle handle, FQuat rotation)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetR;
	cmd.ID = handle.ID;
	cmd.Rotation = rotation;

	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectScaling(FEffekseerHandle handle, FVector scaling)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetS;
	cmd.ID = handle.ID;
	cmd.Scale = scaling;

	currentUpdateData->Commands.Add(cmd);
}

bool UEffekseerSystemComponent::Exists(FEffekseerHandle handle) const
{
	if (handle.Effect == nullptr)
		return false;
	return internalHandle2EfkHandle.Contains(handle.ID);
}

void UEffekseerSystemComponent::SendTrigger(FEffekseerHandle handle, int index)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SendTrigger;
	cmd.ID = handle.ID;
	cmd.TriggerIndex = index;

	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::Stop(FEffekseerHandle handle)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::Stop;
	cmd.ID = handle.ID;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::StopRoot(FEffekseerHandle handle)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopRoot;
	cmd.ID = handle.ID;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectSpeed(FEffekseerHandle handle, float speed)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetSpeed;
	cmd.ID = handle.ID;
	cmd.Speed = speed;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectAllColor(FEffekseerHandle handle, FColor color)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetAllColor;
	cmd.ID = handle.ID;
	cmd.AllColor = color;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::SetEffectDynamicInput(FEffekseerHandle handle, int index, float value)
{
	if (handle.Effect == nullptr)
		return;

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::SetDynamicInput;
	cmd.ID = handle.ID;
	cmd.DynamicInput.Value = value;
	cmd.DynamicInput.Index = index;
	currentUpdateData->Commands.Add(cmd);
}

void UEffekseerSystemComponent::StartNetwork()
{
	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StartNetwork;
	cmd.ID = NetworkPort;
	currentUpdateData->Commands.Add(cmd);
	isNetworkRunning_ = true;
}

void UEffekseerSystemComponent::StopNetwork()
{
	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::StopNetwork;
	currentUpdateData->Commands.Add(cmd);
	isNetworkRunning_ = false;
}