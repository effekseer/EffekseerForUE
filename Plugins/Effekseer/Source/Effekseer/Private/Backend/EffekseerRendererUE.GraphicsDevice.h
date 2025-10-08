#pragma once

#include "CoreMinimal.h"
#include <Effekseer/Backend/Effekseer.GraphicsDevice.h>
#include <vector>

namespace EffekseerRendererUE
{
namespace Backend
{

class GraphicsDevice
	: public Effekseer::Backend::GraphicsDevice
{
public:
	GraphicsDevice() = default;
	virtual ~GraphicsDevice() override = default;

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;
	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;
};

} // namespace Backend
} // namespace EffekseerRendererUE
