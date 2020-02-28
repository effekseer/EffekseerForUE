
#include "EffekseerEffectFactory.h"
#include "EffekseerEffect.h"
#include "HAL/FileManager.h"

#include "Runtime/Launch/Resources/Version.h"

UEffekseerEffectFactory::UEffekseerEffectFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UEffekseerEffect::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("efk;Effekseer"));
	Formats.Add(TEXT("efkefc;Effekseer"));
}
bool UEffekseerEffectFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UEffekseerEffect::StaticClass());
}
UClass* UEffekseerEffectFactory::ResolveSupportedClass()
{
	return UEffekseerEffect::StaticClass();
}

UObject* UEffekseerEffectFactory::FactoryCreateBinary(
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
#if ENGINE_MINOR_VERSION <= 17
	UEffekseerEffect* asset = CastChecked<UEffekseerEffect>(StaticConstructObject(InClass, InParent, InName, Flags));
#else
	UEffekseerEffect* asset = CastChecked<UEffekseerEffect>(NewObject<UEffekseerEffect>(InParent, InClass, InName, Flags));
#endif

	if (asset)
	{
		auto path = asset->GetPathName();
		asset->Load(Buffer, (int32_t)(BufferEnd - Buffer), *path);

		if (!asset->AssetImportData)
		{
#if ENGINE_MINOR_VERSION <= 17
			//asset->AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
			asset->AssetImportData = ConstructObject<UAssetImportData>(UAssetImportData::StaticClass(), asset);
#else
			asset->AssetImportData = NewObject<UAssetImportData>(asset, UAssetImportData::StaticClass());
#endif
		}

		asset->AssetImportData->Update(CurrentFilename);
	}
	
	return asset;
}

bool UEffekseerEffectFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UEffekseerEffect* asset = Cast<UEffekseerEffect>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UEffekseerEffectFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UEffekseerEffect* asset = Cast<UEffekseerEffect>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UEffekseerEffectFactory::Reimport(UObject* Obj)
{
	UEffekseerEffect* asset = Cast<UEffekseerEffect>(Obj);
	if (!asset)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = asset->AssetImportData->GetFirstFilename();
	auto scale = asset->Scale;

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

		asset->Scale = scale;

		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}