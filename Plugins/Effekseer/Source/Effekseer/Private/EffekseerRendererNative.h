
#pragma once
#include "CoreMinimal.h"

#undef far

#ifndef __EFFEKSEERRENDERER_COMMON_UTILS_H__
#define __EFFEKSEERRENDERER_COMMON_UTILS_H__

#include "EffekseerNative.h"
#include <algorithm>
#include <array>
#include <assert.h>
#include <functional>
#include <math.h>
#include <string.h>

namespace EffekseerRenderer
{

using VertexFloat3 = ::Effekseer::Vector3D;
using VertexColor = ::Effekseer::Color;

struct DynamicVertex
{
	VertexFloat3 Pos;
	VertexColor Col;
	//! packed vector
	VertexColor Normal;
	//! packed vector
	VertexColor Tangent;

	union
	{
		//! UV1 (for template)
		float UV[2];
		float UV1[2];
	};

	float UV2[2];

	void SetAlphaUV(float value, int index)
	{
	}

	void SetUVDistortionUV(float value, int index)
	{
	}

	void SetBlendUV(float value, int index)
	{
	}

	void SetBlendAlphaUV(float value, int index)
	{
	}

	void SetBlendUVDistortionUV(float value, int index)
	{
	}

	void SetFlipbookIndexAndNextRate(float value)
	{
	}

	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct DynamicVertexWithCustomData
{
	DynamicVertex V;
	std::array<float, 4> CustomData1;
	std::array<float, 4> CustomData2;
};

struct LightingVertex
{
	VertexFloat3 Pos;
	VertexColor Col;
	//! packed vector
	VertexColor Normal;
	//! packed vector
	VertexColor Tangent;

	union
	{
		//! UV1 (for template)
		float UV[2];
		float UV1[2];
	};

	float UV2[2];

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct SimpleVertex
{
	VertexFloat3 Pos;
	VertexColor Col;

	union
	{
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct VertexDistortion
{
	VertexFloat3 Pos;
	VertexColor Col;

	union
	{
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
		Tangent = tangent;
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
		Binormal = binormal;
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct AdvancedLightingVertex
{
	VertexFloat3 Pos;
	VertexColor Col;
	//! packed vector
	VertexColor Normal;
	//! packed vector
	VertexColor Tangent;

	union
	{
		//! UV1 (for template)
		float UV[2];
		float UV1[2];
	};

	float UV2[2];

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct AdvancedSimpleVertex
{
	VertexFloat3 Pos;
	VertexColor Col;

	union
	{
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct AdvancedVertexDistortion
{
	VertexFloat3 Pos;
	VertexColor Col;

	union
	{
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
		Tangent = tangent;
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
		Binormal = binormal;
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

static int32_t GetMaximumVertexSizeInAllTypes()
{
	size_t size = sizeof(DynamicVertexWithCustomData);
	size = (std::max)(size, sizeof(VertexDistortion));
	size = (std::max)(size, sizeof(SimpleVertex));
	size = (std::max)(size, sizeof(LightingVertex));
	size = (std::max)(size, sizeof(AdvancedVertexDistortion));
	size = (std::max)(size, sizeof(AdvancedSimpleVertex));
	size = (std::max)(size, sizeof(AdvancedLightingVertex));

	return static_cast<int32_t>(size);
};

template <typename T>
inline bool IsDistortionVertex()
{
	return false;
}

template <>
inline bool IsDistortionVertex<VertexDistortion>()
{
	return true;
}

template <>
inline bool IsDistortionVertex<AdvancedVertexDistortion>()
{
	return true;
}

template <typename T>
inline bool IsDynamicVertex()
{
	return false;
}

template <>
inline bool IsDynamicVertex<DynamicVertex>()
{
	return true;
}

template <typename T>
inline bool IsLightingVertex()
{
	return false;
}

template <>
inline bool IsLightingVertex<LightingVertex>()
{
	return true;
}

template <>
inline bool IsLightingVertex<AdvancedLightingVertex>()
{
	return true;
}

/**
	@brief	a view class to access an array with a stride
*/
template <typename T>
struct StrideView
{
	int32_t stride_;
	uint8_t* pointer_;
	uint8_t* pointerOrigin_;

#ifndef NDEBUG
	int32_t offset_;
	int32_t elementCount_;
#endif

	StrideView(void* pointer, int32_t stride, int32_t elementCount)
		: stride_(stride)
		, pointer_(reinterpret_cast<uint8_t*>(pointer))
		, pointerOrigin_(reinterpret_cast<uint8_t*>(pointer))
#ifndef NDEBUG
		, offset_(0)
		, elementCount_(elementCount)
#endif
	{
	}

	T& operator[](int i) const
	{
#ifndef NDEBUG
		assert(i >= 0);
		assert(i + offset_ < elementCount_);
#endif
		return *reinterpret_cast<T*>((pointer_ + stride_ * i));
	}

	StrideView& operator+=(const int& rhs)
	{
#ifndef NDEBUG
		offset_ += rhs;
#endif
		pointer_ += stride_ * rhs;
		return *this;
	}

	void Reset()
	{
#ifndef NDEBUG
		offset_ = 0;
#endif
		pointer_ = pointerOrigin_;
	}
};

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::Mat43f& dst,
				   ::Effekseer::Vec3f& s,
				   ::Effekseer::Vec3f& R,
				   ::Effekseer::Vec3f& F,
				   const ::Effekseer::Mat43f& src,
				   const ::Effekseer::Vec3f& frontDirection);

/**
	@brief Spline generator
	@note
	Reference https://qiita.com/edo_m18/items/f2f0c6bf9032b0ec12d4
*/
class SplineGenerator
{
	std::vector<Effekseer::Vec3f> a;
	std::vector<Effekseer::Vec3f> b;
	std::vector<Effekseer::Vec3f> c;
	std::vector<Effekseer::Vec3f> d;
	std::vector<Effekseer::Vec3f> w;
	std::vector<bool> isSame;

public:
	void AddVertex(const Effekseer::Vec3f& v);

	void Calculate();

	void Reset();

	Effekseer::Vec3f GetValue(float t) const;
};

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::Vec3f& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  ::Effekseer::Vec3f& translationValues,
						  ::Effekseer::Vec3f& scaleValues,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat44f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyViewOffset(::Effekseer::Mat43f& mat,
					 const ::Effekseer::Mat44f& camera,
					 float distance);

void ApplyViewOffset(::Effekseer::Mat44f& mat,
					 const ::Effekseer::Mat44f& camera,
					 float distance);

template <typename Vertex>
inline void TransformStandardVertexes(Vertex& vertexes, int32_t count, const ::Effekseer::Mat43f& mat)
{
	using namespace Effekseer;

	SIMD4f m0 = mat.X;
	SIMD4f m1 = mat.Y;
	SIMD4f m2 = mat.Z;
	SIMD4f m3 = SIMD4f::SetZero();
	SIMD4f::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		SIMD4f iPos = SIMD4f::Load3(&vertexes[i].Pos);

		SIMD4f oPos = SIMD4f::MulAddLane<0>(m3, m0, iPos);
		oPos = SIMD4f::MulAddLane<1>(oPos, m1, iPos);
		oPos = SIMD4f::MulAddLane<2>(oPos, m2, iPos);

		SIMD4f::Store3(&vertexes[i].Pos, oPos);
	}
}

template <typename VertexDistortion>
inline void TransformDistortionVertexes(VertexDistortion& vertexes, int32_t count, const ::Effekseer::Mat43f& mat)
{
	using namespace Effekseer;

	SIMD4f m0 = mat.X;
	SIMD4f m1 = mat.Y;
	SIMD4f m2 = mat.Z;
	SIMD4f m3 = SIMD4f::SetZero();
	SIMD4f::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		SIMD4f iPos = SIMD4f::Load3(&vertexes[i].Pos);
		SIMD4f iTangent = SIMD4f::Load3(&vertexes[i].Tangent);
		SIMD4f iBinormal = SIMD4f::Load3(&vertexes[i].Binormal);

		SIMD4f oPos = SIMD4f::MulAddLane<0>(m3, m0, iPos);
		oPos = SIMD4f::MulAddLane<1>(oPos, m1, iPos);
		oPos = SIMD4f::MulAddLane<2>(oPos, m2, iPos);

		SIMD4f oTangent = SIMD4f::MulLane<0>(m0, iTangent);
		oTangent = SIMD4f::MulAddLane<1>(oTangent, m1, iTangent);
		oTangent = SIMD4f::MulAddLane<2>(oTangent, m2, iTangent);

		SIMD4f oBinormal = SIMD4f::MulLane<0>(m0, iBinormal);
		oBinormal = SIMD4f::MulAddLane<1>(oBinormal, m1, iBinormal);
		oBinormal = SIMD4f::MulAddLane<2>(oBinormal, m2, iBinormal);

		SIMD4f::Store3(&vertexes[i].Pos, oPos);
		SIMD4f::Store3(&vertexes[i].Tangent, oTangent);
		SIMD4f::Store3(&vertexes[i].Binormal, oBinormal);
	}
}

inline void TransformVertexes(StrideView<VertexDistortion>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<SimpleVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<DynamicVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<LightingVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedVertexDistortion>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedSimpleVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedLightingVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void SwapRGBAToBGRA(Effekseer::Color& color)
{
	auto temp = color;
	color.B = temp.R;
	color.R = temp.B;
}

inline Effekseer::Color PackVector3DF(const Effekseer::Vec3f& v)
{
	Effekseer::Color ret;
	ret.R = static_cast<uint8_t>(Effekseer::Clamp(((v.GetX() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.G = static_cast<uint8_t>(Effekseer::Clamp(((v.GetY() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.B = static_cast<uint8_t>(Effekseer::Clamp(((v.GetZ() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.A = 255;
	return ret;
}

inline Effekseer::Vec3f SafeNormalize(const Effekseer::Vec3f& v)
{
	auto lengthSq = v.GetSquaredLength();
	auto e = 0.0001f;
	if (lengthSq < e * e)
	{
		return v;
	}

	return v * Effekseer::Rsqrt(lengthSq);
}

struct MaterialShaderParameterGenerator
{
	int32_t VertexSize = 0;
	int32_t VertexShaderUniformBufferSize = 0;
	int32_t PixelShaderUniformBufferSize = 0;

	int32_t VertexCameraMatrixOffset = -1;
	int32_t VertexProjectionMatrixOffset = -1;
	int32_t VertexInversedFlagOffset = -1;
	int32_t VertexPredefinedOffset = -1;
	int32_t VertexCameraPositionOffset = -1;
	int32_t VertexUserUniformOffset = -1;

	int32_t PixelInversedFlagOffset = -1;
	int32_t PixelPredefinedOffset = -1;
	int32_t PixelCameraPositionOffset = -1;
	int32_t PixelLightDirectionOffset = -1;
	int32_t PixelLightColorOffset = -1;
	int32_t PixelLightAmbientColorOffset = -1;
	int32_t PixelCameraMatrixOffset = -1;
	int32_t PixelUserUniformOffset = -1;

	int32_t VertexModelMatrixOffset = -1;
	int32_t VertexModelUVOffset = -1;
	int32_t VertexModelColorOffset = -1;

	int32_t VertexModelCustomData1Offset = -1;
	int32_t VertexModelCustomData2Offset = -1;

	MaterialShaderParameterGenerator(const ::Effekseer::Material& material, bool isModel, int32_t stage, int32_t instanceCount)
	{
		if (isModel)
		{
			VertexSize = sizeof(::Effekseer::Model::Vertex);
		}
		else if (material.GetIsSimpleVertex())
		{
			VertexSize = sizeof(EffekseerRenderer::SimpleVertex);
		}
		else
		{
			VertexSize = sizeof(EffekseerRenderer::DynamicVertex) +
						 sizeof(float) * (material.GetCustomData1Count() + material.GetCustomData2Count());
		}

		if (isModel)
		{
			int32_t vsOffset = 0;
			VertexProjectionMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexModelMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f) * instanceCount;

			VertexModelUVOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * instanceCount;

			VertexModelColorOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * instanceCount;

			VertexInversedFlagOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexPredefinedOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexCameraPositionOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			if (material.GetCustomData1Count() > 0)
			{
				VertexModelCustomData1Offset = vsOffset;
				vsOffset += sizeof(float) * 4 * instanceCount;
			}

			if (material.GetCustomData2Count() > 0)
			{
				VertexModelCustomData2Offset = vsOffset;
				vsOffset += sizeof(float) * 4 * instanceCount;
			}

			VertexUserUniformOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * material.GetUniformCount();

			VertexShaderUniformBufferSize = vsOffset;
		}
		else
		{
			int32_t vsOffset = 0;
			VertexCameraMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexProjectionMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexInversedFlagOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexPredefinedOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexCameraPositionOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexUserUniformOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * material.GetUniformCount();

			VertexShaderUniformBufferSize = vsOffset;
		}

		int32_t psOffset = 0;

		PixelInversedFlagOffset = psOffset;
		psOffset += sizeof(float) * 4;

		PixelPredefinedOffset = psOffset;
		psOffset += sizeof(float) * 4;

		PixelCameraPositionOffset = psOffset;
		psOffset += sizeof(float) * 4;

		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			PixelLightDirectionOffset = psOffset;
			psOffset += sizeof(float) * 4;

			PixelLightColorOffset = psOffset;
			psOffset += sizeof(float) * 4;

			PixelLightAmbientColorOffset = psOffset;
			psOffset += sizeof(float) * 4;
		}

		if (material.GetHasRefraction() && stage == 1)
		{
			PixelCameraMatrixOffset = psOffset;
			psOffset += sizeof(Effekseer::Mat44f);
		}

		PixelUserUniformOffset = psOffset;
		psOffset += sizeof(float) * 4 * material.GetUniformCount();

		PixelShaderUniformBufferSize = psOffset;
	}
};

} // namespace EffekseerRenderer
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__

#ifndef __EFFEKSEERRENDERER_RENDERER_H__
#define __EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

/**
	@brief	背景を歪ませるエフェクトを描画する前に実行されるコールバック
*/
class DistortingCallback
{
public:
	DistortingCallback()
	{
	}
	virtual ~DistortingCallback()
	{
	}

	virtual bool OnDistorting()
	{
		return false;
	}
};

/**
	@brief
	\~english A status of UV when particles are rendered.
	\~japanese パーティクルを描画する時のUVの状態
*/
enum class UVStyle
{
	Normal,
	VerticalFlipped,
};

/**
	@brief
	\~english A type of texture which is rendered when textures are not assigned.
	\~japanese テクスチャが設定されていないときに描画されるテクスチャの種類
*/
enum class ProxyTextureType
{
	White,
	Normal,
};

/**
	@brief
	\~english A class which contains a graphics device
	\~japanese グラフィックデバイスを格納しているクラス
*/
class GraphicsDevice : public ::Effekseer::IReference
{
public:
	GraphicsDevice() = default;
	virtual ~GraphicsDevice() = default;
};

class CommandList : public ::Effekseer::IReference
{
public:
	CommandList() = default;
	virtual ~CommandList() = default;
};

class SingleFrameMemoryPool : public ::Effekseer::IReference
{
public:
	SingleFrameMemoryPool() = default;
	virtual ~SingleFrameMemoryPool() = default;

	/**
		@brief
		\~English	notify that new frame is started.
		\~Japanese	新規フレームが始ったことを通知する。
	*/
	virtual void NewFrame()
	{
	}
};

class Renderer : public ::Effekseer::IReference
{
protected:
	Renderer();
	virtual ~Renderer();

	class Impl;
	Impl* impl = nullptr;

public:
	/**
		@brief	only for Effekseer backend developer. Effekseer User doesn't need it.
	*/
	Impl* GetImpl();

	/**
		@brief	デバイスロストが発生した時に実行する。
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	デバイスがリセットされた時に実行する。
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	virtual void SetRestorationOfStatesFlag(bool flag) = 0;

	/**
		@brief	描画を開始する時に実行する。
	*/
	virtual bool BeginRendering() = 0;

	/**
		@brief	描画を終了する時に実行する。
	*/
	virtual bool EndRendering() = 0;

	/**
		@brief	Get the direction of light
	*/
	virtual ::Effekseer::Vector3D GetLightDirection() const;

	/**
		@brief	Specifiy the direction of light
	*/
	virtual void SetLightDirection(const ::Effekseer::Vector3D& direction);

	/**
		@brief	Get the color of light
	*/
	virtual const ::Effekseer::Color& GetLightColor() const;

	/**
		@brief	Specify the color of light
	*/
	virtual void SetLightColor(const ::Effekseer::Color& color);

	/**
		@brief	Get the color of ambient
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const;

	/**
		@brief	Specify the color of ambient
	*/
	virtual void SetLightAmbientColor(const ::Effekseer::Color& color);

	/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	Get a projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetProjectionMatrix() const;

	/**
		@brief	Set a projection matrix
	*/
	virtual void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraMatrix() const;

	/**
		@brief	Set a camera matrix
	*/
	virtual void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraProjectionMatrix() const;

	/**
		@brief	Get a front direction of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraFrontDirection() const;

	/**
		@brief	Get a position of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraPosition() const;

	/**
		@brief	Set a front direction and position of camera manually
		@param front (Right Hand) a direction from focus to eye, (Left Hand) a direction from eye to focus,
		@note
		These are set based on camera matrix automatically.
		It is failed on some platform.
	*/
	virtual void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	virtual ::Effekseer::SpriteRenderer* CreateSpriteRenderer() = 0;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	virtual ::Effekseer::RibbonRenderer* CreateRibbonRenderer() = 0;

	/**
		@brief	リングレンダラーを生成する。
	*/
	virtual ::Effekseer::RingRenderer* CreateRingRenderer() = 0;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	virtual ::Effekseer::ModelRenderer* CreateModelRenderer() = 0;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	virtual ::Effekseer::TrackRenderer* CreateTrackRenderer() = 0;

	/**
		@brief	標準のテクスチャ読込クラスを生成する。
	*/
	virtual ::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) = 0;

	/**
		@brief	標準のモデル読込クラスを生成する。
	*/
	virtual ::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) = 0;

	/**
	@brief
	\~english Create default material loader
	\~japanese 標準のマテリアル読込クラスを生成する。

	*/
	virtual ::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) = 0;

	/**
		@brief	レンダーステートを強制的にリセットする。
	*/
	virtual void ResetRenderState() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを取得する。
	*/
	virtual DistortingCallback* GetDistortingCallback() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを設定する。
	*/
	virtual void SetDistortingCallback(DistortingCallback* callback) = 0;

	/**
	@brief
	\~english Get draw call count
	\~japanese ドローコールの回数を取得する
	*/
	virtual int32_t GetDrawCallCount() const;

	/**
	@brief
	\~english Get the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual int32_t GetDrawVertexCount() const;

	/**
	@brief
	\~english Reset draw call count
	\~japanese ドローコールの回数をリセットする
	*/
	virtual void ResetDrawCallCount();

	/**
	@brief
	\~english Reset the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual void ResetDrawVertexCount();

	/**
	@brief
	\~english Get a render mode.
	\~japanese 描画モードを取得する。
	*/
	virtual Effekseer::RenderMode GetRenderMode() const;

	/**
	@brief
	\~english Specify a render mode.
	\~japanese 描画モードを設定する。
	*/
	virtual void SetRenderMode(Effekseer::RenderMode renderMode);

	/**
	@brief
	\~english Get an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を取得する。
	*/
	virtual UVStyle GetTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を設定する。
	*/
	virtual void SetTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を取得する。
	*/
	virtual UVStyle GetBackgroundTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を設定する。
	*/
	virtual void SetBackgroundTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get a current time (s)
	\~japanese 現在の時間を取得する。(秒)
	*/
	virtual float GetTime() const;

	/**
	@brief
	\~english Set a current time (s)
	\~japanese 現在の時間を設定する。(秒)
	*/
	virtual void SetTime(float time);

	/**
	@brief
	\~English	specify a command list to render.  This function is available except DirectX9, DirectX11 and OpenGL.
	\~Japanese	描画に使用するコマンドリストを設定する。この関数はDirectX9、DirectX11、OpenGL以外で使用できる。
	*/
	virtual void SetCommandList(CommandList* commandList)
	{
	}

	/**
	@brief
	\~English	Specify a background texture.
	\~Japanese	背景のテクスチャを設定する。
	@note
	\~English	Specified texture is not deleted by the renderer. This function is available except DirectX9, DirectX11.
	\~Japanese	設定されたテクスチャはレンダラーによって削除されない。この関数はDirectX9、DirectX11以外で使用できる。
	*/
	virtual void SetBackgroundTexture(::Effekseer::TextureData* textureData);

	/**
	@brief
	\~English	Create a proxy texture
	\~Japanese	代替のテクスチャを生成する
	*/
	virtual Effekseer::TextureData* CreateProxyTexture(ProxyTextureType type)
	{
		return nullptr;
	}

	/**
	@brief
	\~English	Delete a proxy texture
	\~Japanese	代替のテクスチャを削除する
	*/
	virtual void DeleteProxyTexture(Effekseer::TextureData* data)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RENDERER_H__

#ifndef __EFFEKSEERRENDERER_RENDERER_IMPL_H__
#define __EFFEKSEERRENDERER_RENDERER_IMPL_H__

#include "EffekseerNative.h"


namespace EffekseerRenderer
{

class Renderer::Impl : public ::Effekseer::AlignedAllocationPolicy<16>
{
private:
	::Effekseer::Mat44f projectionMat_;
	::Effekseer::Mat44f cameraMat_;
	::Effekseer::Mat44f cameraProjMat_;

	::Effekseer::Vec3f cameraPosition_;
	::Effekseer::Vec3f cameraFrontDirection_;

	::Effekseer::Vec3f lightDirection_ = ::Effekseer::Vec3f(1.0f, 1.0f, 1.0f);
	::Effekseer::Color lightColor_ = ::Effekseer::Color(255, 255, 255, 255);
	::Effekseer::Color lightAmbient_ = ::Effekseer::Color(40, 40, 40, 255);

	UVStyle textureUVStyle = UVStyle::Normal;
	UVStyle backgroundTextureUVStyle = UVStyle::Normal;
	float time_ = 0.0f;

	Effekseer::RenderMode renderMode_ = Effekseer::RenderMode::Normal;

	::Effekseer::TextureData* whiteProxyTexture_ = nullptr;
	::Effekseer::TextureData* normalProxyTexture_ = nullptr;

public:
	int32_t drawcallCount = 0;
	int32_t drawvertexCount = 0;
	bool isRenderModeValid = true;

	::Effekseer::Vector3D GetLightDirection() const;

	void SetLightDirection(const ::Effekseer::Vector3D& direction);

	const ::Effekseer::Color& GetLightColor() const;

	void SetLightColor(const ::Effekseer::Color& color);

	const ::Effekseer::Color& GetLightAmbientColor() const;

	void SetLightAmbientColor(const ::Effekseer::Color& color);

	void CalculateCameraProjectionMatrix();

	::Effekseer::Matrix44 GetProjectionMatrix() const;

	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	::Effekseer::Matrix44 GetCameraMatrix() const;

	void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	::Effekseer::Vector3D GetCameraFrontDirection() const;

	::Effekseer::Vector3D GetCameraPosition() const;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	::Effekseer::Matrix44 GetCameraProjectionMatrix() const;

	void CreateProxyTextures(Renderer* renderer);

	void DeleteProxyTextures(Renderer* renderer);

	::Effekseer::TextureData* GetProxyTexture(EffekseerRenderer::ProxyTextureType type);

	UVStyle GetTextureUVStyle() const;

	void SetTextureUVStyle(UVStyle style);

	UVStyle GetBackgroundTextureUVStyle() const;

	void SetBackgroundTextureUVStyle(UVStyle style);

	int32_t GetDrawCallCount() const;

	int32_t GetDrawVertexCount() const;

	void ResetDrawCallCount();

	void ResetDrawVertexCount();

	float GetTime() const;

	void SetTime(float time);

	Effekseer::RenderMode GetRenderMode() const;

	void SetRenderMode(Effekseer::RenderMode renderMode);
};

} // namespace EffekseerRenderer

#endif

#ifndef __EFFEKSEERRENDERER_RENDERSTATE_BASE_H__
#define __EFFEKSEERRENDERER_RENDERSTATE_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <stack>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderStateBase
{
public:
	struct State
	{
		bool DepthTest : 1;
		bool DepthWrite : 1;
		::Effekseer::AlphaBlendType AlphaBlend;
		::Effekseer::CullingType CullingType;
		std::array<::Effekseer::TextureFilterType, Effekseer::TextureSlotMax> TextureFilterTypes;
		std::array<::Effekseer::TextureWrapType, Effekseer::TextureSlotMax> TextureWrapTypes;

		//! for OpenGL
		std::array<uint64_t, Effekseer::TextureSlotMax> TextureIDs;

		State();

		void Reset();

		void CopyTo(State& state);
	};

protected:
	std::stack<State> m_stateStack;
	State m_active;
	State m_next;

public:
	RenderStateBase();
	virtual ~RenderStateBase();

	virtual void Update(bool forced) = 0;

	State& Push();
	void Pop();
	State& GetActiveState();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RENDERSTATE_BASE_H__

#ifndef __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__
#define __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBufferBase
{
protected:
	bool m_isDynamic;
	int m_size;
	int m_offset;
	uint8_t* m_resource;
	bool m_isLock;

public:
	VertexBufferBase(int size, bool isDynamic);
	virtual ~VertexBufferBase();

	virtual void Lock() = 0;
	virtual bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	/**
		@brief	try lock as a ring buffer. if failed, it does't lock it.
	*/
	virtual bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	virtual void Unlock() = 0;
	virtual void Push(const void* buffer, int size);
	virtual int GetMaxSize() const;
	virtual int GetSize() const;
	virtual void* GetBufferDirect(int size);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__

#ifndef __EFFEKSEERRENDERER_INDEXBUFFER_BASE_H__
#define __EFFEKSEERRENDERER_INDEXBUFFER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class IndexBufferBase
{
protected:
	int m_indexMaxCount;
	int m_indexCount;
	bool m_isDynamic;
	bool m_isLock;
	uint8_t* m_resource;
	int32_t stride_ = 2;

public:
	IndexBufferBase(int maxCount, bool isDynamic);
	virtual ~IndexBufferBase();

	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	void Push(const void* buffer, int count);
	int GetCount() const;
	int GetMaxCount() const;
	void* GetBufferDirect(int count);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_INDEXBUFFER_BASE_H__

#ifndef __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

#include "EffekseerNative.h"
#include <algorithm>
#include <functional>
#include <vector>


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

enum class StandardRendererShaderType
{
	Unlit,
	Lit,
	BackDistortion,
	AdvancedUnlit,
	AdvancedLit,
	AdvancedBackDistortion,
};

struct StandardRendererState
{
	bool DepthTest;
	bool DepthWrite;
	bool Distortion;
	float DistortionIntensity;
	bool Wireframe;
	bool Refraction;

	::Effekseer::AlphaBlendType AlphaBlend;
	::Effekseer::CullingType CullingType;
	::Effekseer::TextureFilterType TextureFilter1;
	::Effekseer::TextureWrapType TextureWrap1;
	::Effekseer::TextureFilterType TextureFilter2;
	::Effekseer::TextureWrapType TextureWrap2;
	::Effekseer::TextureFilterType TextureFilter3;
	::Effekseer::TextureWrapType TextureWrap3;
	::Effekseer::TextureFilterType TextureFilter4;
	::Effekseer::TextureWrapType TextureWrap4;
	::Effekseer::TextureFilterType TextureFilter5;
	::Effekseer::TextureWrapType TextureWrap5;
	::Effekseer::TextureFilterType TextureFilter6;
	::Effekseer::TextureWrapType TextureWrap6;
	::Effekseer::TextureFilterType TextureFilter7;
	::Effekseer::TextureWrapType TextureWrap7;

	::Effekseer::TextureData* TexturePtr;
	::Effekseer::TextureData* NormalTexturePtr;
	::Effekseer::TextureData* AlphaTexturePtr;
	::Effekseer::TextureData* UVDistortionTexturePtr;
	::Effekseer::TextureData* BlendTexturePtr;
	::Effekseer::TextureData* BlendAlphaTexturePtr;
	::Effekseer::TextureData* BlendUVDistortionTexturePtr;

	int32_t EnableInterpolation;
	int32_t UVLoopType;
	int32_t InterpolationType;
	int32_t FlipbookDivideX;
	int32_t FlipbookDivideY;

	float UVDistortionIntensity;

	int32_t TextureBlendType;

	float BlendUVDistortionIntensity;

	int32_t EmissiveScaling;

	float EdgeThreshold;
	uint8_t EdgeColor[4];
	int32_t EdgeColorScaling;
	bool IsAlphaCuttoffEnabled = false;

	::Effekseer::RendererMaterialType MaterialType;
	::Effekseer::MaterialData* MaterialPtr;
	int32_t MaterialUniformCount = 0;
	std::array<std::array<float, 4>, 16> MaterialUniforms;
	int32_t MaterialTextureCount = 0;
	std::array<::Effekseer::TextureData*, 16> MaterialTextures;
	int32_t CustomData1Count = 0;
	int32_t CustomData2Count = 0;

	StandardRendererState()
	{
		DepthTest = false;
		DepthWrite = false;
		Distortion = false;
		DistortionIntensity = 1.0f;
		Wireframe = true;
		Refraction = false;

		AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		CullingType = ::Effekseer::CullingType::Front;
		TextureFilter1 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap1 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter2 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap2 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter3 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap3 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter4 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap4 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter5 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap5 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter6 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap6 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter7 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap7 = ::Effekseer::TextureWrapType::Repeat;

		TexturePtr = nullptr;
		NormalTexturePtr = nullptr;
		AlphaTexturePtr = nullptr;
		UVDistortionTexturePtr = nullptr;
		BlendTexturePtr = nullptr;
		BlendAlphaTexturePtr = nullptr;
		BlendUVDistortionTexturePtr = nullptr;

		EnableInterpolation = 0;
		UVLoopType = 0;
		InterpolationType = 0;
		FlipbookDivideX = 0;
		FlipbookDivideY = 0;

		UVDistortionIntensity = 1.0f;

		TextureBlendType = 0;

		BlendUVDistortionIntensity = 1.0f;

		EmissiveScaling = 1;

		EdgeThreshold = 0.0f;
		EdgeColor[0] = EdgeColor[1] = EdgeColor[2] = EdgeColor[3] = 0;
		EdgeColorScaling = 1;

		MaterialPtr = nullptr;

		MaterialType = ::Effekseer::RendererMaterialType::Default;
		MaterialPtr = nullptr;
		MaterialUniformCount = 0;
		MaterialTextureCount = 0;
		CustomData1Count = 0;
		CustomData2Count = 0;
	}

	bool IsAdvanced() const
	{
		// TODO : merge with IsAdvanced in ModelRenderer
		if (MaterialType == ::Effekseer::RendererMaterialType::File)
		{
			return false;
		}

		if (AlphaTexturePtr != nullptr && AlphaTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (UVDistortionTexturePtr != nullptr && UVDistortionTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendTexturePtr != nullptr && BlendTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendAlphaTexturePtr != nullptr && BlendAlphaTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendUVDistortionTexturePtr != nullptr && BlendUVDistortionTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (EnableInterpolation != 0 ||
			TextureBlendType != -1 ||
			EdgeThreshold != 0 ||
			EmissiveScaling != 1.0f ||
			IsAlphaCuttoffEnabled)
		{
			return true;
		}

		return false;
	}

	bool operator!=(const StandardRendererState state)
	{
		if (DepthTest != state.DepthTest)
			return true;
		if (DepthWrite != state.DepthWrite)
			return true;
		if (Distortion != state.Distortion)
			return true;
		if (DistortionIntensity != state.DistortionIntensity)
			return true;
		if (AlphaBlend != state.AlphaBlend)
			return true;
		if (CullingType != state.CullingType)
			return true;
		if (TextureFilter1 != state.TextureFilter1)
			return true;
		if (TextureWrap1 != state.TextureWrap1)
			return true;
		if (TextureFilter2 != state.TextureFilter2)
			return true;
		if (TextureWrap2 != state.TextureWrap2)
			return true;
		if (TextureFilter3 != state.TextureFilter3)
			return true;
		if (TextureWrap3 != state.TextureWrap3)
			return true;
		if (TextureFilter4 != state.TextureFilter4)
			return true;
		if (TextureWrap4 != state.TextureWrap4)
			return true;
		if (TextureFilter5 != state.TextureFilter5)
			return true;
		if (TextureWrap5 != state.TextureWrap5)
			return true;
		if (TextureFilter6 != state.TextureFilter6)
			return true;
		if (TextureWrap6 != state.TextureWrap6)
			return true;
		if (TextureFilter7 != state.TextureFilter7)
			return true;
		if (TextureWrap7 != state.TextureWrap7)
			return true;
		if (TexturePtr != state.TexturePtr)
			return true;
		if (NormalTexturePtr != state.NormalTexturePtr)
			return true;
		if (AlphaTexturePtr != state.AlphaTexturePtr)
			return true;
		if (UVDistortionTexturePtr != state.UVDistortionTexturePtr)
			return true;
		if (BlendTexturePtr != state.BlendTexturePtr)
			return true;
		if (BlendAlphaTexturePtr != state.BlendAlphaTexturePtr)
			return true;
		if (BlendUVDistortionTexturePtr != state.BlendUVDistortionTexturePtr)
			return true;

		if (EnableInterpolation != state.EnableInterpolation)
			return true;
		if (UVLoopType != state.UVLoopType)
			return true;
		if (InterpolationType != state.InterpolationType)
			return true;
		if (FlipbookDivideX != state.FlipbookDivideX)
			return true;
		if (FlipbookDivideY != state.FlipbookDivideY)
			return true;

		if (UVDistortionIntensity != state.UVDistortionIntensity)
			return true;
		if (TextureBlendType != state.TextureBlendType)
			return true;
		if (BlendUVDistortionIntensity != state.BlendUVDistortionIntensity)
			return true;
		if (EmissiveScaling != state.EmissiveScaling)
			return true;

		if (EdgeThreshold != state.EdgeThreshold)
			return true;
		if (EdgeColor[0] != state.EdgeColor[0] ||
			EdgeColor[1] != state.EdgeColor[1] ||
			EdgeColor[2] != state.EdgeColor[2] ||
			EdgeColor[3] != state.EdgeColor[3])
			return true;
		if (EdgeColorScaling != state.EdgeColorScaling)
			return true;

		if (IsAlphaCuttoffEnabled != state.IsAlphaCuttoffEnabled)
			return true;
		if (MaterialType != state.MaterialType)
			return true;
		if (MaterialPtr != state.MaterialPtr)
			return true;
		if (MaterialUniformCount != state.MaterialUniformCount)
			return true;
		if (MaterialTextureCount != state.MaterialTextureCount)
			return true;
		if (Refraction != state.Refraction)
			return true;

		for (int32_t i = 0; i < state.MaterialUniformCount; i++)
		{
			if (MaterialUniforms[i] != state.MaterialUniforms[i])
				return true;
		}

		for (int32_t i = 0; i < state.MaterialTextureCount; i++)
		{
			if (MaterialTextures[i] != state.MaterialTextures[i])
				return true;
		}

		if (CustomData1Count != state.CustomData1Count)
			return true;

		if (CustomData1Count != state.CustomData1Count)
			return true;

		return false;
	}

	void CopyMaterialFromParameterToState(Effekseer::Effect* effect,
										  Effekseer::MaterialParameter* materialParam,
										  int32_t colorTextureIndex,
										  int32_t texture2Index
										  ,
										  int32_t texture3Index,
										  int32_t texture4Index,
										  int32_t texture5Index,
										  int32_t texture6Index,
										  int32_t texture7Index
	)
	{
		if (materialParam != nullptr)
		{
			if (materialParam->MaterialIndex >= 0 && effect->GetMaterial(materialParam->MaterialIndex) != nullptr)
			{
				MaterialPtr = effect->GetMaterial(materialParam->MaterialIndex);

				CustomData1Count = MaterialPtr->CustomData1;
				CustomData2Count = MaterialPtr->CustomData2;

				MaterialUniformCount =
					static_cast<int32_t>(Effekseer::Min(materialParam->MaterialUniforms.size(), MaterialUniforms.size()));
				for (size_t i = 0; i < MaterialUniformCount; i++)
				{
					MaterialUniforms[i] = materialParam->MaterialUniforms[i];
				}

				MaterialTextureCount =
					static_cast<int32_t>(Effekseer::Min(materialParam->MaterialTextures.size(), MaterialTextures.size()));
				for (size_t i = 0; i < MaterialTextureCount; i++)
				{
					if (materialParam->MaterialTextures[i].Type == 1)
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							MaterialTextures[i] = effect->GetNormalImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							MaterialTextures[i] = nullptr;
						}
					}
					else
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							MaterialTextures[i] = effect->GetColorImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							MaterialTextures[i] = nullptr;
						}
					}
				}
			}
		}
		else
		{
			if (colorTextureIndex >= 0)
			{
				if (Distortion)
				{
					TexturePtr = effect->GetDistortionImage(colorTextureIndex);
				}
				else
				{
					TexturePtr = effect->GetColorImage(colorTextureIndex);
				}
			}
			else
			{
				TexturePtr = nullptr;
			}

			if (texture2Index >= 0)
			{
				NormalTexturePtr = effect->GetNormalImage(texture2Index);
			}
			else
			{
				NormalTexturePtr = nullptr;
			}

			if (texture3Index >= 0)
			{
				if (Distortion)
				{
					AlphaTexturePtr = effect->GetDistortionImage(texture3Index);
				}
				else
				{
					AlphaTexturePtr = effect->GetColorImage(texture3Index);
				}
			}
			else
			{
				AlphaTexturePtr = nullptr;
			}

			if (texture4Index >= 0)
			{
				if (Distortion)
				{
					UVDistortionTexturePtr = effect->GetDistortionImage(texture4Index);
				}
				else
				{
					UVDistortionTexturePtr = effect->GetColorImage(texture4Index);
				}
			}
			else
			{
				UVDistortionTexturePtr = nullptr;
			}

			if (texture5Index >= 0)
			{
				if (Distortion)
				{
					BlendTexturePtr = effect->GetDistortionImage(texture5Index);
				}
				else
				{
					BlendTexturePtr = effect->GetColorImage(texture5Index);
				}
			}
			else
			{
				BlendTexturePtr = nullptr;
			}

			if (texture6Index >= 0)
			{
				if (Distortion)
				{
					BlendAlphaTexturePtr = effect->GetDistortionImage(texture6Index);
				}
				else
				{
					BlendAlphaTexturePtr = effect->GetColorImage(texture6Index);
				}
			}
			else
			{
				BlendAlphaTexturePtr = nullptr;
			}

			if (texture7Index >= 0)
			{
				if (Distortion)
				{
					BlendUVDistortionTexturePtr = effect->GetDistortionImage(texture7Index);
				}
				else
				{
					BlendUVDistortionTexturePtr = effect->GetColorImage(texture7Index);
				}
			}
			else
			{
				BlendUVDistortionTexturePtr = nullptr;
			}

			Refraction = false;
			CustomData1Count = 0;
			CustomData2Count = 0;
		}
	}
};

template <typename RENDERER, typename SHADER>
class StandardRenderer
{
private:
	enum class RenderingMode
	{
		Unlit,
		Lit,
		BackDistortion,
		AdvancedUnlit,
		AdvancedLit,
		AdvancedBackDistortion,
		Material,
	};

	RENDERER* m_renderer;

	Effekseer::TextureData* m_texture;

	StandardRendererState m_state;

	std::vector<uint8_t> vertexCaches;
	int32_t squareMaxSize_ = 0;
	RenderingMode renderingMode_ = RenderingMode::Unlit;

	struct VertexConstantBuffer
	{
		Effekseer::Matrix44 constantVSBuffer[2];
		float uvInversed[4];

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float enableInterpolation;
					float loopType;
					float divideX;
					float divideY;
				};
			};
		} flipbookParameter;
	};

	struct PixelConstantBuffer
	{
		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float enableInterpolation;
					float interpolationType;
				};
			};
		} flipbookParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float intensity;
					float blendIntensity;
					float uvInversed[2];
				};
			};
		} uvDistortionParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float blendType;
				};
			};
		} blendTextureParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float emissiveScaling;
				};
			};
		};

		struct
		{
			float Color[4];

			union
			{
				float Buffer[4];

				struct
				{
					float Threshold;
					float ColorScaling;
				};
			};
		} edgeParameter;
	};

	struct DistortionPixelConstantBuffer
	{
		float scale[4];
		float uvInversed[4];

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float enableInterpolation;
					float interpolationType;
				};
			};
		} flipbookParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float intensity;
					float blendIntensity;
					float uvInversed[2];
				};
			};
		} uvDistortionParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float blendType;
				};
			};
		} blendTextureParameter;
	};

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	void VectorToFloat4(const ::Effekseer::Vec3f& v, float fc[4])
	{
		::Effekseer::SIMD4f::Store3(fc, v.s);
		fc[3] = 1.0f;
	}

