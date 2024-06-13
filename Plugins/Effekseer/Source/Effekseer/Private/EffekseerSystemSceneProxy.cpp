#include "EffekseerSystemSceneProxy.h"

#include "EffekseerSystemComponent.h"

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

EffekseerUpdateData::~EffekseerUpdateData()
{
	for (auto c : Commands)
	{
		if (c.Effect == nullptr)
			continue;
		::Effekseer::RefPtr<::Effekseer::Effect>::Unpin(c.Effect);
	}
}

void FEffekseerSystemSceneProxy::CalculateCameraDirectionAndPosition(const Effekseer::Matrix44& matrix, Effekseer::Vector3D& direction, Effekseer::Vector3D& position) const
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

FEffekseerSystemSceneProxy::FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent, int32_t maxSprite, int32_t threadCount, EEffekseerColorSpaceType colorSpace)
	: FPrimitiveSceneProxy(InComponent)
	, maxSprite_(maxSprite)
{
	system_ = std::make_shared<EffekseerRendererUE::EffekseerSystem>();
	system_->Init(maxSprite, threadCount, colorSpace);

	// To avoid verify error (I don't know why caused error)
	bVerifyUsedMaterials = false;
}

FEffekseerSystemSceneProxy::~FEffekseerSystemSceneProxy()
{
	system_.reset();
}

SIZE_T FEffekseerSystemSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

void FEffekseerSystemSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
#if WITH_EDITOR
	if (GShaderCompilingManager->IsCompiling())
	{
	//	return;
	}
#endif

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (!(VisibilityMap & (1 << ViewIndex)))
			continue;

		const auto& view = Views[ViewIndex];

		{
			std::lock_guard<std::mutex> lock(system_->Mutex);

			system_->Renderer->SetMaterials(&OpaqueDynamicMaterials, 0);
			system_->Renderer->SetMaterials(&TranslucentDynamicMaterials, 1);
			system_->Renderer->SetMaterials(&AdditiveDynamicMaterials, 2);
			system_->Renderer->SetMaterials(&SubtractiveDynamicMaterials, 3);
			system_->Renderer->SetMaterials(&ModulateDynamicMaterials, 4);
			system_->Renderer->SetMaterials(&LightingDynamicMaterials, 5);

			system_->Renderer->SetNMaterials(NMaterials);

			// system_->Render(GetLocalToWorld(), view->ViewMatrices.GetViewMatrix(), ViewIndex, Collector);
		}
	}
}

FPrimitiveViewRelevance FEffekseerSystemSceneProxy::GetViewRelevance(const FSceneView* View) const
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

void FEffekseerSystemSceneProxy::ConvertInternalHandleToEffekseerHandles(const std::vector<int32_t>& internalHandles, std::vector<int32_t>& efkHandles)
{
#if ENGINE_MAJOR_VERSION == 4
	check(IsInRenderingThread());
#endif

	std::lock_guard<std::mutex> lock(mutex_removedHandles_);

	efkHandles.clear();

	for (auto& h : internalHandles)
	{
		if (internalHandle2EfkHandle_.Contains(h))
		{
			efkHandles.push_back(internalHandle2EfkHandle_[h]);
		}
		else
		{
			efkHandles.push_back(-1);
		}
	}
}

