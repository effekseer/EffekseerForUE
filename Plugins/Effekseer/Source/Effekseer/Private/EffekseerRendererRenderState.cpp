
#include "EffekseerRendererRenderState.h"

namespace EffekseerRendererUE
{
RenderState::RenderState()
{
}

RenderState::~RenderState()
{
}

void RenderState::Update(bool forced)
{
	active_ = next_;
}
} // namespace EffekseerRendererUE