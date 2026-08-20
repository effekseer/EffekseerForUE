#include "Effekseer.EfkEfcFactory.h"
#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

EfkEfcFile::EfkEfcFile(const void* data, int32_t size)
	: data_(data)
	, size_(size)
{
	if (data == nullptr || size < 0)
		return;
	BinaryReader<true> binaryReader(reinterpret_cast<const uint8_t*>(data_), static_cast<size_t>(size_));

	// EFKP
	int head = 0;
	if (!binaryReader.Read(head) || memcmp(&head, "EFKE", 4) != 0)
		return;

	if (!binaryReader.Read(version_))
		return;

	isValid_ = true;
}

EfkEfcFile::Chunk EfkEfcFile::ReadChunk(const char* forcc) const
{
	if (!IsValid())
		return {};

	BinaryReader<true> binaryReader(reinterpret_cast<const uint8_t*>(data_), static_cast<size_t>(size_));

	// Skip forcc and version
	binaryReader.AddOffset(8);

	// read chunk
	while (binaryReader.GetOffset() < size_)
	{
		int chunkForcc = 0;
		if (!binaryReader.Read(chunkForcc))
			return {};
		int chunkSize = 0;
		if (!binaryReader.Read(chunkSize) || chunkSize < 0 || !binaryReader.CanRead(static_cast<size_t>(chunkSize)))
			return {};

		if (memcmp(&chunkForcc, forcc, 4) == 0)
		{
			Chunk chunk;
			chunk.data = reinterpret_cast<const uint8_t*>(data_) + binaryReader.GetOffset();
			chunk.size = chunkSize;
			return chunk;
		}

		if (!binaryReader.Skip(static_cast<size_t>(chunkSize)))
			return {};
	}

	return {};
}

EfkEfcFile::Chunk EfkEfcFile::ReadInfo() const
{
	return ReadChunk("INFO");
}

EfkEfcFile::Chunk EfkEfcFile::ReadEditerData() const
{
	return ReadChunk("EDIT");
}

EfkEfcFile::Chunk EfkEfcFile::ReadRuntimeData() const
{
	return ReadChunk("BIN_");
}

bool EfkEfcFactory::OnLoading(Effect* effect, const void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	EfkEfcFile file(data, size);

	if (!file.IsValid())
	{
		return false;
	}

	auto chunk = file.ReadRuntimeData();
	if (chunk.data == nullptr)
	{
		return false;
	}

	return LoadBody(effect, chunk.data, chunk.size, magnification, materialPath);
}

bool EfkEfcFactory::OnCheckIsBinarySupported(const void* data, int32_t size)
{
	EfkEfcFile file(data, size);

	return file.IsValid();
}

bool EfkEfcProperty::Load(const void* data, int32_t size)
{
	EfkEfcFile file(data, size);

	if (!file.IsValid())
	{
		return false;
	}

	auto chunk = file.ReadInfo();
	if (chunk.data == nullptr)
	{
		return false;
	}

	BinaryReader<true> binaryReader(static_cast<const uint8_t*>(chunk.data), static_cast<size_t>(chunk.size));

	std::vector<char16_t> strBuf;
	auto loadStr = [&binaryReader, &strBuf](std::u16string& dst) -> bool
	{
		int32_t length = 0;
		if (!binaryReader.Read(length) || length <= 0 || length > 32768 ||
			!binaryReader.CanReadElements(length, sizeof(char16_t)))
			return false;
		strBuf.resize(static_cast<size_t>(length));
		if (!binaryReader.Read(strBuf.data(), length) || strBuf.back() != u'\0')
			return false;
		dst.assign(strBuf.data(), static_cast<size_t>(length - 1));
		return true;
	};

	constexpr int32_t elementCountMax = 1024;
	auto loadStrArray = [&binaryReader, &loadStr, elementCountMax](std::vector<std::u16string>& dst) -> bool
	{
		int32_t dataCount = 0;
		if (!binaryReader.Read(dataCount) || dataCount < 0 || dataCount > elementCountMax)
			return false;
		dst.resize(static_cast<size_t>(dataCount));

		for (int32_t i = 0; i < dataCount; i++)
		{
			if (!loadStr(dst.at(static_cast<size_t>(i))))
				return false;
		}
		return true;
	};

	int32_t infoVersion = 0;
	if (!binaryReader.Read(infoVersion))
		return false;

	if (infoVersion < 1500)
	{
		// old formats have no version field and start with the first element count
		infoVersion = 0;
		if (!binaryReader.SetOffset(0))
			return false;
	}

	if (infoVersion < 1700)
	{
		if (!loadStrArray(colorImages_) || !loadStrArray(normalImages_) || !loadStrArray(distortionImages_) ||
			!loadStrArray(models_) || !loadStrArray(sounds_))
			return false;

		if (infoVersion >= 1500 && !loadStrArray(materials_))
			return false;

		if (infoVersion >= 1610 && !loadStrArray(curves_))
			return false;
	}
	else
	{
		// 1700 and later store a flat dependency list of {fileType, flag, path}
		constexpr int32_t fileTypeTexture = 1;
		constexpr int32_t fileTypeSound = 2;
		constexpr int32_t fileTypeModel = 3;
		constexpr int32_t fileTypeMaterial = 4;
		constexpr int32_t fileTypeCurve = 5;
		constexpr int32_t textureFlagSRGB = 1 << 0;
		constexpr int32_t textureFlagLinear = 1 << 1;

		int32_t dependencyCount = 0;
		if (!binaryReader.Read(dependencyCount) || dependencyCount < 0 || dependencyCount > elementCountMax)
			return false;

		for (int32_t i = 0; i < dependencyCount; i++)
		{
			int32_t fileType = 0;
			int32_t flag = 0;
			std::u16string path;
			if (!binaryReader.Read(fileType) || !binaryReader.Read(flag) || !loadStr(path))
				return false;

			switch (fileType)
			{
			case fileTypeTexture:
				// this format keeps a single linear flag, so normal and distortion usages are merged
				if ((flag & textureFlagSRGB) != 0)
					colorImages_.emplace_back(path);
				if ((flag & textureFlagLinear) != 0)
					normalImages_.emplace_back(std::move(path));
				break;
			case fileTypeSound:
				sounds_.emplace_back(std::move(path));
				break;
			case fileTypeModel:
				models_.emplace_back(std::move(path));
				break;
			case fileTypeMaterial:
				materials_.emplace_back(std::move(path));
				break;
			case fileTypeCurve:
				curves_.emplace_back(std::move(path));
				break;
			default:
				break;
			}
		}
	}

	return binaryReader.GetStatus() != BinaryReaderStatus::Failed;
}

const std::vector<std::u16string>& EfkEfcProperty::GetColorImages() const
{
	return colorImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetNormalImages() const
{
	return normalImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetDistortionImages() const
{
	return distortionImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetSounds() const
{
	return sounds_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetModels() const
{
	return models_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetMaterials() const
{
	return materials_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetCurves() const
{
	return curves_;
}

} // namespace Effekseer
