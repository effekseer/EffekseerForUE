
#include "EffekseerRendererRenderState.h"

namespace EffekseerRendererUE4
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
}