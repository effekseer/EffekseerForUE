#include "EffekseerPrivatePCH.h"  // UE4

namespace EffekseerRendererUE4
{
	Shader::Shader()
	{
		vertexConstantBuffer.resize(sizeof(::Effekseer::Matrix44) * 2);
		pixelConstantBuffer.resize(sizeof(float));
	}

	Shader::~Shader()
	{

	}
}