public:
	StandardRenderer(RENDERER* renderer)
		: squareMaxSize_(renderer->GetSquareMaxCount())
	{
		m_renderer = renderer;
		vertexCaches.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
	}

	virtual ~StandardRenderer()
	{
	}

	int32_t CalculateCurrentStride() const
	{
		size_t stride = 0;
		if (renderingMode_ == RenderingMode::Material)
		{
			stride = sizeof(DynamicVertex);
			stride += (m_state.CustomData1Count + m_state.CustomData2Count) * sizeof(float);
		}
		else if (renderingMode_ == RenderingMode::Lit)
		{
			stride = sizeof(LightingVertex);
		}
		else if (renderingMode_ == RenderingMode::BackDistortion)
		{
			stride = sizeof(VertexDistortion);
		}
		else if (renderingMode_ == RenderingMode::Unlit)
		{
			stride = sizeof(SimpleVertex);
		}
		else if (renderingMode_ == RenderingMode::AdvancedLit)
		{
			stride = sizeof(AdvancedLightingVertex);
		}
		else if (renderingMode_ == RenderingMode::AdvancedBackDistortion)
		{
			stride = sizeof(AdvancedVertexDistortion);
		}
		else if (renderingMode_ == RenderingMode::AdvancedUnlit)
		{
			stride = sizeof(AdvancedSimpleVertex);
		}

		return static_cast<int32_t>(stride);
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if (m_state != state)
		{
			Rendering();
		}

		m_state = state;

		bool isAdvanced = m_state.IsAdvanced();

		if (m_state.MaterialPtr != nullptr && !m_state.MaterialPtr->IsSimpleVertex)
		{
			renderingMode_ = RenderingMode::Material;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedLit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedBackDistortion;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Default && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedUnlit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			renderingMode_ = RenderingMode::Lit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
		{
			renderingMode_ = RenderingMode::BackDistortion;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Default)
		{
			renderingMode_ = RenderingMode::Unlit;
		}
		else
		{
			assert(0);
		}
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int& stride, void*& data)
	{
		stride = CalculateCurrentStride();

		{
			int32_t renderVertexMaxSize = squareMaxSize_ * stride * 4;

			if (count * stride + (int32_t)vertexCaches.size() > renderVertexMaxSize)
			{
				Rendering();
			}

			auto old = vertexCaches.size();
			vertexCaches.resize(count * stride + vertexCaches.size());
			data = (vertexCaches.data() + old);
		}
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();

		// It is always initialized with the next drawing.
		m_state.TexturePtr = (Effekseer::TextureData*)0x1;
		m_state.NormalTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.AlphaTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.UVDistortionTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendAlphaTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendUVDistortionTexturePtr = (Effekseer::TextureData*)0x1;
	}

	const StandardRendererState& GetState()
	{
		return m_state;
	}

	void Rendering(const Effekseer::Mat44f& mCamera, const Effekseer::Mat44f& mProj)
	{
		if (vertexCaches.size() == 0)
			return;

		int32_t stride = CalculateCurrentStride();

		int32_t passNum = 1;

		if (m_state.MaterialPtr != nullptr)
		{
			if (m_state.MaterialPtr->RefractionUserPtr != nullptr)
			{
				// refraction and standard
				passNum = 2;
			}
		}

		for (int32_t passInd = 0; passInd < passNum; passInd++)
		{
			int32_t offset = 0;

			while (true)
			{
				// only sprite
				int32_t renderBufferSize = (int32_t)vertexCaches.size() - offset;

				int32_t renderVertexMaxSize = squareMaxSize_ * stride * 4;

				if (renderBufferSize > renderVertexMaxSize)
				{
					renderBufferSize =
						(Effekseer::Min(renderVertexMaxSize, (int32_t)vertexCaches.size() - offset) / (stride * 4)) * (stride * 4);
				}

				Rendering_(mCamera, mProj, offset, renderBufferSize, stride, passInd);

				offset += renderBufferSize;

				if (offset == vertexCaches.size())
					break;
			}
		}

		vertexCaches.clear();
	}

	void Rendering_(const Effekseer::Mat44f& mCamera,
					const Effekseer::Mat44f& mProj,
					int32_t bufferOffset,
					int32_t bufferSize,
					int32_t stride,
					int32_t renderPass)
	{
		bool isBackgroundRequired = false;

		isBackgroundRequired |= m_state.Distortion;
		isBackgroundRequired |= (m_state.MaterialPtr != nullptr && m_state.MaterialPtr->IsRefractionRequired && renderPass == 0);

		if (isBackgroundRequired)
		{
			auto callback = m_renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting())
				{
					return;
				}
			}
		}

		if (isBackgroundRequired && m_renderer->GetBackground() == 0)
		{
			return;
		}

		int32_t vertexSize = bufferSize;
		int32_t vbOffset = 0;
		{
			VertexBufferBase* vb = m_renderer->GetVertexBuffer();

			void* vbData = nullptr;

			if (vb->RingBufferLock(vertexSize, vbOffset, vbData, stride * 4))
			{
				assert(vbData != nullptr);
				memcpy(vbData, vertexCaches.data() + bufferOffset, vertexSize);
				vb->Unlock();
			}
			else
			{
				return;
			}
		}

		SHADER* shader_ = nullptr;

		bool distortion = m_state.Distortion;
		bool renderDistortedBackground = false;

		if (m_state.MaterialPtr != nullptr)
		{
			if (m_state.MaterialPtr->IsRefractionRequired)
			{
				if (renderPass == 0)
				{
					if (m_renderer->GetBackground() == 0)
					{
						return;
					}

					shader_ = (SHADER*)m_state.MaterialPtr->RefractionUserPtr;
					renderDistortedBackground = true;
				}
				else
				{
					shader_ = (SHADER*)m_state.MaterialPtr->UserPtr;
				}
			}
			else
			{
				shader_ = (SHADER*)m_state.MaterialPtr->UserPtr;
			}

			// validate
			if (shader_ == nullptr)
				return;

			if (m_state.MaterialPtr->UniformCount != m_state.MaterialUniformCount)
				return;

			if (m_state.MaterialPtr->TextureCount != m_state.MaterialTextureCount)
				return;
		}
		else
		{
			auto isAdvanced = m_state.IsAdvanced();

			StandardRendererShaderType type;
			if (m_state.MaterialType == Effekseer::RendererMaterialType::Default && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedUnlit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedLit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedBackDistortion;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Default)
			{
				type = StandardRendererShaderType::Unlit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				type = StandardRendererShaderType::Lit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{
				type = StandardRendererShaderType::BackDistortion;
			}

			shader_ = m_renderer->GetShader(type);
		}

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = m_state.DepthTest;
		state.DepthWrite = m_state.DepthWrite;
		state.CullingType = m_state.CullingType;
		state.AlphaBlend = m_state.AlphaBlend;

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		m_renderer->BeginShader(shader_);

		if (m_state.MaterialPtr != nullptr)
		{
			std::array<Effekseer::TextureData*, 16> textures;
			textures.fill(nullptr);
			int32_t textureCount = Effekseer::Min(m_state.MaterialTextureCount, textures.size() - 1);

			if (m_state.MaterialTextureCount > 0)
			{
				auto textureSize = static_cast<int32_t>(textures.size());

				for (size_t i = 0; i < Effekseer::Min(m_state.MaterialTextureCount, textureSize); i++)
				{
					textures[i] = m_state.MaterialTextures[i];
					state.TextureFilterTypes[i] = Effekseer::TextureFilterType::Linear;
					state.TextureWrapTypes[i] = m_state.MaterialPtr->TextureWrapTypes[i];
				}
			}

			if (m_renderer->GetBackground() != 0)
			{
				textures[textureCount] = m_renderer->GetBackground();
				state.TextureFilterTypes[textureCount] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[textureCount] = Effekseer::TextureWrapType::Clamp;
				textureCount += 1;
			}

			m_renderer->SetTextures(shader_, textures.data(), textureCount);
		}
		else
		{
			state.TextureFilterTypes[0] = m_state.TextureFilter1;
			state.TextureWrapTypes[0] = m_state.TextureWrap1;

			if (distortion)
			{
				state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;

				state.TextureFilterTypes[2] = m_state.TextureFilter3;
				state.TextureWrapTypes[2] = m_state.TextureWrap3;

				state.TextureFilterTypes[3] = m_state.TextureFilter4;
				state.TextureWrapTypes[3] = m_state.TextureWrap4;

				state.TextureFilterTypes[4] = m_state.TextureFilter5;
				state.TextureWrapTypes[4] = m_state.TextureWrap5;

				state.TextureFilterTypes[5] = m_state.TextureFilter6;
				state.TextureWrapTypes[5] = m_state.TextureWrap6;

				state.TextureFilterTypes[6] = m_state.TextureFilter7;
				state.TextureWrapTypes[6] = m_state.TextureWrap7;
			}
			else
			{
				if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
				{
					state.TextureFilterTypes[1] = m_state.TextureFilter2;
					state.TextureWrapTypes[1] = m_state.TextureWrap2;

					state.TextureFilterTypes[2] = m_state.TextureFilter3;
					state.TextureWrapTypes[2] = m_state.TextureWrap3;

					state.TextureFilterTypes[3] = m_state.TextureFilter4;
					state.TextureWrapTypes[3] = m_state.TextureWrap4;

					state.TextureFilterTypes[4] = m_state.TextureFilter5;
					state.TextureWrapTypes[4] = m_state.TextureWrap5;

					state.TextureFilterTypes[5] = m_state.TextureFilter6;
					state.TextureWrapTypes[5] = m_state.TextureWrap6;

					state.TextureFilterTypes[6] = m_state.TextureFilter7;
					state.TextureWrapTypes[6] = m_state.TextureWrap7;
				}
				else
				{
					state.TextureFilterTypes[1] = m_state.TextureFilter3;
					state.TextureWrapTypes[1] = m_state.TextureWrap3;

					state.TextureFilterTypes[2] = m_state.TextureFilter4;
					state.TextureWrapTypes[2] = m_state.TextureWrap4;

					state.TextureFilterTypes[3] = m_state.TextureFilter5;
					state.TextureWrapTypes[3] = m_state.TextureWrap5;

					state.TextureFilterTypes[4] = m_state.TextureFilter6;
					state.TextureWrapTypes[4] = m_state.TextureWrap6;

					state.TextureFilterTypes[5] = m_state.TextureFilter7;
					state.TextureWrapTypes[5] = m_state.TextureWrap7;
				}
			}

			std::array<Effekseer::TextureData*, 7> textures;
			textures.fill(nullptr);

			if (m_state.TexturePtr != nullptr && m_state.TexturePtr != (Effekseer::TextureData*)0x01 &&
				m_renderer->GetRenderMode() == Effekseer::RenderMode::Normal)
			{
				textures[0] = m_state.TexturePtr;
			}
			else
			{
				textures[0] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
			}

			if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
			{
				textures[1] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);

				if (m_state.NormalTexturePtr != nullptr && m_state.NormalTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[1] = m_state.NormalTexturePtr;
				}
				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[6] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[6] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 7);
			}
			else if (distortion)
			{
				textures[1] = m_renderer->GetBackground();

				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[6] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[6] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 7);
			}
			else
			{
				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[1] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[1] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 6);
			}
		}

		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;
		std::array<float, 4> uvInversedMaterial;

		if (m_renderer->GetTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversed[0] = 1.0f;
			uvInversed[1] = -1.0f;
		}
		else
		{
			uvInversed[0] = 0.0f;
			uvInversed[1] = 1.0f;
		}

		if (m_renderer->GetBackgroundTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversedBack[0] = 1.0f;
			uvInversedBack[1] = -1.0f;
		}
		else
		{
			uvInversedBack[0] = 0.0f;
			uvInversedBack[1] = 1.0f;
		}

		uvInversedMaterial[0] = uvInversed[0];
		uvInversedMaterial[1] = uvInversed[1];
		uvInversedMaterial[2] = uvInversedBack[0];
		uvInversedMaterial[3] = uvInversedBack[1];

		if (m_state.MaterialPtr != nullptr)
		{
			Effekseer::Matrix44 mstCamera = ToStruct(mCamera);
			Effekseer::Matrix44 mstProj = ToStruct(mProj);

			// camera
			float cameraPosition[4];
			::Effekseer::Vec3f cameraPosition3 = m_renderer->GetCameraPosition();
			VectorToFloat4(cameraPosition3, cameraPosition);
			// time
			std::array<float, 4> predefined_uniforms;
			predefined_uniforms.fill(0.5f);
			predefined_uniforms[0] = m_renderer->GetTime();

			// vs
			int32_t vsOffset = 0;
			m_renderer->SetVertexBufferToShader(&mstCamera, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(&mstProj, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			m_renderer->SetVertexBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			m_renderer->SetVertexBufferToShader(cameraPosition, sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			for (size_t i = 0; i < m_state.MaterialUniformCount; i++)
			{
				m_renderer->SetVertexBufferToShader(m_state.MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
				vsOffset += (sizeof(float) * 4);
			}

			// ps
			int32_t psOffset = 0;
			m_renderer->SetPixelBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			// shader model
			if (m_state.MaterialPtr->ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{

				float lightDirection[4];
				float lightColor[4];
				float lightAmbientColor[4];

				::Effekseer::Vec3f lightDirection3 = m_renderer->GetLightDirection();
				lightDirection3 = lightDirection3.Normalize();
				VectorToFloat4(lightDirection3, lightDirection);
				ColorToFloat4(m_renderer->GetLightColor(), lightColor);
				ColorToFloat4(m_renderer->GetLightAmbientColor(), lightAmbientColor);

				m_renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);

				m_renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);

				m_renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}

			// refraction
			if (m_state.MaterialPtr->RefractionUserPtr != nullptr && renderPass == 0)
			{
				auto mat = m_renderer->GetCameraMatrix();
				m_renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
				psOffset += (sizeof(float) * 16);
			}

			for (size_t i = 0; i < m_state.MaterialUniformCount; i++)
			{
				m_renderer->SetPixelBufferToShader(m_state.MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			VertexConstantBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(m_state.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(m_state.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(m_state.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			// ps
			int32_t psOffset = 0;
			float lightDirection[4];
			float lightColor[4];
			float lightAmbientColor[4];

			::Effekseer::Vec3f lightDirection3 = m_renderer->GetLightDirection();
			lightDirection3 = lightDirection3.Normalize();
			VectorToFloat4(lightDirection3, lightDirection);
			ColorToFloat4(m_renderer->GetLightColor(), lightColor);
			ColorToFloat4(m_renderer->GetLightAmbientColor(), lightAmbientColor);

			m_renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			PixelConstantBuffer pcb;
			pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

			pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
			pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
			pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
			pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

			pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

			pcb.emissiveScaling = static_cast<float>(m_state.EmissiveScaling);

			ColorToFloat4(Effekseer::Color(m_state.EdgeColor[0], m_state.EdgeColor[1], m_state.EdgeColor[2], m_state.EdgeColor[3]), pcb.edgeParameter.Color);
			pcb.edgeParameter.Threshold = m_state.EdgeThreshold;
			pcb.edgeParameter.ColorScaling = static_cast<float>(m_state.EdgeColorScaling);

			m_renderer->SetPixelBufferToShader(&pcb.flipbookParameter, sizeof(PixelConstantBuffer), psOffset);
		}
		else
		{
			VertexConstantBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];
			vcb.uvInversed[2] = 0.0f;
			vcb.uvInversed[3] = 0.0f;

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(m_state.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(m_state.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(m_state.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			if (distortion)
			{
				DistortionPixelConstantBuffer pcb;
				pcb.scale[0] = m_state.DistortionIntensity;
				pcb.uvInversed[0] = uvInversedBack[0];
				pcb.uvInversed[1] = uvInversedBack[1];

				pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
				pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

				pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
				pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
				pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
				pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

				pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(DistortionPixelConstantBuffer), 0);
			}
			else
			{
				PixelConstantBuffer pcb;
				pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
				pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

				pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
				pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
				pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
				pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

				pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

				pcb.emissiveScaling = static_cast<float>(m_state.EmissiveScaling);

				ColorToFloat4(Effekseer::Color(m_state.EdgeColor[0], m_state.EdgeColor[1], m_state.EdgeColor[2], m_state.EdgeColor[3]), pcb.edgeParameter.Color);
				pcb.edgeParameter.Threshold = m_state.EdgeThreshold;
				pcb.edgeParameter.ColorScaling = static_cast<float>(m_state.EdgeColorScaling);

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(PixelConstantBuffer), 0);
			}
		}

		shader_->SetConstantBuffer();

		m_renderer->GetRenderState()->Update(distortion);

		assert(vbOffset % stride == 0);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), stride);
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexSize / stride / 4, vbOffset / stride);

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();
	}

	void Rendering()
	{
		Rendering(m_renderer->GetCameraMatrix(), m_renderer->GetProjectionMatrix());
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_STANDARD_RENDERER_H__


#ifndef __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__

#include "EffekseerNative.h"
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <vector>


namespace EffekseerRenderer
{

typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <int MODEL_COUNT>
struct ModelRendererVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];

	void SetModelFlipbookParameter(float enableInterpolation, float loopType, float divideX, float divideY)
	{
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
	}

	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];
};

template <int MODEL_COUNT>
struct ModelRendererAdvancedVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];

	float ModelAlphaUV[MODEL_COUNT][4];

	float ModelUVDistortionUV[MODEL_COUNT][4];

	float ModelBlendUV[MODEL_COUNT][4];

	float ModelBlendAlphaUV[MODEL_COUNT][4];

	float ModelBlendUVDistortionUV[MODEL_COUNT][4];

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float EnableInterpolation;
				float LoopType;
				float DivideX;
				float DivideY;
			};
		};
	} ModelFlipbookParameter;

	float ModelFlipbookIndexAndNextRate[MODEL_COUNT][4];

	float ModelAlphaThreshold[MODEL_COUNT][4];

	void SetModelFlipbookParameter(float enableInterpolation, float loopType, float divideX, float divideY)
	{
		ModelFlipbookParameter.EnableInterpolation = enableInterpolation;
		ModelFlipbookParameter.LoopType = loopType;
		ModelFlipbookParameter.DivideX = divideX;
		ModelFlipbookParameter.DivideY = divideY;
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
		ModelAlphaUV[index][0] = x;
		ModelAlphaUV[index][1] = y;
		ModelAlphaUV[index][2] = w;
		ModelAlphaUV[index][3] = h;
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
		ModelUVDistortionUV[index][0] = x;
		ModelUVDistortionUV[index][1] = y;
		ModelUVDistortionUV[index][2] = w;
		ModelUVDistortionUV[index][3] = h;
	}

	void SetModelBlendUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendUV[index][0] = x;
		ModelBlendUV[index][1] = y;
		ModelBlendUV[index][2] = w;
		ModelBlendUV[index][3] = h;
	}

	void SetModelBlendAlphaUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendAlphaUV[index][0] = x;
		ModelBlendAlphaUV[index][1] = y;
		ModelBlendAlphaUV[index][2] = w;
		ModelBlendAlphaUV[index][3] = h;
	}

	void SetModelBlendUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendUVDistortionUV[index][0] = x;
		ModelBlendUVDistortionUV[index][1] = y;
		ModelBlendUVDistortionUV[index][2] = w;
		ModelBlendUVDistortionUV[index][3] = h;
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
		ModelFlipbookIndexAndNextRate[index][0] = value;
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
		ModelAlphaThreshold[index][0] = value;
	}

	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];
};

