
#pragma once

#include "CoreMinimal.h"
#include "EffekseerRenderingUserData.h"
#include <EffekseerEffect.h>
#include <EffekseerRenderer.ModelRendererBase.h>
#include <EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer.ShaderBase.h>
#include <EffekseerRenderer.StandardRenderer.h>
#include <map>
#include <memory>

namespace EffekseerRendererUE
{
class RendererImplemented;
class RenderState;
class VertexBuffer;
class IndexBuffer;
class Shader;

using Vertex = EffekseerRenderer::SimpleVertex;
using VertexDistortion = EffekseerRenderer::LightingVertex;
using VertexLighting = EffekseerRenderer::LightingVertex;

using AdvancedVertex = EffekseerRenderer::AdvancedSimpleVertex;
using AdvancedVertexDistortion = EffekseerRenderer::AdvancedLightingVertex;
using AdvancedVertexLighting = EffekseerRenderer::AdvancedLightingVertex; // TODO change it in 1.6

typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class ModelRenderer
	: public ::EffekseerRenderer::ModelRendererBase
{
private:
	RendererImplemented* m_renderer;

public:
	ModelRenderer(RendererImplemented* renderer);
	virtual ~ModelRenderer();

	static Effekseer::ModelRendererRef Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);

	void Rendering(const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData);
};

class RendererImplemented
	: public ::EffekseerRenderer::Renderer,
	  public ::Effekseer::ReferenceObject
{
protected:
	::Effekseer::Vector3D m_lightDirection;
	::Effekseer::Color m_lightColor;
	::Effekseer::Color m_lightAmbient;
	int32_t m_squareMaxCount;

	VertexBuffer* m_vertexBuffer = nullptr;

	std::unique_ptr<Shader> stanShader_;
	std::unique_ptr<Shader> backDistortedShader_;
	std::unique_ptr<Shader> lightingShader_;

	std::unique_ptr<Shader> stanShaderAd_;
	std::unique_ptr<Shader> backDistortedShaderAd_;
	std::unique_ptr<Shader> lightingShaderAd_;

	Shader* m_currentShader = nullptr;
	RenderState* m_renderState = nullptr;

	std::array<void*, Effekseer::TextureSlotMax> textures_;

	FMatrix m_localToWorld;
	int32_t m_viewIndex = 0;
	TMap<UTexture2D*, UMaterialInstanceDynamic*>* m_materials[6];
	FMeshElementCollector* m_meshElementCollector = nullptr;
	std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*> m_nmaterials;

	float m_distortionIntensity = 0.0f;

	::Effekseer::Backend::TextureRef backgroundTexture_;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer = nullptr;
	EEffekseerColorSpaceType colorSpace_{};

public:
	static Effekseer::RefPtr<RendererImplemented> Create();

	RendererImplemented();
	virtual ~RendererImplemented();

	void OnLostDevice() override
	{
	}
	void OnResetDevice() override
	{
	}

	bool Initialize(int32_t squareMaxCount, EEffekseerColorSpaceType colorSpace);

	/**
	@brief	�X�e�[�g�𕜋A���邩�ǂ����̃t���O��ݒ肷��B
	*/
	void SetRestorationOfStatesFlag(bool flag) override;

	/**
	@brief	�`����J�n���鎞�Ɏ��s����B
	*/
	bool BeginRendering() override;

	/**
	@brief	�`����I�����鎞�Ɏ��s����B
	*/
	bool EndRendering() override;

	/**
	@brief	���C�g�̕������擾����B
	*/
	::Effekseer::Vector3D GetLightDirection() const override;

	/**
	@brief	���C�g�̕�����ݒ肷��B
	*/
	void SetLightDirection(const ::Effekseer::Vector3D& direction) override;

	/**
	@brief	���C�g�̐F���擾����B
	*/
	const ::Effekseer::Color& GetLightColor() const override;

	/**
	@brief	���C�g�̐F��ݒ肷��B
	*/
	void SetLightColor(const ::Effekseer::Color& color) override;

	/**
	@brief	���C�g�̊����̐F���擾����B
	*/
	const ::Effekseer::Color& GetLightAmbientColor() const override;

	/**
	@brief	���C�g�̊����̐F��ݒ肷��B
	*/
	void SetLightAmbientColor(const ::Effekseer::Color& color) override;

	/**
	@brief	�ő�`��X�v���C�g�����擾����B
	*/
	int32_t GetSquareMaxCount() const override;

	/**
	@brief	�X�v���C�g�����_���[�𐶐�����B
	*/
	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;

	/**
	@brief	���{�������_���[�𐶐�����B
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;

	/**
	@brief	�����O�����_���[�𐶐�����B
	*/
	::Effekseer::RingRendererRef CreateRingRenderer() override;

	/**
	@brief	���f�������_���[�𐶐�����B
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer() override;

	/**
	@brief	�O�Ճ����_���[�𐶐�����B
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer() override;

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override
	{
		return nullptr;
	}

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override
	{
		return nullptr;
	}

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override
	{
		return nullptr;
	}

	/**
	@brief	�����_�[�X�e�[�g�������I�Ƀ��Z�b�g����B
	*/
	void ResetRenderState() override;

	/**
	@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N���擾����B
	*/
	::EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	/**
	@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N��ݒ肷��B
	*/
	void SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback) override;

	const ::Effekseer::Backend::TextureRef& GetBackground() override;

	int32_t GetDrawCallCount() const override
	{
		return 0;
	}

	int32_t GetDrawVertexCount() const override
	{
		return 0;
	}

	void ResetDrawCallCount() override
	{
	}

	void ResetDrawVertexCount() override
	{
	}

	void SetRenderMode(Effekseer::RenderMode renderMode) override
	{
	}

	VertexBuffer* GetVertexBuffer();

	IndexBuffer* GetIndexBuffer();

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer();

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);

	template <Effekseer::RendererMaterialType MaterialType, bool IsAdvanced>
	void AddVertex(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);

	template <>
	void AddVertex<Effekseer::RendererMaterialType::Default, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);
	template <>
	void AddVertex<Effekseer::RendererMaterialType::Default, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);
	template <>
	void AddVertex<Effekseer::RendererMaterialType::BackDistortion, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);
	template <>
	void AddVertex<Effekseer::RendererMaterialType::BackDistortion, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);
	template <>
	void AddVertex<Effekseer::RendererMaterialType::Lighting, false>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);
	template <>
	void AddVertex<Effekseer::RendererMaterialType::Lighting, true>(FDynamicMeshBuilder& meshBuilder, int32_t spriteCount, int32_t vertexOffset);

	void DrawModel(void* model,
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
				   std::vector<std::array<float, 4>>& customData2);

	UMaterialInterface* FindMaterial(EffekseerRenderingUserData* userData);

	Shader* GetShader(::EffekseerRenderer::RendererShaderType shaderType) const;

	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, ::Effekseer::Backend::TextureRef* textures, int32_t count);
	void SetDistortionIntensity(float value)
	{
		m_distortionIntensity = value;
	}

	void SetLocalToWorld(FMatrix localToWorld);
	void SetViewIndex(int32_t viewIndex);
	void SetMaterials(const TMap<UTexture2D*, UMaterialInstanceDynamic*>* materials, int32_t index);
	void SetNMaterials(const std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*>& nmaterials);
	void SetMeshElementCollector(FMeshElementCollector* meshElementCollector);

	virtual int GetRef()
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef()
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release()
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

} // namespace EffekseerRendererUE