void FEffekseerSystemSceneProxy::UpdateData_RenderThread(EffekseerUpdateData* updateData)
{
	check(IsInRenderingThread());

	{
		std::lock_guard<std::mutex> lock_renderThread(system_->Mutex);

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

				auto eid = system_->Manager->Play(effect, position.X, position.Z, position.Y);
				internalHandle2EfkHandle_.Add(cmd.ID, eid);

				if (system_->Server != nullptr)
				{
					if (registeredEffects.count(effect->GetName()) == 0)
					{
						registeredEffects[effect->GetName()] = effect;
						system_->Server->Register(effect->GetName(), effect);
					}
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetP)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					auto position = updateData->Commands[i].Position;
					system_->Manager->SetLocation(eid, position.X, position.Z, position.Y);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetR)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					auto rotation = updateData->Commands[i].Rotation;
					rotation = FQuat(rotation.X, rotation.Z, rotation.Y, -rotation.W);
					FVector axis = rotation.GetRotationAxis();
					system_->Manager->SetRotation(eid, Effekseer::Vector3D(axis.X, axis.Y, axis.Z), rotation.GetAngle());
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetS)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					auto scale = updateData->Commands[i].Scale;
					system_->Manager->SetScale(eid, scale.X, scale.Z, scale.Y);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StopRoot)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->StopRoot(eid);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::Stop)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->StopEffect(eid);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetAllColor)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->SetAllColor(eid, Effekseer::Color(cmd.AllColor.R, cmd.AllColor.G, cmd.AllColor.B, cmd.AllColor.A));
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetDynamicInput)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->SetDynamicInput(eid, cmd.DynamicInput.Index, cmd.DynamicInput.Value);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SetSpeed)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->SetSpeed(eid, cmd.Speed);
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StartNetwork)
			{
				if (system_->Server == nullptr)
				{
					system_->Server = Effekseer::Server::Create();
					if (system_->Server == nullptr)
					{
						if (system_->Server->Start(cmd.ID))
						{
						}
						else
						{
							system_->Server.Reset();
						}
					}
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::StopNetwork)
			{
				if (system_->Server != nullptr)
				{
					system_->Server->Stop();
					system_->Server.Reset();
					registeredEffects.clear();
				}
			}
			else if (cmd.Type == EffekseerUpdateData_CommandType::SendTrigger)
			{
				if (internalHandle2EfkHandle_.Contains(cmd.ID))
				{
					auto eid = internalHandle2EfkHandle_[cmd.ID];
					system_->Manager->SendTrigger(eid, cmd.TriggerIndex);
				}
			}
		}

		// Update effects.
		Time += updateData->DeltaTime;

		auto frame = (int32_t)(Time * 60);
		Time -= frame * (1.0f / 60.0f);

		{
			if (system_->Server != nullptr)
			{
				system_->Server->Update(&system_->Manager, 1, Effekseer::ReloadingThreadType::Render);
			}

			if (system_->Manager != nullptr)
			{
				const auto evmat = system_->Renderer->GetCameraMatrix();

				Effekseer::Vector3D direction;
				Effekseer::Vector3D position;
				CalculateCameraDirectionAndPosition(evmat, direction, position);

				Effekseer::Manager::LayerParameter layerParams;
				layerParams.ViewerPosition = position;
				system_->Manager->SetLayerParameter(0, layerParams);

				system_->Manager->Update(frame);
			}
		}

		// Check existence.
		{
			std::lock_guard<std::mutex> lock(mutex_removedHandles_);

			TArray<int32_t> removingHandle;
			for (auto& kv : internalHandle2EfkHandle_)
			{
				if (!system_->Manager->Exists(kv.Value))
				{
					removingHandle.Add(kv.Key);
					removedHandles_.Add(kv.Key);
				}
			}

			for (auto& k : removingHandle)
			{
				internalHandle2EfkHandle_.Remove(k);
			}
		}

		delete updateData;
	}
}

void FEffekseerSystemSceneProxy::UpdateData(EffekseerUpdateData* updateData)
{
	ENQUEUE_RENDER_COMMAND(
		EffekseerUpdateDataCommand)
	([this, updateData](FRHICommandListImmediate& RHICmdList)
	 { this->UpdateData_RenderThread(updateData); });
}

TArray<int32_t> FEffekseerSystemSceneProxy::PopRemovedHandles()
{
	std::lock_guard<std::mutex> lock(mutex_removedHandles_);
	auto ret = removedHandles_;
	removedHandles_.Reset();
	return ret;
}