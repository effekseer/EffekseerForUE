#pragma once

#include "CoreMinimal.h"
#include <EffekseerRenderer.IndexBufferBase.h>

namespace EffekseerRendererUE
{
class IndexBuffer
	: public EffekseerRenderer::IndexBufferBase
{
public:
	IndexBuffer(int maxCount, bool isDynamic)
		: EffekseerRenderer::IndexBufferBase(maxCount, isDynamic)
	{
	}

	virtual ~IndexBuffer()
	{
	}

	void Lock() override
	{
	}
	void Unlock() override
	{
	}
};
} // namespace EffekseerRendererUE