#include "EffekseerRendererUE.VertexBuffer.h"

#include <algorithm>

namespace EffekseerRendererUE
{
namespace Backend
{

VertexBuffer::VertexBuffer(int size)
{
	data_.resize(size);
	std::fill(data_.begin(), data_.end(), 0);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	if (src == nullptr || size == 0)
	{
		return;
	}

	ensure(size + offset <= data_.size());
	memcpy(data_.data() + offset, src, size);
}

uint8_t* VertexBuffer::GetResource()
{
	return data_.data();
}

} // namespace Backend
} // namespace EffekseerRendererUE