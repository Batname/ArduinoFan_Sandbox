// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AF_EventInterface.generated.h"

/**
* Event driven interface you can attach it to any blueprint to receive events from Arduino
*/
UINTERFACE(MinimalAPI)
class UAF_EventInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ARDUINOFAN_API IAF_EventInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ArduinoMessage", CompactNodeTitle = "", Keywords = "Arduino Message"), Category = "ArduinoFan")
	void ArduinoMessage(const FString& Message);

};