// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AF_Controller.generated.h"


class FAF_Impl;

UCLASS( ClassGroup=ArduinoFan, meta=(BlueprintSpawnableComponent) )
class ARDUINOFAN_API UAF_Controller : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAF_Controller(const FObjectInitializer &ObjectInitializer);

	~UAF_Controller();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	/**
	* Default relative voltage for arduino motors
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArduinoFan")
	uint8 ArduinoMotorVoltageDefault;

	/**
	* Keep in track current arduino voltage
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ArduinoFan")
	uint8 CurrentArduinoMotorVoltage;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	/**
	* Reports whether this Controller is connected.
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "is connected"), Category = "ArduinoFan")
	bool IsConnected() const;

	/**
	* Init Arduino thread
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "init arduino"), Category = "ArduinoFan")
	bool ArduinoInit();

	/**
	* Stop arduino main tread
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "disconnect arduino"), Category = "ArduinoFan")
	bool ArduinoDisconnect();


	/**
	* Start Arduino fans
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "start arduino fans"), Category = "ArduinoFan")
	bool ArduinoMotorForvard();

	/**
	* Stop Arduino fans
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "stop arduino fans"), Category = "ArduinoFan")
	bool ArduinoMotorStop();

	/**
	* Set relative voltage 
	* @param RelativeVoltage relative values for voltage from 0 up to 255
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "stop arduino fans"), Category = "ArduinoFan")
	bool SetArduinoMotorVoltage(uint8 RelativeVoltage);


/*~~~~~~~~~~~~~~~~~~~~~~~~~ Implementation ~~~~~~~~~~~~~~~~~~~~~~~~~*/
private:
	TSharedPtr<FAF_Impl> AF_Impl;

	void InterfaceEventTick(float DeltaTime);
};
