#include "EffekseerModel.h"

#include "EffekseerCustomVersion.h"
#include "EffekseerInternalModel.h"

void UEffekseerModel::BeginDestroy()
{
	ReleaseModel();
	Super::BeginDestroy();
}

void UEffekseerModel::LoadModel(const uint8_t* data, int32_t size, const TCHAR* path)
{
	auto model = new EffekseerInternalModel((void*)data, size);
	modelPtr = model;
}

void UEffekseerModel::ReleaseModel()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	ES_SAFE_RELEASE(p);
	modelPtr = nullptr;
}

void UEffekseerModel::Load(const uint8_t* data, int32_t size, const TCHAR* path)
{
	buffer.Reset(0);
	buffer.Append(data, size);
	LoadModel(data, size, path);
}

void UEffekseerModel::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
#if WITH_EDITORONLY_DATA
	if (AssetImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}
#endif

	Super::GetAssetRegistryTags(OutTags);
}

void UEffekseerModel::AssignInternalPtr()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr)
		return;

	p->UserData = this;
}

FEffekseerModelMesh UEffekseerModel::GetMesh()
{
	FEffekseerModelMesh mesh;

	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr)
		return mesh;

	int vertexOffset = 0;

	for (int f = 0; f < p->GetFrameCount(); f++)
	{
		for (auto i = 0; i < p->GetVertexCount(f); i++)
		{
			auto v = p->GetVertexes(f)[i];

			mesh.Positions.Add(EffekseerUE::UEFVector3f(v.Position.X, v.Position.Z, v.Position.Y));
			mesh.Normal.Add(EffekseerUE::UEFVector3f(v.Normal.X, v.Normal.Z, v.Normal.Y));
			mesh.Binormal.Add(EffekseerUE::UEFVector3f(v.Binormal.X, v.Binormal.Z, v.Binormal.Y));
			mesh.Tangent.Add(EffekseerUE::UEFVector3f(v.Tangent.X, v.Tangent.Z, v.Tangent.Y));
			mesh.UV.Add(EffekseerUE::UEFVector2f(v.UV.X, v.UV.Y));
			mesh.Colors.Add(FColor(v.VColor.R, v.VColor.G, v.VColor.B, v.VColor.A));
		}

		for (auto i = 0; i < p->GetFaceCount(f); i++)
		{
			auto face = p->GetFaces(f)[i];
			mesh.Indexes.Add(face.Indexes[2] + vertexOffset);
			mesh.Indexes.Add(face.Indexes[1] + vertexOffset);
			mesh.Indexes.Add(face.Indexes[0] + vertexOffset);
		}

		vertexOffset += p->GetVertexCount(f);
		// break;
	}

	return mesh;
}

TArray<int> UEffekseerModel::GetAnimationFaceCounts()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr)
		return TArray<int>();

	TArray<int> ret;

	for (int i = 0; i < p->GetFrameCount(); i++)
	{
		auto vc = p->GetFaceCount(i);
		ret.Add(vc);
		// break;
	}

	return ret;
}

TArray<int> UEffekseerModel::GetAnimationFaceOffsets()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr)
		return TArray<int>();

	TArray<int> ret;

	int offset = 0;

	for (int i = 0; i < p->GetFrameCount(); i++)
	{
		auto vc = p->GetFaceCount(i);
		ret.Add(offset);
		offset += vc;
		// break;
	}

	return ret;
}

void UEffekseerModel::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FEffekseerCustomVersion::GUID);

	/*
	#if WITH_EDITORONLY_DATA
	if (Ar.IsLoading() && (Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON) && (AssetImportData == nullptr))
	{
	// AssetImportData should always be valid
	AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}
	#endif
	*/

	if (Ar.IsSaving() || Ar.IsLoading())
	{
		Ar << buffer;
	}

	if (Ar.IsLoading())
	{
		ReleaseModel();

		auto path = GetPathName();
		LoadModel(buffer.GetData(), buffer.Num(), *path);

		AssignInternalPtr();
	}
}