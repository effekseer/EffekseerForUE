#include "EffekseerRendererImplemented.h"

#include "DynamicMeshBuilder.h"
#include "EffekseerInternalModel.h"
#include "EffekseerInternalTexture.h"
#include "EffekseerProceduralModel.h"
#include "EffekseerRendererIndexBuffer.h"
#include "EffekseerRendererRenderState.h"
#include "EffekseerRendererShader.h"
#include "EffekseerRendererVertexBuffer.h"
#include "EffekseerUECompatibility.h"
#include "MaterialShared.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/Color.h"
#include "Runtime/Engine/Public/StaticMeshResources.h"
#include "Runtime/Launch/Resources/Version.h"
#include <EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRenderer.TrackRendererBase.h>

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
#include "Materials/MaterialRenderProxy.h"
#endif

using namespace EffekseerUE;

#define GET_MAT_PARAM_NAME

namespace EffekseerRendererUE
{
using MaterialParameterInfo = FHashedMaterialParameterInfo;

class FCompatibleMaterialRenderProxy : public FMaterialRenderProxy
{
public:
	const FMaterialRenderProxy* Parent = nullptr;

	FCompatibleMaterialRenderProxy(const FString& inMaterialName, const FMaterialRenderProxy* parent)
#if ENGINE_MAJOR_VERSION == 4
		: Parent(parent)
#elif ENGINE_MAJOR_VERSION == 5
		: FMaterialRenderProxy(inMaterialName)
		, Parent(parent)
#endif
	{
	}

	virtual ~FCompatibleMaterialRenderProxy() = default;

	virtual bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		return false;
	}
	virtual bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		return false;
	}
	virtual bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return false;
	}

	bool GetVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		return GetParentVectorValue(ParameterInfo, OutValue, Context);
	}

	bool GetScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		return GetParentScalarValue(ParameterInfo, OutValue, Context);
	}

	bool GetTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return GetParentTextureValue(ParameterInfo, OutValue, Context);
	}

#if ENGINE_MAJOR_VERSION == 4
	bool GetTextureValue(const FHashedMaterialParameterInfo& ParameterInfo, const URuntimeVirtualTexture** OutValue, const FMaterialRenderContext& Context) const override
	{
		return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
	}
#endif

#if ENGINE_MAJOR_VERSION == 5
	bool GetParameterValue(EMaterialParameterType Type, const FHashedMaterialParameterInfo& ParameterInfo, FMaterialParameterValue& OutValue, const FMaterialRenderContext& Context) const override
	{
		if (Type == EMaterialParameterType::Scalar)
		{
			if (GetScalarValue(ParameterInfo, &(OutValue.Float[0]), Context))
			{
				OutValue.Type = Type;
				return true;
			}
		}
		else if (Type == EMaterialParameterType::Vector)
		{
			FLinearColor color;
			if (GetVectorValue(ParameterInfo, &color, Context))
			{
				OutValue.Float[0] = color.R;
				OutValue.Float[1] = color.G;
				OutValue.Float[2] = color.B;
				OutValue.Float[3] = color.A;
				OutValue.Type = Type;

				return true;
			}
		}
		else if (Type == EMaterialParameterType::Texture)
		{
			if (GetTextureValue(ParameterInfo, const_cast<const UTexture**>(&(OutValue.Texture)), Context))
			{
				OutValue.Type = Type;
				return true;
			}
		}

		return Parent->GetParameterValue(Type, ParameterInfo, OutValue, Context);
	}
#endif

	virtual const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return nullptr;
	}

	const FMaterial* GetMaterialNoFallback(ERHIFeatureLevel::Type InFeatureLevel) const override
	{
		return Parent->GetMaterialNoFallback(InFeatureLevel);
	}

	const FMaterialRenderProxy* GetFallback(ERHIFeatureLevel::Type InFeatureLevel) const override
	{
		return Parent->GetFallback(InFeatureLevel);
	}
};

class FFileMaterialRenderProxy : public FCompatibleMaterialRenderProxy
{
	TArray<float> uniformBuffer_;
	const UEffekseerMaterial* const effekseerMaterial_;
	bool isModel_ = false;
	Effekseer::CullingType cullingType_;
	float effectScale_ = 1.0f;

public:
	FLinearColor ModelUV;
	FLinearColor ModelColor;
	FLinearColor CustomData1;
	FLinearColor CustomData2;
	FLinearColor LightDirection;
	FLinearColor LightColor;
	FLinearColor LightAmbientColor;
	float LocalTime;

