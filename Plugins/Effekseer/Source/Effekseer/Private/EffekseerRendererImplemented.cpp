#include "EffekseerRendererImplemented.h"
#include "EffekseerRendererRenderState.h"
#include "EffekseerRendererVertexBuffer.h"
#include "EffekseerRendererIndexBuffer.h"
#include "EffekseerRendererShader.h"

#include "Runtime/Launch/Resources/Version.h"
#include "DynamicMeshBuilder.h"
#include "EffekseerInternalModel.h"
#include "Runtime/Engine/Public/StaticMeshResources.h"
#include "Math/Color.h"
#include "MaterialShared.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRenderer.RingRendererBase.h>

#if ENGINE_MINOR_VERSION < 19
class FMaterialParameterInfo
{
public:
	FName Name;
};

#define GET_MAT_PARAM_NAME .Name
#else

#define GET_MAT_PARAM_NAME
#endif

namespace EffekseerRendererUE4
{
#if ENGINE_MINOR_VERSION >= 25
	using MaterialParameterInfo = FHashedMaterialParameterInfo;
#else
	using MaterialParameterInfo = FMaterialParameterInfo;
#endif

	class FCompatibleMaterialRenderProxy : public FMaterialRenderProxy
	{
	public:
		const FMaterialRenderProxy* Parent = nullptr;

		FCompatibleMaterialRenderProxy(const FMaterialRenderProxy* parent)
			: Parent(parent)
		{
		}
		virtual ~FCompatibleMaterialRenderProxy() = default;

		virtual bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const { return false; }
		virtual bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const { return false; }
		virtual bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const { return false; }

#if ENGINE_MINOR_VERSION < 19
		bool GetVectorValue(const FName ParameterName, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
		{
			FMaterialParameterInfo info;
			info.Name = ParameterName;
			return GetParentVectorValue(info, OutValue, Context);
		}

		bool GetScalarValue(const FName ParameterName, float* OutValue, const FMaterialRenderContext& Context) const
		{
			FMaterialParameterInfo info;
			info.Name = ParameterName;
			return GetParentScalarValue(info, OutValue, Context);
		}

		bool GetTextureValue(const FName ParameterName, const UTexture** OutValue, const FMaterialRenderContext& Context) const
		{
			FMaterialParameterInfo info;
			info.Name = ParameterName;
			return GetParentTextureValue(info, OutValue, Context);
		}
#else
		bool GetVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
		{
			return  GetParentVectorValue(ParameterInfo, OutValue, Context);
		}

		bool GetScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
		{
			return GetParentScalarValue(ParameterInfo, OutValue, Context);
		}

		bool GetTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
		{
			return GetParentTextureValue(ParameterInfo, OutValue, Context);
		}
#endif

#if ENGINE_MINOR_VERSION >= 25
		bool GetTextureValue(const FHashedMaterialParameterInfo& ParameterInfo, const URuntimeVirtualTexture** OutValue, const FMaterialRenderContext& Context) const override
		{
			return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
		}
#elif ENGINE_MINOR_VERSION >= 24
		bool GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const URuntimeVirtualTexture** OutValue, const FMaterialRenderContext& Context) const override
		{
			return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
		}
#endif

		virtual const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const { return nullptr; }

#if ENGINE_MINOR_VERSION < 20
		const FMaterial* GetMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override
		{
			return GetParentMaterial(InFeatureLevel);
		}
#elif ENGINE_MINOR_VERSION < 22
		void GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const override
		{
			OutMaterial = GetParentMaterial(InFeatureLevel);
		}
#else
		const FMaterial& GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const override
		{
			return *(GetParentMaterial(InFeatureLevel));
		}
#endif
	};

	class FFileMaterialRenderProxy : public FCompatibleMaterialRenderProxy
	{
		TArray<float> uniformBuffer_;
		const UEffekseerMaterial* const effekseerMaterial_;
		bool isModel_ = false;
		Effekseer::CullingType cullingType_;
	public:

		FLinearColor ModelUV;
		FLinearColor ModelColor;
		FLinearColor CustomData1;
		FLinearColor CustomData2;
		std::array<void*, Effekseer::TextureSlotMax> Textures;

		

		FFileMaterialRenderProxy(const FMaterialRenderProxy* InParent, const UEffekseerMaterial* effekseerMaterial, float* uniformBufferPtr, int32_t uniformCount, bool isModel, Effekseer::CullingType cullingType)
			: FCompatibleMaterialRenderProxy(InParent)
			, effekseerMaterial_(effekseerMaterial)
			, isModel_(isModel)
			, cullingType_(cullingType)
		{
			Textures.fill(nullptr);

			uniformBuffer_.Reserve(uniformCount * 4);

			for (int32_t i = 0; i < uniformCount * 4; i++)
			{
				uniformBuffer_.Add(uniformBufferPtr[i]);
			}
		}

		const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
		bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
	};

	const FMaterial* FFileMaterialRenderProxy::GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return Parent->GetMaterial(InFeatureLevel);
	}

	bool FFileMaterialRenderProxy::GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		if (isModel_)
		{
			if (ParameterInfo.Name == FName(TEXT("UserUV")))
			{
				*OutValue = ModelUV;
				return true;
			}

			if (ParameterInfo.Name == FName(TEXT("UserColor")))
			{
				*OutValue = ModelColor;
				return true;
			}

			if (ParameterInfo.Name == FName(TEXT("CustomData1")))
			{
				*OutValue = CustomData1;
				return true;
			}

			if (ParameterInfo.Name == FName(TEXT("CustomData2")))
			{
				*OutValue = CustomData2;
				return true;
			}

			if (ParameterInfo.Name == FName(TEXT("Culling")))
			{
				if (cullingType_ == Effekseer::CullingType::Front)
					*OutValue = FLinearColor(1, 0, 0, 0);

				if (cullingType_ == Effekseer::CullingType::Back)
					*OutValue = FLinearColor(0, 1, 0, 0);

				if (cullingType_ == Effekseer::CullingType::Double)
					*OutValue = FLinearColor(1, 1, 0, 0);

				return true;
			}
		}

#if ENGINE_MINOR_VERSION >= 25
		const auto found = effekseerMaterial_->UniformHashedNameToIndex.Find(ParameterInfo.Name);
#else
		const auto found = effekseerMaterial_->UniformNameToIndex.Find(ParameterInfo.Name.ToString());
