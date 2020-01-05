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

void UEffekseerMaterial::StoreData(const uint8_t* data, uint32_t size)
{
	buffer_.Reset(0);
	buffer_.Append(data, size);
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
	}
}