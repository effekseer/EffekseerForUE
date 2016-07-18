#pragma once

#include "EffekseerRendererNative.h"

namespace EffekseerRendererUE4
{
	class Shader
	{
	public:
		Shader()
		{
		}

		virtual ~Shader()
		{}

		void* GetVertexConstantBuffer()
		{
			// TODO
			return nullptr;
		}

		void* GetPixelConstantBuffer()
		{
			// TODO
			return nullptr;
		}

		void SetConstantBuffer()
		{
			// TODO
		}
	};
}