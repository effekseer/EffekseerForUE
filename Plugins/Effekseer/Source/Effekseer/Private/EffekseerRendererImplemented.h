
#pragma once

#include "EffekseerRendererNative.h"

#include <EffekseerEffect.h>
#include <map>

namespace EffekseerRendererUE4
{
	class RendererImplemented;
	class RenderState;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Color	Col;
		float		UV[2];

		void SetColor(const ::Effekseer::Color& color)
		{
			Col = color;
		}
	};

	struct VertexDistortion
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Color	Col;
		float		UV[2];
		::Effekseer::Vector3D	Tangent;
		::Effekseer::Vector3D	Binormal;

		void SetColor(const ::Effekseer::Color& color)
		{
			Col = color;
		}
	};

	inline void TransformVertexes(Vertex* vertexes, int32_t count, const ::Effekseer::Matrix43& mat)
	{
#if 1
		__m128 r0 = _mm_loadu_ps(mat.Value[0]);
		__m128 r1 = _mm_loadu_ps(mat.Value[1]);
		__m128 r2 = _mm_loadu_ps(mat.Value[2]);
		__m128 r3 = _mm_loadu_ps(mat.Value[3]);

		float tmp_out[4];
		::Effekseer::Vector3D* inout_prev;

		// １ループ目
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[0].Pos;
			__m128 v = _mm_loadu_ps((const float*)inout_cur);

			__m128 x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 a0 = _mm_mul_ps(r0, x);
			__m128 y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 a1 = _mm_mul_ps(r1, y);
			__m128 z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 a2 = _mm_mul_ps(r2, z);

			__m128 a01 = _mm_add_ps(a0, a1);
			__m128 a23 = _mm_add_ps(a2, r3);
			__m128 a = _mm_add_ps(a01, a23);

			// 今回の結果をストアしておく
			_mm_storeu_ps(tmp_out, a);
			inout_prev = inout_cur;
		}

		for (int i = 1; i < count; i++)
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[i].Pos;
			__m128 v = _mm_loadu_ps((const float*)inout_cur);

			__m128 x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 a0 = _mm_mul_ps(r0, x);
			__m128 y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 a1 = _mm_mul_ps(r1, y);
			__m128 z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 a2 = _mm_mul_ps(r2, z);

			__m128 a01 = _mm_add_ps(a0, a1);
			__m128 a23 = _mm_add_ps(a2, r3);
			__m128 a = _mm_add_ps(a01, a23);

			// 直前のループの結果を書き込みます
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];

			// 今回の結果をストアしておく
			_mm_storeu_ps(tmp_out, a);
			inout_prev = inout_cur;
		}

		// 最後のループの結果を書き込み
		{
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];
		}

#else
		for (int i = 0; i < count; i++)
		{
			::Effekseer::Vector3D::Transform(
				vertexes[i].Pos,
				vertexes[i].Pos,
				mat);
		}
#endif
	}

	inline void TransformVertexes(VertexDistortion* vertexes, int32_t count, const ::Effekseer::Matrix43& mat)
	{
#if 1
		__m128 r0 = _mm_loadu_ps(mat.Value[0]);
		__m128 r1 = _mm_loadu_ps(mat.Value[1]);
		__m128 r2 = _mm_loadu_ps(mat.Value[2]);
		__m128 r3 = _mm_loadu_ps(mat.Value[3]);

		float tmp_out[4];
		::Effekseer::Vector3D* inout_prev;

		// １ループ目
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[0].Pos;
			__m128 v = _mm_loadu_ps((const float*)inout_cur);

			__m128 x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 a0 = _mm_mul_ps(r0, x);
			__m128 y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 a1 = _mm_mul_ps(r1, y);
			__m128 z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 a2 = _mm_mul_ps(r2, z);

			__m128 a01 = _mm_add_ps(a0, a1);
			__m128 a23 = _mm_add_ps(a2, r3);
			__m128 a = _mm_add_ps(a01, a23);

			// 今回の結果をストアしておく
			_mm_storeu_ps(tmp_out, a);
			inout_prev = inout_cur;
		}

		for (int i = 1; i < count; i++)
		{
			::Effekseer::Vector3D* inout_cur = &vertexes[i].Pos;
			__m128 v = _mm_loadu_ps((const float*)inout_cur);

			__m128 x = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 a0 = _mm_mul_ps(r0, x);
			__m128 y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			__m128 a1 = _mm_mul_ps(r1, y);
			__m128 z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			__m128 a2 = _mm_mul_ps(r2, z);

			__m128 a01 = _mm_add_ps(a0, a1);
			__m128 a23 = _mm_add_ps(a2, r3);
			__m128 a = _mm_add_ps(a01, a23);

			// 直前のループの結果を書き込みます
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];

			// 今回の結果をストアしておく
			_mm_storeu_ps(tmp_out, a);
			inout_prev = inout_cur;
		}

		// 最後のループの結果を書き込み
		{
			inout_prev->X = tmp_out[0];
			inout_prev->Y = tmp_out[1];
			inout_prev->Z = tmp_out[2];
		}