template <int MODEL_COUNT>
struct ModelRendererMaterialVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];
	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];

	void SetModelFlipbookParameter(float enableInterpolation, float loopType, float divideX, float divideY)
	{
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelBlendAlphaUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUVDistortionUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
	}
};

struct ModelRendererPixelConstantBuffer
{
	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];

	void SetModelFlipbookParameter(float enableInterpolation, float interpolationType)
	{
	}

	void SetModelUVDistortionParameter(float intensity, float blendIntensity, const std::array<float, 2>& uvInversed)
	{
	}

	void SetModelBlendTextureParameter(float blendType)
	{
	}

	void SetCameraFrontDirection(float x, float y, float z)
	{
	}

	void SetFalloffParameter(float enable, float colorBlendType, float pow, const std::array<float, 4>& beginColor, const std::array<float, 4>& endColor)
	{
	}

	void SetEmissiveScaling(float emissiveScaling)
	{
	}

	void SetEdgeColor(const std::array<float, 4>& edgeColor)
	{
	}

	void SetEdgeParameter(float threshold, float colorScaling)
	{
	}
};

struct ModelRendererAdvancedPixelConstantBuffer
{
	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float EnableInterpolation;
				float InterpolationType;
			};
		};
	} ModelFlipbookParameter;

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float Intensity;
				float BlendIntensity;
				float UVInversed[2];
			};
		};
	} ModelUVDistortionParameter;

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float BlendType;
			};
		};
	} ModelBlendTextureParameter;

	float CameraFrontDirection[4];

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float Enable;
				float ColorBlendType;
				float Pow;
			};
		};

		float BeginColor[4];
		float EndColor[4];

	} FalloffParameter;

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float EmissiveScaling;
			};
		};
	};

	float EdgeColor[4];

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float Threshold;
				float ColorScaling;
			};
		};
	} EdgeParameter;

	void SetModelFlipbookParameter(float enableInterpolation, float interpolationType)
	{
		ModelFlipbookParameter.EnableInterpolation;
		ModelFlipbookParameter.InterpolationType;
	}

	void SetModelUVDistortionParameter(float intensity, float blendIntensity, const std::array<float, 2>& uvInversed)
	{
		ModelUVDistortionParameter.Intensity = intensity;
		ModelUVDistortionParameter.BlendIntensity = blendIntensity;
		ModelUVDistortionParameter.UVInversed[0] = uvInversed[0];
		ModelUVDistortionParameter.UVInversed[1] = uvInversed[1];
	}

	void SetModelBlendTextureParameter(float blendType)
	{
		ModelBlendTextureParameter.BlendType = blendType;
	}

	void SetCameraFrontDirection(float x, float y, float z)
	{
		CameraFrontDirection[0] = x;
		CameraFrontDirection[1] = y;
		CameraFrontDirection[2] = z;
		CameraFrontDirection[3] = 0.0f;
	}

	void SetFalloffParameter(float enable, float colorBlendType, float pow, const std::array<float, 4>& beginColor, const std::array<float, 4>& endColor)
	{
		FalloffParameter.Enable = enable;
		FalloffParameter.ColorBlendType = colorBlendType;
		FalloffParameter.Pow = pow;

		for (size_t i = 0; i < 4; i++)
		{
			FalloffParameter.BeginColor[i] = beginColor[i];
		}

		for (size_t i = 0; i < 4; i++)
		{
			FalloffParameter.EndColor[i] = endColor[i];
		}
	}

	void SetEmissiveScaling(float emissiveScaling)
	{
		EmissiveScaling = emissiveScaling;
	}

	void SetEdgeColor(const std::array<float, 4>& edgeColor)
	{
		for (size_t i = 0; i < 4; i++)
		{
			EdgeColor[i] = edgeColor[i];
		}
	}

	void SetEdgeParameter(float threshold, float colorScaling)
	{
		EdgeParameter.Threshold = threshold;
		EdgeParameter.ColorScaling = colorScaling;
	}
};


struct ModelRendererDistortionPixelConstantBuffer
{
	float DistortionIntencity[4];
	float UVInversedBack[4];

	//! unused in none advanced renderer
	float Interpolation[4];
	float UVDistortion[4];
	float TextureBlendType[4];
};

class ModelRendererBase : public ::Effekseer::ModelRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	struct KeyValue
	{
		float Key;
		int Value;
	};

	std::vector<KeyValue> keyValues_;

	std::vector<Effekseer::Matrix44> matrixesSorted_;
	std::vector<Effekseer::RectF> uvSorted_;
	std::vector<Effekseer::RectF> alphaUVSorted_;
	std::vector<Effekseer::RectF> uvDistortionUVSorted_;
	std::vector<Effekseer::RectF> blendUVSorted_;
	std::vector<Effekseer::RectF> blendAlphaUVSorted_;
	std::vector<Effekseer::RectF> blendUVDistortionUVSorted_;
	std::vector<float> flipbookIndexAndNextRateSorted_;
	std::vector<float> alphaThresholdSorted_;
	std::vector<float> viewOffsetDistanceSorted_;

	std::vector<Effekseer::Color> colorsSorted_;
	std::vector<int32_t> timesSorted_;
	std::vector<std::array<float, 4>> customData1Sorted_;
	std::vector<std::array<float, 4>> customData2Sorted_;

	std::vector<Effekseer::Matrix44> m_matrixes;
	std::vector<Effekseer::RectF> m_uv;

	std::vector<Effekseer::RectF> m_alphaUV;
	std::vector<Effekseer::RectF> m_uvDistortionUV;
	std::vector<Effekseer::RectF> m_blendUV;
	std::vector<Effekseer::RectF> m_blendAlphaUV;
	std::vector<Effekseer::RectF> m_blendUVDistortionUV;
	std::vector<float> m_flipbookIndexAndNextRate;
	std::vector<float> m_alphaThreshold;
	std::vector<float> m_viewOffsetDistance;

	std::vector<Effekseer::Color> m_colors;
	std::vector<int32_t> m_times;
	std::vector<std::array<float, 4>> customData1_;
	std::vector<std::array<float, 4>> customData2_;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	//! TODO : refactor
	bool isAdvanced_ = false;

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	std::array<float, 4> ColorToFloat4(::Effekseer::Color color)
	{
		std::array<float, 4> fc;
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
		return fc;
	}

	void VectorToFloat4(const ::Effekseer::Vec3f& v, float fc[4])
	{
		::Effekseer::SIMD4f::Store3(fc, v.s);
		fc[3] = 1.0f;
	}

	ModelRendererBase()
	{
	}

	template <typename RENDERER>
	void GetInversedFlags(RENDERER* renderer, std::array<float, 4>& uvInversed, std::array<float, 4>& uvInversedBack)
	{
		if (renderer->GetTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversed[0] = 1.0f;
			uvInversed[1] = -1.0f;
		}
		else
		{
			uvInversed[0] = 0.0f;
			uvInversed[1] = 1.0f;
		}

		if (renderer->GetBackgroundTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversedBack[0] = 1.0f;
			uvInversedBack[1] = -1.0f;
		}
		else
		{
			uvInversedBack[0] = 0.0f;
			uvInversedBack[1] = 1.0f;
		}
	}

	template <typename RENDERER>
	void SortTemporaryValues(RENDERER* renderer, const efkModelNodeParam& param)
	{
		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None)
		{
			keyValues_.resize(m_matrixes.size());
			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				efkVector3D t(m_matrixes[i].Values[3][0], m_matrixes[i].Values[3][1], m_matrixes[i].Values[3][2]);

				auto frontDirection = renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection.Z = -frontDirection.Z;
				}

				keyValues_[i].Key = Effekseer::Vec3f::Dot(t, frontDirection);
				keyValues_[i].Value = static_cast<int32_t>(i);
			}

			if (param.DepthParameterPtr->ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key < b.Key; });
			}
			else
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key > b.Key; });
			}

			matrixesSorted_.resize(m_matrixes.size());
			uvSorted_.resize(m_matrixes.size());
			alphaUVSorted_.resize(m_matrixes.size());
			uvDistortionUVSorted_.resize(m_matrixes.size());
			blendUVSorted_.resize(m_matrixes.size());
			blendAlphaUVSorted_.resize(m_matrixes.size());
			flipbookIndexAndNextRateSorted_.resize(m_matrixes.size());
			alphaThresholdSorted_.resize(m_matrixes.size());
			viewOffsetDistanceSorted_.resize(m_matrixes.size());
			colorsSorted_.resize(m_matrixes.size());
			timesSorted_.resize(m_matrixes.size());

			if (customData1Count_ > 0)
			{
				customData1Sorted_.resize(m_matrixes.size());
			}

			if (customData2Count_ > 0)
			{
				customData2Sorted_.resize(m_matrixes.size());
			}

			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				matrixesSorted_[keyValues_[i].Value] = m_matrixes[i];
				uvSorted_[keyValues_[i].Value] = m_uv[i];
				alphaUVSorted_[keyValues_[i].Value] = m_alphaUV[i];
				uvDistortionUVSorted_[keyValues_[i].Value] = m_uvDistortionUV[i];
				blendUVSorted_[keyValues_[i].Value] = m_blendUV[i];
				blendAlphaUVSorted_[keyValues_[i].Value] = m_blendAlphaUV[i];
				blendUVDistortionUVSorted_[keyValues_[i].Value] = m_blendUVDistortionUV[i];
				flipbookIndexAndNextRateSorted_[keyValues_[i].Value] = m_flipbookIndexAndNextRate[i];
				alphaThresholdSorted_[keyValues_[i].Value] = m_alphaThreshold[i];
				viewOffsetDistanceSorted_[keyValues_[i].Value] = m_viewOffsetDistance[i];
				colorsSorted_[keyValues_[i].Value] = m_colors[i];
				timesSorted_[keyValues_[i].Value] = m_times[i];
			}

			if (customData1Count_ > 0)
			{
				for (size_t i = 0; i < keyValues_.size(); i++)
				{
					customData1Sorted_[keyValues_[i].Value] = customData1_[i];
				}
			}

			if (customData2Count_ > 0)
			{
				for (size_t i = 0; i < keyValues_.size(); i++)
				{
					customData2Sorted_[keyValues_[i].Value] = customData2_[i];
				}
			}

			m_matrixes = matrixesSorted_;
			m_uv = uvSorted_;
			m_alphaUV = alphaUVSorted_;
			m_uvDistortionUV = uvDistortionUVSorted_;
			m_blendUV = blendUVSorted_;
			m_blendAlphaUV = blendAlphaUVSorted_;
			m_blendUVDistortionUV = blendUVDistortionUVSorted_;
			m_flipbookIndexAndNextRate = flipbookIndexAndNextRateSorted_;
			m_alphaThreshold = alphaThresholdSorted_;
			m_viewOffsetDistance = viewOffsetDistanceSorted_;
			m_colors = colorsSorted_;
			m_times = timesSorted_;
			customData1_ = customData1Sorted_;
			customData2_ = customData2Sorted_;
		}
	}

	template <typename RENDERER, typename SHADER, int InstanceCount>
	void StoreFileUniform(RENDERER* renderer,
						  SHADER* shader_,
						  Effekseer::MaterialData* material,
						  Effekseer::MaterialParameter* materialParam,
						  const efkModelNodeParam& param,
						  int32_t renderPassInd,
						  float*& cutomData1Ptr,
						  float*& cutomData2Ptr)
	{
		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;
		cutomData1Ptr = nullptr;
		cutomData2Ptr = nullptr;

		GetInversedFlags(renderer, uvInversed, uvInversedBack);

		std::array<float, 4> uvInversedMaterial;
		uvInversedMaterial[0] = uvInversed[0];
		uvInversedMaterial[1] = uvInversed[1];
		uvInversedMaterial[2] = uvInversedBack[0];
		uvInversedMaterial[3] = uvInversedBack[1];

		// camera
		float cameraPosition[4];
		::Effekseer::Vec3f cameraPosition3 = renderer->GetCameraPosition();
		VectorToFloat4(cameraPosition3, cameraPosition);

		// time
		std::array<float, 4> predefined_uniforms;
		predefined_uniforms.fill(0.5f);
		predefined_uniforms[0] = renderer->GetTime();

		// vs
		int32_t vsOffset = sizeof(Effekseer::Matrix44) + (sizeof(Effekseer::Matrix44) + sizeof(float) * 4 * 2) * InstanceCount;

		renderer->SetVertexBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		renderer->SetVertexBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		renderer->SetVertexBufferToShader(cameraPosition, sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		// vs - custom data
		if (customData1Count_ > 0)
		{
			cutomData1Ptr = (float*)((uint8_t*)shader_->GetVertexConstantBuffer() + vsOffset);
			vsOffset += (sizeof(float) * 4) * InstanceCount;
		}

		if (customData2Count_ > 0)
		{
			cutomData2Ptr = (float*)((uint8_t*)shader_->GetVertexConstantBuffer() + vsOffset);
			vsOffset += (sizeof(float) * 4) * InstanceCount;
		}

		for (size_t i = 0; i < materialParam->MaterialUniforms.size(); i++)
		{
			renderer->SetVertexBufferToShader(materialParam->MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);
		}

		// ps
		int32_t psOffset = 0;
		renderer->SetPixelBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		// shader model
		material = param.EffectPointer->GetMaterial(materialParam->MaterialIndex);

		if (material->ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			float lightDirection[4];
			float lightColor[4];
			float lightAmbientColor[4];

			::Effekseer::Vec3f lightDirection3 = renderer->GetLightDirection();
			lightDirection3 = lightDirection3.Normalize();

			VectorToFloat4(lightDirection3, lightDirection);
			ColorToFloat4(renderer->GetLightColor(), lightColor);
			ColorToFloat4(renderer->GetLightAmbientColor(), lightAmbientColor);

			renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);
		}

		// refraction
		if (material->RefractionModelUserPtr != nullptr && renderPassInd == 0)
		{
			auto mat = renderer->GetCameraMatrix();
			renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
			psOffset += (sizeof(float) * 16);
		}

		for (size_t i = 0; i < materialParam->MaterialUniforms.size(); i++)
		{
			renderer->SetPixelBufferToShader(materialParam->MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);
		}
	}

