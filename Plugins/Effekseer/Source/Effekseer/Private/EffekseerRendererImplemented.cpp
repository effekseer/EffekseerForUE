#include "EffekseerRendererImplemented.h"
#include "EffekseerRendererRenderState.h"
#include "EffekseerRendererVertexBuffer.h"
#include "EffekseerRendererIndexBuffer.h"
#include "EffekseerRendererShader.h"

#include "Runtime/Launch/Resources/Version.h"
#include "DynamicMeshBuilder.h"
#include "EffekseerInternalModel.h"
#include "Runtime/Engine/Public/StaticMeshResources.h"
#include "Runtime/Core/Public/Math/Color.h"
#include "Runtime/Engine/Public/MaterialShared.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

namespace EffekseerRendererUE4
{

	class FFileMaterialRenderProxy : public FMaterialRenderProxy
	{
		TArray<float> uniformBuffer_;
		const UEffekseerMaterial* const effekseerMaterial_;
		
	public:

		const FMaterialRenderProxy* const Parent;

		FFileMaterialRenderProxy(const FMaterialRenderProxy* InParent, const UEffekseerMaterial* effekseerMaterial, float* uniformBufferPtr, int32_t uniformCount)
			: Parent(InParent)
			, effekseerMaterial_(effekseerMaterial)
		{
			uniformBuffer_.Reserve(uniformCount);

			for (int32_t i = 0; i < uniformCount; i++)
			{
				uniformBuffer_.Add(uniformBufferPtr[i]);
			}
		}

#if  ENGINE_MINOR_VERSION < 22
		void GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const override;
#else
		const FMaterial& GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const override;
#endif
		virtual bool GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
	};

#if ENGINE_MINOR_VERSION < 22
void FFileMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const
{
	OutMaterial = Parent->GetMaterial(InFeatureLevel);
}
#else
	const FMaterial& FFileMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const
	{
		return *(Parent->GetMaterial(InFeatureLevel));
	}
#endif

	bool FFileMaterialRenderProxy::GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		const auto found = effekseerMaterial_->UniformNameToIndex.Find(ParameterInfo.Name.ToString());
		if (found != nullptr)
		{
			OutValue->R = uniformBuffer_[(*found) * 4 + 0];
			OutValue->G = uniformBuffer_[(*found) * 4 + 1];
			OutValue->B = uniformBuffer_[(*found) * 4 + 2];
			OutValue->A = uniformBuffer_[(*found) * 4 + 3];
			return true;
		}

