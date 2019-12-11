#include "EffekseerRendererShader.h"
#include "EffekseerMaterial.h"

namespace EffekseerRendererUE4
{
	Shader::Shader(UEffekseerMaterial* material, bool isModel, bool isRefraction)
		: effekseerMaterial_(material)
		, parameterGenerator_(*material->GetNativePtr(), isModel, isRefraction ? 1 : 0, 1)
		, type_(Effekseer::RendererMaterialType::File)
	{
		vertexConstantBuffer.resize(parameterGenerator_.VertexShaderUniformBufferSize);
		pixelConstantBuffer.resize(parameterGenerator_.PixelShaderUniformBufferSize);
	}

	Shader::Shader(Effekseer::RendererMaterialType type)
		: parameterGenerator_(::Effekseer::Material(), false, 0, 1)
		, type_(type)
	{
		vertexConstantBuffer.resize(sizeof(::Effekseer::Matrix44) * 4);
		pixelConstantBuffer.resize(sizeof(float) * 16);
	}

	Shader::~Shader()
	{

	}

	Effekseer::RendererMaterialType Shader::GetType() const
	{
		return type_;
	}

	UEffekseerMaterial* Shader::GetEffekseerMaterial() const
	{
		return effekseerMaterial_;
	}
}