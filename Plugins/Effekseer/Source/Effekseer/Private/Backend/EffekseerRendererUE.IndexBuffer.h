#pragma once

#include "CoreMinimal.h"
#include <Effekseer/Backend/Effekseer.GraphicsDevice.h>
#include <vector>

namespace EffekseerRendererUE
{
namespace Backend
{

class IndexBuffer
	: public Effekseer::Backend::IndexBuffer
{
private:
	std::vector<uint8_t> data_;

public:
	IndexBuffer(Effekseer::Backend::IndexBufferStrideType strideType, int32_t elementCount);
	virtual ~IndexBuffer() override = default;
	void UpdateData(const void* src, int32_t size, int32_t offset) override;

	int32_t GetBufferSize() const;
	uint8_t* GetResource();
};

} // namespace Backend
} // namespace EffekseerRendererUE