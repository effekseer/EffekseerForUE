#include "ModelLoader.h"

#include <limits>

namespace EffekseerRenderer
{

ModelLoader::ModelLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						 ::Effekseer::FileInterfaceRef fileInterface)
	: graphicsDevice_(graphicsDevice)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

ModelLoader::~ModelLoader()
{
}

::Effekseer::ModelRef ModelLoader::Load(const char16_t* path)
{
	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
	{
		return nullptr;
	}

	size_t size = reader->GetLength();
	if (size == 0 || size > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
	{
		return nullptr;
	}

	Effekseer::CustomAlignedVector<uint8_t> data;
	data.resize(size);

	if (reader->Read(data.data(), size) != size)
	{
		return nullptr;
	}

	auto model = Load(data.data(), (int32_t)size);

	return model;
}

::Effekseer::ModelRef ModelLoader::Load(const void* data, int32_t size)
{
	auto model = ::Effekseer::MakeRefPtr<::Effekseer::Model>((const uint8_t*)data, size);
	if (!model->GetIsValid())
	{
		return nullptr;
	}

	return model;
}

void ModelLoader::Unload(::Effekseer::ModelRef data)
{
}

} // namespace EffekseerRenderer
