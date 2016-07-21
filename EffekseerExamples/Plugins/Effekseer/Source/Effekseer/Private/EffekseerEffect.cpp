#include "EffekseerPrivatePCH.h"
#include "EffekseerEffect.h"
#include "EffekseerNative.h"

static ::Effekseer::Setting* CreateSetting()
{
	auto setting = ::Effekseer::Setting::Create();
	return setting;
}

void UEffekseerEffect::LoadEffect(const uint8_t* data, int32_t size)
{

	::Effekseer::Setting* setting = CreateSetting();
	const EFK_CHAR* path = nullptr;
	auto effect = ::Effekseer::Effect::Create(setting, (void*)data, size, Scale, path);
	setting->Release();

	effectPtr = effect;

	// î•ñŽæ“¾
	if (effect != nullptr)
	{
		Version = effect->GetVersion();
	}
}

void UEffekseerEffect::ReleaseEffect()
{
	auto p = (::Effekseer::Effect*)effectPtr;
	ES_SAFE_RELEASE(p);
	effectPtr = nullptr;

}

void UEffekseerEffect::Load(const uint8_t* data, int32_t size)
{
	ReleaseEffect();
	buffer.Reset(0);
	buffer.Append(data, size);
	LoadEffect(data, size);
}

void UEffekseerEffect::BeginDestroy()
{
	ReleaseEffect();
	Super::BeginDestroy();
}

void UEffekseerEffect::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	const int32 myVersion = 1;
	const FGuid myGUID(123, 444, 563, 464);
	FCustomVersionRegistration registerMyAssetCustomVersion(myGUID, myVersion, TEXT("AssetVersion"));

	Ar.UsingCustomVersion(myGUID);

	if (Ar.IsSaving() || Ar.IsLoading())
	{
		Ar << buffer;
	}

	if (Ar.IsLoading())
	{
		ReleaseEffect();
		LoadEffect(buffer.GetData(), buffer.Num());
	}
}