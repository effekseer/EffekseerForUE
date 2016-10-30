#include "EffekseerPrivatePCH.h"
#include "EffekseerModel.h"
#include "EffekseerNative.h"

void UEffekseerModel::LoadModel(const uint8_t* data, int32_t size, const TCHAR* path)
{

}

void UEffekseerModel::ReleaseModel()
{
	auto p = (::Effekseer::Model*)modelPtr;
	ES_SAFE_DELETE(p);
	modelPtr = nullptr;
}