public:
	virtual ~ModelRendererBase()
	{
	}

	template <typename RENDERER>
	void BeginRendering_(RENDERER* renderer, const efkModelNodeParam& parameter, int32_t count, void* userData)
	{
		keyValues_.clear();

		m_matrixes.clear();
		m_uv.clear();
		m_alphaUV.clear();
		m_uvDistortionUV.clear();
		m_blendUV.clear();
		m_blendAlphaUV.clear();
		m_blendUVDistortionUV.clear();
		m_flipbookIndexAndNextRate.clear();
		m_alphaThreshold.clear();
		m_viewOffsetDistance.clear();
		m_colors.clear();
		m_times.clear();
		customData1_.clear();
		customData2_.clear();

		matrixesSorted_.clear();
		uvSorted_.clear();
		alphaUVSorted_.clear();
		uvDistortionUVSorted_.clear();
		blendUVSorted_.clear();
		blendAlphaUVSorted_.clear();
		blendUVDistortionUVSorted_.clear();
		flipbookIndexAndNextRateSorted_.clear();
		alphaThresholdSorted_.clear();
		viewOffsetDistanceSorted_.clear();
		colorsSorted_.clear();
		timesSorted_.clear();
		customData1Sorted_.clear();
		customData2Sorted_.clear();

		if (parameter.BasicParameterPtr->MaterialType == ::Effekseer::RendererMaterialType::File &&
			parameter.BasicParameterPtr->MaterialParameterPtr != nullptr &&
			parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0 &&
			parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex) != nullptr)
		{
			auto material = parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			customData1Count_ = material->CustomData1;
			customData2Count_ = material->CustomData2;
		}
		else
		{
			customData1Count_ = 0;
			customData2Count_ = 0;
		}

		renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();

		isAdvanced_ = IsAdvanced(parameter);
	}

	bool IsAdvanced(const efkModelNodeParam& parameter) const
	{
		// TODO : merge with IsAdvanced in StandererdRenderer
		if (parameter.BasicParameterPtr->MaterialType == ::Effekseer::RendererMaterialType::File)
		{
			return false;
		}

		if (parameter.BasicParameterPtr->Texture3Index >= 0)
			return true;

		if (parameter.BasicParameterPtr->Texture4Index >= 0)
			return true;

		if (parameter.BasicParameterPtr->Texture5Index >= 0)
			return true;

		if (parameter.BasicParameterPtr->Texture6Index >= 0)
			return true;

		if (parameter.BasicParameterPtr->Texture7Index >= 0)
			return true;

		if (parameter.BasicParameterPtr->EnableInterpolation ||
			parameter.BasicParameterPtr->TextureBlendType != -1 ||
			parameter.BasicParameterPtr->EdgeThreshold != 0 ||
			parameter.BasicParameterPtr->EmissiveScaling != 1.0f ||
			parameter.BasicParameterPtr->IsAlphaCutoffEnabled ||
			parameter.EnableFalloff != 0)
		{
			return true;
		}

		return false;
	}

	template <typename RENDERER>
	void Rendering_(RENDERER* renderer, const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData)
	{
		::Effekseer::BillboardType btype = parameter.Billboard;
		Effekseer::Mat44f mat44;

		if (btype == ::Effekseer::BillboardType::Fixed)
		{
			mat44 = instanceParameter.SRTMatrix43;
		}
		else
		{
			Effekseer::Mat43f mat43;
			Effekseer::Vec3f s;
			Effekseer::Vec3f R;
			Effekseer::Vec3f F;

			CalcBillboard(btype, mat43, s, R, F, instanceParameter.SRTMatrix43, renderer->GetCameraFrontDirection());

			mat44 = ::Effekseer::Mat43f::Scaling(s) * mat43;
		}

		if (parameter.Magnification != 1.0f)
		{
			mat44 = Effekseer::Mat44f::Scaling(::Effekseer::Vec3f(parameter.Magnification)) * mat44;
		}

		if (!parameter.IsRightHand)
		{
			mat44 = Effekseer::Mat44f::Scaling(1.0f, 1.0f, -1.0f) * mat44;
		}

		m_matrixes.push_back(ToStruct(mat44));
		m_uv.push_back(instanceParameter.UV);
		m_alphaUV.push_back(instanceParameter.AlphaUV);
		m_uvDistortionUV.push_back(instanceParameter.UVDistortionUV);
		m_blendUV.push_back(instanceParameter.BlendUV);
		m_blendAlphaUV.push_back(instanceParameter.BlendAlphaUV);
		m_blendUVDistortionUV.push_back(instanceParameter.BlendUVDistortionUV);
		m_flipbookIndexAndNextRate.push_back(instanceParameter.FlipbookIndexAndNextRate);
		m_alphaThreshold.push_back(instanceParameter.AlphaThreshold);
		m_viewOffsetDistance.push_back(instanceParameter.ViewOffsetDistance);
		m_colors.push_back(instanceParameter.AllColor);
		m_times.push_back(instanceParameter.Time);

		if (customData1Count_ > 0)
		{
			customData1_.push_back(instanceParameter.CustomData1);
		}

		if (customData2Count_ > 0)
		{
			customData2_.push_back(instanceParameter.CustomData2);
		}

		//parameter.BasicParameterPtr
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount>
	void EndRendering_(RENDERER* renderer,
					   SHADER* advanced_shader_lit,
					   SHADER* advanced_shader_unlit,
					   SHADER* advanced_shader_distortion,
					   SHADER* shader_lit,
					   SHADER* shader_unlit,
					   SHADER* shader_distortion,
					   const efkModelNodeParam& param)
	{
		if (m_matrixes.size() == 0)
			return;
		if (param.ModelIndex < 0)
			return;

		int32_t renderPassCount = 1;

		if (param.BasicParameterPtr->MaterialParameterPtr != nullptr && param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0)
		{
			auto materialData = param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			if (materialData != nullptr && materialData->IsRefractionRequired)
			{
				// refraction, standard
				renderPassCount = 2;
			}
		}

		// sort
		SortTemporaryValues(renderer, param);

		for (int32_t renderPassInd = 0; renderPassInd < renderPassCount; renderPassInd++)
		{
			Effekseer::MaterialParameter* materialParam = param.BasicParameterPtr->MaterialParameterPtr;

			if (materialParam != nullptr && materialParam->MaterialIndex >= 0 &&
				param.EffectPointer->GetMaterial(materialParam->MaterialIndex) != nullptr)
			{
				RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererMaterialVertexConstantBuffer<InstanceCount>, ModelRendererPixelConstantBuffer>(
					renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd);
			}
			else
			{
				if (isAdvanced_)
				{
					RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererAdvancedVertexConstantBuffer<InstanceCount>, ModelRendererAdvancedPixelConstantBuffer>(
						renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd);
				}
				else
				{
					RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererVertexConstantBuffer<InstanceCount>, ModelRendererPixelConstantBuffer>(
						renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd);

				}
			}
		}
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount, typename VertexConstantBufferType, typename PixelConstantBufferType>
	void RenderPass(RENDERER* renderer,
					SHADER* advanced_shader_lit,
					SHADER* advanced_shader_unlit,
					SHADER* advanced_shader_distortion,
					SHADER* shader_lit,
					SHADER* shader_unlit,
					SHADER* shader_distortion,
					const efkModelNodeParam& param,
					int32_t renderPassInd)
	{
		if (m_matrixes.size() == 0)
			return;
		if (param.ModelIndex < 0)
			return;

		MODEL* model = (MODEL*)param.EffectPointer->GetModel(param.ModelIndex);
		if (model == NULL)
			return;

		bool isBackgroundRequired = false;

		isBackgroundRequired |= (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion);

		if (param.BasicParameterPtr->MaterialParameterPtr != nullptr && param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0)
		{
			auto materialData = param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			if (materialData != nullptr && materialData->IsRefractionRequired && renderPassInd == 0)
			{
				isBackgroundRequired = true;
			}
		}

		if (isBackgroundRequired)
		{
			auto callback = renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting())
				{
					return;
				}
			}
		}

		auto distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;

		if (isBackgroundRequired && renderer->GetBackground() == 0)
			return;

		// select shader
		Effekseer::MaterialParameter* materialParam = param.BasicParameterPtr->MaterialParameterPtr;
		// materialParam = nullptr;
		Effekseer::MaterialData* material = nullptr;
		SHADER* shader_ = nullptr;
		bool renderDistortedBackground = false;

		if (materialParam != nullptr && materialParam->MaterialIndex >= 0 &&
			param.EffectPointer->GetMaterial(materialParam->MaterialIndex) != nullptr)
		{
			material = param.EffectPointer->GetMaterial(materialParam->MaterialIndex);

			if (material != nullptr && material->IsRefractionRequired)
			{
				if (renderPassInd == 0)
				{
					shader_ = (SHADER*)material->RefractionModelUserPtr;
					renderDistortedBackground = true;
				}
				else
				{
					shader_ = (SHADER*)material->ModelUserPtr;
				}
			}
			else
			{
				shader_ = (SHADER*)material->ModelUserPtr;
			}

			// validate
			if (shader_ == nullptr)
			{
				return;
			}

			if (material != nullptr && (material->TextureCount != materialParam->MaterialTextures.size() ||
										material->UniformCount != materialParam->MaterialUniforms.size()))
			{
				return;
			}
		}
		else
		{
			if (isAdvanced_)
			{
				if (distortion)
				{
					shader_ = advanced_shader_distortion;
				}
				else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
				{
					shader_ = advanced_shader_lit;
				}
				else
				{
					shader_ = advanced_shader_unlit;
				}
			}
			else
			{
				if (distortion)
				{
					shader_ = shader_distortion;
				}
				else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
				{
					shader_ = shader_lit;
				}
				else
				{
					shader_ = shader_unlit;
				}
			}
		}

		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = param.Culling;

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		renderer->BeginShader(shader_);

		// Select texture
		if (materialParam != nullptr && material != nullptr)
		{
			int32_t textureCount = 0;
			std::array<Effekseer::TextureData*, ::Effekseer::TextureSlotMax> textures;

			if (materialParam->MaterialTextures.size() > 0)
			{
				textureCount = Effekseer::Min(static_cast<int32_t>(materialParam->MaterialTextures.size()), ::Effekseer::UserTextureSlotMax);

				auto effect = param.EffectPointer;

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

					state.TextureFilterTypes[i] = Effekseer::TextureFilterType::Linear;
					state.TextureWrapTypes[i] = material->TextureWrapTypes[i];
				}
			}

			if (renderer->GetBackground() != 0)
			{
				textures[textureCount] = renderer->GetBackground();
				state.TextureFilterTypes[textureCount] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[textureCount] = Effekseer::TextureWrapType::Clamp;
				textureCount += 1;
			}

			if (textureCount > 0)
			{
				renderer->SetTextures(shader_, textures.data(), textureCount);
			}
		}
		else
		{
			Effekseer::TextureData* textures[7] = {nullptr};

			if (distortion)
			{
				if (param.BasicParameterPtr->Texture1Index >= 0)
				{
					textures[0] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture1Index);
				}
				else
				{
					textures[0] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				textures[1] = renderer->GetBackground();

				if (param.BasicParameterPtr->Texture3Index >= 0)
				{
					textures[2] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture3Index);
				}
				else
				{
					textures[2] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture4Index >= 0)
				{
					textures[3] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture4Index);
				}
				if (textures[3] == nullptr)
				{
					textures[3] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (param.BasicParameterPtr->Texture5Index >= 0)
				{
					textures[4] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture5Index);
				}
				if (textures[4] == nullptr)
				{
					textures[4] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture6Index >= 0)
				{
					textures[5] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture6Index);
				}
				if (textures[5] == nullptr)
				{
					textures[5] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture7Index >= 0)
				{
					textures[6] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture7Index);
				}
				if (textures[6] == nullptr)
				{
					textures[6] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}
			}
			else
			{
				if (param.BasicParameterPtr->Texture1Index >= 0)
				{
					textures[0] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture1Index);
				}

				if (textures[0] == nullptr)
				{
					textures[0] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture2Index >= 0)
				{
					textures[1] = param.EffectPointer->GetNormalImage(param.BasicParameterPtr->Texture2Index);
				}

				if (textures[1] == nullptr)
				{
					textures[1] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (param.BasicParameterPtr->Texture3Index >= 0)
				{
					textures[2] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture3Index);
				}
				if (textures[2] == nullptr)
				{
					textures[2] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture4Index >= 0)
				{
					textures[3] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture4Index);
				}
				if (textures[3] == nullptr)
				{
					textures[3] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (param.BasicParameterPtr->Texture5Index >= 0)
				{
					textures[4] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture5Index);
				}
				if (textures[4] == nullptr)
				{
					textures[4] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture6Index >= 0)
				{
					textures[5] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture6Index);
				}
				if (textures[5] == nullptr)
				{
					textures[5] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture7Index >= 0)
				{
					textures[6] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture7Index);
				}
				if (textures[6] == nullptr)
				{
					textures[6] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}
			}

			state.TextureFilterTypes[0] = param.BasicParameterPtr->TextureFilter1;
			state.TextureWrapTypes[0] = param.BasicParameterPtr->TextureWrap1;

			if (distortion)
			{
				state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;
			}
			else
			{
				state.TextureFilterTypes[1] = param.BasicParameterPtr->TextureFilter2;
				state.TextureWrapTypes[1] = param.BasicParameterPtr->TextureWrap2;
			}

			state.TextureFilterTypes[2] = param.BasicParameterPtr->TextureFilter3;
			state.TextureWrapTypes[2] = param.BasicParameterPtr->TextureWrap3;
			state.TextureFilterTypes[3] = param.BasicParameterPtr->TextureFilter4;
			state.TextureWrapTypes[3] = param.BasicParameterPtr->TextureWrap4;
			state.TextureFilterTypes[4] = param.BasicParameterPtr->TextureFilter5;
			state.TextureWrapTypes[4] = param.BasicParameterPtr->TextureWrap5;
			state.TextureFilterTypes[5] = param.BasicParameterPtr->TextureFilter6;
			state.TextureWrapTypes[5] = param.BasicParameterPtr->TextureWrap6;
			state.TextureFilterTypes[6] = param.BasicParameterPtr->TextureFilter7;
			state.TextureWrapTypes[6] = param.BasicParameterPtr->TextureWrap7;

			renderer->SetTextures(shader_, textures, 7);
		}

		renderer->GetRenderState()->Update(distortion);

		VertexConstantBufferType* vcb = (VertexConstantBufferType*)shader_->GetVertexConstantBuffer();

		float* cutomData1Ptr = nullptr;
		float* cutomData2Ptr = nullptr;

		if (materialParam != nullptr && material != nullptr)
		{
			StoreFileUniform<RENDERER, SHADER, InstanceCount>(
				renderer, shader_, material, materialParam, param, renderPassInd, cutomData1Ptr, cutomData2Ptr);
		}
		else
		{
			std::array<float, 4> uvInversed;
			std::array<float, 4> uvInversedBack;

			GetInversedFlags(renderer, uvInversed, uvInversedBack);

			vcb->UVInversed[0] = uvInversed[0];
			vcb->UVInversed[1] = uvInversed[1];

			if (distortion)
			{
				ModelRendererDistortionPixelConstantBuffer* pcb = (ModelRendererDistortionPixelConstantBuffer*)shader_->GetPixelConstantBuffer();
				pcb->DistortionIntencity[0] = param.BasicParameterPtr->DistortionIntensity;

				pcb->UVInversedBack[0] = uvInversedBack[0];
				pcb->UVInversedBack[1] = uvInversedBack[1];

				pcb->Interpolation[0] = static_cast<float>(param.BasicParameterPtr->EnableInterpolation);
				pcb->Interpolation[1] = static_cast<float>(param.BasicParameterPtr->InterpolationType);

				pcb->UVDistortion[0] = param.BasicParameterPtr->UVDistortionIntensity;
				pcb->UVDistortion[1] = param.BasicParameterPtr->BlendUVDistortionIntensity;
				pcb->UVDistortion[2] = uvInversed[0];
				pcb->UVDistortion[3] = uvInversed[1];

				pcb->TextureBlendType[0] = static_cast<float>(param.BasicParameterPtr->TextureBlendType);
			}
			else
			{
				PixelConstantBufferType* pcb = (PixelConstantBufferType*)shader_->GetPixelConstantBuffer();

				// specify predefined parameters
				if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
				{
					::Effekseer::Vec3f lightDirection = renderer->GetLightDirection();
					lightDirection = lightDirection.Normalize();
					VectorToFloat4(lightDirection, vcb->LightDirection);
					VectorToFloat4(lightDirection, pcb->LightDirection);
				}

				{
					ColorToFloat4(renderer->GetLightColor(), vcb->LightColor);
					ColorToFloat4(renderer->GetLightColor(), pcb->LightColor);
				}

				{
					ColorToFloat4(renderer->GetLightAmbientColor(), vcb->LightAmbientColor);
					ColorToFloat4(renderer->GetLightAmbientColor(), pcb->LightAmbientColor);
				}

				pcb->SetModelFlipbookParameter(param.BasicParameterPtr->EnableInterpolation, static_cast<float>(param.BasicParameterPtr->InterpolationType));
				pcb->SetModelUVDistortionParameter(param.BasicParameterPtr->UVDistortionIntensity, param.BasicParameterPtr->BlendUVDistortionIntensity, {uvInversed[0], uvInversed[1]});
				pcb->SetModelBlendTextureParameter(static_cast<float>(param.BasicParameterPtr->TextureBlendType));

				::Effekseer::Vector3D CameraFront = renderer->GetCameraFrontDirection();
				pcb->SetCameraFrontDirection(-CameraFront.X, -CameraFront.Y, -CameraFront.Z);
				pcb->SetFalloffParameter(
					static_cast<float>(param.EnableFalloff),
					static_cast<float>(param.FalloffParam.ColorBlendType),
					static_cast<float>(param.FalloffParam.Pow),
					ColorToFloat4(param.FalloffParam.BeginColor),
					ColorToFloat4(param.FalloffParam.EndColor));

				pcb->SetEmissiveScaling(static_cast<float>(param.BasicParameterPtr->EmissiveScaling));
				pcb->SetEdgeColor(ColorToFloat4(Effekseer::Color(
					param.BasicParameterPtr->EdgeColor[0],
					param.BasicParameterPtr->EdgeColor[1],
					param.BasicParameterPtr->EdgeColor[2],
					param.BasicParameterPtr->EdgeColor[3])));

				pcb->SetEdgeParameter(param.BasicParameterPtr->EdgeThreshold, static_cast<float>(param.BasicParameterPtr->EdgeColorScaling));
			}
		}

		vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();

		vcb->SetModelFlipbookParameter(static_cast<float>(param.BasicParameterPtr->EnableInterpolation),
									   static_cast<float>(param.BasicParameterPtr->UVLoopType),
									   static_cast<float>(param.BasicParameterPtr->FlipbookDivideX),
									   static_cast<float>(param.BasicParameterPtr->FlipbookDivideY));

		// Check time
		auto stTime0 = m_times[0] % model->GetFrameCount();
		auto isTimeSame = true;

		for (auto t : m_times)
		{
			t = t % model->GetFrameCount();
			if (t != stTime0)
			{
				isTimeSame = false;
				break;
			}
		}

		if (Instancing && isTimeSame)
		{
			auto& imodel = model->InternalModels[stTime0];

			// Invalid unless layout is set after buffer
			renderer->SetVertexBuffer(imodel.VertexBuffer, model->GetVertexSize());
			renderer->SetIndexBuffer(imodel.IndexBuffer);
			renderer->SetLayout(shader_);

			for (size_t loop = 0; loop < m_matrixes.size();)
			{
				int32_t modelCount = Effekseer::Min(static_cast<int32_t>(m_matrixes.size()) - loop, model->ModelCount);

				for (int32_t num = 0; num < modelCount; num++)
				{
					vcb->ModelMatrix[num] = m_matrixes[loop + num];

					// DepthParameter
					::Effekseer::Mat44f modelMatrix = vcb->ModelMatrix[num];

					if (param.EnableViewOffset)
					{
						ApplyViewOffset(modelMatrix, renderer->GetCameraMatrix(), m_viewOffsetDistance[loop + num]);
					}

					ApplyDepthParameters(modelMatrix,
										 renderer->GetCameraFrontDirection(),
										 renderer->GetCameraPosition(),
										 param.DepthParameterPtr,
										 param.IsRightHand);
					vcb->ModelMatrix[num] = ToStruct(modelMatrix);

					vcb->ModelUV[num][0] = m_uv[loop + num].X;
					vcb->ModelUV[num][1] = m_uv[loop + num].Y;
					vcb->ModelUV[num][2] = m_uv[loop + num].Width;
					vcb->ModelUV[num][3] = m_uv[loop + num].Height;

					vcb->SetModelAlphaUV(
						num, m_alphaUV[loop + num].X, m_alphaUV[loop + num].Y, m_alphaUV[loop + num].Width, m_alphaUV[loop + num].Height);
					vcb->SetModelUVDistortionUV(num,
												m_uvDistortionUV[loop + num].X,
												m_uvDistortionUV[loop + num].Y,
												m_uvDistortionUV[loop + num].Width,
												m_uvDistortionUV[loop + num].Height);
					vcb->SetModelBlendUV(
						num, m_blendUV[loop + num].X, m_blendUV[loop + num].Y, m_blendUV[loop + num].Width, m_blendUV[loop + num].Height);
					vcb->SetModelBlendAlphaUV(num,
											  m_blendAlphaUV[loop + num].X,
											  m_blendAlphaUV[loop + num].Y,
											  m_blendAlphaUV[loop + num].Width,
											  m_blendAlphaUV[loop + num].Height);
					vcb->SetModelBlendUVDistortionUV(num,
													 m_blendUVDistortionUV[loop + num].X,
													 m_blendUVDistortionUV[loop + num].Y,
													 m_blendUVDistortionUV[loop + num].Width,
													 m_blendUVDistortionUV[loop + num].Height);
					vcb->SetModelFlipbookIndexAndNextRate(num, m_flipbookIndexAndNextRate[loop + num]);
					vcb->SetModelAlphaThreshold(num, m_alphaThreshold[loop + num]);

					ColorToFloat4(m_colors[loop + num], vcb->ModelColor[num]);

					if (cutomData1Ptr != nullptr)
					{
						cutomData1Ptr[num * 4 + 0] = customData1_[loop + num][0];
						cutomData1Ptr[num * 4 + 1] = customData1_[loop + num][1];
						cutomData1Ptr[num * 4 + 2] = customData1_[loop + num][2];
						cutomData1Ptr[num * 4 + 3] = customData1_[loop + num][3];
					}

					if (cutomData2Ptr != nullptr)
					{
						cutomData2Ptr[num * 4 + 0] = customData2_[loop + num][0];
						cutomData2Ptr[num * 4 + 1] = customData2_[loop + num][1];
						cutomData2Ptr[num * 4 + 2] = customData2_[loop + num][2];
						cutomData2Ptr[num * 4 + 3] = customData2_[loop + num][3];
					}
				}

				shader_->SetConstantBuffer();

				renderer->DrawPolygon(imodel.VertexCount * modelCount, imodel.IndexCount * modelCount);

				loop += modelCount;
			}
		}
		else
		{
			for (size_t loop = 0; loop < m_matrixes.size();)
			{
				auto stTime = m_times[loop] % model->GetFrameCount();
				auto& imodel = model->InternalModels[stTime];

				// Invalid unless layout is set after buffer
				renderer->SetVertexBuffer(imodel.VertexBuffer, model->GetVertexSize());
				renderer->SetIndexBuffer(imodel.IndexBuffer);
				renderer->SetLayout(shader_);

				vcb->ModelMatrix[0] = m_matrixes[loop];
				vcb->ModelUV[0][0] = m_uv[loop].X;
				vcb->ModelUV[0][1] = m_uv[loop].Y;
				vcb->ModelUV[0][2] = m_uv[loop].Width;
				vcb->ModelUV[0][3] = m_uv[loop].Height;

				vcb->SetModelAlphaUV(0, m_alphaUV[loop].X, m_alphaUV[loop].Y, m_alphaUV[loop].Width, m_alphaUV[loop].Height);
				vcb->SetModelUVDistortionUV(
					0, m_uvDistortionUV[loop].X, m_uvDistortionUV[loop].Y, m_uvDistortionUV[loop].Width, m_uvDistortionUV[loop].Height);
				vcb->SetModelBlendUV(0, m_blendUV[loop].X, m_blendUV[loop].Y, m_blendUV[loop].Width, m_blendUV[loop].Height);
				vcb->SetModelBlendAlphaUV(
					0, m_blendAlphaUV[loop].X, m_blendAlphaUV[loop].Y, m_blendAlphaUV[loop].Width, m_blendAlphaUV[loop].Height);
				vcb->SetModelUVDistortionUV(
					0, m_blendUVDistortionUV[loop].X, m_blendUVDistortionUV[loop].Y, m_blendUVDistortionUV[loop].Width, m_blendUVDistortionUV[loop].Height);
				vcb->SetModelFlipbookIndexAndNextRate(0, m_flipbookIndexAndNextRate[loop]);
				vcb->SetModelAlphaThreshold(0, m_alphaThreshold[loop]);

				// DepthParameters
				::Effekseer::Mat44f modelMatrix = vcb->ModelMatrix[0];
				if (param.EnableViewOffset == true)
				{
					ApplyViewOffset(modelMatrix, renderer->GetCameraMatrix(), m_viewOffsetDistance[0]);
				}

				ApplyDepthParameters(modelMatrix,
									 renderer->GetCameraFrontDirection(),
									 renderer->GetCameraPosition(),
									 param.DepthParameterPtr,
									 param.IsRightHand);
				vcb->ModelMatrix[0] = ToStruct(modelMatrix);
				ColorToFloat4(m_colors[loop], vcb->ModelColor[0]);

				if (cutomData1Ptr != nullptr)
				{
					cutomData1Ptr[0] = customData1_[loop][0];
					cutomData1Ptr[1] = customData1_[loop][1];
					cutomData1Ptr[2] = customData1_[loop][2];
					cutomData1Ptr[3] = customData1_[loop][3];
				}

				if (cutomData2Ptr != nullptr)
				{
					cutomData2Ptr[0] = customData2_[loop][0];
					cutomData2Ptr[1] = customData2_[loop][1];
					cutomData2Ptr[2] = customData2_[loop][2];
					cutomData2Ptr[3] = customData2_[loop][3];
				}

				shader_->SetConstantBuffer();
				renderer->DrawPolygon(imodel.VertexCount, imodel.IndexCount);

				loop += 1;
			}
		}

		renderer->EndShader(shader_);

		renderer->GetRenderState()->Pop();
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_MODEL_RENDERER_H__


