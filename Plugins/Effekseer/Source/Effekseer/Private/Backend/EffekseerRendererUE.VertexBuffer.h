#pragma once

#include "CoreMinimal.h"
#include <Effekseer/Backend/Effekseer.GraphicsDevice.h>
#include <vector>

namespace EffekseerRendererUE
{
namespace Backend
{

// It can optimize more

class VertexBuffer
	: public Effekseer::Backend::VertexBuffer
{
private:
	std::vector<uint8_t> data_;

public:
	VertexBuffer(int size);
	virtual ~VertexBuffer() override = default;

	void UpdateData(const void* src, int32_t size, int32_t offset) override;

	uint8_t* GetResource();
};

} // namespace Backend
} // namespace EffekseerRendererUE