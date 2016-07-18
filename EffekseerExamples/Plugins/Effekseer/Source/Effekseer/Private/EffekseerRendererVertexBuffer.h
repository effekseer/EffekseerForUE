#pragma once

#include "EffekseerRendererNative.h"

namespace EffekseerRendererUE4
{
	class VertexBuffer
		: public EffekseerRenderer::VertexBufferBase
	{
	public:
		VertexBuffer(int size, bool isDynamic)
			: EffekseerRenderer::VertexBufferBase(size, isDynamic)
		{
		}

		virtual ~VertexBuffer()
		{}

		void Lock() override {}
		bool RingBufferLock(int32_t size, int32_t& offset, void*& data) override {}
		bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data) override {}
		void Unlock() override {}
	};
}