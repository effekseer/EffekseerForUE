#include "EffekseerRendererShader.h"

#include "EffekseerMaterial.h"
#include "EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h"
#include "EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"

namespace EffekseerRendererUE
{
Shader::Shader(UEffekseerMaterial* material, bool isModel, bool isRefraction)
	: effekseerMaterial_(material)
	, parameterGenerator_(*material->GetNativePtr(), isModel, isRefraction ? 1 : 0, 1)
	, type_(Effekseer::RendererMaterialType::File)
{
	vertexConstantBuffer.resize(parameterGenerator_.VertexShaderUniformBufferSize);
	pixelConstantBuffer.resize(parameterGenerator_.PixelShaderUniformBufferSize);

	for (const auto method : effekseerMaterial_->GetNativePtr()->RequiredMethods)
	{
		RequiredPredefinedMethodTypes.insert(method);
	}
}

Shader::Shader(Effekseer::RendererMaterialType type, bool isAdvancedMaterial)
	: parameterGenerator_(::Effekseer::MaterialFile(), false, 0, 1)
	, type_(type)
	, isAdvancedMaterial_(isAdvancedMaterial)
{
	auto vertexConstantBufferSize = sizeof(EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<1>);
	vertexConstantBufferSize = std::max(vertexConstantBufferSize, sizeof(EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
	vertexConstantBufferSize = std::max(vertexConstantBufferSize, sizeof(EffekseerRenderer::StandardRendererVertexBuffer));

	vertexConstantBuffer.resize(vertexConstantBufferSize);

	pixelConstantBuffer.resize(std::max(sizeof(EffekseerRenderer::PixelConstantBuffer), sizeof(EffekseerRenderer::PixelConstantBufferDistortion)));
}

Shader::~Shader()
{
}

Effekseer::RendererMaterialType Shader::GetType() const
{
	return type_;
}

bool Shader::IsAdvancedMaterial() const
{
	return isAdvancedMaterial_;
}

UEffekseerMaterial* Shader::GetEffekseerMaterial() const
{
	return effekseerMaterial_;
}
} // namespace EffekseerRendererUE