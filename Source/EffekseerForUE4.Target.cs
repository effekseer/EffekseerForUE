// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EffekseerForUE4Target : TargetRules
{
    public EffekseerForUE4Target(TargetInfo Target)
    : base(Target)
    {
        Type = TargetType.Game;
        ExtraModuleNames.Add("EffekseerForUE4");
    }
}
