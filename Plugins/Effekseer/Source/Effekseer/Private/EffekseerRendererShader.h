#pragma once

#include "CoreMinimal.h"
#include "EffekseerRendererNative.h"
#include <vector>

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

	public:

		/**
			@brief	Constructor for material
		*/
		Shader(UEffekseerMaterial* material, bool isModel, bool isRefraction);

		Shader(Effekseer::RendererMaterialType type);

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

		UEffekseerMaterial* GetEffekseerMaterial() const;
	};
}