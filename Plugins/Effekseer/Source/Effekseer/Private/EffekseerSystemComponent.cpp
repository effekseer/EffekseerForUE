#include "EffekseerSystemComponent.h"

#include "DynamicMeshBuilder.h"
#include "EffekseerRendererImplemented.h"
#include "EffekseerSystem.h"
#include <Effekseer.h>
#include <mutex>

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

#include "EffekseerSystemSceneProxy.h"

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
	auto sp = static_cast<FEffekseerSystemSceneProxy*>(sceneProxy_);
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

		currentUpdateData->Materials = GeneratedFixedMaterials;
		currentUpdateData->NMaterials = NMaterials;

		currentUpdateData->DeltaTime = DeltaTime;

		auto removedHandles = sp->PopRemovedHandles();
		for (auto& h : removedHandles)
		{
			internalHandle2EfkHandle_.Remove(h);
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UEffekseerSystemComponent::CreateSceneProxy()
{
	auto sp = new FEffekseerSystemSceneProxy(this, MaxSprite, ThreadCount, ColorSpace);
	sceneProxy_ = sp;
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

void UEffekseerSystemComponent::AssignMaterials(UEffekseerEffect* effect, TArray<UMaterialInterface*>* currentMaterials)
{
	if (effect == nullptr || effect->GetNativePtr() == nullptr)
	{
		return;
	}

	if (isNetworkRunning_)
	{
		effect->GenerateRenderingDataIfRequired();
	}

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
		if (paramHolder->Material != nullptr)
		{
			paramHolder->Material->GenerateColorSpaceMaterial(paramHolder->AlphaBlend, ColorSpace);
		}
		else
		{
			if (GeneratedFixedMaterials.Contains(paramHolder))
			{
				continue;
			}

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
					FName name = FName(baseMat->GetName() + TEXT("_Base_Dynamic_") + FString::FromInt(dynamicMaterialCount_));
					dynamicMaterialCount_++;

					auto createdMaterial = UMaterialInstanceDynamic::Create(baseMat, this, name);

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

				GeneratedFixedMaterials.Add(paramHolder, created);
			}
		}
	}

	if (currentMaterials != nullptr)
	{
		currentMaterials->Empty();
		for (auto paramHolder : effect->EffekseerMaterials)
		{
			UMaterialInterface* mat = nullptr;

			if (paramHolder->Material != nullptr)
			{
				mat = paramHolder->Material->FindMatrial(paramHolder->AlphaBlend);
			}
			else
			{
				mat = GeneratedFixedMaterials[paramHolder];
			}

			if (mat != nullptr)
			{
				currentMaterials->Add(mat);
			}
		}
	}

	effect->ReloadIfRequired();
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

	AssignMaterials(effect, nullptr);

	auto efk = ::Effekseer::RefPtr<::Effekseer::Effect>::FromPinned(effect->GetNativePtr());
	auto p = efk.Pin();

	auto handle = nextInternalHandle_;

	internalHandle2EfkHandle_.Add(handle, -1);

	EffekseerUpdateData_Command cmd;
	cmd.Type = EffekseerUpdateData_CommandType::Play;
	cmd.Effect = p;
	cmd.ID = handle;
	cmd.Position = position;

	currentUpdateData->Commands.Add(cmd);

	nextInternalHandle_++;

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
	return internalHandle2EfkHandle_.Contains(handle.ID);
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