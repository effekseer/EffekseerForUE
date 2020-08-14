
#pragma once
#include "CoreMinimal.h"
#include "UnrealEd.h"
#include "AssetTypeActions_Base.h"
#include "EffekseerModel.h"

class FAssetTypeActions_EffekseerModel : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override
	{
		return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EffekseerModel", "EffekseerModel");
	}
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
	virtual UClass* GetSupportedClass() const override;
	bool IsImportedAsset() const override;
	void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;

	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	void ExecuteAssignResources(TArray<TWeakObjectPtr<class UEffekseerModel>> Objects);
};