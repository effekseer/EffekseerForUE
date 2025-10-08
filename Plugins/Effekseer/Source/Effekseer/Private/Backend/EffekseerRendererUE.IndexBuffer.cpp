#include "EffekseerRendererUE.IndexBuffer.h"

namespace EffekseerRendererUE
{
namespace Backend
{

IndexBuffer::IndexBuffer(Effekseer::Backend::IndexBufferStrideType strideType, int32_t elementCount)
{
	strideType_ = strideType;
	elementCount_ = elementCount;

	int elmSize = 0;
	if (strideType_ == Effekseer::Backend::IndexBufferStrideType::Stride2)
	{
		elmSize = 2;
	}
	else if (strideType_ == Effekseer::Backend::IndexBufferStrideType::Stride4)
	{
		elmSize = 4;
	}

	auto size = elmSize * elementCount_;
	data_.resize(size);
	std::fill(data_.begin(), data_.end(), 0);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	if (src == nullptr || size == 0)
	{
		return;
	}

	ensure(size + offset <= data_.size());
	memcpy(data_.data() + offset, src, size);
}

int32_t IndexBuffer::GetBufferSize() const
{
	return static_cast<int32_t>(data_.size());
}

uint8_t* IndexBuffer::GetResource()
{
	return data_.data();
}

} // namespace Backend
} // namespace EffekseerRendererUE
