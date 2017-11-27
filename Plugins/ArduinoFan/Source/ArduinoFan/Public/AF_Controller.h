// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Components/ActorComponent.h"
#include "AF_Impl.h"
#include "AF_Controller.generated.h"

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	//Leap Event Interface forwarding, automatically set since 0.6.2, available for event redirection
	UFUNCTION(BlueprintCallable, meta = (Keywords = "set delegate self"), Category = "ArduinoFan")
	void SetInterfaceDelegate(UObject* NewDelegate);

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

	/**
	* Status of arduino
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "stop arduino fans"), Category = "ArduinoFan")
	EArduinoFanState GetArduinoFanState() { return AF_Impl->GetArduinoFanState(); }

/*~~~~~~~~~~~~~~~~~~~~~~~~~ Arduino manage variables ~~~~~~~~~~~~~~~~~~~~~~~~~*/
protected:
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

	/**
	* Connection delay for arduino
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArduinoFan")
	float ArduinoWaitTime;

	/**
	* Data buffer for arduino communication
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArduinoFan")
	int32 ArduinoMaxDataLength;

	/**
	* Communication delay
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArduinoFan")
	float ArduinoCommunicationDelay;


	/**
	* Arduino USB port name
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ArduinoFan")
	FString ArduinoPortName;



/*~~~~~~~~~~~~~~~~~~~~~~~~~ Implementation ~~~~~~~~~~~~~~~~~~~~~~~~~*/
private:
	FAF_Impl* AF_Impl;

	void InterfaceEventTick(float DeltaTime);

/*~~~~~~~~~~~~~~~~~~~~~~~~~ Interface delegation ~~~~~~~~~~~~~~~~~~~~~~~~~*/
private:
	UObject* InterfaceDelegate;

};