		return Parent->GetVectorValue(ParameterInfo, OutValue, Context);
	}

	bool FFileMaterialRenderProxy::GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		const auto found = effekseerMaterial_->UniformNameToIndex.Find(ParameterInfo.Name.ToString());
		if (found != nullptr)
		{
			*OutValue = uniformBuffer_[(*found) * 4 + 0];
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo, OutValue, Context);
	}

	bool FFileMaterialRenderProxy::GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
	}

	class FDistortionMaterialRenderProxy : public FMaterialRenderProxy
	{
	public:

		const FMaterialRenderProxy* const Parent;
		float			distortionIntensity;

		/** Initialization constructor. */
		FDistortionMaterialRenderProxy(const FMaterialRenderProxy* InParent, float distortionIntensity) :
			Parent(InParent),
			distortionIntensity(distortionIntensity)
		{}

		// FMaterialRenderProxy interface.
#if ENGINE_MINOR_VERSION < 20
		virtual const class FMaterial* GetMaterial(ERHIFeatureLevel::Type InFeatureLevel) const override;
#elif  ENGINE_MINOR_VERSION < 22
		virtual void GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const override;
#else
		virtual const FMaterial& GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const override;
#endif

#if ENGINE_MINOR_VERSION < 19
		virtual bool GetVectorValue(const FName ParameterName, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetScalarValue(const FName ParameterName, float* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetTextureValue(const FName ParameterName, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
#else
		virtual bool GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
#endif
	};

#if ENGINE_MINOR_VERSION < 20
	const FMaterial* FDistortionMaterialRenderProxy::GetMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return Parent->GetMaterial(InFeatureLevel);
	}
#elif ENGINE_MINOR_VERSION < 22
	void FDistortionMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const
	{
		OutMaterial = Parent->GetMaterial(InFeatureLevel);
	}
#else
	const FMaterial& FDistortionMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const
	{
		return *(Parent->GetMaterial(InFeatureLevel));
	}
#endif

#if ENGINE_MINOR_VERSION < 19
	bool FDistortionMaterialRenderProxy::GetVectorValue(const FName ParameterName, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetVectorValue(ParameterName, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetScalarValue(const FName ParameterName, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterName == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterName, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetTextureValue(const FName ParameterName, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterName, OutValue, Context);
	}
#else
	bool FDistortionMaterialRenderProxy::GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetVectorValue(ParameterInfo, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo, OutValue, Context);
	}

	bool FDistortionMaterialRenderProxy::GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
	}
#endif

	class FModelMaterialRenderProxy : public FMaterialRenderProxy
	{
	public:

		const FMaterialRenderProxy* const Parent;
		FLinearColor	uv;
		FLinearColor	color;
		float			distortionIntensity;

		/** Initialization constructor. */
		FModelMaterialRenderProxy(const FMaterialRenderProxy* InParent, FLinearColor uv, FLinearColor color, float distortionIntensity) :
			Parent(InParent),
			uv(uv),
			color(color),
			distortionIntensity(distortionIntensity)
		{}

#if ENGINE_MINOR_VERSION < 20
		// FMaterialRenderProxy interface.
		virtual const class FMaterial* GetMaterial(ERHIFeatureLevel::Type InFeatureLevel) const;
#elif ENGINE_MINOR_VERSION < 22
		virtual void GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const override;
#else
		virtual const FMaterial& GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const override;
#endif

#if ENGINE_MINOR_VERSION < 19
		virtual bool GetVectorValue(const FName ParameterName, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetScalarValue(const FName ParameterName, float* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetTextureValue(const FName ParameterName, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
#else
		virtual bool GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const override;
		virtual bool GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const override;
#endif
	};

#if ENGINE_MINOR_VERSION < 20
	const FMaterial* FModelMaterialRenderProxy::GetMaterial(ERHIFeatureLevel::Type InFeatureLevel) const
	{
		return Parent->GetMaterial(InFeatureLevel);
	}
#elif ENGINE_MINOR_VERSION < 22
	void FModelMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutMaterialRenderProxy, const class FMaterial*& OutMaterial) const
	{
		OutMaterial = Parent->GetMaterial(InFeatureLevel);
	}
#else
	const FMaterial& FModelMaterialRenderProxy::GetMaterialWithFallback(ERHIFeatureLevel::Type InFeatureLevel, const FMaterialRenderProxy*& OutFallbackMaterialRenderProxy) const
	{
		return *(Parent->GetMaterial(InFeatureLevel));
	}
#endif

#if ENGINE_MINOR_VERSION < 19
	bool FModelMaterialRenderProxy::GetVectorValue(const FName ParameterName, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterName == FName(TEXT("UserUV")))
		{
			*OutValue = uv;
			return true;
		}
		
		if (ParameterName == FName(TEXT("UserColor")))
		{
			*OutValue = color;
			return true;
		}

		return Parent->GetVectorValue(ParameterName, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetScalarValue(const FName ParameterName, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterName == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterName, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetTextureValue(const FName ParameterName, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterName, OutValue, Context);
	}
#else
	bool FModelMaterialRenderProxy::GetVectorValue(const FMaterialParameterInfo& ParameterInfo, FLinearColor* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("UserUV")))
		{
			*OutValue = uv;
			return true;
		}

		if (ParameterInfo.Name == FName(TEXT("UserColor")))
		{
			*OutValue = color;
			return true;
		}

		return Parent->GetVectorValue(ParameterInfo, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetScalarValue(const FMaterialParameterInfo& ParameterInfo, float* OutValue, const FMaterialRenderContext& Context) const
	{
		if (ParameterInfo.Name == FName(TEXT("DistortionIntensity")))
		{
			*OutValue = distortionIntensity;
			return true;
		}

		return Parent->GetScalarValue(ParameterInfo, OutValue, Context);
	}

	bool FModelMaterialRenderProxy::GetTextureValue(const FMaterialParameterInfo& ParameterInfo, const UTexture** OutValue, const FMaterialRenderContext& Context) const
	{
		return Parent->GetTextureValue(ParameterInfo, OutValue, Context);
	}
#endif

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

		::EffekseerRenderer::RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = parameter.ZTest;
		state.DepthWrite = parameter.ZWrite;
		state.AlphaBlend = parameter.BasicParameterPtr->AlphaBlend;
		state.CullingType = parameter.Culling;
		
		m_renderer->GetRenderState()->Update(false);

		SortTemporaryValues(m_renderer, parameter);

		Shader* shader = nullptr;
		if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::File)
		{
			// Not supported yet
			assert(0);
		}
		else
		{
			shader = m_renderer->GetShader(true, param.BasicParameterPtr->MaterialType);
		}

		m_renderer->BeginShader(shader);

		Effekseer::TextureData* textures[1];

		if (parameter.BasicParameterPtr->Texture1Index >= 0)
		{
			textures[0] = parameter.EffectPointer->GetColorImage(parameter.BasicParameterPtr->Texture1Index);
		}
		else
		{
			textures[0] = nullptr;
		}

		m_renderer->SetTextures(nullptr, textures, 1);

		m_renderer->DrawModel(model, m_matrixes, m_uv, m_colors, m_times);

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

		m_textures[0] = nullptr;
		m_textures[1] = nullptr;
		m_textures[2] = nullptr;
		m_textures[3] = nullptr;
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
		m_vertexBuffer = new VertexBuffer(sizeof(Vertex) * m_squareMaxCount * 4, true);
		stanShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Default));
		backDistortedShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::BackDistortion));
		lightingShader_ = std::unique_ptr<Shader>(new Shader(Effekseer::RendererMaterialType::Lighting));

		m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(
			this,
			nullptr,
			nullptr);

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
		::Effekseer::Matrix44::Mul(GetCameraProjectionMatrix(), GetCameraMatrix(), GetProjectionMatrix());

		// レンダラーリセット
		m_standardRenderer->ResetAndRenderingIfRequired();

		//GLCheckError();

		return true;
	}

	bool RendererImplemented::EndRendering()
	{
		// レンダラーリセット
		m_standardRenderer->ResetAndRenderingIfRequired();

		return true;
	}

	const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
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
		return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
	}

	::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
	{
		return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
	}

	::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
	{
		return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
	}

	::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
	{
		return ModelRenderer::Create(this);
	}

	::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
	{
		return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
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

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* RendererImplemented::GetStandardRenderer()
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
			assert(!m_currentShader->GetEffekseerMaterial()->GetNativePtr()->GetIsSimpleVertex());

			auto* vs = (EffekseerRenderer::DynamicVertex*)m_vertexBuffer->GetResource();

#if ENGINE_MINOR_VERSION < 19
			FDynamicMeshBuilder meshBuilder;
#else
			int32_t customDataCount = 0;
			FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel(), 1 + customDataCount);
