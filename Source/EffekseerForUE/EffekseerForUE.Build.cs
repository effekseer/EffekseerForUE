// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class EffekseerForUE : ModuleRules
{
	public EffekseerForUE(ReadOnlyTargetRules Target)
		: base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