#ifndef __EFFEKSEERRENDERER_SHADER_BASE_H__
#define __EFFEKSEERRENDERER_SHADER_BASE_H__

#include "EffekseerNative.h"
#include <assert.h>
#include <sstream>
#include <string.h>

namespace EffekseerRenderer
{
class ShaderBase
{
public:
	ShaderBase()
	{
	}
	virtual ~ShaderBase()
	{
	}

	virtual void SetVertexConstantBufferSize(int32_t size) = 0;
	virtual void SetPixelConstantBufferSize(int32_t size) = 0;

	virtual void* GetVertexConstantBuffer() = 0;
	virtual void* GetPixelConstantBuffer() = 0;

	virtual void SetConstantBuffer() = 0;
};

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_SHADER_BASE_H__

#ifndef __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <string.h>


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RibbonRenderer::NodeParameter efkRibbonNodeParam;
typedef ::Effekseer::RibbonRenderer::InstanceParameter efkRibbonInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class RibbonRendererBase : public ::Effekseer::RibbonRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
private:
protected:
	RENDERER* m_renderer;
	int32_t m_ribbonCount;

	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	efkRibbonNodeParam innstancesNodeParam;
	Effekseer::CustomAlignedVector<efkRibbonInstanceParam> instances;
	SplineGenerator spline_left;
	SplineGenerator spline_right;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	template <typename VERTEX, int TARGET>
	void AssignUV(StrideView<VERTEX> v, float uvX1, float uvX2, float uvY1, float uvY2)
	{
		if (TARGET == 0)
		{
			v[0].UV[0] = uvX1;
			v[0].UV[1] = uvY1;

			v[1].UV[0] = uvX2;
			v[1].UV[1] = uvY1;

			v[2].UV[0] = uvX1;
			v[2].UV[1] = uvY2;

			v[3].UV[0] = uvX2;
			v[3].UV[1] = uvY2;
		}
		else if (TARGET == 1)
		{
			v[0].UV2[0] = uvX1;
			v[0].UV2[1] = uvY1;

			v[1].UV2[0] = uvX2;
			v[1].UV2[1] = uvY1;

			v[2].UV2[0] = uvX1;
			v[2].UV2[1] = uvY2;

			v[3].UV2[0] = uvX2;
			v[3].UV2[1] = uvY2;
		}
		else if (TARGET == 2)
		{
			v[0].SetAlphaUV(uvX1, 0);
			v[0].SetAlphaUV(uvY1, 1);

			v[1].SetAlphaUV(uvX2, 0);
			v[1].SetAlphaUV(uvY1, 1);

			v[2].SetAlphaUV(uvX1, 0);
			v[2].SetAlphaUV(uvY2, 1);

			v[3].SetAlphaUV(uvX2, 0);
			v[3].SetAlphaUV(uvY2, 1);
		}
		else if (TARGET == 3)
		{
			v[0].SetUVDistortionUV(uvX1, 0);
			v[0].SetUVDistortionUV(uvY1, 1);

			v[1].SetUVDistortionUV(uvX2, 0);
			v[1].SetUVDistortionUV(uvY1, 1);

			v[2].SetUVDistortionUV(uvX1, 0);
			v[2].SetUVDistortionUV(uvY2, 1);

			v[3].SetUVDistortionUV(uvX2, 0);
			v[3].SetUVDistortionUV(uvY2, 1);
		}
		else if (TARGET == 4)
		{
			v[0].SetBlendUV(uvX1, 0);
			v[0].SetBlendUV(uvY1, 1);

			v[1].SetBlendUV(uvX2, 0);
			v[1].SetBlendUV(uvY1, 1);

			v[2].SetBlendUV(uvX1, 0);
			v[2].SetBlendUV(uvY2, 1);

			v[3].SetBlendUV(uvX2, 0);
			v[3].SetBlendUV(uvY2, 1);
		}
		else if (TARGET == 5)
		{
			v[0].SetBlendAlphaUV(uvX1, 0);
			v[0].SetBlendAlphaUV(uvY1, 1);

			v[1].SetBlendAlphaUV(uvX2, 0);
			v[1].SetBlendAlphaUV(uvY1, 1);

			v[2].SetBlendAlphaUV(uvX1, 0);
			v[2].SetBlendAlphaUV(uvY2, 1);

			v[3].SetBlendAlphaUV(uvX2, 0);
			v[3].SetBlendAlphaUV(uvY2, 1);
		}
		else if (TARGET == 6)
		{
			v[0].SetBlendUVDistortionUV(uvX1, 0);
			v[0].SetBlendUVDistortionUV(uvY1, 1);

			v[1].SetBlendUVDistortionUV(uvX2, 0);
			v[1].SetBlendUVDistortionUV(uvY1, 1);

			v[2].SetBlendUVDistortionUV(uvX1, 0);
			v[2].SetBlendUVDistortionUV(uvY2, 1);

			v[3].SetBlendUVDistortionUV(uvX2, 0);
			v[3].SetBlendUVDistortionUV(uvY2, 1);
		}
	}

	template <typename VERTEX, int TARGET>
	void AssignUVs(efkRibbonNodeParam& parameter, StrideView<VERTEX> verteies)
	{
		float uvx = 0.0f;
		float uvw = 1.0f;
		float uvy = 0.0f;
		float uvh = 1.0f;

		if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Strech)
		{
			verteies.Reset();

			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				const auto& param = instances[loop];
				if (TARGET == 0)
				{
					uvx = param.UV.X;
					uvw = param.UV.Width;
					uvy = param.UV.Y;
					uvh = param.UV.Height;
				}
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
				}

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
									 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
									 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					auto uvX1 = uvx;
					auto uvX2 = uvx + uvw;
					auto uvY1 = uvy + percent1 * uvh;
					auto uvY2 = uvy + percent2 * uvh;

					AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

					verteies += 4;
				}
			}
		}
		else if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Tile)
		{
			const auto& uvParam = *parameter.TextureUVTypeParameterPtr;

			verteies.Reset();

			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];
				if (TARGET == 0)
				{
					uvx = param.UV.X;
					uvw = param.UV.Width;
					uvy = param.UV.Y;
					uvh = param.UV.Height;
				}
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
				}

				if (loop < uvParam.TileEdgeTail)
				{
					float uvBegin = uvy;
					float uvEnd = uvy + uvh * uvParam.TileLoopAreaBegin;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
										 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

						float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
										 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

						verteies += 4;
					}
				}
				else if (loop >= param.InstanceCount - 1 - uvParam.TileEdgeHead)
				{
					float uvBegin = uvy + uvh * uvParam.TileLoopAreaEnd;
					float uvEnd = uvy + uvh * 1.0f;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
									sploop) /
							(float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

						float percent2 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
									sploop + 1) /
							(float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

						verteies += 4;
					}
				}
				else
				{
					float uvBegin = uvy + uvh * uvParam.TileLoopAreaBegin;
					float uvEnd = uvy + uvh * uvParam.TileLoopAreaEnd;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						bool isFirst = param.InstanceIndex == 0 && sploop == 0;
						bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

						float percent1 = (float)(sploop) / (float)(parameter.SplineDivision);

						float percent2 = (float)(sploop + 1) / (float)(parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

						verteies += 4;
					}
				}
			}
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void RenderSplines(const ::Effekseer::Mat44f& camera)
	{
		if (instances.size() == 0)
		{
			return;
		}

		auto& parameter = innstancesNodeParam;

		// Calculate spline
		if (parameter.SplineDivision > 1)
		{
			spline_left.Reset();
			spline_right.Reset();

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto& param = instances[loop];

				efkVector3D pl(param.Positions[0], 0.0f, 0.0f);
				efkVector3D pr(param.Positions[1], 0.0f, 0.0f);

				if (parameter.ViewpointDependent)
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset == true)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					::Effekseer::Vec3f s;
					::Effekseer::Mat43f r;
					::Effekseer::Vec3f t;
					mat.GetSRT(s, r, t);

					ApplyDepthParameters(r,
										 t,
										 s,
										 m_renderer->GetCameraFrontDirection(),
										 m_renderer->GetCameraPosition(),
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					// extend
					pl.SetX(pl.GetX() * s.GetX());
					pr.SetX(pr.GetX() * s.GetX());

					::Effekseer::Vec3f F;
					::Effekseer::Vec3f R;
					::Effekseer::Vec3f U;

					U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.X.GetY());
					F = ::Effekseer::Vec3f(-m_renderer->GetCameraFrontDirection()).Normalize();
					R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
					F = ::Effekseer::Vec3f::Cross(R, U).Normalize();

					::Effekseer::Mat43f mat_rot(-R.GetX(),
												-R.GetY(),
												-R.GetZ(),
												U.GetX(),
												U.GetY(),
												U.GetZ(),
												F.GetX(),
												F.GetY(),
												F.GetZ(),
												t.GetX(),
												t.GetY(),
												t.GetZ());

					pl = ::Effekseer::Vec3f::Transform(pl, mat_rot);
					pr = ::Effekseer::Vec3f::Transform(pr, mat_rot);

					spline_left.AddVertex(pl);
					spline_right.AddVertex(pr);
				}
				else
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset == true)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					ApplyDepthParameters(mat,
										 m_renderer->GetCameraFrontDirection(),
										 m_renderer->GetCameraPosition(),
										 // s,
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					pl = ::Effekseer::Vec3f::Transform(pl, mat);
					pr = ::Effekseer::Vec3f::Transform(pr, mat);

					spline_left.AddVertex(pl);
					spline_right.AddVertex(pr);
				}
			}

			spline_left.Calculate();
			spline_right.Calculate();
		}

		StrideView<VERTEX> verteies(m_ringBufferData, stride_, vertexCount_);
		for (size_t loop = 0; loop < instances.size(); loop++)
		{
			auto& param = instances[loop];

			for (auto sploop = 0; sploop < parameter.SplineDivision; sploop++)
			{
				bool isFirst = param.InstanceIndex == 0 && sploop == 0;
				bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

				float percent_instance = sploop / (float)parameter.SplineDivision;

				if (parameter.SplineDivision > 1)
				{
					verteies[0].Pos = ToStruct(spline_left.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));
					verteies[1].Pos = ToStruct(spline_right.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));

					verteies[0].SetColor(Effekseer::Color::Lerp(param.Colors[0], param.Colors[2], percent_instance), FLIP_RGB);
					verteies[1].SetColor(Effekseer::Color::Lerp(param.Colors[1], param.Colors[3], percent_instance), FLIP_RGB);
				}
				else
				{
					for (int i = 0; i < 2; i++)
					{
						verteies[i].Pos.X = param.Positions[i];
						verteies[i].Pos.Y = 0.0f;
						verteies[i].Pos.Z = 0.0f;
						verteies[i].SetColor(param.Colors[i], FLIP_RGB);
						verteies[i].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
						verteies[i].SetAlphaThreshold(param.AlphaThreshold);
					}
				}

				float percent = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
								(float)((param.InstanceCount - 1) * parameter.SplineDivision);

				if (parameter.ViewpointDependent)
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset == true)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					::Effekseer::Vec3f s;
					::Effekseer::Mat43f r;
					::Effekseer::Vec3f t;
					mat.GetSRT(s, r, t);

					ApplyDepthParameters(r,
										 t,
										 s,
										 m_renderer->GetCameraFrontDirection(),
										 m_renderer->GetCameraPosition(),
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					if (parameter.SplineDivision > 1)
					{
					}
					else
					{
						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos.X = verteies[i].Pos.X * s.GetX();
						}

						::Effekseer::Vec3f F;
						::Effekseer::Vec3f R;
						::Effekseer::Vec3f U;

						U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());

						F = ::Effekseer::Vec3f(-m_renderer->GetCameraFrontDirection()).Normalize();
						R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
						F = ::Effekseer::Vec3f::Cross(R, U).Normalize();

						::Effekseer::Mat43f mat_rot(-R.GetX(),
													-R.GetY(),
													-R.GetZ(),
													U.GetX(),
													U.GetY(),
													U.GetZ(),
													F.GetX(),
													F.GetY(),
													F.GetZ(),
													t.GetX(),
													t.GetY(),
													t.GetZ());

						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos = ToStruct(::Effekseer::Vec3f::Transform(verteies[i].Pos, mat_rot));
						}
					}
				}
				else
				{
					if (parameter.SplineDivision > 1)
					{
					}
					else
					{
						::Effekseer::Mat43f mat = param.SRTMatrix43;

						if (parameter.EnableViewOffset == true)
						{
							ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
						}

						ApplyDepthParameters(mat,
											 m_renderer->GetCameraFrontDirection(),
											 m_renderer->GetCameraPosition(),
											 // s,
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos = ToStruct(::Effekseer::Vec3f::Transform(verteies[i].Pos, mat));
						}
					}
				}

				if (isFirst || isLast)
				{
					verteies += 2;
				}
				else
				{
					verteies[2] = verteies[0];
					verteies[3] = verteies[1];
					verteies += 4;
				}

				if (!isFirst)
				{
					m_ribbonCount++;
				}

				if (isLast)
				{
					break;
				}
			}
		}

		// calculate UV
		AssignUVs<VERTEX, 0>(parameter, verteies);

		if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
		{
			AssignUVs<VERTEX, 1>(parameter, verteies);
		}

		AssignUVs<VERTEX, 2>(parameter, verteies);
		AssignUVs<VERTEX, 3>(parameter, verteies);
		AssignUVs<VERTEX, 4>(parameter, verteies);
		AssignUVs<VERTEX, 5>(parameter, verteies);
		AssignUVs<VERTEX, 6>(parameter, verteies);

		// Apply distortion
		if (IsDistortionVertex<VERTEX>())
		{
			StrideView<VERTEX> vs_(m_ringBufferData, stride_, vertexCount_);
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

				Effekseer::Vec3f axis;

				if (isFirst_)
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;
				}
				else if (isLast_)
				{
					axis = axisBefore;
				}
				else
				{
					Effekseer::Vec3f axisOld = axisBefore;
					axis = (vs_[5].Pos - vs_[3].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					axis = SafeNormalize(axis);
				}

				Effekseer::Vec3f tangent = vs_[1].Pos - vs_[0].Pos;
				tangent = tangent.Normalize();

				if (isFirst_ || isLast_)
				{
					const auto binormalVector = ToStruct(axis);
					const auto tangentVector = ToStruct(tangent);

					for (int32_t j = 0; j < 2; j++)
					{
						vs_[j].SetBinormal(binormalVector);
						vs_[j].SetTangent(tangentVector);
					}
					vs_ += 2;
				}
				else
				{
					const auto binormalVector = ToStruct(axis);
					const auto tangentVector = ToStruct(tangent);

					for (int32_t j = 0; j < 4; j++)
					{
						vs_[j].SetBinormal(binormalVector);
						vs_[j].SetTangent(tangentVector);
					}
					vs_ += 4;
				}
			}
		}
		else if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
		{
			StrideView<VERTEX> vs_(m_ringBufferData, stride_, vertexCount_);
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

				Effekseer::Vec3f axis;

				if (isFirst_)
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;
				}
				else if (isLast_)
				{
					axis = axisBefore;
				}
				else
				{
					Effekseer::Vec3f axisOld = axisBefore;
					axis = (vs_[5].Pos - vs_[3].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					axis = SafeNormalize(axis);
				}

				Effekseer::Vec3f tangent = vs_[1].Pos - vs_[0].Pos;
				tangent = SafeNormalize(tangent);

				Effekseer::Vec3f normal = Effekseer::Vec3f::Cross(axis, tangent);
				normal = SafeNormalize(normal);

				if (!parameter.IsRightHand)
				{
					normal = -normal;
				}

				if (isFirst_)
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					vs_[0].SetPackedNormal(packedNormal);
					vs_[0].SetPackedTangent(packedTangent);
					vs_[1].SetPackedNormal(packedNormal);
					vs_[1].SetPackedTangent(packedTangent);

					vs_ += 2;
				}
				else if (isLast_)
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					vs_[0].SetPackedNormal(packedNormal);
					vs_[0].SetPackedTangent(packedTangent);
					vs_[1].SetPackedNormal(packedNormal);
					vs_[1].SetPackedTangent(packedTangent);

					vs_ += 2;
				}
				else
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					vs_[0].SetPackedNormal(packedNormal);
					vs_[0].SetPackedTangent(packedTangent);
					vs_[1].SetPackedNormal(packedNormal);
					vs_[1].SetPackedTangent(packedTangent);
					vs_[2].SetPackedNormal(packedNormal);
					vs_[2].SetPackedTangent(packedTangent);
					vs_[3].SetPackedNormal(packedNormal);
					vs_[3].SetPackedTangent(packedTangent);

					vs_ += 4;
				}
			}
		}

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 4; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData1.data(), sizeof(float) * customData1Count_);
						custom += 1;
					}
				}
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 4; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData2.data(), sizeof(float) * customData2Count_);
						custom += 1;
					}
				}
			}
		}
	}

