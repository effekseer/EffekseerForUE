#include "Effekseer.CompiledMaterial.h"
#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

class CompiledMaterialBinaryInternal : public CompiledMaterialBinary, public ReferenceObject
{
private:
	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

	std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

public:
	CompiledMaterialBinaryInternal()
	{
	}

	virtual ~CompiledMaterialBinaryInternal()
	{
	}

	void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		vertexShaders_.at(static_cast<size_t>(type)) = data;
	}

	void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data)
	{
		pixelShaders_.at(static_cast<size_t>(type)) = data;
	}

	const uint8_t* GetVertexShaderData(MaterialShaderType type) const override
	{
		return vertexShaders_.at(static_cast<size_t>(type)).data();
	}

	int32_t GetVertexShaderSize(MaterialShaderType type) const override
	{
		return static_cast<int32_t>(vertexShaders_.at(static_cast<size_t>(type)).size());
	}

	const uint8_t* GetPixelShaderData(MaterialShaderType type) const override
	{
		return pixelShaders_.at(static_cast<size_t>(type)).data();
	}

	int32_t GetPixelShaderSize(MaterialShaderType type) const override
	{
		return static_cast<int32_t>(pixelShaders_.at(static_cast<int>(type)).size());
	}

	int AddRef() override
	{
		return ReferenceObject::AddRef();
	}

	int Release() override
	{
		return ReferenceObject::Release();
	}

	int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
};

const std::vector<uint8_t>& CompiledMaterial::GetOriginalData() const
{
	return originalData_;
}

bool CompiledMaterial::Load(const uint8_t* data, int32_t size)
{
	if (data == nullptr || size < 0)
		return false;

	constexpr int32_t PlatformCountMax = 256;
	constexpr int32_t BinarySizeMax = 256 * 1024 * 1024;
	BinaryReader<true> reader(data, static_cast<size_t>(size));
	std::array<char, 4> prefix{};
	int version = 0;
	if (!reader.Read(prefix.data(), static_cast<int32_t>(prefix.size())) ||
		memcmp(prefix.data(), "eMCB", prefix.size()) != 0 || !reader.Read(version))
		return false;

	// bacause of camera position node, structure of uniform is changed, etc
	if (version < OldestSupportVersion)
	{
		return false;
	}

	// Too new
	if (version > LatestSupportVersion)
	{
		return false;
	}

	uint64_t guid = 0;
	if (!reader.Read(guid))
		return false;

	// info
	int32_t platformCount = 0;
	if (!reader.Read(platformCount, 0, PlatformCountMax) ||
		!reader.Skip(sizeof(uint32_t) * static_cast<size_t>(platformCount)))
		return false;

	// data
	int32_t originalDataSize = 0;
	std::vector<uint8_t> originalData;
	if (!reader.Read(originalDataSize, 0, BinarySizeMax) || !reader.Read(originalData, originalDataSize))
		return false;

	decltype(platforms) loadedPlatforms;
	while (reader.GetRemainingSize() > 0)
	{
		int chunk = 0;
		int chunk_size = 0;
		if (!reader.Read(chunk) || !reader.Read(chunk_size, 0, BinarySizeMax) ||
			!reader.CanRead(static_cast<size_t>(chunk_size)))
			return false;
		BinaryReader<true> chunkReader(reader.GetCurrentData(), static_cast<size_t>(chunk_size));
		if (!reader.Skip(static_cast<size_t>(chunk_size)))
			return false;

		auto loadFunc = [BinarySizeMax](BinaryReader<true>& source, std::vector<uint8_t>& buffer)
		{
			int32_t binarySize = 0;
			return source.Read(binarySize, 0, BinarySizeMax) && source.Read(buffer, binarySize);
		};

		std::vector<uint8_t> standardVS;
		std::vector<uint8_t> standardPS;
		std::vector<uint8_t> modelVS;
		std::vector<uint8_t> modelPS;
		std::vector<uint8_t> standardRefractionVS;
		std::vector<uint8_t> standardRefractionPS;
		std::vector<uint8_t> modelRefractionVS;
		std::vector<uint8_t> modelRefractionPS;

		if (!loadFunc(chunkReader, standardVS) || !loadFunc(chunkReader, standardPS) ||
			!loadFunc(chunkReader, modelVS) || !loadFunc(chunkReader, modelPS) ||
			!loadFunc(chunkReader, standardRefractionVS) || !loadFunc(chunkReader, standardRefractionPS) ||
			!loadFunc(chunkReader, modelRefractionVS) || !loadFunc(chunkReader, modelRefractionPS) ||
			chunkReader.GetStatus() != BinaryReaderStatus::Complete)
			return false;

		auto binary = new CompiledMaterialBinaryInternal();

		binary->SetVertexShaderData(MaterialShaderType::Standard, standardVS);
		binary->SetPixelShaderData(MaterialShaderType::Standard, standardPS);
		binary->SetVertexShaderData(MaterialShaderType::Model, modelVS);
		binary->SetPixelShaderData(MaterialShaderType::Model, modelPS);
		binary->SetVertexShaderData(MaterialShaderType::Refraction, standardRefractionVS);
		binary->SetPixelShaderData(MaterialShaderType::Refraction, standardRefractionPS);
		binary->SetVertexShaderData(MaterialShaderType::RefractionModel, modelRefractionVS);
		binary->SetPixelShaderData(MaterialShaderType::RefractionModel, modelRefractionPS);

		loadedPlatforms[static_cast<CompiledMaterialPlatformType>(chunk)] = CreateUniqueReference(static_cast<CompiledMaterialBinary*>(binary));
	}
	originalData_ = std::move(originalData);
	platforms = std::move(loadedPlatforms);
	return true;
}

