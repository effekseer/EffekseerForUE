#include "EffekseerRendererNative.h"
#undef far



namespace EffekseerRenderer
{

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::Mat43f& dst,
				   ::Effekseer::Vec3f& s,
				   ::Effekseer::Vec3f& R,
				   ::Effekseer::Vec3f& F,
				   const ::Effekseer::Mat43f& src,
				   const ::Effekseer::Vec3f& frontDirection)
{
	auto frontDir = frontDirection;

	assert(abs(frontDir.GetLength() - 1.0f) < 0.0001f);

	if (billboardType == ::Effekseer::BillboardType::Billboard || billboardType == ::Effekseer::BillboardType::RotatedBillboard ||
		billboardType == ::Effekseer::BillboardType::YAxisFixed)
	{
		::Effekseer::Mat43f r;
		::Effekseer::Vec3f t;
		src.GetSRT(s, r, t);

		::Effekseer::Vec3f U;

		if (billboardType == ::Effekseer::BillboardType::Billboard)
		{
			::Effekseer::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::Vec3f::Cross(F, R).Normalize();
		}
		else if (billboardType == ::Effekseer::BillboardType::RotatedBillboard)
		{
			::Effekseer::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::Vec3f::Cross(F, R).Normalize();

			float c_zx2 = Effekseer::Vec3f::Dot(r.Y, r.Y) - r.Y.GetZ() * r.Y.GetZ();
			float c_zx = sqrt(std::max(0.0f, c_zx2));
			float s_z = 0.0f;
			float c_z = 0.0f;

			if (fabsf(c_zx) > 0.001f)
			{
				s_z = r.Y.GetX() / c_zx;
				c_z = r.Y.GetY() / c_zx;
			}
			else
			{
				s_z = 0.0f;
				c_z = 1.0f;
			}

			::Effekseer::Vec3f r_temp = R;
			::Effekseer::Vec3f u_temp = U;

			R = r_temp * c_z + u_temp * s_z;
			U = u_temp * c_z - r_temp * s_z;
		}
		else if (billboardType == ::Effekseer::BillboardType::YAxisFixed)
		{
			U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());
			F = frontDir;
			R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
			F = ::Effekseer::Vec3f::Cross(R, U).Normalize();
		}

		dst.X = {R.GetX(), U.GetX(), F.GetX(), t.GetX()};
		dst.Y = {R.GetY(), U.GetY(), F.GetY(), t.GetY()};
		dst.Z = {R.GetZ(), U.GetZ(), F.GetZ(), t.GetZ()};
	}
}

void SplineGenerator::AddVertex(const Effekseer::Vec3f& v)
{
	a.push_back(v);
	if (a.size() >= 2)
	{
		isSame.push_back(a[a.size() - 1] == a[a.size() - 2]);
	}
}

void SplineGenerator::Calculate()
{
	b.resize(a.size());
	c.resize(a.size());
	d.resize(a.size());
	w.resize(a.size());

	b[0] = b[b.size() - 1] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);
	c[0] = c[c.size() - 1] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);
	w[0] = ::Effekseer::Vec3f(0.0f, 0.0f, 0.0f);

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		c[i] = (a[i - 1] + a[i] * (-2.0) + a[i + 1]) * 3.0;
	}

	for (size_t i = 1; i < a.size() - 1; i++)
	{
		auto tmp = Effekseer::Vec3f(4.0, 4.0, 4.0) - w[i - 1];
		c[i] = (c[i] - c[i - 1]) / tmp;
		w[i] = Effekseer::Vec3f(1.0, 1.0, 1.0) / tmp;
	}

	for (size_t i = (a.size() - 1) - 1; i > 0; i--)
	{
		c[i] = c[i] - c[i + 1] * w[i];
	}

	for (size_t i = 0; i < a.size() - 1; i++)
	{
		d[i] = (c[i + 1] - c[i]) / 3.0;
		b[i] = a[i + 1] - a[i] - c[i] - d[i];
	}
}

void SplineGenerator::Reset()
{
	a.clear();
	b.clear();
	c.clear();
	d.clear();
	w.clear();
	isSame.clear();
}