	std::array<void*, Effekseer::TextureSlotMax> Textures;

	FFileMaterialRenderProxy(const FMaterialRenderProxy* InParent, const UEffekseerMaterial* effekseerMaterial, float* uniformBufferPtr, int32_t uniformCount, bool isModel, Effekseer::CullingType cullingType, float effectScale)
		: FCompatibleMaterialRenderProxy(FString("FileMaterialRenderProxy"), InParent)
		, effekseerMaterial_(effekseerMaterial)
		, isModel_(isModel)
		, cullingType_(cullingType)
		, effectScale_(effectScale)
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
	return Parent->GetMaterialNoFallback(InFeatureLevel);
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

	if (ParameterInfo.Name == FName(TEXT("LightDirection")))
	{
		*OutValue = LightDirection;
		return true;
	}

	if (ParameterInfo.Name == FName(TEXT("LightColor")))
	{
		*OutValue = LightColor;
		return true;
	}

	if (ParameterInfo.Name == FName(TEXT("LightAmbientColor")))
	{
		*OutValue = LightAmbientColor;
		return true;
	}

	const auto found = effekseerMaterial_->UniformHashedNameToIndex.Find(ParameterInfo.Name.ToString());

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

	if (ParameterInfo.Name == FName(TEXT("EffectScale")))
	{
		*OutValue = effectScale_;
		return true;
	}

	if (ParameterInfo.Name == FName(TEXT("LocalTime")))
	{
		*OutValue = LocalTime;
		return true;
	}

	const auto found = effekseerMaterial_->UniformHashedNameToIndex.Find(ParameterInfo.Name.ToString());

	if (found != nullptr)
	{
		*OutValue = uniformBuffer_[(*found) * 4 + 0];
		return true;
	}

	return Parent->GetScalarValue(ParameterInfo GET_MAT_PARAM_NAME, OutValue, Context);
}

bool FFileMaterialRenderProxy::GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
{
	const auto found = effekseerMaterial_->TextureHashedNameToIndex.Find(ParameterInfo.Name.ToString());

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
	FDistortionMaterialRenderProxy(const FMaterialRenderProxy* InParent, float distortionIntensity)
		: FCompatibleMaterialRenderProxy(FString("FDistortionMaterialRenderProxy"), InParent)
		, distortionIntensity(distortionIntensity)
	{
	}

	const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
	bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
	bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
	bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
};