void CompiledMaterial::Save(std::vector<uint8_t>& dst, uint64_t guid, std::vector<uint8_t>& originalData)
{
	dst.reserve(1024 * 64);
	size_t offset = 0;

	struct Header
	{
		char header[4];
		int version = Version;
		uint64_t guid = 0;
	};

	Header h;
	h.header[0] = 'e';
	h.header[1] = 'M';
	h.header[2] = 'C';
	h.header[3] = 'B';
	h.guid = guid;

	dst.resize(sizeof(Header));
	memcpy(dst.data() + offset, &h, sizeof(Header));
	offset = dst.size();

	// info
	uint32_t platformCount = static_cast<uint32_t>(platforms.size());
	dst.resize(dst.size() + sizeof(uint32_t));
	memcpy(dst.data() + offset, &platformCount, sizeof(uint32_t));
	offset = dst.size();

	for (auto& kv : platforms)
	{
		auto platform = kv.first;
		dst.resize(dst.size() + sizeof(uint32_t));
		memcpy(dst.data() + offset, &platform, sizeof(uint32_t));
		offset = dst.size();
	}

	// data
	uint32_t originalDataSize = static_cast<uint32_t>(originalData.size());
	dst.resize(dst.size() + sizeof(uint32_t));
	memcpy(dst.data() + offset, &originalDataSize, sizeof(uint32_t));
	offset = dst.size();

	dst.resize(dst.size() + originalData.size());
	memcpy(dst.data() + offset, originalData.data(), originalData.size());
	offset = dst.size();

	// shaders
	for (auto& kv : platforms)
	{
		int32_t bodySize = 0;

		bodySize += sizeof(int) + kv.second->GetVertexShaderSize(MaterialShaderType::Standard);
		bodySize += sizeof(int) + kv.second->GetPixelShaderSize(MaterialShaderType::Standard);
		bodySize += sizeof(int) + kv.second->GetVertexShaderSize(MaterialShaderType::Model);
		bodySize += sizeof(int) + kv.second->GetPixelShaderSize(MaterialShaderType::Model);
		bodySize += sizeof(int) + kv.second->GetVertexShaderSize(MaterialShaderType::Refraction);
		bodySize += sizeof(int) + kv.second->GetPixelShaderSize(MaterialShaderType::Refraction);
		bodySize += sizeof(int) + kv.second->GetVertexShaderSize(MaterialShaderType::RefractionModel);
		bodySize += sizeof(int) + kv.second->GetPixelShaderSize(MaterialShaderType::RefractionModel);

		dst.resize(dst.size() + sizeof(int));
		memcpy(dst.data() + offset, &(kv.first), sizeof(int));
		offset = dst.size();

		dst.resize(dst.size() + sizeof(int));
		memcpy(dst.data() + offset, &(bodySize), sizeof(int));
		offset = dst.size();

		std::array<const uint8_t*, 8> bodies = {
			kv.second->GetVertexShaderData(MaterialShaderType::Standard),
			kv.second->GetPixelShaderData(MaterialShaderType::Standard),
			kv.second->GetVertexShaderData(MaterialShaderType::Model),
			kv.second->GetPixelShaderData(MaterialShaderType::Model),
			kv.second->GetVertexShaderData(MaterialShaderType::Refraction),
			kv.second->GetPixelShaderData(MaterialShaderType::Refraction),
			kv.second->GetVertexShaderData(MaterialShaderType::RefractionModel),
			kv.second->GetPixelShaderData(MaterialShaderType::RefractionModel),
		};

		std::array<int32_t, 8> bodySizes = {
			kv.second->GetVertexShaderSize(MaterialShaderType::Standard),
			kv.second->GetPixelShaderSize(MaterialShaderType::Standard),
			kv.second->GetVertexShaderSize(MaterialShaderType::Model),
			kv.second->GetPixelShaderSize(MaterialShaderType::Model),
			kv.second->GetVertexShaderSize(MaterialShaderType::Refraction),
			kv.second->GetPixelShaderSize(MaterialShaderType::Refraction),
			kv.second->GetVertexShaderSize(MaterialShaderType::RefractionModel),
			kv.second->GetPixelShaderSize(MaterialShaderType::RefractionModel),
		};

		for (size_t i = 0; i < 8; i++)
		{
			int32_t bodySize2 = bodySizes[i];

			dst.resize(dst.size() + sizeof(int));
			memcpy(dst.data() + offset, &(bodySize2), sizeof(int));
			offset = dst.size();

			dst.resize(dst.size() + bodySize2);
			memcpy(dst.data() + offset, bodies[i], bodySize2);
			offset = dst.size();
		}
	}
}

