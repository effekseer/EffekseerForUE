#include "Effekseer.CurveLoader.h"
#include "Utils/Effekseer.BinaryReader.h"

#include <cmath>
#include <limits>

namespace Effekseer
{

namespace
{

constexpr int32_t CurveControlPointCountMax = 65536;
constexpr int32_t CurveOrderMax = 16;
constexpr int32_t CurveKnotCountMax = CurveControlPointCountMax + CurveOrderMax;
constexpr size_t CurveFileSizeMax = sizeof(int32_t) * 7 +
	static_cast<size_t>(CurveControlPointCountMax) * sizeof(dVector4) +
	static_cast<size_t>(CurveKnotCountMax) * sizeof(double);

} // namespace

bool Curve::Load(const void* data, int32_t size)
{
	auto invalidate = [this]()
	{
		controlPointCount_ = -1;
		controlPoints_.clear();
		knotCount_ = 0;
		knotValues_.clear();
		order_ = 0;
		step_ = 0;
		type_ = 0;
		dimension_ = 0;
		length_ = 0.0f;
		return false;
	};

	if (data == nullptr || size <= 0 || static_cast<size_t>(size) > CurveFileSizeMax)
	{
		return invalidate();
	}

	BinaryReader<true> reader(static_cast<const uint8_t*>(data), static_cast<size_t>(size));
	int32_t converterVersion = 0;
	if (!reader.Read(converterVersion) || converterVersion != Version ||
		!reader.Read(controlPointCount_, 1, CurveControlPointCountMax) ||
		!reader.Read(controlPoints_, controlPointCount_) ||
		!reader.Read(knotCount_, 1, CurveKnotCountMax) ||
		!reader.Read(knotValues_, knotCount_) ||
		!reader.Read(order_, 0, CurveOrderMax) ||
		!reader.Read(step_, 1, std::numeric_limits<int32_t>::max()) ||
		!reader.Read(type_, 0, 2) ||
		!reader.Read(dimension_, 2, 3) ||
		reader.GetStatus() != BinaryReaderStatus::Complete)
	{
		return invalidate();
	}

	if (order_ > controlPointCount_ || knotCount_ != controlPointCount_ + order_)
	{
		return invalidate();
	}

	const auto floatMax = static_cast<double>(std::numeric_limits<float>::max());
	for (const auto& point : controlPoints_)
	{
		if (!std::isfinite(point.X) || !std::isfinite(point.Y) || !std::isfinite(point.Z) || !std::isfinite(point.W) ||
			std::abs(point.X) > floatMax || std::abs(point.Y) > floatMax || std::abs(point.Z) > floatMax)
		{
			return invalidate();
		}
	}

	for (size_t i = 0; i < knotValues_.size(); i++)
	{
		if (!std::isfinite(knotValues_[i]) || std::abs(knotValues_[i]) > floatMax ||
			(i > 0 && knotValues_[i] < knotValues_[i - 1]))
		{
			return invalidate();
		}
	}

	length_ = 0.0f;
	for (int32_t i = 1; i < controlPointCount_; i++)
	{
		const auto& p0 = controlPoints_[i - 1];
		const auto& p1 = controlPoints_[i];
		const auto dx = p1.X - p0.X;
		const auto dy = p1.Y - p0.Y;
		const auto dz = p1.Z - p0.Z;
		const auto length = std::hypot(std::hypot(dx, dy), dz);
		if (!std::isfinite(length) || length > floatMax - length_)
		{
			return invalidate();
		}
		length_ += static_cast<float>(length);
	}

	return true;
}

CurveLoader::CurveLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	if (fileInterface != nullptr)
	{
		fileInterface_ = fileInterface;
	}
	else
	{
		fileInterface_ = MakeRefPtr<DefaultFileInterface>();
	}
}

CurveRef CurveLoader::Load(const char16_t* path)
{
	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
	{
		return nullptr;
	}

	size_t size = reader->GetLength();
	if (size == 0 || size > CurveFileSizeMax || size > static_cast<size_t>(std::numeric_limits<int32_t>::max()))
	{
		return nullptr;
	}

	std::vector<uint8_t> data;
	data.resize(size);

	if (reader->Read(data.data(), size) != size)
	{
		return nullptr;
	}

	return Load(data.data(), static_cast<int32_t>(size));
}

CurveRef CurveLoader::Load(const void* data, int32_t size)
{
	auto curve = Effekseer::MakeRefPtr<Effekseer::Curve>(data, size);
	return curve->GetIsValid() ? curve : nullptr;
}

void CurveLoader::Unload(CurveRef data)
{
}

} // namespace Effekseer
