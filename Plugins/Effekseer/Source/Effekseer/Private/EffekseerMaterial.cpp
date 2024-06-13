#include "EffekseerMaterial.h"

#include "EffekseerCustomVersion.h"
#include <Effekseer/Material/Effekseer.MaterialFile.h>

void UEffekseerMaterial::LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path)
{
	internal_ = new Effekseer::MaterialFile();
	internal_->Load(data, size);
}

void UEffekseerMaterial::ReleaseMaterial()
{
	ES_SAFE_DELETE(internal_);
	internal_ = nullptr;
}

void UEffekseerMaterial::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
#if WITH_EDITORONLY_DATA
	if (AssetImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}
#endif

	Super::GetAssetRegistryTags(OutTags);
}

#if WITH_EDITOR
void UEffekseerMaterial::ReupdateElements()
{
	FMaterialUpdateContext context;

	for (auto& element : MaterialElements)
	{
		if (element.Material == nullptr)
			continue;

		context.AddMaterialInterface(element.Material);
	}
}

void UEffekseerMaterial::OnMaterialCompilationFinished(UMaterialInterface* MaterialInterface)
{
	if (MaterialInterface == Material)
	{
		ReupdateElements();
	}
}

void UEffekseerMaterial::RegisterMaterialCompicationFinished()
{
	if (!isMaterialCompilationRegistered_)
	{
		UMaterial::OnMaterialCompilationFinished().AddUObject(this, &UEffekseerMaterial::OnMaterialCompilationFinished);
	}
	isMaterialCompilationRegistered_ = true;
}

#endif

const TArray<uint8>& UEffekseerMaterial::GetData() const
{
	return buffer_;
}

void UEffekseerMaterial::StoreData(const uint8_t* data, uint32_t size)
{
	buffer_.Reset(0);
	buffer_.Append(data, size);
}

UMaterialInterface* UEffekseerMaterial::FindMatrial(EEffekseerAlphaBlendType alphaBlend)
{
	auto material = ColorSpaceMaterials.FindOrAdd(alphaBlend);
	if (material != nullptr)
	{
		return material;
	}

	for (const auto& e : MaterialElements)
	{
		if (e.AlphaBlend == alphaBlend && e.Material != nullptr)
		{
			return e.Material;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Material %d is not found."), (int)alphaBlend);
	return Material;
}

bool UEffekseerMaterial::GenerateColorSpaceMaterial(EEffekseerAlphaBlendType alphaBlend, EEffekseerColorSpaceType colorSpaceType)
{
	if (materialPrevious_ != Material)
	{
		ColorSpaceMaterials.Reset();
	}
	materialPrevious_ = Material;

	auto material = ColorSpaceMaterials.Find(alphaBlend);
	if (material != nullptr)
	{
		return true;
	}

	for (const auto& e : MaterialElements)
	{
		if (e.AlphaBlend == alphaBlend && e.Material != nullptr)
		{
			FName name = FName(Material->GetName() + TEXT("_Dynamic_") + FString::FromInt(static_cast<int>(e.AlphaBlend)));
			auto dynamicMaterial = UMaterialInstanceDynamic::Create(e.Material, this, name);
			ColorSpaceMaterials.Add(alphaBlend, dynamicMaterial);

			if (colorSpaceType == EEffekseerColorSpaceType::Gamma)
			{
				dynamicMaterial->SetScalarParameterValue(TEXT("GammaScale"), 2.2f);
				dynamicMaterial->SetScalarParameterValue(TEXT("InvGammaScale"), 1.0f / 2.2f);
				dynamicMaterial->SetScalarParameterValue(TEXT("GammaScaleEnabled"), 1.0f);
			}
			else
			{
				dynamicMaterial->SetScalarParameterValue(TEXT("GammaScale"), 1.0f);
				dynamicMaterial->SetScalarParameterValue(TEXT("InvGammaScale"), 1.0f);
				dynamicMaterial->SetScalarParameterValue(TEXT("GammaScaleEnabled"), 0.0f);
			}

			return true;
		}
	}

	return false;
}

void UEffekseerMaterial::ReassignSearchingMaps()
{
	UniformNameToIndex.Reset();

	for (int32_t i = 0; i < Uniforms.Num(); i++)
	{
		UniformNameToIndex.Add(Uniforms[i].Name, i);
	}

	TextureNameToIndex.Reset();

	for (int32_t i = 0; i < Textures.Num(); i++)
	{
		TextureNameToIndex.Add(Textures[i].Name, i);
	}

	UniformHashedNameToIndex.Reset();

	for (int32_t i = 0; i < Uniforms.Num(); i++)
	{
		UniformHashedNameToIndex.Add(Uniforms[i].Name, i);
	}

	for (int32_t i = 0; i < Gradients.Num(); i++)
	{
		for (int32_t j = 0; j < 13; j++)
		{
			UniformHashedNameToIndex.Add(Gradients[i].Name + "_" + std::to_string(j).c_str(), Uniforms.Num() + i * 13 + j);
		}
	}

	TextureHashedNameToIndex.Reset();

	for (int32_t i = 0; i < Textures.Num(); i++)
	{
		TextureHashedNameToIndex.Add(Textures[i].Name, i);
	}
}

void UEffekseerMaterial::BeginDestroy()
{
#if WITH_EDITOR
	if (isMaterialCompilationRegistered_)
	{
		UMaterial::OnMaterialCompilationFinished().RemoveAll(this);
		isMaterialCompilationRegistered_ = false;
	}
#endif
	ReleaseMaterial();
	Super::BeginDestroy();
}

void UEffekseerMaterial::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEffekseerCustomVersion::GUID);

	if (Ar.IsSaving() || Ar.IsLoading())
	{
		Ar << buffer_;
	}

	if (Ar.IsLoading())
	{
		ReleaseMaterial();

		auto path = GetPathName();
		LoadMaterial(buffer_.GetData(), buffer_.Num(), *path);

		ReassignSearchingMaps();
	}
}