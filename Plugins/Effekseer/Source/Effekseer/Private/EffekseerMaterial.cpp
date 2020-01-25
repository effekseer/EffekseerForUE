#include "EffekseerMaterial.h"
#include "EffekseerNative.h"

#include "EffekseerCustomVersion.h"

void UEffekseerMaterial::LoadMaterial(const uint8_t* data, int32_t size, const TCHAR* path)
{
	internal_ = new Effekseer::Material();
	internal_->Load(data, size);
}

void UEffekseerMaterial::ReleaseMaterial()
{
	ES_SAFE_DELETE(internal_);
	internal_ = nullptr;
}

const TArray<uint8>& UEffekseerMaterial::GetData() const
{
	return buffer_;
}

void UEffekseerMaterial::StoreData(const uint8_t* data, uint32_t size)
{
	buffer_.Reset(0);
	buffer_.Append(data, size);
}

UMaterialInterface* UEffekseerMaterial::FindMatrial(EEffekseerAlphaBlendType alphaBlend) const
{
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