#else
		for (int i = 0; i < count; i++)
		{
			::Effekseer::Vector3D::Transform(
				vertexes[i].Pos,
				vertexes[i].Pos,
				mat);
		}
#endif

		for (int i = 0; i < count; i++)
		{
			auto vs = &vertexes[i];

			::Effekseer::Vector3D::Transform(
				vs->Tangent,
				vs->Tangent,
				mat);

			::Effekseer::Vector3D::Transform(
				vs->Binormal,
				vs->Binormal,
				mat);

			Effekseer::Vector3D zero;
			::Effekseer::Vector3D::Transform(
				zero,
				zero,
				mat);

			::Effekseer::Vector3D::Normal(vs->Tangent, vs->Tangent - zero);
			::Effekseer::Vector3D::Normal(vs->Binormal, vs->Binormal - zero);
		}
	}

	typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
	typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
	typedef ::Effekseer::Vector3D efkVector3D;

	class ModelRenderer
		: public ::EffekseerRenderer::ModelRendererBase
	{
	private:
		RendererImplemented*	m_renderer;
		ModelRenderer(RendererImplemented* renderer);

	public:

		virtual ~ModelRenderer();

		static ModelRenderer* Create(RendererImplemented* renderer);

	public:
		void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData);

		//void Rendering(const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData);

		void EndRendering(const efkModelNodeParam& parameter, void* userData);
	};

	class RendererImplemented
		: public ::EffekseerRenderer::Renderer
		, public ::Effekseer::ReferenceObject
	{
	protected:
		::Effekseer::Vector3D	m_lightDirection;
		::Effekseer::Color		m_lightColor;
		::Effekseer::Color		m_lightAmbient;
		int32_t					m_squareMaxCount;

		::Effekseer::Matrix44	m_proj;
		::Effekseer::Matrix44	m_camera;
		::Effekseer::Matrix44	m_cameraProj;

		VertexBuffer*			m_vertexBuffer = nullptr;
		Shader*					m_stanShader = nullptr;
		Shader*					m_distortionShader = nullptr;
		Shader*					m_currentShader = nullptr;
		RenderState*			m_renderState = nullptr;

		void*					m_textures[16];

		FMatrix					m_localToWorld;
		int32_t					m_viewIndex = 0;
		TMap<UTexture2D*, UMaterialInstanceDynamic*>*	m_materials[6];
		FMeshElementCollector*	m_meshElementCollector = nullptr;
		std::map<EffekseerMaterial, UMaterialInstanceDynamic*>	m_nmaterials;

		bool					m_isDistorting = false;
		float					m_distortionIntensity = 0.0f;
		bool					m_isLighting = false;

		EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>*	m_standardRenderer = nullptr;
	public:
		
		static RendererImplemented* Create();

		RendererImplemented();
		virtual ~RendererImplemented();

		void OnLostDevice() override {}
		void OnResetDevice() override {}

		/**
		@brief	初期化
		*/
		bool Initialize(int32_t squareMaxCount);

		/**
		@brief	このインスタンスを破棄する。
		*/
		void Destroy() override;

		/**
		@brief	ステートを復帰するかどうかのフラグを設定する。
		*/
		void SetRestorationOfStatesFlag(bool flag) override;

		/**
		@brief	描画を開始する時に実行する。
		*/
		bool BeginRendering() override;

		/**
		@brief	描画を終了する時に実行する。
		*/
		bool EndRendering() override;

		/**
		@brief	ライトの方向を取得する。
		*/
		const ::Effekseer::Vector3D& GetLightDirection() const override;

		/**
		@brief	ライトの方向を設定する。
		*/
		void SetLightDirection(::Effekseer::Vector3D& direction) override;

		/**
		@brief	ライトの色を取得する。
		*/
		const ::Effekseer::Color& GetLightColor() const override;

		/**
		@brief	ライトの色を設定する。
		*/
		void SetLightColor(::Effekseer::Color& color) override;

		/**
		@brief	ライトの環境光の色を取得する。
		*/
		const ::Effekseer::Color& GetLightAmbientColor() const override;

		/**
		@brief	ライトの環境光の色を設定する。
		*/
		void SetLightAmbientColor(::Effekseer::Color& color) override;

		/**
		@brief	最大描画スプライト数を取得する。
		*/
		int32_t GetSquareMaxCount() const  override;

		/**
		@brief	投影行列を取得する。
		*/
		const ::Effekseer::Matrix44& GetProjectionMatrix() const  override;

		/**
		@brief	投影行列を設定する。
		*/
		void SetProjectionMatrix(const ::Effekseer::Matrix44& mat)  override;

		/**
		@brief	カメラ行列を取得する。
		*/
		const ::Effekseer::Matrix44& GetCameraMatrix() const  override;

		/**
		@brief	カメラ行列を設定する。
		*/
		void SetCameraMatrix(const ::Effekseer::Matrix44& mat)  override;

		/**
		@brief	カメラプロジェクション行列を取得する。
		*/
		::Effekseer::Matrix44& GetCameraProjectionMatrix()  override;

		/**
		@brief	スプライトレンダラーを生成する。
		*/
		::Effekseer::SpriteRenderer* CreateSpriteRenderer() override;

		/**
		@brief	リボンレンダラーを生成する。
		*/
		::Effekseer::RibbonRenderer* CreateRibbonRenderer() override;

		/**
		@brief	リングレンダラーを生成する。
		*/
		::Effekseer::RingRenderer* CreateRingRenderer() override;

		/**
		@brief	モデルレンダラーを生成する。
		*/
		::Effekseer::ModelRenderer* CreateModelRenderer() override;

		/**
		@brief	軌跡レンダラーを生成する。
		*/
		::Effekseer::TrackRenderer* CreateTrackRenderer() override;

		/**
		@brief	標準のテクスチャ読込クラスを生成する。
		*/
		::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL)  override;

		/**
		@brief	標準のモデル読込クラスを生成する。
		*/
		::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

		/**
		@brief	レンダーステートを強制的にリセットする。
		*/
		void ResetRenderState() override;

		/**
		@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを取得する。
		*/
		::EffekseerRenderer::DistortingCallback* GetDistortingCallback()  override;

		/**
		@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを設定する。
		*/
		void SetDistortingCallback(::EffekseerRenderer::DistortingCallback* callback) override;

		Effekseer::TextureData* GetBackground();

		VertexBuffer* GetVertexBuffer();

		IndexBuffer* GetIndexBuffer();

		EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer();

		::EffekseerRenderer::RenderStateBase* GetRenderState();

		void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
		void SetIndexBuffer(IndexBuffer* indexBuffer);

		void SetLayout(Shader* shader);
		void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
		
		void DrawModel(void* model, std::vector<Effekseer::Matrix44>& matrixes, std::vector<Effekseer::RectF>& uvs, std::vector<Effekseer::Color>& colors);

		UMaterialInstanceDynamic* FindMaterial();

		void BeginShader(Shader* shader);
		void EndShader(Shader* shader);

		void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);
		void SetIsLighting(bool value) { m_isLighting = value; }
		void SetIsDistorting(bool value) { m_isDistorting = value; }
		void SetDistortionIntensity(float value) { m_distortionIntensity = value; }

		void SetLocalToWorld(FMatrix localToWorld);
		void SetViewIndex(int32_t viewIndex);
		void SetMaterials(const TMap<UTexture2D*, UMaterialInstanceDynamic*>* materials, int32_t index);
		void SetNMaterials(const std::map<EffekseerMaterial, UMaterialInstanceDynamic*>& nmaterials);
		void SetMeshElementCollector(FMeshElementCollector* meshElementCollector);

		virtual int GetRef() { return ::Effekseer::ReferenceObject::GetRef(); }
		virtual int AddRef() { return ::Effekseer::ReferenceObject::AddRef(); }
		virtual int Release() { return ::Effekseer::ReferenceObject::Release(); }
	};

}