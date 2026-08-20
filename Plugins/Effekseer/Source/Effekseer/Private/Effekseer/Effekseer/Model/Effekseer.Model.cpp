#include "Effekseer.Model.h"
#include "../Backend/Effekseer.GraphicsDevice.h"

#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

namespace
{

constexpr int32_t ModelFrameCountMax = 65536;

} // namespace

Model::Model(const CustomVector<Vertex>& vertecies, const CustomVector<Face>& faces)
{
	models_.resize(1);
	models_[0].vertexes = vertecies;
	models_[0].faces = faces;
}

Model::Model(const void* data, int32_t size)
{
	auto invalidate = [this]()
	{
		version_ = -1;
		models_.clear();
		models_.resize(1);
	};

	if (data == nullptr || size <= 0)
	{
		invalidate();
		return;
	}

	BinaryReader<true> reader(static_cast<const uint8_t*>(data), static_cast<size_t>(size));
	if (!reader.Read(version_) || version_ < 0 || version_ > LatestVersion)
	{
		invalidate();
		return;
	}

	// Load scale except version 3 (for compatibility).
	if ((version_ == 2 || version_ >= 5) && !reader.Skip(sizeof(int32_t)))
	{
		invalidate();
		return;
	}

	// Kept for compatibility with the file format.
	int32_t modelCount = 0;
	if (!reader.Read(modelCount))
	{
		invalidate();
		return;
	}

	int32_t frameCount = 1;
	if (version_ >= 5 && !reader.Read(frameCount))
	{
		invalidate();
		return;
	}

	// Each frame contains at least a vertex count and a face count. Validate
	// this before allocating to prevent a crafted count from amplifying memory.
	if (frameCount <= 0 || frameCount > ModelFrameCountMax || !reader.CanReadElements(frameCount, sizeof(int32_t) * 2))
	{
		invalidate();
		return;
	}

	models_.resize(frameCount);

	for (int32_t f = 0; f < frameCount; f++)
	{
		int32_t vertexCount = 0;
		if (!reader.Read(vertexCount))
		{
			invalidate();
			return;
		}

		size_t vertexSize = sizeof(Vertex);
		if (version_ < 6)
		{
			vertexSize = sizeof(Vector3D) * 4 + sizeof(Vector2D) + (version_ >= 1 ? sizeof(Color) : 0);
		}

		if (!reader.CanReadElements(vertexCount, vertexSize))
		{
			invalidate();
			return;
		}

		models_[f].vertexes.resize(vertexCount);

		if (version_ >= 6)
		{
			if (!reader.ReadBytes(models_[f].vertexes.data(), sizeof(Vertex) * static_cast<size_t>(vertexCount)))
			{
				invalidate();
				return;
			}
		}
		else
		{
			Vertex* vertexes = models_[f].vertexes.data();
			const Color defaultColor(255, 255, 255, 255);

			for (int32_t i = 0; i < vertexCount; i++)
			{
				if (!reader.Read(vertexes[i].Position) ||
					!reader.Read(vertexes[i].Normal) ||
					!reader.Read(vertexes[i].Binormal) ||
					!reader.Read(vertexes[i].Tangent) ||
					!reader.Read(vertexes[i].UV1))
				{
					invalidate();
					return;
				}

				vertexes[i].UV2 = vertexes[i].UV1;
				if (version_ >= 1)
				{
					if (!reader.Read(vertexes[i].VColor))
					{
						invalidate();
						return;
					}
				}
				else
				{
					vertexes[i].VColor = defaultColor;
				}
			}
		}

		int32_t faceCount = 0;
		if (!reader.Read(faceCount) || !reader.CanReadElements(faceCount, sizeof(Face)))
		{
			invalidate();
			return;
		}

		models_[f].faces.resize(faceCount);
		if (!reader.ReadBytes(models_[f].faces.data(), sizeof(Face) * static_cast<size_t>(faceCount)))
		{
			invalidate();
			return;
		}

		for (const auto& face : models_[f].faces)
		{
			if (face.Indexes[0] < 0 || face.Indexes[0] >= vertexCount ||
				face.Indexes[1] < 0 || face.Indexes[1] >= vertexCount ||
				face.Indexes[2] < 0 || face.Indexes[2] >= vertexCount)
			{
				invalidate();
				return;
			}
		}
	}

}

Model ::~Model()
{
}

const RefPtr<Backend::VertexBuffer>& Model::GetVertexBuffer(int32_t index) const
{
	return models_[index].vertexBuffer;
}

const RefPtr<Backend::IndexBuffer>& Model::GetIndexBuffer(int32_t index) const
{
	return models_[index].indexBuffer;
}

const RefPtr<Backend::IndexBuffer>& Model::GetWireIndexBuffer(int32_t index) const
{
	return models_[index].wireIndexBuffer;
}

const Model::Vertex* Model::GetVertexes(int32_t index) const
{
	return models_[index].vertexes.data();
}

int32_t Model::GetVertexCount(int32_t index) const
{
	return static_cast<int32_t>(models_[index].vertexes.size());
}

const Model::Face* Model::GetFaces(int32_t index) const
{
	return models_[index].faces.data();
}

int32_t Model::GetFaceCount(int32_t index) const
{
	return static_cast<int32_t>(models_[index].faces.size());
}

int32_t Model::GetFrameCount() const
{
	return static_cast<int32_t>(models_.size());
}

bool Model::GetIsValid() const
{
	return version_ >= 0;
}

