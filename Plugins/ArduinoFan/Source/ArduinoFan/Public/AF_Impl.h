// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/Runnable.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

UENUM(BlueprintType)
enum class EArduinoFanState : uint8
{
	AF_None 		UMETA(DisplayName = "State before init"),
	AF_Stop 		UMETA(DisplayName = "Stop fans state"),
	AF_Forward		UMETA(DisplayName = "Forward fans state"),
	AF_Reverse 		UMETA(DisplayName = "Reverse fans state"),
};

class FThreadSafeCounter;
class FAF_Impl;

//~~~~~ Multi Threading ~~~
class FArduinoWorker : public FRunnable
{
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static FArduinoWorker* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** The Data Ptr */
	TArray<uint32>* PrimeNumbers;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	char* ArduinoCommandString(const FString& ArduinoCommand);

private:
	FAF_Impl* AF_Impl;

public:
	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FArduinoWorker(FAF_Impl* AF_Impl);
	virtual ~FArduinoWorker();

	// Begin FRunnable interface.
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();


	//~~~ Starting and Stopping Thread ~~~
	/*
	Start the thread and the worker from static (easy access)!
	*/
	static FArduinoWorker* JoyInit(FAF_Impl* AF_Impl);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();
};


/**
 * Arduino motor driver communication
 */
class  FAF_Impl
{
	friend FArduinoWorker;

public:
	FAF_Impl(FString ArduinoPortName,
		float ArduinoWaitTime,
		int ArduinoMaxDataLength,
		int32 ArduinoMotorVoltageDefault,
		float ArduinoCommunicationDelay);
	~FAF_Impl();


	bool IsConnected() const;

	bool ArduinoInit();

	bool ArduinoDisconnect();

	bool ArduinoMotorForvard();

	bool ArduinoMotorStop();

	bool SetArduinoMotorVoltage(uint8 RelativeVoltage);

/*~~~~~~~~~~~~~~~~~~~~~~~~~ Arduino communication ~~~~~~~~~~~~~~~~~~~~~~~~~*/
public:
	FORCEINLINE const FString& GetArduinoMessage() { return ArduinoMessage; }

private:
	FString ArduinoMessage;

	FString ArduinoCommand;

private:
	FArduinoWorker* ArduinoWorker;

/*~~~~~~~~~~~~~~~~~~~~~~~~~ Usb connection implementation ~~~~~~~~~~~~~~~~~~~~~~~~~*/
public:
	EArduinoFanState GetArduinoFanState() { return ArduinoFanState; }

private:
	int ReadSerialPort(char* Buffer, unsigned int BufSize);
	bool WriteSerialPort(char* Buffer, unsigned int BufSize);

	bool ArduinoUsbConnect();
	bool ArduinoUsbDisconnect();

	FString ArduinoPortName;
	float ArduinoWaitTime;
	int32 ArduinoMaxDataLength;
	int32 ArduinoMotorVoltageDefault;
	float ArduinoCommunicationDelay;

	HANDLE Handler;
	bool bIsConnected;
	COMSTAT Status;
	DWORD Errors;

	EArduinoFanState ArduinoFanState;
};
