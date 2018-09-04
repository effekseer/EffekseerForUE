#include "AssetTypeActions_EffekseerModel.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_EffekseerModel::GetSupportedClass() const
{
	return UEffekseerModel::StaticClass();
}

void FAssetTypeActions_EffekseerModel::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto MyAssetImports = GetTypedWeakObjectPtrs<UEffekseerModel>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("MyAsset_AssignResources", "AssignResources"),
		LOCTEXT("MyAsset_AssignResourcesTooltip", "AssignResources this EffekseerModel"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(
				this, &FAssetTypeActions_EffekseerModel::ExecuteAssignResources, MyAssetImports),
			FCanExecuteAction()
		)
	);
}
void FAssetTypeActions_EffekseerModel::ExecuteAssignResources(TArray<TWeakObjectPtr<class UEffekseerModel>> Objects)
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