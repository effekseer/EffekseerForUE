#include "AssetTypeActions_EffekseerEffect.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_EffekseerEffect::GetSupportedClass() const
{
	return UEffekseerEffect::StaticClass();
}

void FAssetTypeActions_EffekseerEffect::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto MyAssetImports = GetTypedWeakObjectPtrs<UEffekseerEffect>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("MyAsset_AssignResources", "AssignResources"),
		LOCTEXT("MyAsset_AssignResourcesTooltip", "AssignResources this EffekseerEffect"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(
				this, &FAssetTypeActions_EffekseerEffect::ExecuteAssignResources, MyAssetImports),
			FCanExecuteAction()
		)
	);
}
void FAssetTypeActions_EffekseerEffect::ExecuteAssignResources(TArray<TWeakObjectPtr<class UEffekseerEffect>> Objects)
{
	for (auto ObjIt = Objects.CreateIterator(); ObjIt; ++ObjIt)
	{
		auto Object = (*ObjIt).Get();
		if (Object)
		{
			Object->AssignResources();
		}
	}
}

#undef LOCTEXT_NAMESPACE