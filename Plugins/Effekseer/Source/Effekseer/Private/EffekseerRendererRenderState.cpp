
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
	m_active = m_next;
}
} // namespace EffekseerRendererUE