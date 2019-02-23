#include "EffekseerRendererShader.h"

namespace EffekseerRendererUE4
{
	Shader::Shader()
	{
		vertexConstantBuffer.resize(sizeof(::Effekseer::Matrix44) * 4);
		pixelConstantBuffer.resize(sizeof(float) * 8);
	}

	Shader::~Shader()
	{

	}
}