#pragma once

#include "EffekseerRendererNative.h"

namespace EffekseerRendererUE4
{
	class RenderState
		: public EffekseerRenderer::RenderStateBase
	{
	public:
		RenderState();
		~RenderState();
		void Update(bool forced);
	};
}