public:
	RibbonRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_ribbonCount(0)
		, m_ringBufferOffset(0)
		, m_ringBufferData(NULL)
	{
	}

	virtual ~RibbonRendererBase()
	{
	}

protected:
	void Rendering_(const efkRibbonNodeParam& parameter,
					const efkRibbonInstanceParam& instanceParameter,
					void* userData,
					const ::Effekseer::Mat44f& camera)
	{
		const auto& state = m_renderer->GetStandardRenderer()->GetState();
		bool isAdvanced = state.IsAdvanced();

		if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			Rendering_Internal<AdvancedVertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (isAdvanced)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			Rendering_Internal<VertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkRibbonNodeParam& parameter,
							const efkRibbonInstanceParam& instanceParameter,
							void* userData,
							const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == NULL)
			return;
		if (instanceParameter.InstanceCount < 2)
			return;

		bool isFirst = instanceParameter.InstanceIndex == 0;
		bool isLast = instanceParameter.InstanceIndex == (instanceParameter.InstanceCount - 1);

		auto& param = instanceParameter;

		if (isFirst)
		{
			instances.reserve(param.InstanceCount);
			instances.resize(0);
			innstancesNodeParam = parameter;
		}

		instances.push_back(param);

		if (isLast)
		{
			RenderSplines<VERTEX, FLIP_RGB>(camera);
		}
	}

public:
	void BeginRenderingGroup(const efkRibbonNodeParam& param, int32_t count, void* userData) override
	{
		m_ribbonCount = 0;
		int32_t vertexCount = ((count - 1) * param.SplineDivision) * 4;
		if (vertexCount <= 0)
			return;

		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;
		state.TextureFilter4 = param.BasicParameterPtr->TextureFilter4;
		state.TextureWrap4 = param.BasicParameterPtr->TextureWrap4;
		state.TextureFilter5 = param.BasicParameterPtr->TextureFilter5;
		state.TextureWrap5 = param.BasicParameterPtr->TextureWrap5;
		state.TextureFilter6 = param.BasicParameterPtr->TextureFilter6;
		state.TextureWrap6 = param.BasicParameterPtr->TextureWrap6;
		state.TextureFilter7 = param.BasicParameterPtr->TextureFilter7;
		state.TextureWrap7 = param.BasicParameterPtr->TextureWrap7;

		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index
											   ,
											   param.BasicParameterPtr->Texture3Index,
											   param.BasicParameterPtr->Texture4Index,
											   param.BasicParameterPtr->Texture5Index,
											   param.BasicParameterPtr->Texture6Index,
											   param.BasicParameterPtr->Texture7Index
		);
		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		m_renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

		m_renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, stride_, (void*&)m_ringBufferData);
		vertexCount_ = vertexCount;
	}

	void Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData) override
	{
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RIBBON_RENDERER_H__

#ifndef __EFFEKSEERRENDERER_RING_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_RING_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <math.h>
#include <string.h>


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RingRenderer::NodeParameter efkRingNodeParam;
typedef ::Effekseer::RingRenderer::InstanceParameter efkRingInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class RingRendererBase : public ::Effekseer::RingRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	struct KeyValue
	{
		float Key;
		efkRingInstanceParam Value;
	};
	std::vector<KeyValue> instances_;

	RENDERER* m_renderer;
	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	int32_t m_spriteCount;
	int32_t m_instanceCount;
	::Effekseer::Mat44f m_singleRenderingMatrix;
	::Effekseer::RendererMaterialType materialType_ = ::Effekseer::RendererMaterialType::Default;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;
	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

public:
	RingRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_ringBufferOffset(0)
		, m_ringBufferData(NULL)
		, m_spriteCount(0)
		, m_instanceCount(0)
	{
	}

	virtual ~RingRendererBase()
	{
	}

protected:
	void RenderingInstance(const efkRingInstanceParam& inst,
						   const efkRingNodeParam& param,
						   const StandardRendererState& state,
						   const ::Effekseer::Mat44f& camera)
	{
		bool isAdvanced = state.IsAdvanced();

		if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			Rendering_Internal<AdvancedVertexDistortion, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (isAdvanced)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			Rendering_Internal<VertexDistortion, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
	}

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkRingNodeParam& param)
	{
		m_spriteCount = 0;
		int32_t singleVertexCount = param.VertexCount * 8;
		m_instanceCount = count;

		instances_.clear();

		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None)
		{
			instances_.reserve(count);
		}

		if (count == 1)
		{
			renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();
		}

		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;
		state.TextureFilter4 = param.BasicParameterPtr->TextureFilter4;
		state.TextureWrap4 = param.BasicParameterPtr->TextureWrap4;
		state.TextureFilter5 = param.BasicParameterPtr->TextureFilter5;
		state.TextureWrap5 = param.BasicParameterPtr->TextureWrap5;
		state.TextureFilter6 = param.BasicParameterPtr->TextureFilter6;
		state.TextureWrap6 = param.BasicParameterPtr->TextureWrap6;
		state.TextureFilter7 = param.BasicParameterPtr->TextureFilter7;
		state.TextureWrap7 = param.BasicParameterPtr->TextureWrap7;

		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index
											   ,
											   param.BasicParameterPtr->Texture3Index,
											   param.BasicParameterPtr->Texture4Index,
											   param.BasicParameterPtr->Texture5Index,
											   param.BasicParameterPtr->Texture6Index,
											   param.BasicParameterPtr->Texture7Index
		);

		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		materialType_ = param.BasicParameterPtr->MaterialType;

		renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);
		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(count * singleVertexCount, stride_, (void*&)m_ringBufferData);

		vertexCount_ = count * singleVertexCount;
	}

	void Rendering_(const efkRingNodeParam& parameter,
					const efkRingInstanceParam& instanceParameter,
					void* userData,
					const ::Effekseer::Mat44f& camera)
	{
		if (parameter.DepthParameterPtr->ZSort == Effekseer::ZSortType::None || CanSingleRendering())
		{
			const auto& state = m_renderer->GetStandardRenderer()->GetState();

			RenderingInstance(instanceParameter, parameter, state, camera);
		}
		else
		{
			KeyValue kv;
			kv.Value = instanceParameter;
			instances_.push_back(kv);
		}
	}

	bool CanSingleRendering()
	{
		return m_instanceCount <= 1 && materialType_ == ::Effekseer::RendererMaterialType::Default;
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkRingNodeParam& parameter,
							const efkRingInstanceParam& instanceParameter,
							void* userData,
							const ::Effekseer::Mat44f& camera)
	{
		::Effekseer::Mat43f mat43;

		if (parameter.Billboard == ::Effekseer::BillboardType::Billboard ||
			parameter.Billboard == ::Effekseer::BillboardType::RotatedBillboard ||
			parameter.Billboard == ::Effekseer::BillboardType::YAxisFixed)
		{
			Effekseer::Vec3f s;
			Effekseer::Vec3f R;
			Effekseer::Vec3f F;

			if (parameter.EnableViewOffset)
			{
				Effekseer::Mat43f instMat = instanceParameter.SRTMatrix43;

				ApplyViewOffset(instMat, camera, instanceParameter.ViewOffsetDistance);

				CalcBillboard(parameter.Billboard, mat43, s, R, F, instMat, m_renderer->GetCameraFrontDirection());
			}
			else
			{
				CalcBillboard(parameter.Billboard, mat43, s, R, F, instanceParameter.SRTMatrix43, m_renderer->GetCameraFrontDirection());
			}

			ApplyDepthParameters(mat43,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 s,
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			if (CanSingleRendering())
			{
				mat43 = ::Effekseer::Mat43f::Scaling(s) * mat43;
			}
			else
			{
				mat43 = ::Effekseer::Mat43f::Scaling(s) * mat43;
			}
		}
		else if (parameter.Billboard == ::Effekseer::BillboardType::Fixed)
		{
			mat43 = instanceParameter.SRTMatrix43;

			if (parameter.EnableViewOffset)
			{
				ApplyViewOffset(mat43, camera, instanceParameter.ViewOffsetDistance);
			}

			ApplyDepthParameters(mat43,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);
		}

		int32_t singleVertexCount = parameter.VertexCount * 8;
		// Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * vertexCount );

		StrideView<VERTEX> verteies(m_ringBufferData, stride_, singleVertexCount);
		const float circleAngleDegree = (instanceParameter.ViewingAngleEnd - instanceParameter.ViewingAngleStart);
		const float stepAngleDegree = circleAngleDegree / (parameter.VertexCount);
		const float stepAngle = (stepAngleDegree) / 180.0f * 3.141592f;
		const float beginAngle = (instanceParameter.ViewingAngleStart + 90) / 180.0f * 3.141592f;

		const float outerRadius = instanceParameter.OuterLocation.GetX();
		const float innerRadius = instanceParameter.InnerLocation.GetX();
		const float centerRadius = innerRadius + (outerRadius - innerRadius) * instanceParameter.CenterRatio;

		const float outerHeight = instanceParameter.OuterLocation.GetY();
		const float innerHeight = instanceParameter.InnerLocation.GetY();
		const float centerHeight = innerHeight + (outerHeight - innerHeight) * instanceParameter.CenterRatio;

		::Effekseer::Color outerColor = instanceParameter.OuterColor;
		::Effekseer::Color innerColor = instanceParameter.InnerColor;
		::Effekseer::Color centerColor = instanceParameter.CenterColor;
		::Effekseer::Color outerColorNext = instanceParameter.OuterColor;
		::Effekseer::Color innerColorNext = instanceParameter.InnerColor;
		::Effekseer::Color centerColorNext = instanceParameter.CenterColor;

		if (parameter.StartingFade > 0)
		{
			outerColor.A = 0;
			innerColor.A = 0;
			centerColor.A = 0;
		}

		const float stepC = cosf(stepAngle);
		const float stepS = sinf(stepAngle);
		float cos_ = cosf(beginAngle);
		float sin_ = sinf(beginAngle);
		::Effekseer::Vec3f outerCurrent(cos_ * outerRadius, sin_ * outerRadius, outerHeight);
		::Effekseer::Vec3f innerCurrent(cos_ * innerRadius, sin_ * innerRadius, innerHeight);
		::Effekseer::Vec3f centerCurrent(cos_ * centerRadius, sin_ * centerRadius, centerHeight);

		float uv0Current = instanceParameter.UV.X;
		const float uv0Step = instanceParameter.UV.Width / parameter.VertexCount;
		const float uv0v1 = instanceParameter.UV.Y;
		const float uv0v2 = uv0v1 + instanceParameter.UV.Height * 0.5f;
		const float uv0v3 = uv0v1 + instanceParameter.UV.Height;
		float uv0texNext = 0.0f;

		float uv1Current = 0.0f;
		const float uv1Step = 1.0f / parameter.VertexCount;
		const float uv1v1 = 0.0f;
		const float uv1v2 = uv1v1 + 0.5f;
		const float uv1v3 = uv1v1 + 1.0f;
		float uv1texNext = 0.0f;

		const int32_t advancedUVNum = 5;

		float advancedUVCurrent[advancedUVNum] =
		{
			instanceParameter.AlphaUV.X,
			instanceParameter.UVDistortionUV.X,
			instanceParameter.BlendUV.X,
			instanceParameter.BlendAlphaUV.X,
			instanceParameter.BlendUVDistortionUV.X
		};
		const float advancedUVStep[advancedUVNum] =
		{
			instanceParameter.AlphaUV.Width / parameter.VertexCount,
			instanceParameter.UVDistortionUV.Width / parameter.VertexCount,
			instanceParameter.BlendUV.Width / parameter.VertexCount,
			instanceParameter.BlendAlphaUV.Width / parameter.VertexCount,
			instanceParameter.BlendUVDistortionUV.Width / parameter.VertexCount
		};
		const float advancedUVv1[advancedUVNum] =
		{
			instanceParameter.AlphaUV.Y,
			instanceParameter.UVDistortionUV.Y,
			instanceParameter.BlendUV.Y,
			instanceParameter.BlendAlphaUV.Y,
			instanceParameter.BlendUVDistortionUV.Y
		};
		const float advancedUVv2[advancedUVNum] =
		{
			advancedUVv1[0] + instanceParameter.AlphaUV.Height * 0.5f,
			advancedUVv1[1] + instanceParameter.UVDistortionUV.Height * 0.5f,
			advancedUVv1[2] + instanceParameter.BlendUV.Height * 0.5f,
			advancedUVv1[3] + instanceParameter.BlendAlphaUV.Height * 0.5f,
			advancedUVv1[4] + instanceParameter.BlendUVDistortionUV.Height * 0.5f
		};
		const float advancedUVv3[advancedUVNum] =
		{
			advancedUVv1[0] + instanceParameter.AlphaUV.Height,
			advancedUVv1[1] + instanceParameter.UVDistortionUV.Height,
			advancedUVv1[2] + instanceParameter.BlendUV.Height,
			advancedUVv1[3] + instanceParameter.BlendAlphaUV.Height,
			advancedUVv1[4] + instanceParameter.BlendUVDistortionUV.Height
		};
		float advancedUVtexNext[advancedUVNum] = { 0.0f };

		::Effekseer::Vec3f outerNext, innerNext, centerNext;

		float currentAngleDegree = 0;
		float fadeStartAngle = parameter.StartingFade;
		float fadeEndingAngle = parameter.EndingFade;

		for (int i = 0; i < singleVertexCount; i += 8)
		{
			float old_c = cos_;
			float old_s = sin_;

			float t;
			t = cos_ * stepC - sin_ * stepS;
			sin_ = sin_ * stepC + cos_ * stepS;
			cos_ = t;

			outerNext = ::Effekseer::Vec3f{ cos_ * outerRadius, sin_ * outerRadius, outerHeight };
			innerNext = ::Effekseer::Vec3f{ cos_ * innerRadius, sin_ * innerRadius, innerHeight };
			centerNext = ::Effekseer::Vec3f{ cos_ * centerRadius, sin_ * centerRadius, centerHeight };

			currentAngleDegree += stepAngleDegree;

			// for floating decimal point error
			currentAngleDegree = Effekseer::Min(currentAngleDegree, circleAngleDegree);
			float alpha = 1.0f;
			if (currentAngleDegree < fadeStartAngle)
			{
				alpha = currentAngleDegree / fadeStartAngle;
			}
			else if (currentAngleDegree > circleAngleDegree - fadeEndingAngle)
			{
				alpha = 1.0f - (currentAngleDegree - (circleAngleDegree - fadeEndingAngle)) / fadeEndingAngle;
			}

			outerColorNext = instanceParameter.OuterColor;
			innerColorNext = instanceParameter.InnerColor;
			centerColorNext = instanceParameter.CenterColor;

			if (alpha != 1.0f)
			{
				outerColorNext.A = static_cast<uint8_t>(outerColorNext.A * alpha);
				innerColorNext.A = static_cast<uint8_t>(innerColorNext.A * alpha);
				centerColorNext.A = static_cast<uint8_t>(centerColorNext.A * alpha);
			}

			uv0texNext = uv0Current + uv0Step;

			StrideView<VERTEX> v(&verteies[i], stride_, 8);
			v[0].Pos = ToStruct(outerCurrent);
			v[0].SetColor(outerColor, FLIP_RGB);
			v[0].UV[0] = uv0Current;
			v[0].UV[1] = uv0v1;

			v[1].Pos = ToStruct(centerCurrent);
			v[1].SetColor(centerColor, FLIP_RGB);
			v[1].UV[0] = uv0Current;
			v[1].UV[1] = uv0v2;

			v[2].Pos = ToStruct(outerNext);
			v[2].SetColor(outerColorNext, FLIP_RGB);
			v[2].UV[0] = uv0texNext;
			v[2].UV[1] = uv0v1;

			v[3].Pos = ToStruct(centerNext);
			v[3].SetColor(centerColorNext, FLIP_RGB);
			v[3].UV[0] = uv0texNext;
			v[3].UV[1] = uv0v2;

			v[4] = v[1];

			v[5].Pos = ToStruct(innerCurrent);
			v[5].SetColor(innerColor, FLIP_RGB);
			v[5].UV[0] = uv0Current;
			v[5].UV[1] = uv0v3;

			v[6] = v[3];

			v[7].Pos = ToStruct(innerNext);
			v[7].SetColor(innerColorNext, FLIP_RGB);
			v[7].UV[0] = uv0texNext;
			v[7].UV[1] = uv0v3;

			for (int32_t uvi = 0; uvi < advancedUVNum; uvi++)
			{
				advancedUVtexNext[uvi] = advancedUVCurrent[uvi] + advancedUVStep[uvi];
			}

			v[0].SetAlphaUV(advancedUVCurrent[0], 0);
			v[0].SetAlphaUV(advancedUVv1[0], 1);
			v[0].SetUVDistortionUV(advancedUVCurrent[1], 0);
			v[0].SetUVDistortionUV(advancedUVv1[1], 1);
			v[0].SetBlendUV(advancedUVCurrent[2], 0);
			v[0].SetBlendUV(advancedUVv1[2], 1);
			v[0].SetBlendAlphaUV(advancedUVCurrent[3], 0);
			v[0].SetBlendAlphaUV(advancedUVv1[3], 1);
			v[0].SetBlendUVDistortionUV(advancedUVCurrent[4], 0);
			v[0].SetBlendUVDistortionUV(advancedUVv1[4], 1);

			v[1].SetAlphaUV(advancedUVCurrent[0], 0);
			v[1].SetAlphaUV(advancedUVv2[0], 1);
			v[1].SetUVDistortionUV(advancedUVCurrent[1], 0);
			v[1].SetUVDistortionUV(advancedUVv2[1], 1);
			v[1].SetBlendUV(advancedUVCurrent[2], 0);
			v[1].SetBlendUV(advancedUVv2[2], 1);
			v[1].SetBlendAlphaUV(advancedUVCurrent[3], 0);
			v[1].SetBlendAlphaUV(advancedUVv2[3], 1);
			v[1].SetBlendUVDistortionUV(advancedUVCurrent[4], 0);
			v[1].SetBlendUVDistortionUV(advancedUVv2[4], 1);

			v[2].SetAlphaUV(advancedUVtexNext[0], 0);
			v[2].SetAlphaUV(advancedUVv1[0], 1);
			v[2].SetUVDistortionUV(advancedUVtexNext[1], 0);
			v[2].SetUVDistortionUV(advancedUVv1[1], 1);
			v[2].SetBlendUV(advancedUVtexNext[2], 0);
			v[2].SetBlendUV(advancedUVv1[2], 1);
			v[2].SetBlendAlphaUV(advancedUVtexNext[3], 0);
			v[2].SetBlendAlphaUV(advancedUVv1[3], 1);
			v[2].SetBlendUVDistortionUV(advancedUVtexNext[4], 0);
			v[2].SetBlendUVDistortionUV(advancedUVv1[4], 1);

			v[3].SetAlphaUV(advancedUVtexNext[0], 0);
			v[3].SetAlphaUV(advancedUVv2[0], 1);
			v[3].SetUVDistortionUV(advancedUVtexNext[1], 0);
			v[3].SetUVDistortionUV(advancedUVv2[1], 1);
			v[3].SetBlendUV(advancedUVtexNext[2], 0);
			v[3].SetBlendUV(advancedUVv2[2], 1);
			v[3].SetBlendAlphaUV(advancedUVtexNext[3], 0);
			v[3].SetBlendAlphaUV(advancedUVv2[3], 1);
			v[3].SetBlendUVDistortionUV(advancedUVtexNext[4], 0);
			v[3].SetBlendUVDistortionUV(advancedUVv2[4], 1);

			v[4] = v[1];

			v[5].SetAlphaUV(advancedUVCurrent[0], 0);
			v[5].SetAlphaUV(advancedUVv3[0], 1);
			v[5].SetUVDistortionUV(advancedUVCurrent[1], 0);
			v[5].SetUVDistortionUV(advancedUVv3[1], 1);
			v[5].SetBlendUV(advancedUVCurrent[2], 0);
			v[5].SetBlendUV(advancedUVv3[2], 1);
			v[5].SetBlendAlphaUV(advancedUVCurrent[3], 0);
			v[5].SetBlendAlphaUV(advancedUVv3[3], 1);
			v[5].SetBlendUVDistortionUV(advancedUVCurrent[4], 0);
			v[5].SetBlendUVDistortionUV(advancedUVv3[4], 1);

			v[6] = v[3];

			v[7].SetAlphaUV(advancedUVtexNext[0], 0);
			v[7].SetAlphaUV(advancedUVv3[0], 1);
			v[7].SetUVDistortionUV(advancedUVtexNext[1], 0);
			v[7].SetUVDistortionUV(advancedUVv3[1], 1);
			v[7].SetBlendUV(advancedUVtexNext[2], 0);
			v[7].SetBlendUV(advancedUVv3[2], 1);
			v[7].SetBlendAlphaUV(advancedUVtexNext[3], 0);
			v[7].SetBlendAlphaUV(advancedUVv3[3], 1);
			v[7].SetBlendUVDistortionUV(advancedUVtexNext[4], 0);
			v[7].SetBlendUVDistortionUV(advancedUVv3[4], 1);

			for (int32_t vi = 0; vi < 8; vi++)
			{
				v[vi].SetFlipbookIndexAndNextRate(instanceParameter.FlipbookIndexAndNextRate);
				v[vi].SetAlphaThreshold(instanceParameter.AlphaThreshold);
			}

			// distortion
			if (IsDistortionVertex<VERTEX>())
			{
				StrideView<VERTEX> vs(&verteies[i], stride_, 8);
				const auto binormalCurrent = ToStruct(v[5].Pos - v[0].Pos);
				const auto binormalNext = ToStruct(v[7].Pos - v[2].Pos);

				// return back
				float t_b;
				t_b = old_c * (stepC)-old_s * (-stepS);
				auto s_b = old_s * (stepC)+old_c * (-stepS);
				auto c_b = t_b;

				::Effekseer::Vec3f outerBefore(c_b * outerRadius, s_b * outerRadius, outerHeight);

				// next
				auto t_n = cos_ * stepC - sin_ * stepS;
				auto s_n = sin_ * stepC + cos_ * stepS;
				auto c_n = t_n;

				::Effekseer::Vec3f outerNN(c_n * outerRadius, s_n * outerRadius, outerHeight);

				::Effekseer::Vec3f tangent0 = (outerCurrent - outerBefore).Normalize();
				::Effekseer::Vec3f tangent1 = (outerNext - outerCurrent).Normalize();
				::Effekseer::Vec3f tangent2 = (outerNN - outerNext).Normalize();

				const auto tangentCurrent = ToStruct((tangent0 + tangent1) / 2.0f);
				const auto tangentNext = ToStruct((tangent1 + tangent2) / 2.0f);

				vs[0].SetTangent(tangentCurrent);
				vs[0].SetBinormal(binormalCurrent);
				vs[1].SetTangent(tangentCurrent);
				vs[1].SetBinormal(binormalCurrent);
				vs[2].SetTangent(tangentNext);
				vs[2].SetBinormal(binormalNext);
				vs[3].SetTangent(tangentNext);
				vs[3].SetBinormal(binormalNext);

				vs[4].SetTangent(tangentCurrent);
				vs[4].SetBinormal(binormalCurrent);
				vs[5].SetTangent(tangentCurrent);
				vs[5].SetBinormal(binormalCurrent);
				vs[6].SetTangent(tangentNext);
				vs[6].SetBinormal(binormalNext);
				vs[7].SetTangent(tangentNext);
				vs[7].SetBinormal(binormalNext);
			}
			else if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
			{
				StrideView<VERTEX> vs(&verteies[i], stride_, 8);

				// return back
				float t_b;
				t_b = old_c * (stepC)-old_s * (-stepS);
				auto s_b = old_s * (stepC)+old_c * (-stepS);
				auto c_b = t_b;

				::Effekseer::Vec3f outerBefore{ c_b * outerRadius, s_b * outerRadius, outerHeight };

				// next
				auto t_n = cos_ * stepC - sin_ * stepS;
				auto s_n = sin_ * stepC + cos_ * stepS;
				auto c_n = t_n;

				::Effekseer::Vec3f outerNN;
				outerNN.SetX(c_n * outerRadius);
				outerNN.SetY(s_n * outerRadius);
				outerNN.SetZ(outerHeight);

				::Effekseer::Vec3f tangent0 = (outerCurrent - outerBefore).Normalize();
				::Effekseer::Vec3f tangent1 = (outerNext - outerCurrent).Normalize();
				::Effekseer::Vec3f tangent2 = (outerNN - outerNext).Normalize();

				auto tangentCurrent = (tangent0 + tangent1) / 2.0f;
				auto tangentNext = (tangent1 + tangent2) / 2.0f;

				auto binormalCurrent = v[5].Pos - v[0].Pos;
				auto binormalNext = v[7].Pos - v[2].Pos;

				::Effekseer::Vec3f normalCurrent;
				::Effekseer::Vec3f normalNext;

				normalCurrent = ::Effekseer::Vec3f::Cross(tangentCurrent, binormalCurrent);
				normalNext = ::Effekseer::Vec3f::Cross(tangentNext, binormalNext);

				if (!parameter.IsRightHand)
				{
					normalCurrent = -normalCurrent;
					normalNext = -normalNext;
				}

				// rotate directions
				::Effekseer::Mat43f matRot = mat43;
				matRot.SetTranslation({ 0.0f, 0.0f, 0.0f });

				normalCurrent = ::Effekseer::Vec3f::Transform(normalCurrent, matRot);
				normalNext = ::Effekseer::Vec3f::Transform(normalNext, matRot);
				tangentCurrent = ::Effekseer::Vec3f::Transform(tangentCurrent, matRot);
				tangentNext = ::Effekseer::Vec3f::Transform(tangentNext, matRot);

				normalCurrent = normalCurrent.Normalize();
				normalNext = normalNext.Normalize();
				tangentCurrent = tangentCurrent.Normalize();
				tangentNext = tangentNext.Normalize();

				const auto packedNormalCurrent = PackVector3DF(normalCurrent);
				const auto packedNormalNext = PackVector3DF(normalNext);
				const auto packedTangentCurrent = PackVector3DF(tangentCurrent);
				const auto packedTangentNext = PackVector3DF(tangentNext);

				vs[0].SetPackedNormal(packedNormalCurrent);
				vs[1].SetPackedNormal(packedNormalCurrent);
				vs[2].SetPackedNormal(packedNormalNext);
				vs[3].SetPackedNormal(packedNormalNext);

				vs[4].SetPackedNormal(packedNormalCurrent);
				vs[5].SetPackedNormal(packedNormalCurrent);
				vs[6].SetPackedNormal(packedNormalNext);
				vs[7].SetPackedNormal(packedNormalNext);

				vs[0].SetPackedTangent(packedTangentCurrent);
				vs[1].SetPackedTangent(packedTangentCurrent);
				vs[2].SetPackedTangent(packedTangentNext);
				vs[3].SetPackedTangent(packedTangentNext);

				vs[4].SetPackedTangent(packedTangentCurrent);
				vs[5].SetPackedTangent(packedTangentCurrent);
				vs[6].SetPackedTangent(packedTangentNext);
				vs[7].SetPackedTangent(packedTangentNext);

				// uv1
				uv1texNext = uv1Current + uv1Step;

				vs[0].SetUV2(uv1Current, uv1v1);
				vs[1].SetUV2(uv1Current, uv1v2);
				vs[2].SetUV2(uv1texNext, uv1v1);
				vs[3].SetUV2(uv1texNext, uv1v2);

				vs[4].SetUV2(uv1Current, uv1v2);

				vs[5].SetUV2(uv1Current, uv1v3);

				vs[6].SetUV2(uv1texNext, uv1v2);

				vs[7].SetUV2(uv1texNext, uv1v3);
			}

			outerCurrent = outerNext;
			innerCurrent = innerNext;
			centerCurrent = centerNext;
			uv0Current = uv0texNext;
			uv1Current = uv1texNext;
			for (int32_t uvi = 0; uvi < advancedUVNum; uvi++)
			{
				advancedUVCurrent[uvi] = advancedUVtexNext[uvi];
			}

			outerColor = outerColorNext;
			innerColor = innerColorNext;
			centerColor = centerColorNext;
		}

		if (CanSingleRendering())
		{
			m_singleRenderingMatrix = mat43;
		}
		else
		{
			TransformVertexes(verteies, singleVertexCount, mat43);
		}

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData1.data(), sizeof(float) * customData1Count_);
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(
				m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData2.data(), sizeof(float) * customData2Count_);
			}
		}

		m_spriteCount += 2 * parameter.VertexCount;
		m_ringBufferData += stride_ * singleVertexCount;
	}

	void EndRendering_(RENDERER* renderer, const efkRingNodeParam& param, void* userData, const ::Effekseer::Mat44f& camera)
	{
		if (CanSingleRendering())
		{
			::Effekseer::Mat44f mat = m_singleRenderingMatrix * renderer->GetCameraMatrix();

			renderer->GetStandardRenderer()->Rendering(mat, renderer->GetProjectionMatrix());
		}

		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None && !CanSingleRendering())
		{
			for (auto& kv : instances_)
			{
				efkVector3D t = kv.Value.SRTMatrix43.GetTranslation();

				Effekseer::Vec3f frontDirection = m_renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection.SetZ(-frontDirection.GetZ());
				}

				kv.Key = Effekseer::Vec3f::Dot(t, frontDirection);
			}

			if (param.DepthParameterPtr->ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(instances_.begin(), instances_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key < b.Key; });
			}
			else
			{
				std::sort(instances_.begin(), instances_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key > b.Key; });
			}

			const auto& state = m_renderer->GetStandardRenderer()->GetState();

			for (auto& kv : instances_)
			{
				RenderingInstance(kv.Value, param, state, camera);
			}
		}
	}

