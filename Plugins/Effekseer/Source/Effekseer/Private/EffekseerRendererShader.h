#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>
#include <Effekseer/Material/Effekseer.MaterialFile.h>
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h>
#include <unordered_set>
#include <vector>

class UEffekseerMaterial;

namespace EffekseerRendererUE
{
class Shader
	: public EffekseerRenderer::ShaderBase
{
private:
	UEffekseerMaterial* effekseerMaterial_ = nullptr;
	EffekseerRenderer::MaterialShaderParameterGenerator parameterGenerator_;
	Effekseer::RendererMaterialType type_;
	std::vector<uint8_t> vertex_constant_buffer_;
	std::vector<uint8_t> pixel_constant_buffer_;
	bool is_advanced_material_ = false;

public:
	std::unordered_set<Effekseer::MaterialFile::RequiredPredefinedMethodType> RequiredPredefinedMethodTypes;

	/**
		@brief	Constructor for material
	*/
	Shader(UEffekseerMaterial* material, bool isModel, bool isRefraction);

	Shader(Effekseer::RendererMaterialType type, bool is_advanced_material);

	virtual ~Shader() override = default;

	void SetVertexConstantBufferSize(int32_t size) override
	{
		vertex_constant_buffer_.resize(size);
	}

	void SetPixelConstantBufferSize(int32_t size) override
	{
		pixel_constant_buffer_.resize(size);
	}

	void* GetVertexConstantBuffer() override
	{
		return vertex_constant_buffer_.data();
	}

	void* GetPixelConstantBuffer() override
	{
		return pixel_constant_buffer_.data();
	}

	void SetConstantBuffer() override
	{
	}

	const EffekseerRenderer::MaterialShaderParameterGenerator* GetParameterGenerator() const
	{
		return &parameterGenerator_;
	}

	Effekseer::RendererMaterialType GetType() const;

	bool IsAdvancedMaterial() const;

	UEffekseerMaterial* GetEffekseerMaterial() const;
};
} // namespace EffekseerRendererUE