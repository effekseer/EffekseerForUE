#include "AssetTypeActions_EffekseerMaterial.h"
#include "EffekseerMaterial.h"
#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_EffekseerMaterial::GetSupportedClass() const
{
	return UEffekseerMaterial::StaticClass();
}

void FAssetTypeActions_EffekseerMaterial::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto MyAssetImports = GetTypedWeakObjectPtrs<UEffekseerMaterial>(InObjects);

}

#undef LOCTEXT_NAMESPACE