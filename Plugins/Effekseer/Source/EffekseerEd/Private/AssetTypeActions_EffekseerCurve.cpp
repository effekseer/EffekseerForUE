#include "AssetTypeActions_EffekseerCurve.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_EffekseerCurve::GetSupportedClass() const
{
	return UEffekseerCurve::StaticClass();
}

void FAssetTypeActions_EffekseerCurve::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		const auto asset = CastChecked<UEffekseerCurve>(Asset);
		if (asset && asset->AssetImportData)
		{
			asset->AssetImportData->ExtractFilenames(OutSourceFilePaths);
		}
	}
}

void FAssetTypeActions_EffekseerCurve::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto MyAssetImports = GetTypedWeakObjectPtrs<UEffekseerCurve>(InObjects);
	/*
	MenuBuilder.AddMenuEntry(
		LOCTEXT("MyAsset_AssignResources", "AssignResources"),
		LOCTEXT("MyAsset_AssignResourcesTooltip", "AssignResources this EffekseerCurve"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(
				this, &FAssetTypeActions_EffekseerCurve::ExecuteAssignResources, MyAssetImports),
			FCanExecuteAction()
		)
	);
	*/
}
void FAssetTypeActions_EffekseerCurve::ExecuteAssignResources(TArray<TWeakObjectPtr<class UEffekseerCurve>> Objects)
{
	for (auto ObjIt = Objects.CreateIterator(); ObjIt; ++ObjIt)
	{
		auto Object = (*ObjIt).Get();
		if (Object)
		{
			//Object->AssignResources();
		}
	}
}

#undef LOCTEXT_NAMESPACE