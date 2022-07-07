
#include "EffekseerCurveFactory.h"

#include "Editor.h"
#include "EffekseerCurve.h"
#include "HAL/FileManager.h"
#include "RawMesh.h"
#include "Runtime/Launch/Resources/Version.h"

UEffekseerCurveFactory::UEffekseerCurveFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UEffekseerCurve::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("efkcurve;EffekseerCurve"));
}
bool UEffekseerCurveFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UEffekseerCurve::StaticClass());
}
UClass* UEffekseerCurveFactory::ResolveSupportedClass()
{
	return UEffekseerCurve::StaticClass();
}

UObject* UEffekseerCurveFactory::FactoryCreateBinary(
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

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Type);

	UEffekseerCurve* assetEfkCurve = NewObject<UEffekseerCurve>(InParent, InClass, FName(InName), Flags);

	if (assetEfkCurve)
	{
		auto path = assetEfkCurve->GetPathName();
		assetEfkCurve->Load(Buffer, (int32_t)(BufferEnd - Buffer), *path);

		if (!assetEfkCurve->AssetImportData)
		{
			assetEfkCurve->AssetImportData = NewObject<UAssetImportData>(assetEfkCurve, UAssetImportData::StaticClass());
		}

		assetEfkCurve->AssetImportData->Update(CurrentFilename);
	}

	retAssets.Add(assetEfkCurve);

	assetEfkCurve->AssignInternalPtr();

	for (UObject* Object : retAssets)
	{
		if (Object)
		{
			GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);

			Object->MarkPackageDirty();
			Object->PostEditChange();
		}
	}

	return assetEfkCurve;
}

bool UEffekseerCurveFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UEffekseerCurve* asset = Cast<UEffekseerCurve>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UEffekseerCurveFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UEffekseerCurve* asset = Cast<UEffekseerCurve>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UEffekseerCurveFactory::Reimport(UObject* Obj)
{
	UEffekseerCurve* asset = Cast<UEffekseerCurve>(Obj);
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