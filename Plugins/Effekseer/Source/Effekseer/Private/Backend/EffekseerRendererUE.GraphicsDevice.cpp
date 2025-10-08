#include "EffekseerRendererUE.GraphicsDevice.h"

#include "EffekseerRendererUE.IndexBuffer.h"
#include "EffekseerRendererUE.VertexBuffer.h"

namespace EffekseerRendererUE
{
namespace Backend
{

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = Effekseer::MakeRefPtr<VertexBuffer>(size);
	ret->UpdateData(initialData, size, false);
	return ret;
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(stride, elementCount);
	ret->UpdateData(initialData, ret->GetBufferSize(), false);
	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererUE
