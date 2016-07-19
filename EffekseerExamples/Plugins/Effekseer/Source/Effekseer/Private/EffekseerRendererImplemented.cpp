#include "EffekseerPrivatePCH.h"
#include "EffekseerRendererImplemented.h"
#include "EffekseerRendererRenderState.h"
#include "EffekseerRendererVertexBuffer.h"
#include "EffekseerRendererIndexBuffer.h"
#include "EffekseerRendererShader.h"

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

	RendererImplemented* RendererImplemented::Create()
	{
		return new RendererImplemented();
	}

	RendererImplemented::RendererImplemented()
	{

	}
	
	RendererImplemented::~RendererImplemented()
	{
		ES_SAFE_DELETE(m_renderState);
	}

	bool RendererImplemented::Initialize()
	{
		m_renderState = new RenderState();
		return true;
	}

	int RendererImplemented::AddRef()
	{
		m_reference++;
		return m_reference;
	}

	int RendererImplemented::Release()
	{
		m_reference--;
		int count = m_reference;
		if (count == 0)
		{
			delete this;
		}
		return count;
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
		return false;
	}

	bool RendererImplemented::EndRendering()
	{
		return false;
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
		return nullptr;
	}

	::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
	{
		return nullptr;
	}

	::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
	{
		return nullptr;
	}

	::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
	{
		return nullptr;
	}

	::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
	{
		return nullptr;
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
		// TODO
		return nullptr;
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
		// TODO
	}

	void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
	{
		// TODO
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
		// TODO
	}
}