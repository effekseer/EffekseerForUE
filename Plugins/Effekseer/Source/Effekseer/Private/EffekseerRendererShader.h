#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>
#include <vector>
#include <EffekseerRenderer.CommonUtils.h>

class UEffekseerMaterial;

namespace EffekseerRendererUE4
{
	class Shader
	{
	private:
		UEffekseerMaterial* effekseerMaterial_ = nullptr;
		EffekseerRenderer::MaterialShaderParameterGenerator parameterGenerator_;
		Effekseer::RendererMaterialType type_;
		std::vector<uint8_t>	vertexConstantBuffer;
		std::vector<uint8_t>	pixelConstantBuffer;
		bool isAdvancedMaterial_ = false;

	public:

		/**
			@brief	Constructor for material
		*/
		Shader(UEffekseerMaterial* material, bool isModel, bool isRefraction);

		Shader(Effekseer::RendererMaterialType type, bool isAdvancedMaterial);

		virtual ~Shader();

		void* GetVertexConstantBuffer()
		{
			return vertexConstantBuffer.data();
		}

		void* GetPixelConstantBuffer()
		{
			return pixelConstantBuffer.data();
		}

		void SetConstantBuffer()
		{
		}

		const EffekseerRenderer::MaterialShaderParameterGenerator* GetParameterGenerator() const { return &parameterGenerator_; }

		Effekseer::RendererMaterialType GetType() const;

		bool IsAdvancedMaterial() const;

		UEffekseerMaterial* GetEffekseerMaterial() const;

	};
}