public:
	void BeginRendering(const efkRingNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_(m_renderer, count, parameter);
	}

	void Rendering(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData)
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount())
			return;
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void EndRendering(const efkRingNodeParam& parameter, void* userData)
	{
		if (m_ringBufferData == NULL)
			return;

		if (m_spriteCount == 0 && parameter.DepthParameterPtr->ZSort == Effekseer::ZSortType::None)
			return;

		EndRendering_(m_renderer, parameter, userData, m_renderer->GetCameraMatrix());
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RING_RENDERER_H__

#ifndef __EFFEKSEERRENDERER_SPRITE_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_SPRITE_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <string.h>


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::SpriteRenderer::NodeParameter efkSpriteNodeParam;
typedef ::Effekseer::SpriteRenderer::InstanceParameter efkSpriteInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class SpriteRendererBase : public ::Effekseer::SpriteRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	RENDERER* m_renderer;
	int32_t m_spriteCount;
	uint8_t* m_ringBufferData;

	struct KeyValue
	{
		float Key;
		efkSpriteInstanceParam Value;
	};

	Effekseer::CustomAlignedVector<KeyValue> instances;
	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;
	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

public:
	SpriteRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_spriteCount(0)
		, m_ringBufferData(nullptr)
	{
		// reserve buffers
		instances.reserve(m_renderer->GetSquareMaxCount());
	}

	virtual ~SpriteRendererBase()
	{
	}

protected:
	void RenderingInstance(const efkSpriteInstanceParam& inst,
						   const efkSpriteNodeParam& param,
						   const StandardRendererState& state,
						   const ::Effekseer::Mat44f& camera)
	{

		bool isAdvanced = state.IsAdvanced();

		if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			Rendering_Internal<AdvancedVertexDistortion, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (isAdvanced)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			Rendering_Internal<VertexDistortion, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(param, inst, nullptr, camera);
		}
	}

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkSpriteNodeParam& param)
	{
		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;
		state.TextureFilter4 = param.BasicParameterPtr->TextureFilter4;
		state.TextureWrap4 = param.BasicParameterPtr->TextureWrap4;
		state.TextureFilter5 = param.BasicParameterPtr->TextureFilter5;
		state.TextureWrap5 = param.BasicParameterPtr->TextureWrap5;
		state.TextureFilter6 = param.BasicParameterPtr->TextureFilter6;
		state.TextureWrap6 = param.BasicParameterPtr->TextureWrap6;
		state.TextureFilter7 = param.BasicParameterPtr->TextureFilter7;
		state.TextureWrap7 = param.BasicParameterPtr->TextureWrap7;

		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index
											   ,
											   param.BasicParameterPtr->Texture3Index,
											   param.BasicParameterPtr->Texture4Index,
											   param.BasicParameterPtr->Texture5Index,
											   param.BasicParameterPtr->Texture6Index,
											   param.BasicParameterPtr->Texture7Index
		);
		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(count * 4, stride_, (void*&)m_ringBufferData);
		m_spriteCount = 0;

		vertexCount_ = count * 4;

		instances.clear();
	}

	void Rendering_(const efkSpriteNodeParam& parameter,
					const efkSpriteInstanceParam& instanceParameter,
					void* userData,
					const ::Effekseer::Mat44f& camera)
	{
		if (parameter.ZSort == Effekseer::ZSortType::None)
		{
			auto camera = m_renderer->GetCameraMatrix();
			const auto& state = m_renderer->GetStandardRenderer()->GetState();

			RenderingInstance(instanceParameter, parameter, state, camera);
		}
		else
		{
			KeyValue kv;
			kv.Value = instanceParameter;
			instances.push_back(kv);
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkSpriteNodeParam& parameter,
							const efkSpriteInstanceParam& instanceParameter,
							void* userData,
							const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == nullptr)
			return;

		StrideView<VERTEX> verteies(m_ringBufferData, stride_, 4);

		for (int i = 0; i < 4; i++)
		{
			verteies[i].Pos.X = instanceParameter.Positions[i].GetX();
			verteies[i].Pos.Y = instanceParameter.Positions[i].GetY();
			verteies[i].Pos.Z = 0.0f;

			verteies[i].SetColor(instanceParameter.Colors[i], FLIP_RGB);

			verteies[i].SetFlipbookIndexAndNextRate(instanceParameter.FlipbookIndexAndNextRate);
			verteies[i].SetAlphaThreshold(instanceParameter.AlphaThreshold);
		}

		verteies[0].UV[0] = instanceParameter.UV.X;
		verteies[0].UV[1] = instanceParameter.UV.Y + instanceParameter.UV.Height;

		verteies[1].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
		verteies[1].UV[1] = instanceParameter.UV.Y + instanceParameter.UV.Height;

		verteies[2].UV[0] = instanceParameter.UV.X;
		verteies[2].UV[1] = instanceParameter.UV.Y;

		verteies[3].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
		verteies[3].UV[1] = instanceParameter.UV.Y;

		verteies[0].SetAlphaUV(instanceParameter.AlphaUV.X, 0);
		verteies[0].SetAlphaUV(instanceParameter.AlphaUV.Y + instanceParameter.AlphaUV.Height, 1);

		verteies[1].SetAlphaUV(instanceParameter.AlphaUV.X + instanceParameter.AlphaUV.Width, 0);
		verteies[1].SetAlphaUV(instanceParameter.AlphaUV.Y + instanceParameter.AlphaUV.Height, 1);

		verteies[2].SetAlphaUV(instanceParameter.AlphaUV.X, 0);
		verteies[2].SetAlphaUV(instanceParameter.AlphaUV.Y, 1);

		verteies[3].SetAlphaUV(instanceParameter.AlphaUV.X + instanceParameter.AlphaUV.Width, 0);
		verteies[3].SetAlphaUV(instanceParameter.AlphaUV.Y, 1);

		verteies[0].SetUVDistortionUV(instanceParameter.UVDistortionUV.X, 0);
		verteies[0].SetUVDistortionUV(instanceParameter.UVDistortionUV.Y + instanceParameter.UVDistortionUV.Height, 1);

		verteies[1].SetUVDistortionUV(instanceParameter.UVDistortionUV.X + instanceParameter.UVDistortionUV.Width, 0);
		verteies[1].SetUVDistortionUV(instanceParameter.UVDistortionUV.Y + instanceParameter.UVDistortionUV.Height, 1);

		verteies[2].SetUVDistortionUV(instanceParameter.UVDistortionUV.X, 0);
		verteies[2].SetUVDistortionUV(instanceParameter.UVDistortionUV.Y, 1);

		verteies[3].SetUVDistortionUV(instanceParameter.UVDistortionUV.X + instanceParameter.UVDistortionUV.Width, 0);
		verteies[3].SetUVDistortionUV(instanceParameter.UVDistortionUV.Y, 1);

		verteies[0].SetBlendUV(instanceParameter.BlendUV.X, 0);
		verteies[0].SetBlendUV(instanceParameter.BlendUV.Y + instanceParameter.BlendUV.Height, 1);

		verteies[1].SetBlendUV(instanceParameter.BlendUV.X + instanceParameter.BlendUV.Width, 0);
		verteies[1].SetBlendUV(instanceParameter.BlendUV.Y + instanceParameter.BlendUV.Height, 1);

		verteies[2].SetBlendUV(instanceParameter.BlendUV.X, 0);
		verteies[2].SetBlendUV(instanceParameter.BlendUV.Y, 1);

		verteies[3].SetBlendUV(instanceParameter.BlendUV.X + instanceParameter.BlendUV.Width, 0);
		verteies[3].SetBlendUV(instanceParameter.BlendUV.Y, 1);

		verteies[0].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.X, 0);
		verteies[0].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.Y + instanceParameter.BlendAlphaUV.Height, 1);

		verteies[1].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.X + instanceParameter.BlendAlphaUV.Width, 0);
		verteies[1].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.Y + instanceParameter.BlendAlphaUV.Height, 1);

		verteies[2].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.X, 0);
		verteies[2].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.Y, 1);

		verteies[3].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.X + instanceParameter.BlendAlphaUV.Width, 0);
		verteies[3].SetBlendAlphaUV(instanceParameter.BlendAlphaUV.Y, 1);

		verteies[0].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.X, 0);
		verteies[0].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.Y + instanceParameter.BlendUVDistortionUV.Height, 1);

		verteies[1].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.X + instanceParameter.BlendUVDistortionUV.Width, 0);
		verteies[1].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.Y + instanceParameter.BlendUVDistortionUV.Height, 1);

		verteies[2].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.X, 0);
		verteies[2].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.Y, 1);

		verteies[3].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.X + instanceParameter.BlendUVDistortionUV.Width, 0);
		verteies[3].SetBlendUVDistortionUV(instanceParameter.BlendUVDistortionUV.Y, 1);

		// distortion
		if (IsDistortionVertex<VERTEX>())
		{
			StrideView<VertexDistortion> vs(verteies.pointerOrigin_, stride_, 4);
			for (auto i = 0; i < 4; i++)
			{
				vs[i].SetTangent(Effekseer::Vector3D(1.0f, 0.0f, 0.0f));
				vs[i].SetBinormal(Effekseer::Vector3D(0.0f, 1.0f, 0.0f));
			}
		}
		else if (IsLightingVertex<VERTEX>() || IsDynamicVertex<VERTEX>())
		{
			StrideView<VERTEX> vs(verteies.pointerOrigin_, stride_, 4);
			vs[0].SetUV2(0.0f, 1.0f);
			vs[1].SetUV2(1.0f, 1.0f);
			vs[2].SetUV2(0.0f, 0.0f);
			vs[3].SetUV2(1.0f, 0.0f);
		}

		if (parameter.Billboard == ::Effekseer::BillboardType::Billboard ||
			parameter.Billboard == ::Effekseer::BillboardType::RotatedBillboard ||
			parameter.Billboard == ::Effekseer::BillboardType::YAxisFixed)
		{
			Effekseer::Mat43f mat_rot = Effekseer::Mat43f::Identity;
			Effekseer::Vec3f s;
			Effekseer::Vec3f R;
			Effekseer::Vec3f F;

			if (parameter.EnableViewOffset == true)
			{
				Effekseer::Mat43f instMat = instanceParameter.SRTMatrix43;

				ApplyViewOffset(instMat, camera, instanceParameter.ViewOffsetDistance);

				CalcBillboard(parameter.Billboard, mat_rot, s, R, F, instMat, m_renderer->GetCameraFrontDirection());
			}
			else
			{
				CalcBillboard(parameter.Billboard, mat_rot, s, R, F, instanceParameter.SRTMatrix43, m_renderer->GetCameraFrontDirection());
			}

			for (int i = 0; i < 4; i++)
			{
				verteies[i].Pos.X = verteies[i].Pos.X * s.GetX();
				verteies[i].Pos.Y = verteies[i].Pos.Y * s.GetY();
			}

			ApplyDepthParameters(mat_rot,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 s,
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			TransformVertexes(verteies, 4, mat_rot);

			if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
			{
				if (!parameter.IsRightHand)
				{
					F = -F;
				}

				StrideView<VERTEX> vs(verteies.pointerOrigin_, stride_, 4);
				for (auto i = 0; i < 4; i++)
				{
					vs[i].SetPackedNormal(PackVector3DF(F));
					vs[i].SetPackedTangent(PackVector3DF(R));
				}
			}
		}
		else if (parameter.Billboard == ::Effekseer::BillboardType::Fixed)
		{
			auto mat = instanceParameter.SRTMatrix43;

			if (parameter.EnableViewOffset == true)
			{
				ApplyViewOffset(mat, camera, instanceParameter.ViewOffsetDistance);
			}

			ApplyDepthParameters(mat,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			for (int i = 0; i < 4; i++)
			{
				auto Pos = ::Effekseer::Vec3f::Load(&verteies[i].Pos);
				Pos = ::Effekseer::Vec3f::Transform(Pos, mat);
				::Effekseer::Vec3f::Store(&verteies[i].Pos, Pos);

				// distortion
				if (IsDistortionVertex<VERTEX>())
				{
					auto vs = (VertexDistortion*)&verteies[i];
					auto tangentX = efkVector3D(mat.X.GetX(), mat.Y.GetX(), mat.Z.GetX());
					auto tangentY = efkVector3D(mat.X.GetY(), mat.Y.GetY(), mat.Z.GetY());
					tangentX = tangentX.Normalize();
					tangentY = tangentY.Normalize();

					verteies[i].SetTangent(ToStruct(tangentX));
					verteies[i].SetBinormal(ToStruct(tangentY));
				}
				else if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
				{
					StrideView<VERTEX> vs(verteies.pointerOrigin_, stride_, 4);
					auto tangentX = efkVector3D(mat.X.GetX(), mat.Y.GetX(), mat.Z.GetX());
					auto tangentZ = efkVector3D(mat.X.GetZ(), mat.Y.GetZ(), mat.Z.GetZ());
					tangentX = tangentX.Normalize();
					tangentZ = tangentZ.Normalize();

					if (!parameter.IsRightHand)
					{
						tangentZ = -tangentZ;
					}

					vs[i].SetPackedNormal(PackVector3DF(tangentZ));
					vs[i].SetPackedTangent(PackVector3DF(tangentX));
				}
			}
		}

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, 4);
			for (int i = 0; i < 4; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData1.data(), sizeof(float) * customData1Count_);
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, 4);
			for (int i = 0; i < 4; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData2.data(), sizeof(float) * customData2Count_);
			}
		}

		m_ringBufferData += (stride_ * 4);
		m_spriteCount++;
	}

	void EndRendering_(RENDERER* renderer, const efkSpriteNodeParam& param)
	{
		if (param.ZSort != Effekseer::ZSortType::None)
		{
			for (auto& kv : instances)
			{
				efkVector3D t = kv.Value.SRTMatrix43.GetTranslation();

				auto frontDirection = m_renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection.Z = -frontDirection.Z;
				}

				kv.Key = Effekseer::Vec3f::Dot(t, frontDirection);
			}

			if (param.ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(instances.begin(), instances.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key < b.Key; });
			}
			else
			{
				std::sort(instances.begin(), instances.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key > b.Key; });
			}

			for (auto& kv : instances)
			{
				auto camera = m_renderer->GetCameraMatrix();
				const auto& state = renderer->GetStandardRenderer()->GetState();

				RenderingInstance(kv.Value, param, state, camera);
			}
		}
	}