Model::Emitter Model::GetEmitter(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return GetEmitterFromVertex(g, time, coordinate, magnification);
	}

	int32_t faceInd = static_cast<int32_t>(GetFaceCount(time) * g->GetRand());
	faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p1 = g->GetRand();
	float p2 = g->GetRand();

	// Fit within plane
	if (p1 + p2 > 1.0f)
	{
		p1 = 1.0f - p1;
		p2 = 1.0f - p2;
	}

	float p0 = 1.0f - p1 - p2;

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
	emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
	emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromVertex(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto vertexCount = GetVertexCount(time);
	if (vertexCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t vertexInd = static_cast<int32_t>(GetVertexCount(time) * g->GetRand());
	vertexInd = Clamp(vertexInd, GetVertexCount(time) - 1, 0);
	const Vertex& v = GetVertexes(time)[vertexInd];

	Emitter emitter;
	emitter.Position = v.Position * magnification;
	emitter.Normal = v.Normal;
	emitter.Binormal = v.Binormal;
	emitter.Tangent = v.Tangent;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromVertex(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto vertexCount = GetVertexCount(time);
	if (vertexCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t vertexInd = index % GetVertexCount(time);
	const Vertex& v = GetVertexes(time)[vertexInd];

	Emitter emitter;
	emitter.Position = v.Position * magnification;
	emitter.Normal = v.Normal;
	emitter.Binormal = v.Binormal;
	emitter.Tangent = v.Tangent;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromFace(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t faceInd = static_cast<int32_t>(GetFaceCount(time) * g->GetRand());
	faceInd = Clamp(faceInd, GetFaceCount(time) - 1, 0);
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p0 = 1.0f / 3.0f;
	float p1 = 1.0f / 3.0f;
	float p2 = 1.0f / 3.0f;

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
	emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
	emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

Model::Emitter Model::GetEmitterFromFace(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification)
{
	time = time % GetFrameCount();

	const auto faceCount = GetFaceCount(time);
	if (faceCount == 0)
	{
		return Model::Emitter{};
	}

	int32_t faceInd = index % faceCount;
	const Face& face = GetFaces(time)[faceInd];
	const Vertex& v0 = GetVertexes(time)[face.Indexes[0]];
	const Vertex& v1 = GetVertexes(time)[face.Indexes[1]];
	const Vertex& v2 = GetVertexes(time)[face.Indexes[2]];

	float p0 = 1.0f / 3.0f;
	float p1 = 1.0f / 3.0f;
	float p2 = 1.0f / 3.0f;

	Emitter emitter;
	emitter.Position = (v0.Position * p0 + v1.Position * p1 + v2.Position * p2) * magnification;
	emitter.Normal = v0.Normal * p0 + v1.Normal * p1 + v2.Normal * p2;
	emitter.Binormal = v0.Binormal * p0 + v1.Binormal * p1 + v2.Binormal * p2;
	emitter.Tangent = v0.Tangent * p0 + v1.Tangent * p1 + v2.Tangent * p2;

	if (coordinate == CoordinateSystem::LH)
	{
		emitter.Position.Z = -emitter.Position.Z;
		emitter.Normal.Z = -emitter.Normal.Z;
		emitter.Binormal.Z = -emitter.Binormal.Z;
		emitter.Tangent.Z = -emitter.Tangent.Z;
	}

	return emitter;
}

bool Model::StoreBufferToGPU(Backend::GraphicsDevice* graphicsDevice, bool flipVertexColor)
{
	if (isBufferStoredOnGPU_)
	{
		return false;
	}

	if (graphicsDevice == nullptr)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		if (flipVertexColor)
		{
			auto vdata = models_[f].vertexes;

			for (auto& v : vdata)
			{
				std::swap(v.VColor.R, v.VColor.B);
			}

			models_[f].vertexBuffer = graphicsDevice->CreateVertexBuffer(sizeof(Effekseer::Model::Vertex) * GetVertexCount(f), vdata.data(), false);
			if (models_[f].vertexBuffer == nullptr)
			{
				return false;
			}
		}
		else
		{
			models_[f].vertexBuffer = graphicsDevice->CreateVertexBuffer(sizeof(Effekseer::Model::Vertex) * GetVertexCount(f), models_[f].vertexes.data(), false);
			if (models_[f].vertexBuffer == nullptr)
			{
				return false;
			}
		}

		{
			models_[f].indexBuffer = graphicsDevice->CreateIndexBuffer(3 * GetFaceCount(f), models_[f].faces.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
			if (models_[f].indexBuffer == nullptr)
			{
				return false;
			}
		}
	}

	isBufferStoredOnGPU_ = true;
	return true;
}

bool Model::GetIsBufferStoredOnGPU() const
{
	return isBufferStoredOnGPU_;
}

bool Model::GenerateWireIndexBuffer(Backend::GraphicsDevice* graphicsDevice)
{
	if (isWireIndexBufferGenerated_)
	{
		return true;
	}

	if (graphicsDevice == nullptr)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		CustomVector<int32_t> indexes;
		indexes.reserve(GetFaceCount(f) * 6);

		auto fp = GetFaces(f);

		for (int32_t i = 0; i < GetFaceCount(f); i++)
		{
			indexes.emplace_back(fp->Indexes[0]);
			indexes.emplace_back(fp->Indexes[1]);
			indexes.emplace_back(fp->Indexes[1]);
			indexes.emplace_back(fp->Indexes[2]);
			indexes.emplace_back(fp->Indexes[2]);
			indexes.emplace_back(fp->Indexes[0]);
			fp++;
		}

		{
			models_[f].wireIndexBuffer = graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(indexes.size()), indexes.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
			if (models_[f].wireIndexBuffer == nullptr)
			{
				return false;
			}
		}
	}

	isWireIndexBufferGenerated_ = true;

	return isWireIndexBufferGenerated_;
}

bool Model::GetIsWireIndexBufferGenerated() const
{
	return isWireIndexBufferGenerated_;
}

} // namespace Effekseer