const FMaterial* FDistortionMaterialRenderProxy::GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
{
	return Parent->GetMaterialNoFallback(InFeatureLevel);
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
	FLinearColor uv;

	FLinearColor alphaUV;
	FLinearColor uvDistortionUV;
	FLinearColor blendUV;
	FLinearColor blendAlphaUV;
	FLinearColor blendUVDistortionUV;
	FLinearColor flipbookIndexAndNextRate;
	FLinearColor alphaThreshold;

	FLinearColor color;
	float distortionIntensity;
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
		Effekseer::CullingType cullingType)
		: FCompatibleMaterialRenderProxy(FString("FModelMaterialRenderProxy"), InParent)
		, uv(uv)
		, alphaUV(alphaUV)
		, uvDistortionUV(uvDistortionUV)
		, blendUV(blendUV)
		, blendAlphaUV(blendAlphaUV)
		, blendUVDistortionUV(blendUVDistortionUV)
		, flipbookIndexAndNextRate(FLinearColor(flipbookIndexAndNextRate, 0.0f, 0.0f, 0.0f))
		, alphaThreshold(FLinearColor(0.0f, alphaThreshold, 0.0f, 0.0f))
		, color(color)
		, distortionIntensity(distortionIntensity)
		, cullingType_(cullingType)
	{
	}

	const FMaterial* GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
	bool GetParentVectorValue(const MaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
	bool GetParentScalarValue(const MaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
	bool GetParentTextureValue(const MaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
};

const FMaterial* FModelMaterialRenderProxy::GetParentMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
{
	return Parent->GetMaterialNoFallback(InFeatureLevel);
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
					 std::array<Effekseer::Backend::TextureRef, ::Effekseer::TextureSlotMax>& textures,
					 int32_t& textureCount)
{
	if (param->MaterialType == Effekseer::RendererMaterialType::File)
	{
		auto materialParam = param->MaterialRenderDataPtr;

		textureCount = 0;

		if (materialParam->MaterialTextures.size() > 0)
		{
			textureCount = Effekseer::Min(materialParam->MaterialTextures.size(), ::Effekseer::UserTextureSlotMax);

			for (size_t i = 0; i < textureCount; i++)
			{
				if (materialParam->MaterialTextures[i].Type == 1)
				{
					if (materialParam->MaterialTextures[i].Index >= 0 && effect->GetNormalImage(materialParam->MaterialTextures[i].Index) != nullptr)
					{
						textures[i] = effect->GetNormalImage(materialParam->MaterialTextures[i].Index)->GetBackend();
					}
					else
					{
						textures[i] = nullptr;
					}
				}
				else
				{
					if (materialParam->MaterialTextures[i].Index >= 0 && effect->GetColorImage(materialParam->MaterialTextures[i].Index) != nullptr)
					{
						textures[i] = effect->GetColorImage(materialParam->MaterialTextures[i].Index)->GetBackend();
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

Effekseer::ModelRendererRef ModelRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return Effekseer::MakeRefPtr<ModelRenderer>(renderer);
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
	if (m_matrixes.size() == 0)
		return;
	if (parameter.ModelIndex < 0)
		return;
	auto& param = parameter;
	auto& renderer = m_renderer;

	EffekseerInternalModel* model = nullptr;
	if (param.IsProceduralMode)
	{
		model = (EffekseerInternalModel*)parameter.EffectPointer->GetProceduralModel(parameter.ModelIndex).Get();
	}
	else
	{
		model = (EffekseerInternalModel*)parameter.EffectPointer->GetModel(parameter.ModelIndex).Get();
	}

	if (model == nullptr)
		return;

	if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File)
	{
		auto material = parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex);
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
		param.BasicParameterPtr->MaterialRenderDataPtr != nullptr &&
		param.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex >= 0 &&
		param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex) != nullptr)
	{
		shader = (Shader*)param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex)->ModelUserPtr;
	}

	m_renderer->BeginShader(shader);

	if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File)
	{
		auto materialParam = param.BasicParameterPtr->MaterialRenderDataPtr;

		int32_t textureCount = 0;
		std::array<Effekseer::Backend::TextureRef, ::Effekseer::TextureSlotMax> textures;

		ExtractTextures(parameter.EffectPointer, parameter.BasicParameterPtr, textures, textureCount);

		if (textureCount > 0)
		{
			renderer->SetTextures(nullptr, textures.data(), textureCount);
		}

		float* cutomData1Ptr = nullptr;
		float* cutomData2Ptr = nullptr;
		int stageInd = 1;
		auto material = parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex);

		StoreFileUniform<RendererImplemented, Shader, 1>(
			m_renderer, shader, material, materialParam, parameter, stageInd, cutomData1Ptr, cutomData2Ptr);
	}
	else
	{
		if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			m_renderer->SetDistortionIntensity(parameter.BasicParameterPtr->DistortionIntensity);
		}

		m_renderer->SetTextures(nullptr, collector_.Textures.data(), collector_.TextureCount);
	}

	m_renderer->GetImpl()->CurrentRenderingUserData = param.UserData;

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

Effekseer::RefPtr<RendererImplemented> RendererImplemented::Create()
{
	return Effekseer::MakeRefPtr<RendererImplemented>();
}

RendererImplemented::RendererImplemented()
{
	for (int i = 0; i < 5; i++)
	{
		m_materials[i] = nullptr;
	}

	textures_.fill(nullptr);

	auto internalBackgroundTexture_ = Effekseer::MakeRefPtr<EffekseerInternalTexture>();
	internalBackgroundTexture_->UserData = reinterpret_cast<void*>(0x1);
	backgroundTexture_ = internalBackgroundTexture_;
}

RendererImplemented::~RendererImplemented()
{
	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_vertexBuffer);
}

bool RendererImplemented::Initialize(int32_t squareMaxCount, EEffekseerColorSpaceType colorSpace)
{
	colorSpace_ = colorSpace;
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

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	// TODO
}

bool RendererImplemented::BeginRendering()
{
	impl->CalculateCameraProjectionMatrix();

	// Reset a renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	// GLCheckError();

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

::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return Effekseer::MakeRefPtr<::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>>(this);
}

::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return Effekseer::MakeRefPtr<::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>>(this);
}

