// Fill out your copyright notice in the Description page of Project Settings.

#include "AF_Impl.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

//Thread Worker Starts as NULL, prior to being instanced
FArduinoWorker* FArduinoWorker::Runnable = nullptr;

FArduinoWorker::FArduinoWorker(FAF_Impl * AF_Impl)
	: AF_Impl(AF_Impl)
	, StopTaskCounter(0)
{
	// Create a counter
	FThreadSafeCounter  WorkerCounter;

	FString ThreadName(FString::Printf(TEXT("FArduinoWorker %i"), WorkerCounter.Increment()));

	// windows default = 8mb for thread, could specify more
	Thread = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);
}

FArduinoWorker::~FArduinoWorker()
{
	delete Thread;
	Thread = nullptr;
}

uint32 FArduinoWorker::Run()
{
	// Connect to arduino

	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

	// Arduino main loop
	while (StopTaskCounter.GetValue() == 0 && true)
	{

		//prevent thread from using too many resources
		FPlatformProcess::Sleep(1.00);
		UE_LOG(LogTemp, Warning, TEXT("FArduinoWorker::Run() Arduino main loop"));
	}

	return 0;
}

void FArduinoWorker::Stop()
{
	StopTaskCounter.Increment();
}

void FArduinoWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

FArduinoWorker * FArduinoWorker::JoyInit(FAF_Impl * AF_Impl)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FArduinoWorker(AF_Impl);
	}

	return Runnable;
}

void FArduinoWorker::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

FAF_Impl::FAF_Impl()
	: ArduinoWorker(nullptr)
{
	UE_LOG(LogTemp, Warning, TEXT("FAF_Impl::FAF_Impl"));
}


FAF_Impl::~FAF_Impl()
{
	UE_LOG(LogTemp, Warning, TEXT("FAF_Impl::~FAF_Impl()"));

	ArduinoDisconnect();
}

bool FAF_Impl::IsConnected() const
{
	return true;
}

bool FAF_Impl::ArduinoInit()
{
	if (ArduinoWorker == nullptr)
	{
		ArduinoWorker = FArduinoWorker::JoyInit(this);
	}

	return true;
}

bool FAF_Impl::ArduinoDisconnect()
{
	if (ArduinoWorker != nullptr)
	{
		FArduinoWorker::Shutdown();
		ArduinoWorker = nullptr;
	}

	return true;
}

bool FAF_Impl::ArduinoMotorForvard()
{
	return true;
}

bool FAF_Impl::ArduinoMotorStop()
{
	return true;
}

bool FAF_Impl::SetArduinoMotorVoltage(uint8 RelativeVoltage)
{
	return true;
}
