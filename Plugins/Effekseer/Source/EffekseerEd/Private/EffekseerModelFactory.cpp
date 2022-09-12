
#include "EffekseerModelFactory.h"

#include "Editor.h"
#include "EffekseerModel.h"
#include "EffekseerUECompatibility.h"
#include "HAL/FileManager.h"
#include "RawMesh.h"
#include "Runtime/Launch/Resources/Version.h"

UEffekseerModelFactory::UEffekseerModelFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UEffekseerModel::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("efkmodel;EffekseerModel"));
}
bool UEffekseerModelFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UEffekseerModel::StaticClass());
}
UClass* UEffekseerModelFactory::ResolveSupportedClass()
{
	return UEffekseerModel::StaticClass();
}

UObject* UEffekseerModelFactory::FactoryCreateBinary(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const uint8*& Buffer,
	const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	TArray<UObject*> retAssets;

	// Start impoprting
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Type);

	UEffekseerModel* assetEfkMdl = NewObject<UEffekseerModel>(InParent, InClass, FName(InName), Flags);

	if (assetEfkMdl)
	{
		auto path = assetEfkMdl->GetPathName();
		assetEfkMdl->Load(Buffer, (int32_t)(BufferEnd - Buffer), *path);

		if (!assetEfkMdl->AssetImportData)
		{
			assetEfkMdl->AssetImportData = NewObject<UAssetImportData>(assetEfkMdl, UAssetImportData::StaticClass());
		}

		assetEfkMdl->AssetImportData->Update(CurrentFilename);
	}

	retAssets.Add(assetEfkMdl);

	UStaticMesh* assetSM = NewObject<UStaticMesh>(InParent, UStaticMesh::StaticClass(), FName(*(InName.ToString() + TEXT("_SM"))), Flags);

	if (assetSM)
	{
		assetSM->GetStaticMaterials().Add(FStaticMaterial(nullptr, NAME_None));

		FRawMesh rawMesh;

		auto mesh = assetEfkMdl->GetMesh();

		for (auto i = 0; i < mesh.Positions.Num(); i++)
		{
			rawMesh.VertexPositions.Add(mesh.Positions[i]);
		}

		for (auto i = 0; i < mesh.Indexes.Num(); i++)
		{
			auto ind = mesh.Indexes[i];
			rawMesh.WedgeIndices.Add(ind);
			rawMesh.WedgeTangentX.Add(mesh.Binormal[ind]);
			rawMesh.WedgeTangentY.Add(mesh.Tangent[ind]);
			rawMesh.WedgeTangentZ.Add(mesh.Normal[ind]);
			rawMesh.WedgeTexCoords[0].Add(EffekseerUE::UEFVector2f(mesh.UV[ind]));
			rawMesh.WedgeTexCoords[1].Add(EffekseerUE::UEFVector2f(mesh.UV[ind]));
			rawMesh.WedgeColors.Add(mesh.Colors[ind]);
		}

		int faceInd = 0;
		auto faceOffsets = assetEfkMdl->GetAnimationFaceOffsets();
		for (auto i = 0; i < mesh.Indexes.Num() / 3; i++)
		{
			if (faceInd < faceOffsets.Num() - 1 && faceOffsets[faceInd + 1] <= i)
				faceInd++;

			rawMesh.FaceMaterialIndices.Add(faceInd);
			rawMesh.FaceSmoothingMasks.Add(faceInd);
		}

#if ENGINE_MAJOR_VERSION == 5
		auto& newModel = assetSM->AddSourceModel();
		FStaticMeshSourceModel* lodModel = &newModel;
#else
		FStaticMeshSourceModel* lodModel = new (assetSM->GetSourceModels()) FStaticMeshSourceModel();
#endif

		lodModel->BuildSettings.bUseMikkTSpace = false;
		lodModel->BuildSettings.bRecomputeNormals = false;
		lodModel->BuildSettings.bRecomputeTangents = false;
		lodModel->BuildSettings.bRemoveDegenerates = false;
		lodModel->BuildSettings.bBuildReversedIndexBuffer = false;
		lodModel->BuildSettings.bUseHighPrecisionTangentBasis = false;
		lodModel->BuildSettings.bUseFullPrecisionUVs = false;
		lodModel->BuildSettings.bGenerateLightmapUVs = false;

#if ENGINE_MAJOR_VERSION == 4
		lodModel->BuildSettings.bBuildAdjacencyBuffer = false;
#endif

		lodModel->ScreenSize = 0.1f / FMath::Pow(2.0f, assetSM->GetSourceModels().Num() - 1);

		lodModel->RawMeshBulkData->SaveRawMesh(rawMesh);

		assetSM->Build();
		assetSM->CreateBodySetup();

		if (!assetSM->AssetImportData)
		{
			assetSM->AssetImportData = NewObject<UAssetImportData>(assetSM, UAssetImportData::StaticClass());
		}

		assetSM->AssetImportData->Update(CurrentFilename);
	}

	assetEfkMdl->Mesh = assetSM;
	assetEfkMdl->AnimationFaceOffsets = assetEfkMdl->GetAnimationFaceOffsets();
	assetEfkMdl->AnimationFaceCounts = assetEfkMdl->GetAnimationFaceCounts();

	assetEfkMdl->AssignInternalPtr();

	retAssets.Add(assetSM);

	for (UObject* Object : retAssets)
	{
		if (Object)
		{
			GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);

			Object->MarkPackageDirty();
			Object->PostEditChange();
		}
	}

	return assetEfkMdl;
}

bool UEffekseerModelFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UEffekseerModel* asset = Cast<UEffekseerModel>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UEffekseerModelFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UEffekseerModel* asset = Cast<UEffekseerModel>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UEffekseerModelFactory::Reimport(UObject* Obj)
{
	UEffekseerModel* asset = Cast<UEffekseerModel>(Obj);
	if (!asset)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = asset->AssetImportData->GetFirstFilename();

	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	EReimportResult::Type Result = EReimportResult::Failed;

	if (UFactory::StaticImportObject(
			asset->GetClass(),
			asset->GetOuter(),
			*asset->GetName(),
			RF_Public | RF_Standalone,
			*Filename,
			NULL,
			this))
	{
		if (asset->GetOuter())
		{
			asset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			asset->MarkPackageDirty();
		}
		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}