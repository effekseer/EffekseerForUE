#include "EffekseerPrivatePCH.h"
#include "EffekseerModel.h"
#include "EffekseerNative.h"
#include "EffekseerInternalModel.h"

#include "EffekseerCustomVersion.h"

void UEffekseerModel::LoadModel(const uint8_t* data, int32_t size, const TCHAR* path)
{
	auto model = new EffekseerInternalModel((void*)data, size);
	modelPtr = model;
}

void UEffekseerModel::ReleaseModel()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	ES_SAFE_DELETE(p);
	modelPtr = nullptr;
}

void UEffekseerModel::Load(const uint8_t* data, int32_t size, const TCHAR* path)
{
	buffer.Reset(0);
	buffer.Append(data, size);
	LoadModel(data, size, path);
}

void UEffekseerModel::AssignInternalPtr()
{
	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr) return;

	p->UserData = Mesh;
}

FEffekseerModelMesh UEffekseerModel::GetMesh()
{
	FEffekseerModelMesh mesh;

	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr) return mesh;

	for (auto i = 0; i < p->GetVertexCount(); i++)
	{
		auto v = p->GetVertexes()[i];

		mesh.Positions.Add(FVector(v.Position.X, v.Position.Z, v.Position.Y));
		mesh.Normal.Add(FVector(v.Normal.X, v.Normal.Z, v.Normal.Y));
		mesh.Binormal.Add(FVector(v.Binormal.X, v.Binormal.Z, v.Binormal.Y));
		mesh.Tangent.Add(FVector(v.Tangent.X, v.Tangent.Z, v.Tangent.Y));
		mesh.UV.Add(FVector2D(v.UV.X, v.UV.Y));
	}

	for (auto i = 0; i < p->GetFaceCount(); i++)
	{
		auto f = p->GetFaces()[i];
		mesh.Indexes.Add(f.Indexes[0]);
		mesh.Indexes.Add(f.Indexes[1]);
		mesh.Indexes.Add(f.Indexes[2]);
	}

	return mesh;
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