
#include "EffekseerEdPrivatePCH.h"
#include "EffekseerModelFactory.h"
#include "EffekseerModel.h"

#include "Editor/UnrealEd/Public/Editor.h"
#include "Developer/RawMesh/Public/RawMesh.h"

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
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Type);

	UEffekseerModel* assetEfkMdl = NewObject<UEffekseerModel>(InParent, InClass, FName(InName), Flags);

	if (assetEfkMdl)
	{
		auto path = assetEfkMdl->GetPathName();
		assetEfkMdl->Load(Buffer, (int32_t)(BufferEnd - Buffer), *path);

		if (!assetEfkMdl->AssetImportData)
		{
			assetEfkMdl->AssetImportData = ConstructObject<UAssetImportData>(UAssetImportData::StaticClass(), assetEfkMdl);
		}

		assetEfkMdl->AssetImportData->Update(CurrentFilename);
	}

	retAssets.Add(assetEfkMdl);

	UStaticMesh* assetSM = NewObject<UStaticMesh>(InParent, UStaticMesh::StaticClass(), FName(*(InName.ToString() + _T("_SM"))), Flags);

	if (assetSM)
	{
		FRawMesh rawMesh;

		if (!assetSM->AssetImportData)
		{
			assetSM->AssetImportData = ConstructObject<UAssetImportData>(UAssetImportData::StaticClass(), assetSM);
		}

		assetSM->AssetImportData->Update(CurrentFilename);
	}

	assetEfkMdl->Mesh = assetSM;

	retAssets.Add(assetSM);

	for (UObject* Object : retAssets)
	{
		if (Object)
		{
			FEditorDelegates::OnAssetPostImport.Broadcast(this, Object);
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