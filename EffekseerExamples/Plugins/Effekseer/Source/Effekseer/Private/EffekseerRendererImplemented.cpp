#include "EffekseerPrivatePCH.h"
#include "EffekseerRendererImplemented.h"
#include "EffekseerRendererRenderState.h"
#include "EffekseerRendererVertexBuffer.h"
#include "EffekseerRendererIndexBuffer.h"
#include "EffekseerRendererShader.h"

#include "DynamicMeshBuilder.h"

namespace EffekseerRendererUE4
{
	SpriteRenderer::SpriteRenderer(RendererImplemented* renderer)
		: m_renderer(renderer)
	{
	
	}

	SpriteRenderer::~SpriteRenderer()
	{
	
	}

	SpriteRenderer* SpriteRenderer::Create(RendererImplemented* renderer)
	{
		return new SpriteRenderer(renderer);
	}

	void SpriteRenderer::BeginRendering(const efkSpriteNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_<RendererImplemented>(m_renderer, count, parameter);
	}

	void SpriteRenderer::Rendering(const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData)
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount()) return;
		Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void SpriteRenderer::EndRendering(const efkSpriteNodeParam& parameter, void* userData)
	{
		if (m_ringBufferData == NULL) return;

		if (m_spriteCount == 0) return;

		EndRendering_<RendererImplemented, Shader, void*, Vertex>(m_renderer, parameter);
		//EndRendering_<RendererImplemented, Shader, IDirect3DTexture9*, Vertex>(m_renderer, parameter);
	}

	RibbonRenderer::RibbonRenderer(RendererImplemented* renderer)
		: m_renderer(renderer)
	{
	}

	RibbonRenderer::~RibbonRenderer()
	{
	}

	RibbonRenderer* RibbonRenderer::Create(RendererImplemented* renderer)
	{
		assert(renderer != NULL);

		return new RibbonRenderer(renderer);
	}

	void RibbonRenderer::BeginRendering(const efkRibbonNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
	}

	void RibbonRenderer::Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData)
	{
		Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void RibbonRenderer::EndRendering(const efkRibbonNodeParam& parameter, void* userData)
	{
		if (m_ringBufferData == NULL) return;

		if (m_ribbonCount <= 1) return;

		EndRendering_<RendererImplemented, void*, Vertex>(m_renderer, parameter);
		//EndRendering_<RendererImplemented, GLuint, Vertex>(m_renderer, parameter);
	}

	RingRenderer::RingRenderer(RendererImplemented* renderer)
		: m_renderer(renderer)

	{
	}

	RingRenderer::~RingRenderer()
	{
	}

	RingRenderer* RingRenderer::Create(RendererImplemented* renderer)
	{
		assert(renderer != NULL);

		return new RingRenderer(renderer);
	}

	void RingRenderer::BeginRendering(const efkRingNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
	}

	void RingRenderer::Rendering(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData)
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount()) return;
		Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void RingRenderer::EndRendering(const efkRingNodeParam& parameter, void* userData)
	{
		if (m_spriteCount == 0) return;

		EndRendering_<RendererImplemented, Shader, void*, Vertex>(m_renderer, parameter);
	}

	TrackRenderer::TrackRenderer(RendererImplemented* renderer)
		: m_renderer(renderer)
	{
	}

	TrackRenderer::~TrackRenderer()
	{
	}

	TrackRenderer* TrackRenderer::Create(RendererImplemented* renderer)
	{
		assert(renderer != NULL);

		return new TrackRenderer(renderer);
	}

	void TrackRenderer::BeginRendering(const efkTrackNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_<Vertex>(m_renderer, parameter, count, userData);
	}

	void TrackRenderer::Rendering(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData)
	{
		Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void TrackRenderer::EndRendering(const efkTrackNodeParam& parameter, void* userData)
	{
		if (m_ringBufferData == NULL) return;

		if (m_ribbonCount <= 1) return;

		EndRendering_<RendererImplemented, void*, Vertex>(m_renderer, parameter);
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
	}
	
	RendererImplemented::~RendererImplemented()
	{
		ES_SAFE_DELETE(m_renderState);
		ES_SAFE_DELETE(m_stanShader);
		ES_SAFE_DELETE(m_standardRenderer);
		ES_SAFE_DELETE(m_vertexBuffer);
	}

	bool RendererImplemented::Initialize(int32_t squareMaxCount)
	{
		m_squareMaxCount = squareMaxCount;
		m_renderState = new RenderState();
		m_vertexBuffer = new VertexBuffer(sizeof(Vertex) * m_squareMaxCount * 4, true);
		m_stanShader = new Shader();

		m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, void*, Vertex, VertexDistortion>(
			this,
			m_stanShader,
			m_stanShader,
			m_stanShader,
			m_stanShader);


		return true;
	}

	void RendererImplemented::Destory()
	{
		Release();
	}

	void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
	{
		// TODO
	}

	bool RendererImplemented::BeginRendering()
	{
		::Effekseer::Matrix44::Mul(m_cameraProj, m_camera, m_proj);

//		// ステートを保存する
//		if (m_restorationOfStates)
//		{
//			m_originalState.blend = glIsEnabled(GL_BLEND);
//			m_originalState.cullFace = glIsEnabled(GL_CULL_FACE);
//			m_originalState.depthTest = glIsEnabled(GL_DEPTH_TEST);
//#if !defined(__EFFEKSEER_RENDERER_GL3__) && \
//	!defined(__EFFEKSEER_RENDERER_GLES3__) && \
//	!defined(__EFFEKSEER_RENDERER_GLES2__) && \
//	!defined(EMSCRIPTEN)
//			m_originalState.texture = glIsEnabled(GL_TEXTURE_2D);
//#endif
//			glGetBooleanv(GL_DEPTH_WRITEMASK, &m_originalState.depthWrite);
//			glGetIntegerv(GL_DEPTH_FUNC, &m_originalState.depthFunc);
//			glGetIntegerv(GL_CULL_FACE_MODE, &m_originalState.cullFaceMode);
//			glGetIntegerv(GL_BLEND_SRC_RGB, &m_originalState.blendSrc);
//			glGetIntegerv(GL_BLEND_DST_RGB, &m_originalState.blendDst);
//			glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);
//		}
//
//		glDepthFunc(GL_LEQUAL);
//		glEnable(GL_BLEND);
//		glDisable(GL_CULL_FACE);
//
//		m_renderState->GetActiveState().Reset();
//		m_renderState->Update(true);
//		m_currentTextures.clear();

		// レンダラーリセット
		m_standardRenderer->ResetAndRenderingIfRequired();

		//GLCheckError();

		return true;
	}

	bool RendererImplemented::EndRendering()
	{
//		GLCheckError();

		// レンダラーリセット
		m_standardRenderer->ResetAndRenderingIfRequired();

//		// ステートを復元する
//		if (m_restorationOfStates)
//		{
//			if (m_originalState.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
//			if (m_originalState.cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
//			if (m_originalState.depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
//
//#if !defined(__EFFEKSEER_RENDERER_GL3__) && \
//	!defined(__EFFEKSEER_RENDERER_GLES3__) && \
//	!defined(__EFFEKSEER_RENDERER_GLES2__) && \
//	!defined(EMSCRIPTEN)
//			if (m_originalState.texture) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
//#endif
//
//			glDepthFunc(m_originalState.depthFunc);
//			glDepthMask(m_originalState.depthWrite);
//			glCullFace(m_originalState.cullFaceMode);
//			glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
//			GLExt::glBlendEquation(m_originalState.blendEquation);
//
//#if defined(__EFFEKSEER_RENDERER_GL3__) || defined(__EFFEKSEER_RENDERER_GLES3__)
//			for (int32_t i = 0; i < 4; i++)
//			{
//				GLExt::glBindSampler(i, 0);
//			}
//#endif
//		}
//
//		GLCheckError();

		return true;
	}

	const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
	{
		return m_lightDirection;
	}

	void RendererImplemented::SetLightDirection(::Effekseer::Vector3D& direction)
	{
		m_lightDirection = direction;
	}

	const ::Effekseer::Color& RendererImplemented::GetLightColor() const
	{
		return m_lightColor;
	}

	void RendererImplemented::SetLightColor(::Effekseer::Color& color)
	{
		m_lightColor = color;
	}

	const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
	{
		return m_lightAmbient;
	}

	void RendererImplemented::SetLightAmbientColor(::Effekseer::Color& color)
	{
		m_lightAmbient = color;
	}

	int32_t RendererImplemented::GetSquareMaxCount() const
	{
		return m_squareMaxCount;
	}

	const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
	{
		return m_proj;
	}

	void RendererImplemented::SetProjectionMatrix(const ::Effekseer::Matrix44& mat)
	{
		m_proj = mat;
	}

	const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
	{
		return m_camera;
	}

	void RendererImplemented::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
	{
		m_camera = mat;
	}

	::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
	{
		return m_cameraProj;
	}

	::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
	{
		return SpriteRenderer::Create(this);
	}

	::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
	{
		return RibbonRenderer::Create(this);
	}

	::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
	{
		return RingRenderer::Create(this);
	}

	::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
	{
		// TODO
		return nullptr;
	}

	::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
	{
		return TrackRenderer::Create(this);
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

	void* RendererImplemented::GetBackground()
	{
		// TODO
		return nullptr;
	}

	VertexBuffer* RendererImplemented::GetVertexBuffer()
	{
		// Todo 様々な状態への対応
		return m_vertexBuffer;
	}

	IndexBuffer* RendererImplemented::GetIndexBuffer()
	{
		// TODO
		return nullptr;
	}

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, void*, Vertex, VertexDistortion>* RendererImplemented::GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
	{
		return m_renderState;
	}

	void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
	{
		// TODO
	}

	void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		// TODO
	}

	void RendererImplemented::SetLayout(Shader* shader)
	{
		// TODO
	}

	void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
	{
		// 1つのリング判定
		auto stanMat = ((Effekseer::Matrix44*)m_stanShader->GetVertexConstantBuffer())[0];
		auto cameraMat = m_camera;
		Effekseer::Matrix44 ringMat;

		bool isSingleRing = false;

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
	Exit:;

		if (isSingleRing)
		{
			Effekseer::Matrix44 inv;
			Effekseer::Matrix44::Mul(ringMat, stanMat, Effekseer::Matrix44::Inverse(inv, cameraMat));
		}

		//auto triangles = vertexOffset / 4 * 2;
		//glDrawElements(GL_TRIANGLES, spriteCount * 6, GL_UNSIGNED_SHORT, (void*)(triangles * 3 * sizeof(GLushort)));

		// TODO Vertex決めうち
		Vertex* vs = (Vertex*)m_vertexBuffer->GetResource();

		FDynamicMeshBuilder meshBuilder;

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

			//char temp[200];
			//sprintf_s<200>(temp, "%f,%f,%f,%f,%f,%d,%d,%d,%d", v.Pos.X, v.Pos.Y, v.Pos.Z, v.UV[0], v.UV[1], (int)v.Col.R, (int)v.Col.G, (int)v.Col.B, (int)v.Col.A);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, temp);

			meshBuilder.AddVertex(FVector(v.Pos.X, v.Pos.Z, v.Pos.Y), FVector2D(v.UV[0], v.UV[1]), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor(v.Col.R, v.Col.G, v.Col.B, v.Col.A));
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
		//meshBuilder.AddVertex(FVector(0, 0, 0), FVector2D(0, 0), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);
		//meshBuilder.AddVertex(FVector(0, 100, 0), FVector2D(1, 0), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);
		//meshBuilder.AddVertex(FVector(100, 0, 0), FVector2D(0, 1), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);
		//
		//meshBuilder.AddTriangle(0, 1, 2);

		int32_t blendIndex = (int32_t)m_renderState->GetActiveState().AlphaBlend;

		UMaterialInstanceDynamic* mat = nullptr;
		if (m_materials[blendIndex]->Contains((UTexture2D*)m_textures[0]))
		{
			mat = (*m_materials[blendIndex])[(UTexture2D*)m_textures[0]];
		}

		if (mat != nullptr)
		{
			auto proxy = mat->GetRenderProxy(false);
			meshBuilder.GetMesh(m_localToWorld, proxy, SDPG_World, false, false, m_viewIndex, *m_meshElementCollector);
		}
	}

	void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
	{
		// TODO
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "DrawPolygon");
	}

	void RendererImplemented::BeginShader(Shader* shader)
	{
		// TODO
	}

	void RendererImplemented::EndShader(Shader* shader)
	{
		// TODO
	}

	void RendererImplemented::SetTextures(Shader* shader, void** textures, int32_t count)
	{
		// TODO 幅広い対応
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

	void RendererImplemented::SetMeshElementCollector(FMeshElementCollector* meshElementCollector)
	{
		m_meshElementCollector = meshElementCollector;
	}
}