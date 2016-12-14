#include "EffekseerPrivatePCH.h"
#include "EffekseerModel.h"
#include "EffekseerNative.h"
#include "EffekseerInternalModel.h"

void UEffekseerModel::LoadModel(const uint8_t* data, int32_t size, const TCHAR* path)
{
	auto model = new EffekseerInternalModel((void*)data, size);
	modelPtr = model;
	model->UserData = Mesh;
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

FEffekseerModelMesh UEffekseerModel::GetMesh()
{
	FEffekseerModelMesh mesh;

	auto p = (EffekseerInternalModel*)modelPtr;
	if (p == nullptr) return mesh;

	for (auto i = 0; i < p->GetVertexCount(); i++)
	{
		auto v = p->GetVertexes()[i];

		mesh.Positions.Add(FVector(v.Position.X, v.Position.Y, v.Position.Z));
		mesh.Normal.Add(FVector(v.Normal.X, v.Normal.Y, v.Normal.Z));
		mesh.Binormal.Add(FVector(v.Binormal.X, v.Binormal.Y, v.Binormal.Z));
		mesh.Tangent.Add(FVector(v.Tangent.X, v.Tangent.Y, v.Tangent.Z));
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

	const int32_t Version = 1;
	const FGuid GUID(0x11334A13, 0x22554231, 0xA36291A3, 0xC5A1022C);
	static FCustomVersionRegistration GRegisterPaperCustomVersion(GUID, Version, TEXT("EffekseerModelVer"));
	Ar.UsingCustomVersion(GUID);

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
	}
}