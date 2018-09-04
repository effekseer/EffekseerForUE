#pragma once

#include "CoreMinimal.h"
#include "EffekseerRendererNative.h"

#include <vector>

namespace EffekseerRendererUE4
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
		uint32_t				m_vertexRingStart;
		uint32_t				m_vertexRingOffset;
		bool					m_ringBufferLock;
	public:
		VertexBuffer(int size, bool isDynamic);
		virtual ~VertexBuffer();

		void Lock() override;
		bool RingBufferLock(int32_t size, int32_t& offset, void*& data) override;
		bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data) override;
		void Unlock() override;

		uint8_t* GetResource()
		{
			return m_resource;
		}
	};
}