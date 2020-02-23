#include "AssetTypeActions_EffekseerMaterial.h"
#include "EffekseerMaterial.h"
#include "EffekseerMaterialFunctions.h"
#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_EffekseerMaterial::GetSupportedClass() const
{
	return UEffekseerMaterial::StaticClass();
}

void FAssetTypeActions_EffekseerMaterial::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto MyAssetImports = GetTypedWeakObjectPtrs<UEffekseerMaterial>(InObjects);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("MyAsset_GenerateMaterialElements", "GenerateMaterialElements"),
		LOCTEXT("MyAsset_GenerateMaterialElementsTooltip", "Generate materials for blending based on EffekseerMaterial's properies."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(
				this, &FAssetTypeActions_EffekseerMaterial::ExecuteGenerateMaterialElements, MyAssetImports),
			FCanExecuteAction()
		)
	);

}

void FAssetTypeActions_EffekseerMaterial::ExecuteGenerateMaterialElements(TArray<TWeakObjectPtr<class UEffekseerMaterial>> Objects)
{
	for (auto ObjIt = Objects.CreateIterator(); ObjIt; ++ObjIt)
	{
		auto Object = (*ObjIt).Get();
		if (Object)
		{
			AssignElementMaterials(Object, true);
		}
	}
}

#undef LOCTEXT_NAMESPACE