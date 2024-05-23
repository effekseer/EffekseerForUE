// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EffekseerForUEEditorTarget : TargetRules
{
	public EffekseerForUEEditorTarget(TargetInfo Target)
		: base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		bOverrideBuildEnvironment = true;

		Type = TargetType.Editor;
		ExtraModuleNames.Add("EffekseerForUE");
	}
}