public:
	void BeginRendering(const efkSpriteNodeParam& parameter, int32_t count, void* userData) override
	{
		BeginRendering_(m_renderer, count, parameter);
	}

	void Rendering(const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData) override
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount())
			return;
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void EndRendering(const efkSpriteNodeParam& parameter, void* userData) override
	{
		EndRendering_(m_renderer, parameter);
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_SPRITE_RENDERER_H__


#ifndef __EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <string.h>


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::TrackRenderer::NodeParameter efkTrackNodeParam;
typedef ::Effekseer::TrackRenderer::InstanceParameter efkTrackInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class TrackRendererBase : public ::Effekseer::TrackRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	RENDERER* m_renderer;
	int32_t m_ribbonCount;

	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	efkTrackNodeParam innstancesNodeParam;
	Effekseer::CustomAlignedVector<efkTrackInstanceParam> instances;
	SplineGenerator spline;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	template <typename VERTEX, int TARGET>
	void AssignUV(StrideView<VERTEX>& v, float uvX1, float uvX2, float uvX3, float uvY1, float uvY2)
	{
		if (TARGET == 0)
		{
			v[0].UV[0] = uvX1;
			v[0].UV[1] = uvY1;

			v[1].UV[0] = uvX2;
			v[1].UV[1] = uvY1;

			v[4].UV[0] = uvX2;
			v[4].UV[1] = uvY1;

			v[5].UV[0] = uvX3;
			v[5].UV[1] = uvY1;

			v[2].UV[0] = uvX1;
			v[2].UV[1] = uvY2;

			v[3].UV[0] = uvX2;
			v[3].UV[1] = uvY2;

			v[6].UV[0] = uvX2;
			v[6].UV[1] = uvY2;

			v[7].UV[0] = uvX3;
			v[7].UV[1] = uvY2;
		}
		else if (TARGET == 1)
		{
			v[0].UV2[0] = uvX1;
			v[0].UV2[1] = uvY1;

			v[1].UV2[0] = uvX2;
			v[1].UV2[1] = uvY1;

			v[4].UV2[0] = uvX2;
			v[4].UV2[1] = uvY1;

			v[5].UV2[0] = uvX3;
			v[5].UV2[1] = uvY1;

			v[2].UV2[0] = uvX1;
			v[2].UV2[1] = uvY2;

			v[3].UV2[0] = uvX2;
			v[3].UV2[1] = uvY2;

			v[6].UV2[0] = uvX2;
			v[6].UV2[1] = uvY2;

			v[7].UV2[0] = uvX3;
			v[7].UV2[1] = uvY2;
		}
		else if (TARGET == 2)
		{
			v[0].SetAlphaUV(uvX1, 0);
			v[0].SetAlphaUV(uvY1, 1);

			v[1].SetAlphaUV(uvX2, 0);
			v[1].SetAlphaUV(uvY1, 1);

			v[4].SetAlphaUV(uvX2, 0);
			v[4].SetAlphaUV(uvY1, 1);

			v[5].SetAlphaUV(uvX3, 0);
			v[5].SetAlphaUV(uvY1, 1);

			v[2].SetAlphaUV(uvX1, 0);
			v[2].SetAlphaUV(uvY2, 1);

			v[3].SetAlphaUV(uvX2, 0);
			v[3].SetAlphaUV(uvY2, 1);

			v[6].SetAlphaUV(uvX2, 0);
			v[6].SetAlphaUV(uvY2, 1);

			v[7].SetAlphaUV(uvX3, 0);
			v[7].SetAlphaUV(uvY2, 1);
		}
		else if (TARGET == 3)
		{
			v[0].SetUVDistortionUV(uvX1, 0);
			v[0].SetUVDistortionUV(uvY1, 1);

			v[1].SetUVDistortionUV(uvX2, 0);
			v[1].SetUVDistortionUV(uvY1, 1);

			v[4].SetUVDistortionUV(uvX2, 0);
			v[4].SetUVDistortionUV(uvY1, 1);

			v[5].SetUVDistortionUV(uvX3, 0);
			v[5].SetUVDistortionUV(uvY1, 1);

			v[2].SetUVDistortionUV(uvX1, 0);
			v[2].SetUVDistortionUV(uvY2, 1);

			v[3].SetUVDistortionUV(uvX2, 0);
			v[3].SetUVDistortionUV(uvY2, 1);

			v[6].SetUVDistortionUV(uvX2, 0);
			v[6].SetUVDistortionUV(uvY2, 1);

			v[7].SetUVDistortionUV(uvX3, 0);
			v[7].SetUVDistortionUV(uvY2, 1);
		}
		else if (TARGET == 4)
		{
			v[0].SetBlendUV(uvX1, 0);
			v[0].SetBlendUV(uvY1, 1);

			v[1].SetBlendUV(uvX2, 0);
			v[1].SetBlendUV(uvY1, 1);

			v[4].SetBlendUV(uvX2, 0);
			v[4].SetBlendUV(uvY1, 1);

			v[5].SetBlendUV(uvX3, 0);
			v[5].SetBlendUV(uvY1, 1);

			v[2].SetBlendUV(uvX1, 0);
			v[2].SetBlendUV(uvY2, 1);

			v[3].SetBlendUV(uvX2, 0);
			v[3].SetBlendUV(uvY2, 1);

			v[6].SetBlendUV(uvX2, 0);
			v[6].SetBlendUV(uvY2, 1);

			v[7].SetBlendUV(uvX3, 0);
			v[7].SetBlendUV(uvY2, 1);
		}
		else if (TARGET == 5)
		{
			v[0].SetBlendAlphaUV(uvX1, 0);
			v[0].SetBlendAlphaUV(uvY1, 1);

			v[1].SetBlendAlphaUV(uvX2, 0);
			v[1].SetBlendAlphaUV(uvY1, 1);

			v[4].SetBlendAlphaUV(uvX2, 0);
			v[4].SetBlendAlphaUV(uvY1, 1);

			v[5].SetBlendAlphaUV(uvX3, 0);
			v[5].SetBlendAlphaUV(uvY1, 1);

			v[2].SetBlendAlphaUV(uvX1, 0);
			v[2].SetBlendAlphaUV(uvY2, 1);

			v[3].SetBlendAlphaUV(uvX2, 0);
			v[3].SetBlendAlphaUV(uvY2, 1);

			v[6].SetBlendAlphaUV(uvX2, 0);
			v[6].SetBlendAlphaUV(uvY2, 1);

			v[7].SetBlendAlphaUV(uvX3, 0);
			v[7].SetBlendAlphaUV(uvY2, 1);
		}
		else if (TARGET == 6)
		{
			v[0].SetBlendUVDistortionUV(uvX1, 0);
			v[0].SetBlendUVDistortionUV(uvY1, 1);

			v[1].SetBlendUVDistortionUV(uvX2, 0);
			v[1].SetBlendUVDistortionUV(uvY1, 1);

			v[4].SetBlendUVDistortionUV(uvX2, 0);
			v[4].SetBlendUVDistortionUV(uvY1, 1);

			v[5].SetBlendUVDistortionUV(uvX3, 0);
			v[5].SetBlendUVDistortionUV(uvY1, 1);

			v[2].SetBlendUVDistortionUV(uvX1, 0);
			v[2].SetBlendUVDistortionUV(uvY2, 1);

			v[3].SetBlendUVDistortionUV(uvX2, 0);
			v[3].SetBlendUVDistortionUV(uvY2, 1);

			v[6].SetBlendUVDistortionUV(uvX2, 0);
			v[6].SetBlendUVDistortionUV(uvY2, 1);

			v[7].SetBlendUVDistortionUV(uvX3, 0);
			v[7].SetBlendUVDistortionUV(uvY2, 1);
		}
	}

	template <typename VERTEX, int TARGET>
	void AssignUVs(efkTrackNodeParam& parameter, StrideView<VERTEX> verteies)
	{
		float uvx = 0.0f;
		float uvw = 1.0f;
		float uvy = 0.0f;
		float uvh = 1.0f;

		if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Strech)
		{
			verteies.Reset();
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				const auto& param = instances[loop];
				if (TARGET == 0)
				{
					uvx = param.UV.X;
					uvw = param.UV.Width;
					uvy = param.UV.Y;
					uvh = param.UV.Height;
				}
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
				}

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
									 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
									 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					auto uvX1 = uvx;
					auto uvX2 = uvx + uvw * 0.5f;
					auto uvX3 = uvx + uvw;
					auto uvY1 = uvy + percent1 * uvh;
					auto uvY2 = uvy + percent2 * uvh;

					AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

					verteies += 8;
				}
			}
		}
		else if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Tile)
		{
			const auto& uvParam = *parameter.TextureUVTypeParameterPtr;
			verteies.Reset();

			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];
				if (TARGET == 0)
				{
					uvx = param.UV.X;
					uvw = param.UV.Width;
					uvy = param.UV.Y;
					uvh = param.UV.Height;
				}
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
				}

				if (loop < uvParam.TileEdgeTail)
				{
					float uvBegin = uvy;
					float uvEnd = uvy + uvh * uvParam.TileLoopAreaBegin;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
										 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

						float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
										 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
					}
				}
				else if (loop >= param.InstanceCount - 1 - uvParam.TileEdgeHead)
				{
					float uvBegin = uvy + uvh * uvParam.TileLoopAreaEnd;
					float uvEnd = uvy + uvh * 1.0f;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
									sploop) /
							(float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

						float percent2 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
									sploop + 1) /
							(float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
					}
				}
				else
				{
					float uvBegin = uvy + uvh * uvParam.TileLoopAreaBegin;
					float uvEnd = uvy + uvh * uvParam.TileLoopAreaEnd;

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						bool isFirst = param.InstanceIndex == 0 && sploop == 0;
						bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

						float percent1 = (float)(sploop) / (float)(parameter.SplineDivision);

						float percent2 = (float)(sploop + 1) / (float)(parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
					}
				}
			}
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void RenderSplines(const ::Effekseer::Mat44f& camera)
	{
		if (instances.size() == 0)
		{
			return;
		}

		auto& parameter = innstancesNodeParam;

		// Calculate spline
		if (parameter.SplineDivision > 1)
		{
			spline.Reset();

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto p = efkVector3D();
				auto& param = instances[loop];

				auto mat = param.SRTMatrix43;

				if (parameter.EnableViewOffset == true)
				{
					ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
				}

				ApplyDepthParameters(mat,
									 m_renderer->GetCameraFrontDirection(),
									 m_renderer->GetCameraPosition(),
									 // s,
									 parameter.DepthParameterPtr,
									 parameter.IsRightHand);

				p = mat.GetTranslation();
				spline.AddVertex(p);
			}

			spline.Calculate();
		}

		StrideView<VERTEX> verteies(m_ringBufferData, stride_, vertexCount_);

		for (size_t loop = 0; loop < instances.size(); loop++)
		{
			auto& param = instances[loop];

			for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
			{
				auto mat = param.SRTMatrix43;

				if (parameter.EnableViewOffset == true)
				{
					ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
				}

				::Effekseer::Vec3f s;
				::Effekseer::Mat43f r;
				::Effekseer::Vec3f t;
				mat.GetSRT(s, r, t);

				ApplyDepthParameters(r,
									 t,
									 s,
									 m_renderer->GetCameraFrontDirection(),
									 m_renderer->GetCameraPosition(),
									 parameter.DepthParameterPtr,
									 parameter.IsRightHand);

				bool isFirst = param.InstanceIndex == 0 && sploop == 0;
				bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

				float size = 0.0f;
				::Effekseer::Color leftColor;
				::Effekseer::Color centerColor;
				::Effekseer::Color rightColor;

				float percent = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
								(float)((param.InstanceCount - 1) * parameter.SplineDivision);

				if (param.InstanceIndex < param.InstanceCount / 2)
				{
					float l = percent;
					l = l * 2.0f;
					size = param.SizeFor + (param.SizeMiddle - param.SizeFor) * l;

					leftColor.R = (uint8_t)Effekseer::Clamp(param.ColorLeft.R + (param.ColorLeftMiddle.R - param.ColorLeft.R) * l, 255, 0);
					leftColor.G = (uint8_t)Effekseer::Clamp(param.ColorLeft.G + (param.ColorLeftMiddle.G - param.ColorLeft.G) * l, 255, 0);
					leftColor.B = (uint8_t)Effekseer::Clamp(param.ColorLeft.B + (param.ColorLeftMiddle.B - param.ColorLeft.B) * l, 255, 0);
					leftColor.A = (uint8_t)Effekseer::Clamp(param.ColorLeft.A + (param.ColorLeftMiddle.A - param.ColorLeft.A) * l, 255, 0);

					centerColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
					centerColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
					centerColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
					centerColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

					rightColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
					rightColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
					rightColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
					rightColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
				}
				else
				{
					float l = percent;
					l = 1.0f - (l * 2.0f - 1.0f);
					size = param.SizeBack + (param.SizeMiddle - param.SizeBack) * l;

					leftColor.R = (uint8_t)Effekseer::Clamp(param.ColorLeft.R + (param.ColorLeftMiddle.R - param.ColorLeft.R) * l, 255, 0);
					leftColor.G = (uint8_t)Effekseer::Clamp(param.ColorLeft.G + (param.ColorLeftMiddle.G - param.ColorLeft.G) * l, 255, 0);
					leftColor.B = (uint8_t)Effekseer::Clamp(param.ColorLeft.B + (param.ColorLeftMiddle.B - param.ColorLeft.B) * l, 255, 0);
					leftColor.A = (uint8_t)Effekseer::Clamp(param.ColorLeft.A + (param.ColorLeftMiddle.A - param.ColorLeft.A) * l, 255, 0);

					centerColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
					centerColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
					centerColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
					centerColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

					rightColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
					rightColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
					rightColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
					rightColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
				}

				VERTEX v[3];

				v[0].Pos.X = (-size / 2.0f) * s.GetX();
				v[0].Pos.Y = 0.0f;
				v[0].Pos.Z = 0.0f;
				v[0].SetColor(leftColor, FLIP_RGB);

				v[1].Pos.X = 0.0f;
				v[1].Pos.Y = 0.0f;
				v[1].Pos.Z = 0.0f;
				v[1].SetColor(centerColor, FLIP_RGB);

				v[2].Pos.X = (size / 2.0f) * s.GetX();
				v[2].Pos.Y = 0.0f;
				v[2].Pos.Z = 0.0f;
				v[2].SetColor(rightColor, FLIP_RGB);

				v[0].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
				v[1].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
				v[2].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);

				v[0].SetAlphaThreshold(param.AlphaThreshold);
				v[1].SetAlphaThreshold(param.AlphaThreshold);
				v[2].SetAlphaThreshold(param.AlphaThreshold);

				if (parameter.SplineDivision > 1)
				{
					v[1].Pos = ToStruct(spline.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));
				}
				else
				{
					v[1].Pos = ToStruct(t);
				}

				if (isFirst)
				{
					verteies[0] = v[0];
					verteies[1] = v[1];
					verteies[4] = v[1];
					verteies[5] = v[2];
					verteies += 2;
				}
				else if (isLast)
				{
					verteies[0] = v[0];
					verteies[1] = v[1];
					verteies[4] = v[1];
					verteies[5] = v[2];
					verteies += 6;
					m_ribbonCount += 2;
				}
				else
				{
					verteies[0] = v[0];
					verteies[1] = v[1];
					verteies[4] = v[1];
					verteies[5] = v[2];

					verteies[6] = v[0];
					verteies[7] = v[1];
					verteies[10] = v[1];
					verteies[11] = v[2];

					verteies += 8;
					m_ribbonCount += 2;
				}

				if (isLast)
				{
					break;
				}
			}
		}

		// transform all vertecies
		{
			StrideView<VERTEX> vs_(m_ringBufferData, stride_, vertexCount_);
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));
				Effekseer::Vec3f axis;
				Effekseer::Vec3f pos;

				if (isFirst_)
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;
				}
				else if (isLast_)
				{
					axis = axisBefore;
				}
				else
				{
					Effekseer::Vec3f axisOld = axisBefore;
					axis = vs_[9].Pos - vs_[7].Pos;
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
				}

				pos = vs_[1].Pos;

				VERTEX vl = vs_[0];
				VERTEX vm = vs_[1];
				VERTEX vr = vs_[5];

				vm.Pos.X = 0.0f;
				vm.Pos.Y = 0.0f;
				vm.Pos.Z = 0.0f;

				::Effekseer::Vec3f F;
				::Effekseer::Vec3f R;
				::Effekseer::Vec3f U;

				// It can be optimized because X is only not zero.
				/*
				U = axis;

				F = ::Effekseer::Vec3f(m_renderer->GetCameraFrontDirection()).Normalize();
				R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
				F = ::Effekseer::Vec3f::Cross(R, U).Normalize();

				::Effekseer::Mat43f mat_rot(
					-R.GetX(), -R.GetY(), -R.GetZ(),
					 U.GetX(),  U.GetY(),  U.GetZ(),
					 F.GetX(),  F.GetY(),  F.GetZ(),
					pos.GetX(), pos.GetY(), pos.GetZ());

				vl.Pos = ToStruct(::Effekseer::Vec3f::Transform(vl.Pos, mat_rot));
				vm.Pos = ToStruct(::Effekseer::Vec3f::Transform(vm.Pos, mat_rot));
				vr.Pos = ToStruct(::Effekseer::Vec3f::Transform(vr.Pos,mat_rot));
				*/

				U = axis;
				F = m_renderer->GetCameraFrontDirection();
				R = SafeNormalize(::Effekseer::Vec3f::Cross(U, F));

				assert(vl.Pos.Y == 0.0f);
				assert(vr.Pos.Y == 0.0f);
				assert(vl.Pos.Z == 0.0f);
				assert(vr.Pos.Z == 0.0f);
				assert(vm.Pos.X == 0.0f);
				assert(vm.Pos.Y == 0.0f);
				assert(vm.Pos.Z == 0.0f);

				vl.Pos = ToStruct(-R * vl.Pos.X + pos);
				vm.Pos = ToStruct(pos);
				vr.Pos = ToStruct(-R * vr.Pos.X + pos);

				if (IsDistortionVertex<VERTEX>())
				{
					const auto binormalVector = ToStruct(axis);
					vl.SetBinormal(binormalVector);
					vm.SetBinormal(binormalVector);
					vr.SetBinormal(binormalVector);

					::Effekseer::Vec3f tangent = vl.Pos - vr.Pos;
					tangent.Normalize();

					const auto tangentVector = ToStruct(tangent);

					vl.SetTangent(tangentVector);
					vm.SetTangent(tangentVector);
					vr.SetTangent(tangentVector);
				}
				else if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
				{
					::Effekseer::Vec3f tangent = SafeNormalize(Effekseer::Vec3f(vl.Pos - vr.Pos));
					Effekseer::Vec3f normal = SafeNormalize(Effekseer::Vec3f::Cross(tangent, axis));

					if (!parameter.IsRightHand)
					{
						normal = -normal;
					}

					Effekseer::Color normal_ = PackVector3DF(normal);
					Effekseer::Color tangent_ = PackVector3DF(tangent);

					vl.SetPackedNormal(normal_);
					vm.SetPackedNormal(normal_);
					vr.SetPackedNormal(normal_);

					vl.SetPackedTangent(tangent_);
					vm.SetPackedTangent(tangent_);
					vr.SetPackedTangent(tangent_);
				}

				if (isFirst_)
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;
					vs_ += 2;
				}
				else if (isLast_)
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;
					vs_ += 6;
				}
				else
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;

					vs_[6] = vl;
					vs_[7] = vm;
					vs_[10] = vm;
					vs_[11] = vr;

					vs_ += 8;
				}
			}
		}

		// calculate UV
		AssignUVs<VERTEX, 0>(parameter, verteies);

		if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
		{
			AssignUVs<VERTEX, 1>(parameter, verteies);
		}

		AssignUVs<VERTEX, 2>(parameter, verteies);
		AssignUVs<VERTEX, 3>(parameter, verteies);
		AssignUVs<VERTEX, 4>(parameter, verteies);
		AssignUVs<VERTEX, 5>(parameter, verteies);
		AssignUVs<VERTEX, 6>(parameter, verteies);

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 8; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData1.data(), sizeof(float) * customData1Count_);
						custom += 1;
					}
				}
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 8; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData2.data(), sizeof(float) * customData2Count_);
						custom += 1;
					}
				}
			}
		}
	}

public:
	TrackRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_ribbonCount(0)
		, m_ringBufferOffset(0)
		, m_ringBufferData(NULL)
	{
	}

	virtual ~TrackRendererBase()
	{
	}

protected:
	void Rendering_(const efkTrackNodeParam& parameter,
					const efkTrackInstanceParam& instanceParameter,
					void* userData,
					const ::Effekseer::Mat44f& camera)
	{
		const auto& state = m_renderer->GetStandardRenderer()->GetState();

		bool isAdvanced = state.IsAdvanced();

		if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			Rendering_Internal<AdvancedVertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (isAdvanced)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			Rendering_Internal<VertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkTrackNodeParam& parameter,
							const efkTrackInstanceParam& instanceParameter,
							void* userData,
							const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == NULL)
			return;
		if (instanceParameter.InstanceCount < 2)
			return;

		const efkTrackInstanceParam& param = instanceParameter;

		bool isFirst = param.InstanceIndex == 0;
		bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

		if (isFirst)
		{
			instances.reserve(param.InstanceCount);
			instances.resize(0);
			innstancesNodeParam = parameter;
		}

		instances.push_back(param);

		if (isLast)
		{
			RenderSplines<VERTEX, FLIP_RGB>(camera);
		}
	}

public:
	void Rendering(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData) override
	{
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void BeginRenderingGroup(const efkTrackNodeParam& param, int32_t count, void* userData) override
	{
		m_ribbonCount = 0;
		int32_t vertexCount = ((count - 1) * param.SplineDivision) * 8;
		if (vertexCount <= 0)
			return;

		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;
		state.TextureFilter4 = param.BasicParameterPtr->TextureFilter4;
		state.TextureWrap4 = param.BasicParameterPtr->TextureWrap4;
		state.TextureFilter5 = param.BasicParameterPtr->TextureFilter5;
		state.TextureWrap5 = param.BasicParameterPtr->TextureWrap5;
		state.TextureFilter6 = param.BasicParameterPtr->TextureFilter6;
		state.TextureWrap6 = param.BasicParameterPtr->TextureWrap6;
		state.TextureFilter7 = param.BasicParameterPtr->TextureFilter7;
		state.TextureWrap7 = param.BasicParameterPtr->TextureWrap7;

		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index
											   ,
											   param.BasicParameterPtr->Texture3Index,
											   param.BasicParameterPtr->Texture4Index,
											   param.BasicParameterPtr->Texture5Index,
											   param.BasicParameterPtr->Texture6Index,
											   param.BasicParameterPtr->Texture7Index
		);
		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		m_renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

		m_renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, stride_, (void*&)m_ringBufferData);
		vertexCount_ = vertexCount;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RIBBON_RENDERER_H__

#ifndef __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__
#define __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerNative.h"
#include <assert.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBufferBase
{
protected:
	bool m_isDynamic;
	int m_size;
	int m_offset;
	uint8_t* m_resource;
	bool m_isLock;

public:
	VertexBufferBase(int size, bool isDynamic);
	virtual ~VertexBufferBase();

	virtual void Lock() = 0;
	virtual bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	/**
		@brief	try lock as a ring buffer. if failed, it does't lock it.
	*/
	virtual bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	virtual void Unlock() = 0;
	virtual void Push(const void* buffer, int size);
	virtual int GetMaxSize() const;
	virtual int GetSize() const;
	virtual void* GetBufferDirect(int size);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__
