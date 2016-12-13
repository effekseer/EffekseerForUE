// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EffekseerEdPrivatePCH.h"
#include "AssetTypeActions_EffekseerEffect.h"
#include "AssetTypeActions_EffekseerModel.h"

class FEffekseerEd : public IEffekseerEd
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FAssetTypeActions_EffekseerEffect> EffekseerEffect_AssetTypeActions;
	TSharedPtr<FAssetTypeActions_EffekseerModel> EffekseerModel_AssetTypeActions;
};

IMPLEMENT_MODULE( FEffekseerEd, EffekseerEd)

void FEffekseerEd::StartupModule()
{
	EffekseerEffect_AssetTypeActions = MakeShareable(new FAssetTypeActions_EffekseerEffect);
	EffekseerModel_AssetTypeActions = MakeShareable(new FAssetTypeActions_EffekseerModel);

	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()
		.RegisterAssetTypeActions(EffekseerEffect_AssetTypeActions.ToSharedRef());

	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()
		.RegisterAssetTypeActions(EffekseerModel_AssetTypeActions.ToSharedRef());
}


void FEffekseerEd::ShutdownModule()
{
	if (EffekseerEffect_AssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get()
				.UnregisterAssetTypeActions(EffekseerEffect_AssetTypeActions.ToSharedRef());
		}
		EffekseerEffect_AssetTypeActions.Reset();
	}

	if (EffekseerModel_AssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get()
				.UnregisterAssetTypeActions(EffekseerModel_AssetTypeActions.ToSharedRef());
		}
		EffekseerModel_AssetTypeActions.Reset();
	}
}



