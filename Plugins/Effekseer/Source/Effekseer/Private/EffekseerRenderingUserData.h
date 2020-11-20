
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

		virtual bool Equal(const RenderingUserData* rhs) const
		{
			auto r = reinterpret_cast<const EffekseerRenderingUserData*>(rhs);

			return Key == r->Key;
		}
	};

}