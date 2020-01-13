// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EffekseerForUE4EditorTarget : TargetRules
{
    public EffekseerForUE4EditorTarget(TargetInfo Target)
        : base(Target)
    {
        Type = TargetType.Editor;
        ExtraModuleNames.Add("EffekseerForUE4");
    }
}
