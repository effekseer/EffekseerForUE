#pragma once

#include "CoreMinimal.h"
#include <EffekseerRenderer.RenderStateBase.h>

namespace EffekseerRendererUE
{
class RenderState
	: public EffekseerRenderer::RenderStateBase
{
public:
	RenderState();
	~RenderState();
	void Update(bool forced);
};
} // namespace EffekseerRendererUE