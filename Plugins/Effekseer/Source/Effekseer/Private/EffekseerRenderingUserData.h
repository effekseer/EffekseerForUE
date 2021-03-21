
#pragma once

#include <Effekseer.h>
#include "EffekseerEffect.h"

namespace EffekseerRendererUE4
{

	class EffekseerRenderingUserData : public Effekseer::RenderingUserData
	{
	public:
		EffekseerRenderingUserData() = default;
		virtual ~EffekseerRenderingUserData() = default;

		EffekseerEffectMaterialKey Key;
		float Magnification;

		virtual bool Equal(const RenderingUserData* rhs) const
		{
			auto r = static_cast<const EffekseerRenderingUserData*>(rhs);

			return Key == r->Key && Magnification == r->Magnification;
		}
	};

}