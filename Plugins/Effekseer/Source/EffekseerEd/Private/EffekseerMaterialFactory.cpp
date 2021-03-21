#include "EffekseerMaterialFactory.h"

#include "Editor.h"
#include "HAL/FileManager.h"

#include "Runtime/Launch/Resources/Version.h"
#include "EditorFramework/AssetImportData.h"
#include "NativeEffekseerMaterialContext.h"
#include "Materials/MaterialInstance.h"
#include <map>
#include <memory>
#include <functional>

#include "EffekseerMaterialFunctions.h"


UEffekseerMaterialFactory::UEffekseerMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UEffekseerMaterial::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("efkmat;EffekseerMaterial"));
}
bool UEffekseerMaterialFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UEffekseerMaterial::StaticClass());
}
UClass* UEffekseerMaterialFactory::ResolveSupportedClass()
{
	return UEffekseerMaterial::StaticClass();
}

class MaterialCaller : public UMaterialInstance
{
protected:
	void Update()
	{
		UpdateParameterNames();
	}
public:
	
};

UObject* UEffekseerMaterialFactory::FactoryCreateBinary(
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
#if ENGINE_MINOR_VERSION >= 24
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Type);
#else
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Type);
#endif

	UEffekseerMaterial* assetEfkMat = Cast<UEffekseerMaterial>(FindObject<UEffekseerMaterial>(InParent, *InName.ToString()));

	if (assetEfkMat == nullptr)
	{
		assetEfkMat = NewObject<UEffekseerMaterial>(InParent, InClass, FName(InName), Flags);

		// Add defaults
		FEffekseerMaterialElement e1, e2;
		e1.AlphaBlend = EEffekseerAlphaBlendType::Blend;
		e2.AlphaBlend = EEffekseerAlphaBlendType::Add;
		assetEfkMat->MaterialElements.Add(e1);
		assetEfkMat->MaterialElements.Add(e2);
	}

	if (assetEfkMat)
	{
		auto path = assetEfkMat->GetPathName();

		if (!assetEfkMat->AssetImportData)
		{
			assetEfkMat->AssetImportData = NewObject<UAssetImportData>(assetEfkMat, UAssetImportData::StaticClass());
		}

		assetEfkMat->AssetImportData->Update(CurrentFilename);
	}

	retAssets.Add(assetEfkMat);

	{
		auto native = NativeEffekseerMaterialContext::Load(Buffer, BufferEnd - Buffer, TCHAR_TO_ANSI(*assetEfkMat->GetPathName()));

		UE4MaterialFromEffekseerMaterialOption option;
		auto originalMaterial = CreateUE4MaterialFromEffekseerMaterial(native, InParent->GetFName().ToString() + TEXT("_Mat/"), InName.ToString() + TEXT("_M"), option);
		
		assetEfkMat->Uniforms.Empty();
		for (auto u : native->result.Uniforms)
		{
			FEffekseerUniformProperty prop;
			prop.Name = FString(native->uniformNames[u->GUID].c_str());
			prop.Count = (int)u->Type + 1;
			assetEfkMat->Uniforms.Add(prop);
		}

		assetEfkMat->Textures.Empty();
		for (auto u : native->result.Textures)
		{
			FEffekseerTextureProperty prop;
			prop.Name = FString(native->textureNames[u->GUID].c_str());
			assetEfkMat->Textures.Add(prop);
		}

		assetEfkMat->Material = originalMaterial;
		assetEfkMat->IsEffectScaleRequired = native->isEffectScaleRequired;
		assetEfkMat->StoreData(Buffer, BufferEnd - Buffer);
		assetEfkMat->LoadMaterial(Buffer, BufferEnd - Buffer, nullptr);
		assetEfkMat->ReassignSearchingMaps();
		retAssets.Add(originalMaterial);

		for (auto a : AssignElementMaterials(assetEfkMat, true))
		{
			retAssets.Add(a);
		}

		// To reupdate elements
		assetEfkMat->RegisterMaterialCompicationFinished();
	}

	// notify finished
	for (UObject* Object : retAssets)
	{
		if (Object)
		{
#if ENGINE_MINOR_VERSION >= 24
			GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
#else
			FEditorDelegates::OnAssetPostImport.Broadcast(this, Object);
#endif
			Object->MarkPackageDirty();
			Object->PreEditChange(nullptr);
			Object->PostEditChange();
		}
	}

	return assetEfkMat;
}

bool UEffekseerMaterialFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	return false;
}

void UEffekseerMaterialFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UEffekseerMaterialFactory::Reimport(UObject* Obj)
{
	UEffekseerMaterial* asset = Cast<UEffekseerMaterial>(Obj);
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
		asset,
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