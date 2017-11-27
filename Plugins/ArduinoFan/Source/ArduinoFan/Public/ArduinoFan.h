// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "ModuleInterface.h"


UENUM(BlueprintType)
enum class EArduinoFanState : uint8
{
	AF_None 		UMETA(DisplayName = "State before init"),
	AF_Stop 		UMETA(DisplayName = "Stop fans state"),
	AF_Forward		UMETA(DisplayName = "Forward fans state"),
	AF_Reverse 		UMETA(DisplayName = "Reverse fans state"),
};

class FAF_Impl;

class FArduinoFanModule : public IModuleInterface
{
public:

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline FArduinoFanModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FArduinoFanModule >("ArduinoFan");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FAF_Impl* GetAF_Impl() { return AF_Impl; }

public:
	FAF_Impl* AF_Impl;
};