#include "EffekseerRendererNative.h"




namespace EffekseerRenderer
{
	
void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			std::array<float, 3> scales;
			scales.fill(0.0);

			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scales[c] += mat.Value[c][r] * mat.Value[c][r];
				}
			}

			for (auto c = 0; c < 3; c++)
			{
				scales[c] = sqrt(scales[c]);
			}

			auto scale = (scales[0] + scales[1] + scales[2]) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Value[3][0] + cameraPos.X;
			auto cy = mat.Value[3][1] + cameraPos.Y;
			auto cz = mat.Value[3][2] + cameraPos.Z;
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						mat.Value[c][r] *= scale;
					}
				}
			}
		}

		auto objPos = ::Effekseer::Vector3D(mat.Value[3][0], mat.Value[3][1], mat.Value[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
	}
}

void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, ::Effekseer::Vector3D& scaleValues, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.X + scaleValues.Y + scaleValues.Z) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Value[3][0] + cameraPos.X;
			auto cy = mat.Value[3][1] + cameraPos.Y;
			auto cz = mat.Value[3][2] + cameraPos.Z;
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						mat.Value[c][r] *= scale;
					}
				}
			}
		}

		auto objPos = ::Effekseer::Vector3D(mat.Value[3][0], mat.Value[3][1], mat.Value[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Value[3][0] += dir.X * offset;
			mat.Value[3][1] += dir.Y * offset;
			mat.Value[3][2] += dir.Z * offset;
		}
	}
}

void ApplyDepthOffset(::Effekseer::Matrix44& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand)
{
	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			std::array<float, 3> scales;
			scales.fill(0.0);

			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scales[c] += mat.Values[c][r] * mat.Values[c][r];
				}
			}

			for (auto c = 0; c < 3; c++)
			{
				scales[c] = sqrt(scales[c]);
			}

			auto scale = (scales[0] + scales[1] + scales[2]) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto cx = mat.Values[3][0] + cameraPos.X;
			auto cy = mat.Values[3][1] + cameraPos.Y;
			auto cz = mat.Values[3][2] + cameraPos.Z;
			auto cl = sqrt(cx * cx + cy * cy + cz * cz);

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				for (auto r = 0; r < 3; r++)
				{
					for (auto c = 0; c < 3; c++)
					{
						mat.Values[c][r] *= scale;
					}
				}
			}
		}

		auto objPos = ::Effekseer::Vector3D(mat.Values[3][0], mat.Values[3][1], mat.Values[3][2]);
		auto dir = cameraPos - objPos;
		Effekseer::Vector3D::Normal(dir, dir);

		if (isRightHand)
		{
			mat.Values[3][0] += dir.X * offset;
			mat.Values[3][1] += dir.Y * offset;
			mat.Values[3][2] += dir.Z * offset;
		}
		else
		{
			mat.Values[3][0] += dir.X * offset;
			mat.Values[3][1] += dir.Y * offset;
			mat.Values[3][2] += dir.Z * offset;
		}
	}
}


}

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
	assert( m_isLock );

	memcpy( GetBufferDirect( count ), buffer, count * sizeof(uint16_t) );
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

	pBuffer = (uint8_t*)m_resource + ( m_indexCount * sizeof(uint16_t) );
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

Renderer::Renderer()
{
	impl = new Impl();
}

Renderer::~Renderer()
{
	ES_SAFE_DELETE(impl);
}

UVStyle Renderer::GetTextureUVStyle() const
{
	return impl->GetTextureUVStyle();
}

void Renderer::SetTextureUVStyle(UVStyle style)
{
	impl->SetTextureUVStyle(style);
}

UVStyle Renderer::GetBackgroundTextureUVStyle() const
{
	return impl->GetBackgroundTextureUVStyle();
}

void Renderer::SetBackgroundTextureUVStyle(UVStyle style)
{
	impl->SetBackgroundTextureUVStyle(style);
}

}
namespace EffekseerRenderer
{

UVStyle Renderer::Impl::GetTextureUVStyle() const
{
	return textureUVStyle;
}

void Renderer::Impl::SetTextureUVStyle(UVStyle style)
{
	textureUVStyle = style;
}

UVStyle Renderer::Impl::GetBackgroundTextureUVStyle() const
{
	return backgroundTextureUVStyle;
}

void Renderer::Impl::SetBackgroundTextureUVStyle(UVStyle style)
{
	backgroundTextureUVStyle = style;
}

}
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

	TextureFilterTypes[0] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[1] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[2] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[3] = ::Effekseer::TextureFilterType::Nearest;
	
	TextureWrapTypes[0] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[1] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[2] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[3] = ::Effekseer::TextureWrapType::Clamp;	
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
	state.TextureFilterTypes[0] = TextureFilterTypes[0];
	state.TextureFilterTypes[1] = TextureFilterTypes[1];
	state.TextureFilterTypes[2] = TextureFilterTypes[2];
	state.TextureFilterTypes[3] = TextureFilterTypes[3];

	state.TextureWrapTypes[0] = TextureWrapTypes[0];
	state.TextureWrapTypes[1] = TextureWrapTypes[1];
	state.TextureWrapTypes[2] = TextureWrapTypes[2];
	state.TextureWrapTypes[3] = TextureWrapTypes[3];
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