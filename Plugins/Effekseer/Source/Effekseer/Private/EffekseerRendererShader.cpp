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
	vertex_constant_buffer_.resize(parameterGenerator_.VertexShaderUniformBufferSize);
	pixel_constant_buffer_.resize(parameterGenerator_.PixelShaderUniformBufferSize);

	for (const auto method : effekseerMaterial_->GetNativePtr()->RequiredMethods)
	{
		RequiredPredefinedMethodTypes.insert(method);
	}
}

Shader::Shader(Effekseer::RendererMaterialType type, bool isAdvancedMaterial)
	: parameterGenerator_(::Effekseer::MaterialFile(), false, 0, 1)
	, type_(type)
	, is_advanced_material_(isAdvancedMaterial)
{
	auto vertexConstantBufferSize = sizeof(EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<1>);
	vertexConstantBufferSize = std::max(vertexConstantBufferSize, sizeof(EffekseerRenderer::ModelRendererVertexConstantBuffer<1>));
	vertexConstantBufferSize = std::max(vertexConstantBufferSize, sizeof(EffekseerRenderer::StandardRendererVertexBuffer));

	vertex_constant_buffer_.resize(vertexConstantBufferSize);

	pixel_constant_buffer_.resize(std::max(sizeof(EffekseerRenderer::PixelConstantBuffer), sizeof(EffekseerRenderer::PixelConstantBufferDistortion)));
}

Effekseer::RendererMaterialType Shader::GetType() const
{
	return type_;
}

bool Shader::IsAdvancedMaterial() const
{
	return is_advanced_material_;
}

UEffekseerMaterial* Shader::GetEffekseerMaterial() const
{
	return effekseerMaterial_;
}
} // namespace EffekseerRendererUE