bool CompiledMaterial::GetHasValue(CompiledMaterialPlatformType type) const
{
	auto it = platforms.find(type);
	if (it == platforms.end())
		return false;

	// TODO improve it
	return it->second->GetVertexShaderSize(MaterialShaderType::Standard) > 0;
}

CompiledMaterialBinary* CompiledMaterial::GetBinary(CompiledMaterialPlatformType type) const
{

	auto it = platforms.find(type);
	if (it == platforms.end())
		return nullptr;

	return it->second.get();
}

void CompiledMaterial::UpdateData(const std::vector<uint8_t>& standardVS,
								  const std::vector<uint8_t>& standardPS,
								  const std::vector<uint8_t>& modelVS,
								  const std::vector<uint8_t>& modelPS,
								  const std::vector<uint8_t>& standardRefractionVS,
								  const std::vector<uint8_t>& standardRefractionPS,
								  const std::vector<uint8_t>& modelRefractionVS,
								  const std::vector<uint8_t>& modelRefractionPS,
								  CompiledMaterialPlatformType type)
{
	auto binary = new CompiledMaterialBinaryInternal();

	binary->SetVertexShaderData(MaterialShaderType::Standard, standardVS);
	binary->SetPixelShaderData(MaterialShaderType::Standard, standardPS);
	binary->SetVertexShaderData(MaterialShaderType::Model, modelVS);
	binary->SetPixelShaderData(MaterialShaderType::Model, modelPS);
	binary->SetVertexShaderData(MaterialShaderType::Refraction, standardRefractionVS);
	binary->SetPixelShaderData(MaterialShaderType::Refraction, standardRefractionPS);
	binary->SetVertexShaderData(MaterialShaderType::RefractionModel, modelRefractionVS);
	binary->SetPixelShaderData(MaterialShaderType::RefractionModel, modelRefractionPS);

	platforms[type] = CreateUniqueReference(static_cast<CompiledMaterialBinary*>(binary));
}

} // namespace Effekseer
