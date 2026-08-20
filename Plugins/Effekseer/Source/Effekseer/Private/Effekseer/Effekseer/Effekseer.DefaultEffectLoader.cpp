
#include "Effekseer.DefaultEffectLoader.h"
#include "../Effekseer.h"
#include <limits>
#include <memory>

namespace Effekseer
{

DefaultEffectLoader::DefaultEffectLoader(FileInterfaceRef fileInterface)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == nullptr)
	{
		m_fileInterface = MakeRefPtr<DefaultFileInterface>();
	}
}

DefaultEffectLoader::~DefaultEffectLoader()
{
}

bool DefaultEffectLoader::Load(const char16_t* path, void*& data, int32_t& size)
{
	assert(path != nullptr);

	data = nullptr;
	size = 0;

	auto reader = m_fileInterface->OpenRead(path);
	if (reader == nullptr)
		return false;

	const auto length = reader->GetLength();
	if (length == 0 || length > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
		return false;
	size = static_cast<int32_t>(length);
	data = new uint8_t[size];
	if (reader->Read(data, static_cast<size_t>(size)) != static_cast<size_t>(size))
	{
		delete[] static_cast<uint8_t*>(data);
		data = nullptr;
		size = 0;
		return false;
	}

	return true;
}

void DefaultEffectLoader::Unload(void* data, int32_t size)
{
	uint8_t* data8 = (uint8_t*)data;
	ES_SAFE_DELETE_ARRAY(data8);
}

} // namespace Effekseer
