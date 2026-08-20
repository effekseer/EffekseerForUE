#include "Effekseer.MaterialFile.h"

#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

const int32_t MaterialFile::LatestSupportVersion;
const int32_t MaterialFile::OldestSupportVersion;

bool MaterialFile::Load(const uint8_t* data, int32_t size)
{
	if (data == nullptr || size < 0)
		return false;
	constexpr int32_t CountMax = 1024;
	BinaryReader<true> reader(data, static_cast<size_t>(size));
	std::array<char, 4> prefix{};
	int version = 0;
	uint64_t guid = 0;
	if (!reader.Read(prefix.data(), 4) || memcmp(prefix.data(), "EFKM", 4) != 0 ||
		!reader.Read(version, OldestSupportVersion, LatestSupportVersion) || !reader.Read(guid))
		return false;

	auto readString = [](BinaryReader<true>& source, std::string& value) -> bool
	{
		int32_t length = 0;
		std::vector<char> bytes;
		if (!source.Read(length, 1, 1024 * 1024) || !source.Read(bytes, length) || bytes.back() != '\0')
			return false;
		value.assign(bytes.data(), static_cast<size_t>(length - 1));
		return true;
	};

	textures_.clear();
	uniforms_.clear();
	Gradients.clear();
	FixedGradients.clear();
	RequiredMethods.clear();
	genericCode_.clear();
	while (reader.GetRemainingSize() > 0)
	{
		std::array<char, 4> chunk{};
		int32_t chunkSize = 0;
		if (!reader.Read(chunk.data(), 4) || !reader.Read(chunkSize, 0, 256 * 1024 * 1024) || !reader.CanRead(chunkSize))
			return false;
		BinaryReader<true> body(reader.GetCurrentData(), static_cast<size_t>(chunkSize));
		if (!reader.Skip(static_cast<size_t>(chunkSize)))
			return false;

		if (memcmp(chunk.data(), "PRM_", 4) == 0)
		{
			int hasNormal = 0, hasRefraction = 0;
			if (!body.Read(shadingModel_) || !body.Read(hasNormal) || !body.Read(hasRefraction) ||
				!body.Read(customData1Count_, 0, 4) || !body.Read(customData2Count_, 0, 4))
				return false;
			hasRefraction_ = hasRefraction > 0;
			if (version >= MaterialVersion17Alpha4)
			{
				int32_t count = 0;
				if (!body.Read(count, 0, CountMax))
					return false;
				for (int32_t i = 0; i < count; i++)
				{
					RequiredPredefinedMethodType type{};
					if (!body.Read(type))
						return false;
					RequiredMethods.push_back(type);
				}
			}

			int32_t textureCount = 0;
			if (!body.Read(textureCount, 0, CountMax))
				return false;
			for (int32_t i = 0; i < textureCount; i++)
			{
				std::string name, ignoredPath;
				if (!readString(body, name) || (version >= 3 && !readString(body, name)) || !readString(body, ignoredPath))
					return false;
				int index = 0, priority = 0, param = 0, colorType = 0, sampler = 0;
				if (!body.Read(index) || !body.Read(priority) || !body.Read(param) || !body.Read(colorType) || !body.Read(sampler))
					return false;
				Texture texture;
				texture.Name = name;
				texture.Index = index;
				texture.Wrap = static_cast<TextureWrapType>(sampler);
				texture.ColorType = static_cast<TextureColorType>(colorType);
				textures_.push_back(std::move(texture));
			}

			int32_t uniformCount = 0;
			if (!body.Read(uniformCount, 0, CountMax))
				return false;
			for (int32_t i = 0; i < uniformCount; i++)
			{
				std::string name;
				if (!readString(body, name) || (version >= 3 && !readString(body, name)))
					return false;
				int offsetValue = 0, priority = 0, type = 0;
				std::array<int, 4> defaults{};
				if (!body.Read(offsetValue) || !body.Read(priority) || !body.Read(type) || !body.Read(defaults.data(), 4))
					return false;
				Uniform uniform;
				uniform.Name = name;
				uniform.Index = type;
				uniforms_.push_back(std::move(uniform));
			}

			if (version >= MaterialVersion17Alpha4)
			{
				auto loadGradients = [&](std::vector<GradientParameter>& destination) -> bool
				{
					int32_t count = 0; if (!body.Read(count, 0, CountMax)) return false;
					for (int32_t i = 0; i < count; i++) { GradientParameter gradient; std::string ignored;
						if (!readString(body, ignored) || !readString(body, gradient.Name)) return false;
						int offsetValue = 0, priority = 0; if (!body.Read(offsetValue) || !body.Read(priority) || !LoadGradient(gradient.Data, body, 0)) return false;
						destination.push_back(std::move(gradient)); }
					return true; };
				if (!loadGradients(Gradients) || !loadGradients(FixedGradients))
					return false;
			}
			if (body.GetStatus() != BinaryReaderStatus::Complete)
				return false;
		}
		else if (memcmp(chunk.data(), "GENE", 4) == 0)
		{
			if (!readString(body, genericCode_) || body.GetStatus() != BinaryReaderStatus::Complete)
				return false;
		}
	}
	guid_ = guid;
	return true;
}