#endif
			for (int32_t vi = vertexOffset; vi < vertexOffset + spriteCount * 4; vi++)
			{
				auto& v = vs[vi];

				auto normal = UnpackVector3DF(v.Normal);
				auto tangent = UnpackVector3DF(v.Tangent);
				Effekseer::Vector3D binormal;
				Effekseer::Vector3D::Cross(binormal, normal, tangent);

				meshBuilder.AddVertex(FVector(v.Pos.X, v.Pos.Z, v.Pos.Y), FVector2D(v.UV[0], v.UV[1]),
					FVector(binormal.X, binormal.Z, binormal.Y),
					FVector(tangent.X, tangent.Z, tangent.Y),
					FVector(normal.X, normal.Z, normal.Y),
					FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A));
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

			if (m_currentShader->GetEffekseerMaterial()->UniformNameToIndex.Num() > 0)
			{
				auto uniformOffset = m_currentShader->GetParameterGenerator()->PixelUserUniformOffset;
				auto buffer = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + uniformOffset;
				proxy = new FFileMaterialRenderProxy(proxy, m_currentShader->GetEffekseerMaterial(), reinterpret_cast<float*>(buffer), m_currentShader->GetEffekseerMaterial()->Uniforms.Num());
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

		if (m_currentShader->GetType() == Effekseer::RendererMaterialType::BackDistortion)
		{
			auto intensity = ((float*)m_currentShader->GetPixelConstantBuffer())[0];
			SetDistortionIntensity(intensity);

			VertexDistortion* vs = (VertexDistortion*)m_vertexBuffer->GetResource();

#if ENGINE_MINOR_VERSION < 19
			FDynamicMeshBuilder meshBuilder;
#else
			FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel());
#endif
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

				meshBuilder.AddVertex(FVector(v.Pos.X, v.Pos.Z, v.Pos.Y), FVector2D(v.UV[0], v.UV[1]), 
					FVector(v.Binormal.X, v.Binormal.Z, v.Binormal.Y),
					FVector(v.Tangent.X, v.Tangent.Z, v.Tangent.Y),
					FVector(normal.X, normal.Z, normal.Y),
					FColor(v.Col[0], v.Col[1], v.Col[2], v.Col[3]));
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
			proxy = new FDistortionMaterialRenderProxy(proxy, m_distortionIntensity);
			m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);

			meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
		}
		else
		{
			Vertex* vs = (Vertex*)m_vertexBuffer->GetResource();

#if ENGINE_MINOR_VERSION < 19
			FDynamicMeshBuilder meshBuilder;
#else
			FDynamicMeshBuilder meshBuilder(m_meshElementCollector->GetFeatureLevel());
#endif

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

				meshBuilder.AddVertex(FVector(v.Pos.X, v.Pos.Z, v.Pos.Y), FVector2D(v.UV[0], v.UV[1]), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor(v.Col[0], v.Col[1], v.Col[2], v.Col[3]));
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

			meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
		}
	}

	void RendererImplemented::DrawModel(void* model, std::vector<Effekseer::Matrix44>& matrixes, std::vector<Effekseer::RectF>& uvs, std::vector<Effekseer::Color>& colors, std::vector<int32_t>& times)
	{
		// TODO support material file
		if (m_currentShader != nullptr && m_currentShader->GetType() == Effekseer::RendererMaterialType::File)
		{
			UE_LOG(LogTemp, Warning, TEXT("Model with material file is not supported yet."));
			return;
		}

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
			auto& colorOrigin = colors[objectIndex];

			FMatrix matLocalToWorld = FMatrix(
				FVector(matOrigin.Values[0][0], matOrigin.Values[0][2], matOrigin.Values[0][1]),
				FVector(matOrigin.Values[2][0], matOrigin.Values[2][2], matOrigin.Values[2][1]),
				FVector(matOrigin.Values[1][0], matOrigin.Values[1][2], matOrigin.Values[1][1]),
				FVector(matOrigin.Values[3][0], matOrigin.Values[3][2], matOrigin.Values[3][1])
			);

			FLinearColor uv = FLinearColor(uvOrigin.X, uvOrigin.Y, uvOrigin.Width, uvOrigin.Height);
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

				proxy = new FModelMaterialRenderProxy(proxy, uv, color, m_distortionIntensity);
				m_meshElementCollector->RegisterOneFrameMaterialProxy(proxy);

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
			return m_currentShader->GetEffekseerMaterial()->Material;
		}

		EffekseerEffectMaterial m;

		auto textureData = (Effekseer::TextureData*)m_textures[0];
		if (textureData != nullptr)
		{
			m.Texture = (UTexture2D*)textureData->UserPtr;
		}
		else
		{
			m.Texture = nullptr;
		}

		m.AlphaBlend = (EEffekseerAlphaBlendType)m_renderState->GetActiveState().AlphaBlend;
		m.IsDepthTestDisabled = !m_renderState->GetActiveState().DepthTest;
		m.IsLighting = m_currentShader->GetType() == Effekseer::RendererMaterialType::Lighting;
		m.IsDistorted = m_currentShader->GetType() == Effekseer::RendererMaterialType::BackDistortion;

		UMaterialInstanceDynamic* mat = nullptr;

		auto it = m_nmaterials.find(m);

		if (it != m_nmaterials.end())
		{
			mat = it->second;
		}

		return mat;
	}

	Shader* RendererImplemented::GetShader(bool useTexture, ::Effekseer::RendererMaterialType materialType) const
	{
		if (materialType == ::Effekseer::RendererMaterialType::BackDistortion)
		{
			return backDistortedShader_.get();
		}
		else if (materialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			return lightingShader_.get();
		}
		else if (materialType == ::Effekseer::RendererMaterialType::Default)
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
		// TODO Normal map
		if (count > 0)
		{
			m_textures[0] = textures[0];
		}
		else
		{
			m_textures[0] = nullptr;
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