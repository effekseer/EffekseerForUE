#pragma once

#include "CoreMinimal.h"
#include <EffekseerRenderer.VertexBufferBase.h>
#include <vector>

namespace EffekseerRendererUE
{
/**
	@brief	頂点バッファ
	@note
	現在：メモリを使用したあまり効率の良くない実装
*/
class VertexBuffer
	: public EffekseerRenderer::VertexBufferBase
{
private:
	uint32_t m_vertexRingStart;
	bool m_ringBufferLock;

public:
	VertexBuffer(int size, bool isDynamic);
	virtual ~VertexBuffer();

	void Lock() override;
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	void Unlock() override;

	uint8_t* GetResource()
	{
		return m_resource;
	}
};
} // namespace EffekseerRendererUE