::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	auto ret = Effekseer::MakeRefPtr<::EffekseerRenderer::RingRendererBase<RendererImplemented, false>>(this);
	return ret;
}

::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this);
}

::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return Effekseer::MakeRefPtr<::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>>(this);
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

const ::Effekseer::Backend::TextureRef& RendererImplemented::GetBackground()
{
	return backgroundTexture_;
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
	auto reunderingUserData = static_cast<EffekseerRenderingUserData*>(GetImpl()->CurrentRenderingUserData.Get());
	auto shader = m_currentShader;

	auto mat = FindMaterial(reunderingUserData, shader);
	if (mat == nullptr)
	{
		return;
	}

	if (shader != nullptr && shader->GetType() == Effekseer::RendererMaterialType::File)
	{
		const auto nativeMaterial = shader->GetEffekseerMaterial()->GetNativePtr();
		assert(!nativeMaterial->GetIsSimpleVertex());

		auto* origin = (uint8_t*)m_vertexBuffer->GetResource();

		const int32_t stride = (int32_t)sizeof(EffekseerRenderer::DynamicVertex) + (nativeMaterial->GetCustomData1Count() + nativeMaterial->GetCustomData2Count()) * sizeof(float);
		EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> vs(origin, stride, vertexOffset + spriteCount * 4);
		EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> custom1(origin + sizeof(EffekseerRenderer::DynamicVertex), stride, vertexOffset + spriteCount * 4);
		EffekseerRenderer::StrideView<EffekseerRenderer::DynamicVertex> custom2(origin + sizeof(EffekseerRenderer::DynamicVertex) + sizeof(float) * nativeMaterial->GetCustomData1Count(), stride, vertexOffset + spriteCount * 4);

		int32_t customDataCount = 0;
		if (nativeMaterial->GetCustomData1Count() > 0)
			customDataCount = 2;
		if (nativeMaterial->GetCustomData2Count() > 0)
			customDataCount = 4;

		FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel(), 2 + customDataCount);

		for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
		{
			auto& v = vs[vi];

			FDynamicMeshVertex meshVert;

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

			auto normal = UnpackVector3DF(v.Normal);
			auto tangent = UnpackVector3DF(v.Tangent);
			Effekseer::Vector3D binormal;
			Effekseer::Vector3D::Cross(binormal, normal, tangent);

			meshVert.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
			meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

			meshVert.TextureCoordinate[0] = UEFVector2f(v.UV1[0], v.UV1[1]);
			meshVert.TextureCoordinate[1] = UEFVector2f(v.UV2[0], v.UV2[1]);

			meshVert.SetTangents(
				UEFVector3f(binormal.X, binormal.Z, binormal.Y),
				UEFVector3f(tangent.X, tangent.Z, tangent.Y),
				UEFVector3f(normal.X, normal.Z, normal.Y));

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

		auto proxy = mat->GetRenderProxy();

		const auto hasLight = shader->RequiredPredefinedMethodTypes.count(Effekseer::MaterialFile::RequiredPredefinedMethodType::Light) != 0;
		const auto hasLocalTime = shader->RequiredPredefinedMethodTypes.count(Effekseer::MaterialFile::RequiredPredefinedMethodType::LocalTime) != 0;

		if (shader->GetEffekseerMaterial()->UniformHashedNameToIndex.Num() > 0 ||
			shader->GetEffekseerMaterial()->TextureNameToIndex.Num() > 0 ||
			nativeMaterial->GetCustomData1Count() > 0 ||
			nativeMaterial->GetCustomData2Count() > 0 ||
			shader->GetEffekseerMaterial()->IsEffectScaleRequired ||
			hasLight ||
			hasLocalTime)
		{
			auto uniformOffset = shader->GetParameterGenerator()->PixelUserUniformOffset;
			auto buffer = static_cast<uint8_t*>(shader->GetPixelConstantBuffer()) + uniformOffset;

			const auto uniformCount = shader->GetEffekseerMaterial()->Uniforms.Num() + shader->GetEffekseerMaterial()->Gradients.Num() * 13;
			auto newProxy = new FFileMaterialRenderProxy(proxy, shader->GetEffekseerMaterial(), reinterpret_cast<float*>(buffer), uniformCount, false, m_renderState->GetActiveState().CullingType, reunderingUserData->Magnification);

			auto predefined = reinterpret_cast<float*>(static_cast<uint8_t*>(shader->GetPixelConstantBuffer()) + shader->GetParameterGenerator()->PixelPredefinedOffset);

			newProxy->LightDirection = FLinearColor::White;
			newProxy->LightColor = FLinearColor::White;
			newProxy->LightAmbientColor = FLinearColor::White;
			newProxy->LocalTime = predefined[3];

			newProxy->Textures = textures_;

			proxy = newProxy;
			m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
		}

		meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
		return;
	}

	Effekseer::RendererMaterialType MaterialType = shader->GetType();
	bool IsAdvanced = shader->IsAdvancedMaterial();

	if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
	{
		auto pixelBuffer = reinterpret_cast<EffekseerRenderer::PixelConstantBufferDistortion*>(shader->GetPixelConstantBuffer());
		auto intensity = pixelBuffer->DistortionIntencity[0];
		SetDistortionIntensity(intensity);
	}

	FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel(), 7);

	if (IsAdvanced == false)
	{
		if (MaterialType == Effekseer::RendererMaterialType::Default)
		{
			AddVertex<Effekseer::RendererMaterialType::Default, false>(meshBuilder, spriteCount, vertexOffset);
		}
		else if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			AddVertex<Effekseer::RendererMaterialType::BackDistortion, false>(meshBuilder, spriteCount, vertexOffset);
		}
		else if (MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			AddVertex<Effekseer::RendererMaterialType::Lighting, false>(meshBuilder, spriteCount, vertexOffset);
		}
	}
	else
	{
		if (MaterialType == Effekseer::RendererMaterialType::Default)
		{
			AddVertex<Effekseer::RendererMaterialType::Default, true>(meshBuilder, spriteCount, vertexOffset);
		}
		else if (MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			AddVertex<Effekseer::RendererMaterialType::BackDistortion, true>(meshBuilder, spriteCount, vertexOffset);
		}
		else if (MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			AddVertex<Effekseer::RendererMaterialType::Lighting, true>(meshBuilder, spriteCount, vertexOffset);
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

	auto proxy = mat->GetRenderProxy();

	if (shader->GetType() == Effekseer::RendererMaterialType::BackDistortion)
	{
		proxy = new FDistortionMaterialRenderProxy(proxy, m_distortionIntensity);
		m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);
	}

	meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Default, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	Vertex* vs = (Vertex*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		FDynamicMeshVertex DynamicVertex;
		DynamicVertex.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
		DynamicVertex.SetTangents(UEFVector3f(1, 0, 0), UEFVector3f(1, 1, 0), UEFVector3f(0, 0, 1));
		DynamicVertex.TextureCoordinate[0] = UEFVector2f(v.UV[0], v.UV[1]);
		DynamicVertex.TextureCoordinate[1] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[2] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[3] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[4] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[5] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[6] = UEFVector2f(0.0f);

		meshBuilder.AddVertex(DynamicVertex);
	}
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Default, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	AdvancedVertex* vs = (AdvancedVertex*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		FDynamicMeshVertex DynamicVertex;
		DynamicVertex.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
		DynamicVertex.SetTangents(UEFVector3f(1, 0, 0), UEFVector3f(1, 1, 0), UEFVector3f(0, 0, 1));
		DynamicVertex.TextureCoordinate[0] = UEFVector2f(v.UV[0], v.UV[1]);
		DynamicVertex.TextureCoordinate[1] = UEFVector2f(v.AlphaUV[0], v.AlphaUV[1]);
		DynamicVertex.TextureCoordinate[2] = UEFVector2f(v.UVDistortionUV[0], v.UVDistortionUV[1]);
		DynamicVertex.TextureCoordinate[3] = UEFVector2f(v.BlendUV[0], v.BlendUV[1]);
		DynamicVertex.TextureCoordinate[4] = UEFVector2f(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
		DynamicVertex.TextureCoordinate[5] = UEFVector2f(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
		DynamicVertex.TextureCoordinate[6] = UEFVector2f(v.FlipbookIndexAndNextRate, v.AlphaThreshold);

		meshBuilder.AddVertex(DynamicVertex);
	}
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::BackDistortion, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	VertexDistortion* vs = (VertexDistortion*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		auto normal = UnpackVector3DF(v.Normal);
		auto tangent = UnpackVector3DF(v.Tangent);
		Effekseer::Vector3D binormal;
		Effekseer::Vector3D::Cross(binormal, normal, tangent);

		FDynamicMeshVertex DynamicVertex;
		DynamicVertex.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
		DynamicVertex.SetTangents(
			UEFVector3f(binormal.X, binormal.Z, binormal.Y),
			UEFVector3f(tangent.X, tangent.Z, tangent.Y),
			UEFVector3f(normal.X, normal.Z, normal.Y));
		DynamicVertex.TextureCoordinate[0] = UEFVector2f(v.UV[0], v.UV[1]);
		DynamicVertex.TextureCoordinate[1] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[2] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[3] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[4] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[5] = UEFVector2f(0.0f);
		DynamicVertex.TextureCoordinate[6] = UEFVector2f(0.0f);

		meshBuilder.AddVertex(DynamicVertex);
	}
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::BackDistortion, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	AdvancedVertexDistortion* vs = (AdvancedVertexDistortion*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		auto normal = UnpackVector3DF(v.Normal);
		auto tangent = UnpackVector3DF(v.Tangent);
		Effekseer::Vector3D binormal;
		Effekseer::Vector3D::Cross(binormal, normal, tangent);

		FDynamicMeshVertex DynamicVertex;
		DynamicVertex.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		DynamicVertex.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);
		DynamicVertex.SetTangents(
			UEFVector3f(binormal.X, binormal.Z, binormal.Y),
			UEFVector3f(tangent.X, tangent.Z, tangent.Y),
			UEFVector3f(normal.X, normal.Z, normal.Y));
		DynamicVertex.TextureCoordinate[0] = UEFVector2f(v.UV[0], v.UV[1]);
		DynamicVertex.TextureCoordinate[1] = UEFVector2f(v.AlphaUV[0], v.AlphaUV[1]);
		DynamicVertex.TextureCoordinate[2] = UEFVector2f(v.UVDistortionUV[0], v.UVDistortionUV[1]);
		DynamicVertex.TextureCoordinate[3] = UEFVector2f(v.BlendUV[0], v.BlendUV[1]);
		DynamicVertex.TextureCoordinate[4] = UEFVector2f(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
		DynamicVertex.TextureCoordinate[5] = UEFVector2f(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
		DynamicVertex.TextureCoordinate[6] = UEFVector2f(v.FlipbookIndexAndNextRate, v.AlphaThreshold);

		meshBuilder.AddVertex(DynamicVertex);
	}
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Lighting, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	VertexLighting* vs = (VertexLighting*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		FDynamicMeshVertex meshVert;

		auto normal = UnpackVector3DF(v.Normal);
		auto tangent = UnpackVector3DF(v.Tangent);
		Effekseer::Vector3D binormal;
		Effekseer::Vector3D::Cross(binormal, normal, tangent);

		meshVert.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

		meshVert.TextureCoordinate[0] = UEFVector2f(v.UV1[0], v.UV1[1]);
		meshVert.TextureCoordinate[1] = UEFVector2f(v.UV2[0], v.UV2[1]);
		meshVert.TextureCoordinate[2] = UEFVector2f(0.0f);
		meshVert.TextureCoordinate[3] = UEFVector2f(0.0f);
		meshVert.TextureCoordinate[4] = UEFVector2f(0.0f);
		meshVert.TextureCoordinate[5] = UEFVector2f(0.0f);
		meshVert.TextureCoordinate[6] = UEFVector2f(0.0f);

		meshVert.SetTangents(
			UEFVector3f(binormal.X, binormal.Z, binormal.Y),
			UEFVector3f(tangent.X, tangent.Z, tangent.Y),
			UEFVector3f(normal.X, normal.Z, normal.Y));

		meshBuilder.AddVertex(meshVert);
	}
}

template <>
void RendererImplemented::AddVertex<Effekseer::RendererMaterialType::Lighting, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset)
{
	AdvancedVertexLighting* vs = (AdvancedVertexLighting*)m_vertexBuffer->GetResource();

	for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
	{
		auto& v = vs[vi];

		FDynamicMeshVertex meshVert;

		auto normal = UnpackVector3DF(v.Normal);
		auto tangent = UnpackVector3DF(v.Tangent);
		Effekseer::Vector3D binormal;
		Effekseer::Vector3D::Cross(binormal, normal, tangent);

		meshVert.Position = UEFVector3f(v.Pos.X, v.Pos.Z, v.Pos.Y);
		meshVert.Color = FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A);

		meshVert.TextureCoordinate[0] = UEFVector2f(v.UV1[0], v.UV1[1]);
		meshVert.TextureCoordinate[1] = UEFVector2f(v.AlphaUV[0], v.AlphaUV[1]);
		meshVert.TextureCoordinate[2] = UEFVector2f(v.UVDistortionUV[0], v.UVDistortionUV[1]);
		meshVert.TextureCoordinate[3] = UEFVector2f(v.BlendUV[0], v.BlendUV[1]);
		meshVert.TextureCoordinate[4] = UEFVector2f(v.BlendAlphaUV[0], v.BlendAlphaUV[1]);
		meshVert.TextureCoordinate[5] = UEFVector2f(v.BlendUVDistortionUV[0], v.BlendUVDistortionUV[1]);
		meshVert.TextureCoordinate[6] = UEFVector2f(v.FlipbookIndexAndNextRate, v.AlphaThreshold);

		meshVert.SetTangents(
			UEFVector3f(binormal.X, binormal.Z, binormal.Y),
			UEFVector3f(tangent.X, tangent.Z, tangent.Y),
			UEFVector3f(normal.X, normal.Z, normal.Y));

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
	if (model == nullptr)
		return;
	auto mdl = (EffekseerInternalModel*)model;

	if (mdl->UserData != nullptr)
	{
		if (mdl->UserData->Mesh == nullptr)
		{
			return;
		}

		if (mdl->UserData->Mesh->GetRenderData()->LODResources.Num() == 0)
		{
			return;
		}
	}
	else if (mdl->ProceduralData != nullptr)
	{
		if (!mdl->ProceduralData->GenerateRenderDataIfRequired(m_meshElementCollector->GetFeatureLevel()))
		{
			return;
		}
	}

	auto efkmdl = ((UEffekseerModel*)mdl->UserData);

	// Material
	auto reunderingUserData = static_cast<EffekseerRenderingUserData*>(GetImpl()->CurrentRenderingUserData.Get());
	auto mat = FindMaterial(reunderingUserData, m_currentShader);

	if (mat == nullptr)
	{
		return;
	}

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
			FVector(matOrigin.Values[3][0], matOrigin.Values[3][2], matOrigin.Values[3][1]));

		FLinearColor uv = FLinearColor(uvOrigin.X, uvOrigin.Y, uvOrigin.Width, uvOrigin.Height);

		FLinearColor alphaUV = FLinearColor(alphaUVOrigin.X, alphaUVOrigin.Y, alphaUVOrigin.Width, alphaUVOrigin.Height);
		FLinearColor uvDistortionUV = FLinearColor(uvDistortionUVOrigin.X, uvDistortionUVOrigin.Y, uvDistortionUVOrigin.Width, uvDistortionUVOrigin.Height);
		FLinearColor blendUV = FLinearColor(blendUVOrigin.X, blendUVOrigin.Y, blendUVOrigin.Width, blendUVOrigin.Height);
		FLinearColor blendAlphaUV = FLinearColor(blendAlphaUVOrigin.X, blendAlphaUVOrigin.Y, blendAlphaUVOrigin.Width, blendAlphaUVOrigin.Height);
		FLinearColor blendUVDistortionUV = FLinearColor(blendUVDistortionUVOrigin.X, blendUVDistortionUVOrigin.Y, blendUVDistortionUVOrigin.Width, blendUVDistortionUVOrigin.Height);

		FLinearColor color = FLinearColor(colorOrigin.R / 255.0f, colorOrigin.G / 255.0f, colorOrigin.B / 255.0f, colorOrigin.A / 255.0f);
		int frameTime = times[objectIndex] % mdl->GetFrameCount();

		if (mdl->UserData != nullptr)
		{
			const auto& lodResource = mdl->UserData->Mesh->GetRenderData()->LODResources[0];

			if (mdl->UserData->AnimationFaceOffsets.Num() > 0)
			{
				if (lodResource.Sections.Num() <= frameTime)
				{
					continue;
				}
			}
			else
			{
				frameTime = 0;
			}
		}

		{
			auto proxy = mat->GetRenderProxy();

			if (m_currentShader != nullptr && m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
			{
				const auto nativeMaterial = m_currentShader->GetEffekseerMaterial()->GetNativePtr();
				const auto uniformCount = m_currentShader->GetEffekseerMaterial()->Uniforms.Num() + m_currentShader->GetEffekseerMaterial()->Gradients.Num() * 13;

				auto uniformOffset = m_currentShader->GetParameterGenerator()->PixelUserUniformOffset;
				auto buffer = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + uniformOffset;
				auto newProxy = new FFileMaterialRenderProxy(
					proxy,
					m_currentShader->GetEffekseerMaterial(),
					reinterpret_cast<float*>(buffer),
					uniformCount,
					true,
					m_renderState->GetActiveState().CullingType,
					reunderingUserData->Magnification);

				auto predefined = reinterpret_cast<float*>(static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + m_currentShader->GetParameterGenerator()->PixelPredefinedOffset);

				newProxy->LightDirection = FLinearColor::White;
				newProxy->LightColor = FLinearColor::White;
				newProxy->LightAmbientColor = FLinearColor::White;
				newProxy->LocalTime = predefined[3];

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

			if (mdl->UserData != nullptr)
			{
				const auto& lodResource = mdl->UserData->Mesh->GetRenderData()->LODResources[0];
				auto& section = lodResource.Sections[frameTime];

				FMeshBatch& meshElement = m_meshElementCollector->AllocateMesh();
				auto& element = meshElement.Elements[0];

				FDynamicPrimitiveUniformBuffer& dynamicPrimitiveUniformBuffer = m_meshElementCollector->AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
				auto bounds = FBoxSphereBounds(EForceInit::ForceInit);
				dynamicPrimitiveUniformBuffer.Set(m_meshElementCollector->GetRHICommandList(), matLocalToWorld, matLocalToWorld, bounds, bounds, bounds, false, false, false);
#else
				dynamicPrimitiveUniformBuffer.Set(matLocalToWorld, matLocalToWorld, FBoxSphereBounds(EForceInit::ForceInit), FBoxSphereBounds(EForceInit::ForceInit), false, false, false, false);
#endif

				element.PrimitiveUniformBufferResource = &dynamicPrimitiveUniformBuffer.UniformBuffer;

				meshElement.MaterialRenderProxy = proxy;
				meshElement.VertexFactory = &mdl->UserData->Mesh->GetRenderData()->LODVertexFactories[0].VertexFactory;
				meshElement.Type = PT_TriangleList;

				if (efkmdl->AnimationFaceOffsets.Num() > 0)
				{
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

				element.MinVertexIndex = section.MinVertexIndex;
				element.MaxVertexIndex = section.MaxVertexIndex;
				meshElement.LODIndex = 0;

				element.MaxScreenSize = 0.0f;
				element.MinScreenSize = -1.0f;

				m_meshElementCollector->AddMesh(m_viewIndex, meshElement);
			}
			else if (mdl->ProceduralData != nullptr)
			{
				if (!mdl->ProceduralData->GenerateRenderDataIfRequired(m_meshElementCollector->GetFeatureLevel()))
				{
					return;
				}

				mdl->ProceduralData->Render(m_viewIndex, *m_meshElementCollector, matLocalToWorld, proxy);
			}
		}
	}
}

UMaterialInterface* RendererImplemented::FindMaterial(EffekseerRenderingUserData* userData, Shader* shader)
{
	if (shader->GetType() == Effekseer::RendererMaterialType::File)
	{
		auto alphaBlend = static_cast<EEffekseerAlphaBlendType>(m_renderState->GetActiveState().AlphaBlend);
		return shader->GetEffekseerMaterial()->FindMatrial(alphaBlend);
	}

	if (userData == nullptr)
	{
		return nullptr;
	}

	EffekseerEffectMaterialKey m = userData->Key;
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

void RendererImplemented::SetTextures(Shader* shader, ::Effekseer::Backend::TextureRef* textures, int32_t count)
{
	if (count > 0)
	{
		for (int32_t i = 0; i < count; i++)
		{
			auto texture = static_cast<EffekseerInternalTexture*>(textures[i].Get());
			if (texture != nullptr && texture != (void*)1)
			{
				textures_[i] = texture->UserData;
			}
			else
			{
				textures_[i] = nullptr;
			}
		}
	}
	else
	{
		textures_.fill(nullptr);
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

void RendererImplemented::SetNMaterials(const std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*>& nmaterials)
{
	m_nmaterials = nmaterials;
}

void RendererImplemented::SetMeshElementCollector(FMeshElementCollector* meshElementCollector)
{
	m_meshElementCollector = meshElementCollector;
}
} // namespace EffekseerRendererUE