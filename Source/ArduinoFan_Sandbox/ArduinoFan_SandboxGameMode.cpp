// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArduinoFan_SandboxGameMode.h"
#include "ArduinoFan_SandboxPawn.h"
#include "ArduinoFan_SandboxHud.h"

AArduinoFan_SandboxGameMode::AArduinoFan_SandboxGameMode()
{
	DefaultPawnClass = AArduinoFan_SandboxPawn::StaticClass();
	HUDClass = AArduinoFan_SandboxHud::StaticClass();
}
