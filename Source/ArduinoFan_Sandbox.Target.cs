// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ArduinoFan_SandboxTarget : TargetRules
{
	public ArduinoFan_SandboxTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("ArduinoFan_Sandbox");
	}
}