#endif

		if (found != nullptr)
		{
			OutValue->R = uniformBuffer_[(*found) * 4 + 0];
			OutValue->G = uniformBuffer_[(*found) * 4 + 1];
			OutValue->B = uniformBuffer_[(*found) * 4 + 2];
			OutValue->A = uniformBuffer_[(*found) * 4 + 3];
			return true;
		}

		return Parent->GetVectorValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FFileMaterialRenderProxy::GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("Model")))
		{
			*OutValue = isModel_ ? 1.0f : 0.0f;
			return true;
		}

#if ENGINE_MINOR_VERSION >= 25
		const auto found = effekseerMaterial_->UniformHashedNameToIndex.Find(ParameterInfo.Name);
#else
		const auto found = effekseerMaterial_->UniformNameToIndex.Find(ParameterInfo.Name.ToString());
#endif
		if (found != nullptr)
		{
			*OutValue = uniformBuffer_[(*found) * 4 + 0];
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FFileMaterialRenderProxy::GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
#if ENGINE_MINOR_VERSION >= 25
		const auto found = effekseerMaterial_->TextureHashedNameToIndex.Find(ParameterInfo.Name);
#else
		const auto found = effekseerMaterial_->TextureNameToIndex.Find(ParameterInfo.Name.ToString());
#endif
		if (found != nullptr)
		{
			*OutValue = (UTexture*)Textures[*found];
			return true;
		}

		return Parent->GetTextureValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	class FDistortionMaterialRenderProxy : public FCompatibleMaterialRenderProxy
	{
	public:
		float distortionIntensity;

		/** Initialization constructor. */
		FDistortionMaterialRenderProxy(const FMaterialRenderProxy* InParent, float distortionIntensity) :
			FCompatibleMaterialRenderProxy(InParent),
			distortionIntensity(distortionIntensity)
		{}

		const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
		bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
	};

	const FMaterial* FDistortionMaterialRenderProxy::GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return Parent->GetMaterial(InFeatureLevel);
	}

	bool FDistortionMaterialRenderProxy::GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetVectorValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}


	class FModelMaterialRenderProxy : public FCompatibleMaterialRenderProxy
	{
	public:
		FLinearColor	uv;

		FLinearColor	alphaUV;
		FLinearColor	uvDistortionUV;
		FLinearColor	blendUV;
		FLinearColor	blendAlphaUV;
		FLinearColor	blendUVDistortionUV;
		FLinearColor	flipbookIndexAndNextRate;
		FLinearColor	alphaThreshold;

		FLinearColor	color;
		float			distortionIntensity;
		Effekseer::CullingType cullingType_;

		/** Initialization constructor. */

		FModelMaterialRenderProxy(
			const FMaterialRenderProxy* InParent,
			FLinearColor uv,
			FLinearColor alphaUV,
			FLinearColor uvDistortionUV,
			FLinearColor blendUV,
			FLinearColor blendAlphaUV,
			FLinearColor blendUVDistortionUV,
			float flipbookIndexAndNextRate,
			float alphaThreshold,
			FLinearColor color, 
			float distortionIntensity, 
			Effekseer::CullingType cullingType) :
			FCompatibleMaterialRenderProxy(InParent),
			uv(uv),
			alphaUV(alphaUV),
			uvDistortionUV(uvDistortionUV),
			blendUV(blendUV),
			blendAlphaUV(blendAlphaUV),
			blendUVDistortionUV(blendUVDistortionUV),
			flipbookIndexAndNextRate(FLinearColor(flipbookIndexAndNextRate, 0.0f, 0.0f, 0.0f)),
			alphaThreshold(FLinearColor(0.0f, alphaThreshold, 0.0f, 0.0f)),
			color(color),
			distortionIntensity(distortionIntensity),
			cullingType_(cullingType)
		{}

		const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
		bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
	};


	const FMaterial* FModelMaterialRenderProxy::GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return Parent->GetMaterial(InFeatureLevel);
	}

	bool FModelMaterialRenderProxy::GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("UserUV")))
		{
			*OutValue = uv;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("AlphaUV")))
		{
			*OutValue = alphaUV;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("UVDistortionUV")))
		{
			*OutValue = uvDistortionUV;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("BlendUV")))
		{
			*OutValue = blendUV;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("BlendAlphaUV")))
		{
			*OutValue = blendAlphaUV;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("BlendUVDistortionUV")))
		{
			*OutValue = blendUVDistortionUV;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("ModelFlipbookIndexAndNextRate")))
		{
			*OutValue = flipbookIndexAndNextRate;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("ModelAlphaThreshold")))
		{
			*OutValue = alphaThreshold;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("UserColor")))
		{
			*OutValue = color;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("Culling")))
		{
			if (cullingType_ == Effekseer::CullingType::Front)
				*OutValue = FLinearColor(1, 0, 0, 0);

			if (cullingType_ == Effekseer::CullingType::Back)
				*OutValue = FLinearColor(0, 1, 0, 0);

			if (cullingType_ == Effekseer::CullingType::Double)
				*OutValue = FLinearColor(1, 1, 0, 0);

			return true;
		}

		return Parent->GetVectorValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
	}


	void ExtractTextures(const Effekseer::Effect* effect,
		const Effekseer::NodeRendererBasicParameter* param,
		std::array<Effekseer::TextureData*, ::Effekseer::TextureSlotMax>& textures,
		int32_t& textureCount)
	{
		if (param->MaterialType == Effekseer::RendererMaterialType::File)
		{
			auto materialParam = param->MaterialParameterPtr;

			textureCount = 0;

			if (materialParam->MaterialTextures.size() > 0)
			{
				textureCount = Effekseer::Min(materialParam->MaterialTextures.size(), ::Effekseer::UserTextureSlotMax);

				for (size_t i = 0; i < textureCount; i++)
				{
					if (materialParam->MaterialTextures[i].Type == 1)
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							textures[i] = effect->GetNormalImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							textures[i] = nullptr;
						}
					}
					else
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							textures[i] = effect->GetColorImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							textures[i] = nullptr;
						}
					}
				}
			}
		}
	}

	ModelRenderer::ModelRenderer(RendererImplemented* renderer)
		: m_renderer(renderer)
	{

	}


	ModelRenderer::~ModelRenderer()
	{

	}

	ModelRenderer* ModelRenderer::Create(RendererImplemented* renderer)
	{
		assert(renderer != NULL);

		return new ModelRenderer(renderer);
	}

	void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_(m_renderer, parameter, count, userData);
	}

	void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData)
	{
		Rendering_(m_renderer, parameter, instanceParameter, userData);
	}

	void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
	{
		if (m_matrixes.size() == 0) return;
		if (parameter.ModelIndex < 0) return;
		auto& param = parameter;
		auto& renderer = m_renderer;

		EffekseerInternalModel* model = (EffekseerInternalModel*)parameter.EffectPointer->GetModel(parameter.ModelIndex);
		if (model == nullptr) return;

		if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File)
		{
			Effekseer::MaterialData* material =
				parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			if (material == nullptr)
				return;
		}

		::EffekseerRenderer::RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = parameter.ZTest;
		state.DepthWrite = parameter.ZWrite;
		state.AlphaBlend = parameter.BasicParameterPtr->AlphaBlend;
		state.CullingType = parameter.Culling;

		m_renderer->GetRenderState()->Update(false);

		SortTemporaryValues(m_renderer, parameter);

		EffekseerRenderer::RendererShaderType type = collector_.ShaderType;

		Shader* shader = m_renderer->GetShader(type);

		if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File &&
			param.BasicParameterPtr->MaterialParameterPtr != nullptr &&
			param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0 &&
			param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex) != nullptr)
		{
			shader = (Shader*)param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex)->ModelUserPtr;
		}

		m_renderer->BeginShader(shader);

		if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File)
		{
			auto materialParam = param.BasicParameterPtr->MaterialParameterPtr;

			int32_t textureCount = 0;
			std::array<Effekseer::TextureData*, ::Effekseer::TextureSlotMax> textures;

			ExtractTextures(parameter.EffectPointer, parameter.BasicParameterPtr, textures, textureCount);

			if (textureCount > 0)
			{
				renderer->SetTextures(nullptr, textures.data(), textureCount);
			}


			float* cutomData1Ptr = nullptr;
			float* cutomData2Ptr = nullptr;
			int stageInd = 1;
			Effekseer::MaterialData* material =
				parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);

			StoreFileUniform<RendererImplemented, Shader, 1>(
				m_renderer, shader, material, materialParam, parameter, stageInd, cutomData1Ptr, cutomData2Ptr);

		}
		else
		{
			if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{

				m_renderer->SetDistortionIntensity(parameter.BasicParameterPtr->DistortionIntensity);
			}

			Effekseer::TextureData* textures[7] = { nullptr };

			if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{
				textures[0] = (parameter.BasicParameterPtr->Texture1Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture1Index) : nullptr;
				textures[2] = (parameter.BasicParameterPtr->Texture3Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture3Index) : nullptr;
				textures[3] = (parameter.BasicParameterPtr->Texture4Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture4Index) : nullptr;
				textures[4] = (parameter.BasicParameterPtr->Texture5Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture5Index) : nullptr;
				textures[5] = (parameter.BasicParameterPtr->Texture6Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture6Index) : nullptr;
				textures[6] = (parameter.BasicParameterPtr->Texture7Index >= 0) ? parameter.EffectPointer->GetDistortionImage(parameter.BasicParameterPtr->Texture7Index) : nullptr;
			}	
			else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				textures[0] = (parameter.BasicParameterPtr->Texture1Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture1Index) : nullptr;
				textures[2] = (parameter.BasicParameterPtr->Texture3Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture3Index) : nullptr;
				textures[3] = (parameter.BasicParameterPtr->Texture4Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture4Index) : nullptr;
				textures[4] = (parameter.BasicParameterPtr->Texture5Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture5Index) : nullptr;
				textures[5] = (parameter.BasicParameterPtr->Texture6Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture6Index) : nullptr;
				textures[6] = (parameter.BasicParameterPtr->Texture7Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture7Index) : nullptr;
			}
			else
			{
				textures[0] = (parameter.BasicParameterPtr->Texture1Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture1Index) : nullptr;
				textures[1] = (parameter.BasicParameterPtr->Texture3Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture3Index) : nullptr;
				textures[2] = (parameter.BasicParameterPtr->Texture4Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture4Index) : nullptr;
				textures[3] = (parameter.BasicParameterPtr->Texture5Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture5Index) : nullptr;
				textures[4] = (parameter.BasicParameterPtr->Texture6Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture6Index) : nullptr;
				textures[5] = (parameter.BasicParameterPtr->Texture7Index >= 0) ? parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture7Index) : nullptr;
			}

			m_renderer->SetTextures(nullptr, textures, 7);
		}

		m_renderer->DrawModel(
			model, 
			m_matrixes, 
			m_uv, 
			m_alphaUV, 
			m_uvDistortionUV,
			m_blendUV,
			m_blendAlphaUV,
			m_blendUVDistortionUV,
			m_flipbookIndexAndNextRate, 
			m_alphaThreshold,
			m_colors, 
			m_times, 
			customData1_, 
			customData2_);

		m_renderer->EndShader(shader);

		m_renderer->GetRenderState()->Pop();
	}

	RendererImplemented* RendererImplemented::Create()
	{
		return new RendererImplemented();
	}

	RendererImplemented::RendererImplemented()
	{
		for (int i = 0; i < 5; i++)
		{
			m_materials[i] = nullptr;
		}

		textures_.fill(nullptr);
	}

	RendererImplemented::~RendererImplemented()
	{
		ES_SAFE_DELETE(m_renderState);
		ES_SAFE_DELETE(m_standardRenderer);
		ES_SAFE_DELETE(m_vertexBuffer);
	}

	bool RendererImplemented::Initialize(int32_t squareMaxCount)
	{
		m_squareMaxCount = squareMaxCount;
		m_renderState = new RenderState();
		m_vertexBuffer = new VertexBuffer(EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true);
		stanShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Default, false));
		backDistortedShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::BackDistortion, false));
		lightingShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Lighting, false));
		stanShaderAd_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Default, true));
		backDistortedShaderAd_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::BackDistortion, true));
		lightingShaderAd_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Lighting, true));

		m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

		return true;
	}

	void RendererImplemented::Destroy()
	{
		Release();
	}

	void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
	{
		// TODO
	}

	bool RendererImplemented::BeginRendering()
	{
		impl->CalculateCameraProjectionMatrix();

		// Reset a renderer
		m_standardRenderer->ResetAndRenderingIfRequired();

		//GLCheckError();

		return true;
	}

	bool RendererImplemented::EndRendering()
	{
		// Reset a renderer
		m_standardRenderer->ResetAndRenderingIfRequired();

		return true;
	}

	::Effekseer::Vector3D RendererImplemented::GetLightDirection() const
	{
		return m_lightDirection;
	}

	void RendererImplemented::SetLightDirection(const ::Effekseer::Vector3D& direction)
	{
		m_lightDirection = direction;
	}

	const ::Effekseer::Color& RendererImplemented::GetLightColor() const
	{
		return m_lightColor;
	}

	void RendererImplemented::SetLightColor(const ::Effekseer::Color& color)
	{
		m_lightColor = color;
	}

	const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
	{
		return m_lightAmbient;
	}

	void RendererImplemented::SetLightAmbientColor(const ::Effekseer::Color& color)
	{
		m_lightAmbient = color;
	}

	int32_t RendererImplemented::GetSquareMaxCount() const
	{
		return m_squareMaxCount;
	}

	::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
	{
		return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this);
	}

	::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
	{
		return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this);
	}

	::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
	{
		return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this);
	}

	::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
	{
		return ModelRenderer::Create(this);
	}

	::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
	{
		return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this);
	}

	::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
	{
		return nullptr;
	}

	::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
	{
		return nullptr;
	}

	void RendererImplemented::ResetRenderState()
	{
	}

	::EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
	{
		return nullptr;
	}

	void RendererImplemented::SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback)
	{

	}

	Effekseer::TextureData* RendererImplemented::GetBackground()
	{
		return (Effekseer::TextureData*)1;
	}

	VertexBuffer* RendererImplemented::GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	IndexBuffer* RendererImplemented::GetIndexBuffer()
	{
		return nullptr;
	}

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* RendererImplemented::GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
	{
		return m_renderState;
	}

	void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
	{
	}

	void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
	}

	void RendererImplemented::SetLayout(Shader* shader)
	{
	}

	inline Effekseer::Vector3D UnpackVector3DF(const Effekseer::Color& v)
	{
		Effekseer::Vector3D ret;
		ret.X = (v.R / 255.0 * 2.0f - 1.0f);
		ret.Y = (v.G / 255.0 * 2.0f - 1.0f);
		ret.Z = (v.B / 255.0 * 2.0f - 1.0f);
		return ret;
	}

	void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
	{
		auto mat = FindMaterial();
		if (mat == nullptr) return;

		if (m_currentShader != nullptr && m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
		{
			const auto nativeMaterial = m_currentShader->GetEffekseerMaterial()->GetNativePtr();
			assert(!nativeMaterial->GetIsSimpleVertex());

			auto* origin = (uint8_t*)m_vertexBuffer->GetResource();

			const int32_t stride = (int32_t)sizeof(EffekseerRenderer::DynamicVertex) + (nativeMaterial->GetCustomData1Count() + nativeMaterial->GetCustomData2Count()) * sizeof(float);
			EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> vs(origin, stride, vertexOffset + spriteCount * 4);
			EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> custom1(origin + sizeof(EffekseerRenderer::DynamicVertex), stride, vertexOffset + spriteCount * 4);
			EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> custom2(origin + sizeof(EffekseerRenderer::DynamicVertex) + sizeof(float) * nativeMaterial->GetCustomData1Count(), stride, vertexOffset + spriteCount * 4);

#if ENGINE_MINOR_VERSION < 19
			FDynamicMeshBuilder meshBuilder;
#else
			int32_t customDataCount = 0;
			if (nativeMaterial->GetCustomData1Count() > 0) customDataCount = 2;
			if (nativeMaterial->GetCustomData2Count() > 0) customDataCount = 4;

			FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel(), 2 + customDataCount);
#endif
			for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
			{
				auto& v = vs[vi];

				FDynamicMeshVertex meshVert;

#if ENGINE_MINOR_VERSION >= 20
				if (nativeMaterial->GetCustomData1Count() > 0)
				{
					std::array<float, 4> customData1;
					auto c = (float*)(&custom1[vi]);
					memcpy(customData1.data(), c, sizeof(float) * nativeMaterial->GetCustomData1Count());
					meshVert.TextureCoordinate[2].X = customData1[0];
					meshVert.TextureCoordinate[2].Y = customData1[1];
					meshVert.TextureCoordinate[3].X = customData1[2];
					meshVert.TextureCoordinate[3].Y = customData1[3];
				}

				if (nativeMaterial->GetCustomData2Count() > 0)
				{
					std::array<float, 4> customData2;
					auto c = (float*)(&custom2[vi]);
					memcpy(customData2.data(), c, sizeof(float) * nativeMaterial->GetCustomData2Count());
					meshVert.TextureCoordinate[4].X = customData2[0];
					meshVert.TextureCoordinate[4].Y = customData2[1];
					meshVert.TextureCoordinate[5].X = customData2[2];
					meshVert.TextureCoordinate[5].Y = customData2[3];
				}
#endif
				auto normal = UnpackVector3DF(v.Normal);
				auto tangent = UnpackVector3DF(v.Tangent);
				Effekseer::Vector3D binormal;
				Effekseer::Vector3D::Cross(binormal, normal, tangent);

				meshVert.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
				meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

#if ENGINE_MINOR_VERSION < 19
				meshVert.TextureCoordinate = FVector2D(v.UV1[0], v.UV1[1]);

#else
				meshVert.TextureCoordinate[0] = FVector2D(v.UV1[0], v.UV1[1]);
				meshVert.TextureCoordinate[1] = FVector2D(v.UV2[0], v.UV2[1]);
#endif

				meshVert.SetTangents(
					FVector(binormal.X, binormal.Z, binormal.Y),
					FVector(tangent.X, tangent.Z, tangent.Y),
					FVector(normal.X, normal.Z, normal.Y));

				meshBuilder.AddVertex(meshVert);
			}

			for (int32_t si = 0; si < spriteCount; si++)
			{
				meshBuilder.AddTriangle(
					si * 4 + 0,
					si * 4 + 1,
					si * 4 + 2);

				meshBuilder.AddTriangle(
					si * 4 + 2,
					si * 4 + 1,
					si * 4 + 3);
			}

#if ENGINE_MINOR_VERSION < 22
			auto proxy = mat->GetRenderProxy(false);
#else
			auto proxy = mat->GetRenderProxy();
#endif

			if (m_currentShader->GetEffekseerMaterial()->UniformNameToIndex.Num() > 0 ||
				m_currentShader->GetEffekseerMaterial()->TextureNameToIndex.Num() > 0 ||
				nativeMaterial->GetCustomData1Count() > 0 ||
				nativeMaterial->GetCustomData2Count() > 0)
			{
				auto uniformOffset = m_currentShader->GetParameterGenerator()->PixelUserUniformOffset;
				auto buffer = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + uniformOffset;
				auto newProxy = new FFileMaterialRenderProxy(proxy, m_currentShader->GetEffekseerMaterial(), reinterpret_cast<float*>(buffer), m_currentShader->GetEffekseerMaterial()->Uniforms.Num(), false, m_renderState->GetActiveState().CullingType);

				newProxy->Textures = textures_;

				proxy = newProxy;
				m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
			}

			meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
			return;
		}

		// is single ring?
		bool isSingleRing = false;
		auto stanMat = ((Effekseer::Matrix44*)m_currentShader->GetVertexConstantBuffer())[0];
		auto cameraMat = GetCameraMatrix();
		Effekseer::Matrix44 ringMat;

		if (m_currentShader != nullptr && m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
		{
		}
		else
		{
			for (int32_t r = 0; r < 4; r++)
			{
				for (int32_t c = 0; c < 4; c++)
				{
					if (stanMat.Values[r][c] != cameraMat.Values[r][c])
					{
						isSingleRing = true;
						goto Exit;
					}
				}
			}
		}

	Exit:;

		if (isSingleRing)
		{
			Effekseer::Matrix44 inv;
			Effekseer::Matrix44::Mul(ringMat, stanMat, Effekseer::Matrix44::Inverse(inv, cameraMat));
		}

		Effekseer::RendererMaterialType MaterialType = m_currentShader->GetType();
		bool IsAdvanced = m_currentShader->IsAdvancedMaterial();

		if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			auto intensity = ((float*)m_currentShader->GetPixelConstantBuffer())[0];
			SetDistortionIntensity(intensity);
		}

#if ENGINE_MINOR_VERSION < 19
		FDynamicMeshBuilder meshBuilder;
#else
		FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel(), 7);
