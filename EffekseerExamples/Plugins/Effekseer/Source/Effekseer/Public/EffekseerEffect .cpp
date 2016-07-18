#include "../Private/EffekseerPrivatePCH.h"
#include "EffekseerEffect.h"

#include "../Private/EffekseerNative.h"

static ::Effekseer::Setting* CreateSetting()
{
	auto setting = ::Effekseer::Setting::Create();
	return setting;
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

	::Effekseer::Setting* setting = CreateSetting();
	const EFK_CHAR* path = nullptr;
	auto effect = ::Effekseer::Effect::Create(setting, (void*)data, size, 1.0f, path);
	setting->Release();

	effectPtr = effect;

	// î•ñŽæ“¾
	if (effect != nullptr)
	{
		Version = effect->GetVersion();
	}
}

void UEffekseerEffect::BeginDestroy()
{
	ReleaseEffect();
}