ShadingModelType MaterialFile::GetShadingModel() const
{
	return shadingModel_;
}

void MaterialFile::SetShadingModel(ShadingModelType shadingModel)
{
	shadingModel_ = shadingModel;
}

bool MaterialFile::GetIsSimpleVertex() const
{
	return isSimpleVertex_;
}

void MaterialFile::SetIsSimpleVertex(bool isSimpleVertex)
{
	isSimpleVertex_ = isSimpleVertex;
}

bool MaterialFile::GetHasRefraction() const
{
	return hasRefraction_;
}

void MaterialFile::SetHasRefraction(bool hasRefraction)
{
	hasRefraction_ = hasRefraction;
}

const char* MaterialFile::GetGenericCode() const
{
	return genericCode_.c_str();
}

void MaterialFile::SetGenericCode(const char* code)
{
	genericCode_ = code;
}

uint64_t MaterialFile::GetGUID() const
{
	return guid_;
}

void MaterialFile::SetGUID(uint64_t guid)
{
	guid_ = guid;
}

TextureColorType MaterialFile::GetTextureColorType(int32_t index) const
{
	return textures_.at(index).ColorType;
}

TextureWrapType MaterialFile::GetTextureWrap(int32_t index) const
{
	return textures_.at(index).Wrap;
}

void MaterialFile::SetTextureWrap(int32_t index, TextureWrapType value)
{
	textures_.at(index).Wrap = value;
}

int32_t MaterialFile::GetTextureIndex(int32_t index) const
{
	return textures_.at(index).Index;
}

void MaterialFile::SetTextureIndex(int32_t index, int32_t value)
{
	textures_.at(index).Index = value;
}

const char* MaterialFile::GetTextureName(int32_t index) const
{
	return textures_.at(index).Name.c_str();
}

void MaterialFile::SetTextureName(int32_t index, const char* name)
{
	textures_.at(index).Name = name;
}

int32_t MaterialFile::GetTextureCount() const
{
	return static_cast<int32_t>(textures_.size());
}

void MaterialFile::SetTextureCount(int32_t count)
{
	textures_.resize(count);
}

int32_t MaterialFile::GetUniformIndex(int32_t index) const
{
	return uniforms_.at(index).Index;
}

void MaterialFile::SetUniformIndex(int32_t index, int32_t value)
{
	uniforms_.at(index).Index = value;
}

const char* MaterialFile::GetUniformName(int32_t index) const
{
	return uniforms_.at(index).Name.c_str();
}

void MaterialFile::SetUniformName(int32_t index, const char* name)
{
	uniforms_.at(index).Name = name;
}

int32_t MaterialFile::GetUniformCount() const
{
	return static_cast<int32_t>(uniforms_.size());
}

void MaterialFile::SetUniformCount(int32_t count)
{
	uniforms_.resize(count);
}

int32_t MaterialFile::GetCustomData1Count() const
{
	if (customData1Count_ == 0)
		return 0;

	// because opengl doesn't support swizzle with float
	return std::max(customDataMinCount_, customData1Count_);
}

void MaterialFile::SetCustomData1Count(int32_t count)
{
	customData1Count_ = count;
}

int32_t MaterialFile::GetCustomData2Count() const
{
	if (customData2Count_ == 0)
		return 0;

	// because opengl doesn't support swizzle with float
	return std::max(customDataMinCount_, customData2Count_);
}

void MaterialFile::SetCustomData2Count(int32_t count)
{
	customData2Count_ = count;
}

} // namespace Effekseer