Effekseer::Vec3f SplineGenerator::GetValue(float t) const
{
	int32_t j = (int32_t)floorf(t);

	if (j < 0)
	{
		j = 0;
	}

	if (j > (int32_t)a.size())
	{
		j = (int32_t)a.size() - 1;
	}

	auto dt = t - j;

	if (j < (int32_t)isSame.size() && isSame[j])
		return a[j];

	return a[j] + (b[j] + (c[j] + d[j] * dt) * dt) * dt;
}

static void FastScale(::Effekseer::Mat43f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

static void FastScale(::Effekseer::Mat44f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();
		// auto cl = cameraFront.X * cx + cameraFront.Y * cy * cameraFront.Z * cz;

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  ::Effekseer::Vec3f& translationValues,
						  ::Effekseer::Vec3f& scaleValues,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto c = translationValues - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				scaleValues *= scale;
			}
		}

		auto objPos = translationValues;
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			translationValues += dir * offset;
		}
		else
		{
			translationValues += dir * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cam2t = translationValues - cameraPos;
		auto cl = cam2t.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scaleValues *= scale;
				}
			}
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::Vec3f& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::Mat44f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

} // namespace EffekseerRenderer


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBufferBase::IndexBufferBase( int maxCount, bool isDynamic )
	: m_indexMaxCount	( maxCount )
	, m_indexCount		( 0 )
	, m_isDynamic		( false )
	, m_isLock			( false )
	, m_resource		( NULL )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBufferBase::~IndexBufferBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBufferBase::Push( const void* buffer, int count )
{
	assert(m_isLock);
	memcpy(GetBufferDirect(count), buffer, count * stride_);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int IndexBufferBase::GetCount() const
{
	return m_indexCount;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int IndexBufferBase::GetMaxCount() const
{
	return m_indexMaxCount;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void* IndexBufferBase::GetBufferDirect( int count )
{
	assert( m_isLock );
	assert( m_indexMaxCount >= m_indexCount + count );

	uint8_t* pBuffer = NULL;

	pBuffer = (uint8_t*)m_resource + (m_indexCount * stride_);
	m_indexCount += count;

	return pBuffer;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------



namespace EffekseerRenderer
{

Renderer::Renderer() { impl = new Impl(); }

Renderer::~Renderer() { ES_SAFE_DELETE(impl); }

Renderer::Impl* Renderer::GetImpl() { return impl; }

::Effekseer::Vector3D Renderer::GetLightDirection() const { return impl->GetLightDirection(); }

void Renderer::SetLightDirection(const ::Effekseer::Vector3D& direction) { impl->SetLightDirection(direction); }

const ::Effekseer::Color& Renderer::GetLightColor() const { return impl->GetLightColor(); }

void Renderer::SetLightColor(const ::Effekseer::Color& color) { impl->SetLightColor(color); }

const ::Effekseer::Color& Renderer::GetLightAmbientColor() const { return impl->GetLightAmbientColor(); }

void Renderer::SetLightAmbientColor(const ::Effekseer::Color& color) { impl->SetLightAmbientColor(color); }

::Effekseer::Matrix44 Renderer::GetProjectionMatrix() const { return impl->GetProjectionMatrix(); }

void Renderer::SetProjectionMatrix(const ::Effekseer::Matrix44& mat) { impl->SetProjectionMatrix(mat); }

::Effekseer::Matrix44 Renderer::GetCameraMatrix() const { return impl->GetCameraMatrix(); }

void Renderer::SetCameraMatrix(const ::Effekseer::Matrix44& mat) { impl->SetCameraMatrix(mat); }

::Effekseer::Matrix44 Renderer::GetCameraProjectionMatrix() const { return impl->GetCameraProjectionMatrix(); }

::Effekseer::Vector3D Renderer::GetCameraFrontDirection() const { return impl->GetCameraFrontDirection(); }

::Effekseer::Vector3D Renderer::GetCameraPosition() const { return impl->GetCameraPosition(); }

void Renderer::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	impl->SetCameraParameter(front, position);
}

int32_t Renderer::GetDrawCallCount() const { return impl->GetDrawCallCount(); }

int32_t Renderer::GetDrawVertexCount() const { return impl->GetDrawVertexCount(); }

void Renderer::ResetDrawCallCount() { impl->ResetDrawCallCount(); }

void Renderer::ResetDrawVertexCount() { impl->ResetDrawVertexCount(); }

Effekseer::RenderMode Renderer::GetRenderMode() const { return impl->GetRenderMode(); }

void Renderer::SetRenderMode(Effekseer::RenderMode renderMode) { impl->SetRenderMode(renderMode); }

UVStyle Renderer::GetTextureUVStyle() const { return impl->GetTextureUVStyle(); }

void Renderer::SetTextureUVStyle(UVStyle style) { impl->SetTextureUVStyle(style); }

UVStyle Renderer::GetBackgroundTextureUVStyle() const { return impl->GetBackgroundTextureUVStyle(); }

void Renderer::SetBackgroundTextureUVStyle(UVStyle style) { impl->SetBackgroundTextureUVStyle(style); }

float Renderer::GetTime() const { return impl->GetTime(); }

void Renderer::SetTime(float time) { impl->SetTime(time); }

void Renderer::SetBackgroundTexture(::Effekseer::TextureData* textureData)
{
	// not implemented
	assert(0);
}

} // namespace EffekseerRenderer

namespace EffekseerRenderer
{

::Effekseer::Vector3D Renderer::Impl::GetLightDirection() const {return ToStruct(lightDirection_); }

void Renderer::Impl::SetLightDirection(const ::Effekseer::Vector3D& direction) { lightDirection_ = direction; }

const ::Effekseer::Color& Renderer::Impl::GetLightColor() const { return lightColor_; }

void Renderer::Impl::SetLightColor(const ::Effekseer::Color& color) { lightColor_ = color; }

const ::Effekseer::Color& Renderer::Impl::GetLightAmbientColor() const { return lightAmbient_; }

void Renderer::Impl::SetLightAmbientColor(const ::Effekseer::Color& color) { lightAmbient_ = color; }

void Renderer::Impl::CalculateCameraProjectionMatrix() { cameraProjMat_ = cameraMat_ * projectionMat_; }

::Effekseer::Matrix44 Renderer::Impl::GetProjectionMatrix() const { return ToStruct(projectionMat_); }

void Renderer::Impl::SetProjectionMatrix(const ::Effekseer::Matrix44& mat) { projectionMat_ = mat; }

::Effekseer::Matrix44 Renderer::Impl::GetCameraMatrix() const { return ToStruct(cameraMat_); }

void Renderer::Impl::SetCameraMatrix(const ::Effekseer::Matrix44& mat)
{
	cameraFrontDirection_ = ::Effekseer::Vec3f(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);

	auto localPos = ::Effekseer::Vec3f(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
	auto f = cameraFrontDirection_;
	auto r = ::Effekseer::Vec3f(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
	auto u = ::Effekseer::Vec3f(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

	cameraPosition_ = r * localPos.GetX() + u * localPos.GetY() + f * localPos.GetZ();

	// To optimize particle, cameraFontDirection_ is normalized
	cameraFrontDirection_ = cameraFrontDirection_.NormalizePrecisely();
	cameraMat_ = mat;
}

::Effekseer::Matrix44 Renderer::Impl::GetCameraProjectionMatrix() const { return ToStruct(cameraProjMat_); }

::Effekseer::Vector3D Renderer::Impl::GetCameraFrontDirection() const { return ToStruct(cameraFrontDirection_); }

::Effekseer::Vector3D Renderer::Impl::GetCameraPosition() const { return ToStruct(cameraPosition_); }

void Renderer::Impl::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	cameraFrontDirection_ = front;
	cameraPosition_ = position;

	// To optimize particle, cameraFontDirection_ is normalized
	cameraFrontDirection_ = cameraFrontDirection_.NormalizePrecisely();
}

void Renderer::Impl::CreateProxyTextures(Renderer* renderer)
{
	whiteProxyTexture_ = renderer->CreateProxyTexture(::EffekseerRenderer::ProxyTextureType::White);
	normalProxyTexture_ = renderer->CreateProxyTexture(::EffekseerRenderer::ProxyTextureType::Normal);
}

void Renderer::Impl::DeleteProxyTextures(Renderer* renderer)
{
	renderer->DeleteProxyTexture(whiteProxyTexture_);
	renderer->DeleteProxyTexture(normalProxyTexture_);
	whiteProxyTexture_ = nullptr;
	normalProxyTexture_ = nullptr;
}

::Effekseer::TextureData* Renderer::Impl::GetProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	if (type == EffekseerRenderer::ProxyTextureType::White)
		return whiteProxyTexture_;
	if (type == EffekseerRenderer::ProxyTextureType::Normal)
		return normalProxyTexture_;
	return nullptr;
}

UVStyle Renderer::Impl::GetTextureUVStyle() const { return textureUVStyle; }

void Renderer::Impl::SetTextureUVStyle(UVStyle style) { textureUVStyle = style; }

UVStyle Renderer::Impl::GetBackgroundTextureUVStyle() const { return backgroundTextureUVStyle; }

void Renderer::Impl::SetBackgroundTextureUVStyle(UVStyle style) { backgroundTextureUVStyle = style; }

int32_t Renderer::Impl::GetDrawCallCount() const { return drawcallCount; }

int32_t Renderer::Impl::GetDrawVertexCount() const { return drawvertexCount; }

void Renderer::Impl::ResetDrawCallCount() { drawcallCount = 0; }

void Renderer::Impl::ResetDrawVertexCount() { drawvertexCount = 0; }

float Renderer::Impl::GetTime() const { return time_; }

void Renderer::Impl::SetTime(float time) { time_ = time; }

Effekseer::RenderMode Renderer::Impl::GetRenderMode() const
{
	if (!isRenderModeValid)
	{
		printf("RenderMode is not implemented.\n");
		return Effekseer::RenderMode::Normal;
	}

	return renderMode_;
}

void Renderer::Impl::SetRenderMode(Effekseer::RenderMode renderMode) { renderMode_ = renderMode; }

} // namespace EffekseerRenderer

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State::State()
{
	Reset();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::State::Reset()
{
	DepthTest = false;
	DepthWrite = false;
	AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	CullingType = ::Effekseer::CullingType::Double;
	TextureFilterTypes.fill(::Effekseer::TextureFilterType::Nearest);
	TextureWrapTypes.fill(::Effekseer::TextureWrapType::Clamp);
	TextureIDs.fill(0);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::State::CopyTo( State& state )
{
	state.DepthTest = DepthTest;
	state.DepthWrite = DepthWrite;
	state.AlphaBlend = AlphaBlend;
	state.CullingType = CullingType;
	state.TextureFilterTypes = TextureFilterTypes;
	state.TextureWrapTypes = TextureWrapTypes;
	state.TextureIDs = TextureIDs;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::RenderStateBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::~RenderStateBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State& RenderStateBase::Push()
{
	State newState;
	m_next.CopyTo( newState );
	m_stateStack.push( m_next );
	m_next = newState;
	return m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::Pop()
{
	assert( !m_stateStack.empty() );

	State top = m_stateStack.top();
	m_stateStack.pop();

	m_next = top;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State& RenderStateBase::GetActiveState()
{
	return m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBufferBase::VertexBufferBase( int size, bool isDynamic )
	: m_isDynamic		( isDynamic )
	, m_size			( size )
	, m_offset			( 0 )
	, m_resource		( NULL )
	, m_isLock			( false )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBufferBase::~VertexBufferBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBufferBase::Push( const void* buffer, int size )
{
	assert( m_isLock );
	memcpy( GetBufferDirect( size ), buffer, size );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int VertexBufferBase::GetMaxSize() const
{
	return m_size;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
int VertexBufferBase::GetSize() const
{
	return m_offset;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void* VertexBufferBase::GetBufferDirect( int size )
{
	assert( m_isLock );
	if( m_offset + size > m_size ) return NULL;

	void* pBuffer = NULL;

	// バッファに追記
	pBuffer = m_resource + m_offset;
	m_offset += size;
	
	return pBuffer;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