#endif

		if (IsAdvanced == false)
		{
			if (MaterialType == Effekseer::RendererMaterialType::Default)
			{
				AddVertex<Effekseer::RendererMaterialType::Default, false>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
			else if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{
				AddVertex<Effekseer::RendererMaterialType::BackDistortion, false>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
			else if (MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				AddVertex<Effekseer::RendererMaterialType::Lighting, false>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
		}
		else
		{
			if (MaterialType == Effekseer::RendererMaterialType::Default)
			{
				AddVertex<Effekseer::RendererMaterialType::Default, true>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
			else if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{
				AddVertex<Effekseer::RendererMaterialType::BackDistortion, true>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
			else if (MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				AddVertex<Effekseer::RendererMaterialType::Lighting, true>(meshBuilder, spriteCount, vertexOffset, isSingleRing, ringMat);
			}
		}
		
		for (int32_t si = 0; si < spriteCount; si++)
		{
			meshBuilder.AddTriangle(
				si * 4 + 0,
				si * 4 + 1,
				si * 4 + 2);

			meshBuilder.AddTriangle(
				si * 4 + 2,
				si * 4 + 1,
				si * 4 + 3);
		}

#if ENGINE_MINOR_VERSION < 22
		auto proxy = mat->GetRenderProxy(false);
#else
		auto proxy = mat->GetRenderProxy();
#endif

		if (m_currentShader->GetType() == Effekseer::RendererMaterialType::BackDistortion)
		{
			proxy = new FDistortionMaterialRenderProxy(proxy, m_distortionIntensity);
			m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
		}

		meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Default, false>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		Vertex* vs = (Vertex*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			FDynamicMeshVertex DynamicVertex;
			DynamicVertex.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
			DynamicVertex.SetTangents(FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1));
			DynamicVertex.TextureCoordinate[0] = FVector2D(v.UV[0], v.UV[1]);
			DynamicVertex.TextureCoordinate[1] =
			DynamicVertex.TextureCoordinate[2] =
			DynamicVertex.TextureCoordinate[3] =
			DynamicVertex.TextureCoordinate[4] =
			DynamicVertex.TextureCoordinate[5] =
			DynamicVertex.TextureCoordinate[6] = FVector2D(0.0f);

			meshBuilder.AddVertex(DynamicVertex);
		}
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Default, true>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		AdvancedVertex* vs = (AdvancedVertex*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			FDynamicMeshVertex DynamicVertex;
			DynamicVertex.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
			DynamicVertex.SetTangents(FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1));
			DynamicVertex.TextureCoordinate[0] = FVector2D(v.UV[0], v.UV[1]);
			DynamicVertex.TextureCoordinate[1] = FVector2D(v.AlphaUV[0], v.AlphaUV[1]);
			DynamicVertex.TextureCoordinate[2] = FVector2D(v.UVDistortionUV[0], v.UVDistortionUV[1]);
			DynamicVertex.TextureCoordinate[3] = FVector2D(v.BlendUV[0], v.BlendUV[1]);
			DynamicVertex.TextureCoordinate[4] = FVector2D(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
			DynamicVertex.TextureCoordinate[5] = FVector2D(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
			DynamicVertex.TextureCoordinate[6] = FVector2D(v.FlipbookIndexAndNextRate, v.AlphaThreshold);

			meshBuilder.AddVertex(DynamicVertex);
		}
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::BackDistortion, false>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		VertexDistortion* vs = (VertexDistortion*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			Effekseer::Vector3D normal;
			Effekseer::Vector3D::Cross(normal, v.Binormal, v.Tangent);

			FDynamicMeshVertex DynamicVertex;
			DynamicVertex.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
			DynamicVertex.SetTangents(
				FVector(v.Binormal.X, v.Binormal.Z ,v.Binormal.Y), 
				FVector(v.Tangent.X, v.Tangent.Z, v.Tangent.Y), 
				FVector(normal.X, normal.Z, normal.Y));
			DynamicVertex.TextureCoordinate[0] = FVector2D(v.UV[0], v.UV[1]);
			DynamicVertex.TextureCoordinate[1] =
			DynamicVertex.TextureCoordinate[2] =
			DynamicVertex.TextureCoordinate[3] =
			DynamicVertex.TextureCoordinate[4] =
			DynamicVertex.TextureCoordinate[5] =
			DynamicVertex.TextureCoordinate[6] = FVector2D(0.0f);

			meshBuilder.AddVertex(DynamicVertex);
		}
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::BackDistortion, true>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		AdvancedVertexDistortion* vs = (AdvancedVertexDistortion*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			Effekseer::Vector3D normal;
			Effekseer::Vector3D::Cross(normal, v.Binormal, v.Tangent);

			FDynamicMeshVertex DynamicVertex;
			DynamicVertex.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
			DynamicVertex.SetTangents(
				FVector(v.Binormal.X, v.Binormal.Z ,v.Binormal.Y), 
				FVector(v.Tangent.X, v.Tangent.Z, v.Tangent.Y), 
				FVector(normal.X, normal.Z, normal.Y));
			DynamicVertex.TextureCoordinate[0] = FVector2D(v.UV[0], v.UV[1]);
			DynamicVertex.TextureCoordinate[1] = FVector2D(v.AlphaUV[0], v.AlphaUV[1]);
			DynamicVertex.TextureCoordinate[2] = FVector2D(v.UVDistortionUV[0], v.UVDistortionUV[1]);
			DynamicVertex.TextureCoordinate[3] = FVector2D(v.BlendUV[0], v.BlendUV[1]);
			DynamicVertex.TextureCoordinate[4] = FVector2D(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
			DynamicVertex.TextureCoordinate[5] = FVector2D(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
			DynamicVertex.TextureCoordinate[6] = FVector2D(v.FlipbookIndexAndNextRate, v.AlphaThreshold);

			meshBuilder.AddVertex(DynamicVertex);
		}
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Lighting, false>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		VertexLighting* vs = (VertexLighting*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			FDynamicMeshVertex meshVert;

			auto normal = UnpackVector3DF(v.Normal);
			auto tangent = UnpackVector3DF(v.Tangent);
			Effekseer::Vector3D binormal;
			Effekseer::Vector3D::Cross(binormal, normal, tangent);

			meshVert.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

#if ENGINE_MINOR_VERSION < 19
			meshVert.TextureCoordinate = FVector2D(v.UV1[0], v.UV1[1]);
#else
			meshVert.TextureCoordinate[0] = FVector2D(v.UV1[0], v.UV1[1]);
			meshVert.TextureCoordinate[1] = FVector2D(v.UV2[0], v.UV2[1]);
			meshVert.TextureCoordinate[2] =
			meshVert.TextureCoordinate[3] =
			meshVert.TextureCoordinate[4] =
			meshVert.TextureCoordinate[5] =
			meshVert.TextureCoordinate[6] = FVector2D(0.0f);
#endif

			meshVert.SetTangents(
				FVector(binormal.X, binormal.Z, binormal.Y),
				FVector(tangent.X, tangent.Z, tangent.Y),
				FVector(normal.X, normal.Z, normal.Y));

			meshBuilder.AddVertex(meshVert);
		}
	}

	template<> void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Lighting, true>
		(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset, bool isSingleRing, const Effekseer::Matrix44& ringMat)
	{
		AdvancedVertexLighting* vs = (AdvancedVertexLighting*)m_vertexBuffer->GetResource();

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			if (isSingleRing)
			{
				Effekseer::Matrix44 trans;
				trans.Translation(v.Pos.X, v.Pos.Y, v.Pos.Z);
				Effekseer::Matrix44::Mul(trans, trans, ringMat);
				v.Pos.X = trans.Values[3][0];
				v.Pos.Y = trans.Values[3][1];
				v.Pos.Z = trans.Values[3][2];
			}

			FDynamicMeshVertex meshVert;

			auto normal = UnpackVector3DF(v.Normal);
			auto tangent = UnpackVector3DF(v.Tangent);
			Effekseer::Vector3D binormal;
			Effekseer::Vector3D::Cross(binormal, normal, tangent);

			meshVert.Position = FVector(v.Pos.X, v.Pos.Z, v.Pos.Y);
			meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

#if ENGINE_MINOR_VERSION < 19
			meshVert.TextureCoordinate = FVector2D(v.UV1[0], v.UV1[1]);
#else
			meshVert.TextureCoordinate[0] = FVector2D(v.UV1[0], v.UV1[1]);
			meshVert.TextureCoordinate[1] = FVector2D(v.AlphaUV[0], v.AlphaUV[1]);
			meshVert.TextureCoordinate[2] = FVector2D(v.UVDistortionUV[0], v.UVDistortionUV[1]);
			meshVert.TextureCoordinate[3] = FVector2D(v.BlendUV[0], v.BlendUV[1]);
			meshVert.TextureCoordinate[4] = FVector2D(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
			meshVert.TextureCoordinate[5] = FVector2D(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
			meshVert.TextureCoordinate[6] = FVector2D(v.FlipbookIndexAndNextRate, v.AlphaThreshold);
#endif

			meshVert.SetTangents(
				FVector(binormal.X, binormal.Z, binormal.Y),
				FVector(tangent.X, tangent.Z, tangent.Y),
				FVector(normal.X, normal.Z, normal.Y));

			meshBuilder.AddVertex(meshVert);
		}
	}

	void RendererImplemented::DrawModel(void* model, 
				   std::vector<Effekseer::Matrix44>& matrixes, 
				   std::vector<Effekseer::RectF>& uvs, 
				   std::vector<Effekseer::RectF>& alphaUVs,
				   std::vector<Effekseer::RectF>& uvDistortionUVs,
				   std::vector<Effekseer::RectF>& blendUVs,
				   std::vector<Effekseer::RectF>& blendAlphaUVs,
				   std::vector<Effekseer::RectF>& blendUVDistortionUVs,
				   std::vector<float>& flipbookIndexAndNextRates,
				   std::vector<float>& alphaThresholds,
				   std::vector<Effekseer::Color>& colors, 
				   std::vector<int32_t>& times, 
				   std::vector<std::array<float, 4>>& customData1, 
				   std::vector<std::array<float, 4>>& customData2)
	{
		// StaticMesh
		if (model == nullptr) return;
		auto mdl = (EffekseerInternalModel*)model;
		auto efkmdl = ((UEffekseerModel*)mdl->UserData);
		UStaticMesh* sm = efkmdl->Mesh;
		if (sm == nullptr) return;

		auto& renderData = sm->RenderData;

		// Material
		auto mat = FindMaterial();

		if (mat == nullptr) return;

		if (renderData->LODResources.Num() == 0) return;
		const auto& lodResource = renderData->LODResources[0];

		for (int32_t objectIndex = 0; objectIndex < matrixes.size(); objectIndex++)
		{
			auto& matOrigin = matrixes[objectIndex];
			auto& uvOrigin = uvs[objectIndex];
			auto& alphaUVOrigin = alphaUVs[objectIndex];
			auto& uvDistortionUVOrigin = uvDistortionUVs[objectIndex];
			auto& blendUVOrigin = blendUVs[objectIndex];
			auto& blendAlphaUVOrigin = blendAlphaUVs[objectIndex];
			auto& blendUVDistortionUVOrigin = blendUVDistortionUVs[objectIndex];

			auto& flipbookIndexAndNextRate = flipbookIndexAndNextRates[objectIndex];
			auto& alphaThreshold = alphaThresholds[objectIndex];
			auto& colorOrigin = colors[objectIndex];

			FMatrix matLocalToWorld = FMatrix(
				FVector(matOrigin.Values[0][0], matOrigin.Values[0][2], matOrigin.Values[0][1]),
				FVector(matOrigin.Values[2][0], matOrigin.Values[2][2], matOrigin.Values[2][1]),
				FVector(matOrigin.Values[1][0], matOrigin.Values[1][2], matOrigin.Values[1][1]),
				FVector(matOrigin.Values[3][0], matOrigin.Values[3][2], matOrigin.Values[3][1])
			);

			FLinearColor uv = FLinearColor(uvOrigin.X, uvOrigin.Y, uvOrigin.Width, uvOrigin.Height);

			FLinearColor alphaUV = FLinearColor(alphaUVOrigin.X, alphaUVOrigin.Y, alphaUVOrigin.Width, alphaUVOrigin.Height);
			FLinearColor uvDistortionUV = FLinearColor(uvDistortionUVOrigin.X, uvDistortionUVOrigin.Y, uvDistortionUVOrigin.Width, uvDistortionUVOrigin.Height);
			FLinearColor blendUV = FLinearColor(blendUVOrigin.X, blendUVOrigin.Y, blendUVOrigin.Width, blendUVOrigin.Height);
			FLinearColor blendAlphaUV = FLinearColor(blendAlphaUVOrigin.X, blendAlphaUVOrigin.Y, blendAlphaUVOrigin.Width, blendAlphaUVOrigin.Height);
			FLinearColor blendUVDistortionUV = FLinearColor(blendUVDistortionUVOrigin.X, blendUVDistortionUVOrigin.Y, blendUVDistortionUVOrigin.Width, blendUVDistortionUVOrigin.Height);

			FLinearColor color = FLinearColor(colorOrigin.R / 255.0f, colorOrigin.G / 255.0f, colorOrigin.B / 255.0f, colorOrigin.A / 255.0f);
			int frameTime = times[objectIndex] % mdl->GetFrameCount();

			for (int32 sectionIndex = 0; sectionIndex < lodResource.Sections.Num(); sectionIndex++)
			{
				auto& section = lodResource.Sections[sectionIndex];
				if (section.NumTriangles <= 0) continue;

				// Is it no problem?
				if (efkmdl->AnimationFaceOffsets.Num() > 0)
				{
					if (sectionIndex != frameTime) continue;
		}

				FMeshBatch& meshElement = m_meshElementCollector->AllocateMesh();
				auto& element = meshElement.Elements[0];

#if ENGINE_MINOR_VERSION < 22
				element.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(
					matLocalToWorld,
					FBoxSphereBounds(EForceInit::ForceInit),
					FBoxSphereBounds(EForceInit::ForceInit),
					false,
					false);
#else

				FDynamicPrimitiveUniformBuffer& dynamicPrimitiveUniformBuffer = m_meshElementCollector->AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
#if ENGINE_MINOR_VERSION < 23
				dynamicPrimitiveUniformBuffer.Set(matLocalToWorld, matLocalToWorld, FBoxSphereBounds(EForceInit::ForceInit), FBoxSphereBounds(EForceInit::ForceInit), false, false, false);
#else
				dynamicPrimitiveUniformBuffer.Set(matLocalToWorld, matLocalToWorld, FBoxSphereBounds(EForceInit::ForceInit), FBoxSphereBounds(EForceInit::ForceInit), false, false, false, false);
#endif

				element.PrimitiveUniformBufferResource = &dynamicPrimitiveUniformBuffer.UniformBuffer;
#endif

#if ENGINE_MINOR_VERSION < 22
				auto proxy = mat->GetRenderProxy(false);
#else
				auto proxy = mat->GetRenderProxy();
#endif

				if (m_currentShader != nullptr && m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
				{
					const auto nativeMaterial = m_currentShader->GetEffekseerMaterial()->GetNativePtr();

					auto uniformOffset = m_currentShader->GetParameterGenerator()->PixelUserUniformOffset;
					auto buffer = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + uniformOffset;
					auto newProxy = new FFileMaterialRenderProxy(
						proxy,
						m_currentShader->GetEffekseerMaterial(),
						reinterpret_cast<float*>(buffer),
						m_currentShader->GetEffekseerMaterial()->Uniforms.Num(),
						true,
						m_renderState->GetActiveState().CullingType);

					newProxy->ModelUV = uv;
					newProxy->ModelColor = color;
					newProxy->Textures = textures_;

					if (nativeMaterial->GetCustomData1Count() > 0)
					{
						auto cd = customData1[objectIndex];
						newProxy->CustomData1 = FLinearColor(cd[0], cd[1], cd[2], cd[3]);
					}

					if (nativeMaterial->GetCustomData2Count() > 0)
					{
						auto cd = customData2[objectIndex];
						newProxy->CustomData2 = FLinearColor(cd[0], cd[1], cd[2], cd[3]);
					}

					proxy = newProxy;
					m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
					}
				else
				{
					proxy = new FModelMaterialRenderProxy(
						proxy, 
						uv, 
						alphaUV,
						uvDistortionUV,
						blendUV,
						blendAlphaUV,
						blendUVDistortionUV,
						flipbookIndexAndNextRate,
						alphaThreshold,
						color, 
						m_distortionIntensity, 
						m_renderState->GetActiveState().CullingType);
					m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
				}

				meshElement.MaterialRenderProxy = proxy;
#if ENGINE_MINOR_VERSION < 19
				meshElement.VertexFactory = &lodResource.VertexFactory;
#else
				meshElement.VertexFactory = &renderData->LODVertexFactories[0].VertexFactory;
#endif
				meshElement.Type = PT_TriangleList;

				if (efkmdl->AnimationFaceOffsets.Num() > 0)
				{
					//element.IndexBuffer = &(lodResource.IndexBuffer);
					//element.FirstIndex = efkmdl->AnimationFaceOffsets[0] + section.FirstIndex;
					//element.NumPrimitives = efkmdl->AnimationFaceCounts[0];
					element.IndexBuffer = &(lodResource.IndexBuffer);
					element.FirstIndex = section.FirstIndex;
					element.NumPrimitives = section.NumTriangles;
				}
				else
				{
					element.IndexBuffer = &(lodResource.IndexBuffer);
					element.FirstIndex = section.FirstIndex;
					element.NumPrimitives = section.NumTriangles;
				}

#if ENGINE_MINOR_VERSION < 19
				meshElement.DynamicVertexData = NULL;
#else
				//meshElement.DynamicVertexData = NULL;
#endif
				//meshElement.LCI = &ProxyLODInfo;

				element.MinVertexIndex = section.MinVertexIndex;
				element.MaxVertexIndex = section.MaxVertexIndex;
				meshElement.LODIndex = 0;

#if ENGINE_MINOR_VERSION < 19
				meshElement.UseDynamicData = false;
#else
				//meshElement.UseDynamicData = false;
#endif
				element.MaxScreenSize = 0.0f;
				element.MinScreenSize = -1.0f;

				m_meshElementCollector->AddMesh(m_viewIndex, meshElement);
				}
				}
	}

	UMaterialInterface* RendererImplemented::FindMaterial()
	{
		if (m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
		{
			return m_currentShader->GetEffekseerMaterial()->FindMatrial((EEffekseerAlphaBlendType)m_renderState->GetActiveState().AlphaBlend);
		}

		EffekseerEffectMaterial m;
		m.Texture = (UTexture2D*)textures_[0];
		m.AlphaBlend = (EEffekseerAlphaBlendType)m_renderState->GetActiveState().AlphaBlend;
		m.IsDepthTestDisabled = !m_renderState->GetActiveState().DepthTest;
		m.IsLighting = m_currentShader->GetType() == Effekseer::RendererMaterialType::Lighting;
		m.IsDistorted = m_currentShader->GetType() == Effekseer::RendererMaterialType::BackDistortion;

		if (m.IsLighting || m.IsDistorted)
		{
			m.AlphaTexture = (UTexture2D*)textures_[2];
			m.UVDistortionTexture = (UTexture2D*)textures_[3];
			m.BlendTexture = (UTexture2D*)textures_[4];
			m.BlendAlphaTexture = (UTexture2D*)textures_[5];
			m.BlendUVDistortionTexture = (UTexture2D*)textures_[6];
		}
		else
		{
			m.AlphaTexture = (UTexture2D*)textures_[1];
			m.UVDistortionTexture = (UTexture2D*)textures_[2];
			m.BlendTexture = (UTexture2D*)textures_[3];
			m.BlendAlphaTexture = (UTexture2D*)textures_[4];
			m.BlendUVDistortionTexture = (UTexture2D*)textures_[5];
		}

		UMaterialInstanceDynamic* mat = nullptr;

		auto it = m_nmaterials.find(m);

		if (it != m_nmaterials.end())
		{
			mat = it->second;
		}

		return mat;
	}

	Shader* RendererImplemented::GetShader(::EffekseerRenderer::RendererShaderType shaderType) const
	{
		if (shaderType == ::EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
		{
			return backDistortedShaderAd_.get();
		}
		else if (shaderType == ::EffekseerRenderer::RendererShaderType::AdvancedLit)
		{
			return lightingShaderAd_.get();
		}
		else if (shaderType == ::EffekseerRenderer::RendererShaderType::AdvancedUnlit)
		{
			return stanShaderAd_.get();
		}
		else if (shaderType == ::EffekseerRenderer::RendererShaderType::BackDistortion)
		{
			return backDistortedShader_.get();
		}
		else if (shaderType == ::EffekseerRenderer::RendererShaderType::Lit)
		{
			return lightingShader_.get();
		}
		else if (shaderType == ::EffekseerRenderer::RendererShaderType::Unlit)
		{
			return stanShader_.get();
		}

		// return as default shader
		return stanShader_.get();
	}

	void RendererImplemented::BeginShader(Shader* shader)
	{
		m_currentShader = shader;
	}

	void RendererImplemented::RendererImplemented::EndShader(Shader* shader)
	{

	}

	void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
	{
		assert(currentShader != nullptr);
		auto p = static_cast<uint8_t*>(m_currentShader->GetVertexConstantBuffer()) + dstOffset;
		memcpy(p, data, size);
	}

	void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
	{
		assert(currentShader != nullptr);
		auto p = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + dstOffset;
		memcpy(p, data, size);
	}

	void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
	{
		if (count > 0)
		{
			for (int32_t i = 0; i < count; i++)
			{
				if (textures[i] != nullptr && textures[i] != (void*)1)
				{
					textures_[i] = textures[i]->UserPtr;
				}
				else
				{
					textures_[i] = nullptr;
				}
			}
		}
		else
		{
			textures_[0] = nullptr;
		}
	}

	void RendererImplemented::SetLocalToWorld(FMatrix localToWorld)
	{
		m_localToWorld = localToWorld;
	}

	void RendererImplemented::SetViewIndex(int32_t viewIndex)
	{
		m_viewIndex = viewIndex;
	}

	void RendererImplemented::SetMaterials(const TMap<UTexture2D*, UMaterialInstanceDynamic*>* materials, int32_t index)
	{
		m_materials[index] = (TMap<UTexture2D*, UMaterialInstanceDynamic*>*)materials;
	}

	void RendererImplemented::SetNMaterials(const std::map<EffekseerEffectMaterial, UMaterialInstanceDynamic*>& nmaterials)
	{
		m_nmaterials = nmaterials;
	}

	void RendererImplemented::SetMeshElementCollector(FMeshElementCollector* meshElementCollector)
	{
		m_meshElementCollector = meshElementCollector;
	}
}