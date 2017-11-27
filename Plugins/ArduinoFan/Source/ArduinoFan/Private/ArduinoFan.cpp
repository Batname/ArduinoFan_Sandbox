// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArduinoFan.h"

#define LOCTEXT_NAMESPACE "FArduinoFanModule"
#include "AF_Impl.h"


void FArduinoFanModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module


	AF_Impl = new FAF_Impl("\\\\.\\COM3", 2.f, 255, 0, 0.1f);
	AF_Impl->ArduinoInit();
}

void FArduinoFanModule::ShutdownModule()
{
	if (AF_Impl != nullptr)
	{
		AF_Impl->ArduinoDisconnect();

		delete AF_Impl;
		AF_Impl = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArduinoFanModule, ArduinoFan)