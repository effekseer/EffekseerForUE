// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EffekseerForUETarget : TargetRules
{
	public EffekseerForUETarget(TargetInfo Target)
	: base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		bOverrideBuildEnvironment = true;

		Type = TargetType.Game;
		ExtraModuleNames.Add("EffekseerForUE");
	}
}
