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
	bool Connected = AF_Impl->ArduinoUsbConnect();

	if (Connected)
	{
		// Wait before start
		FPlatformProcess::Sleep(AF_Impl->ArduinoWaitTime);
	}

	// Arduino main loop
	while (Connected && StopTaskCounter.GetValue() == 0 && true)
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

	// Stop communication with arduino
	AF_Impl->ArduinoUsbDisconnect();
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

FAF_Impl::FAF_Impl(FString ArduinoPortName, float ArduinoWaitTime, int ArduinoMaxDataLength, int32 ArduinoMotorVoltageDefault)
	: ArduinoWorker(nullptr)
	, ArduinoPortName(ArduinoPortName)
	, ArduinoWaitTime(ArduinoWaitTime)
	, ArduinoMaxDataLength(ArduinoMaxDataLength)
	, ArduinoMotorVoltageDefault(ArduinoMotorVoltageDefault)
	, bIsConnected(false)
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

int FAF_Impl::ReadSerialPort(char * Buffer, unsigned int BufSize)
{
	return 0;
}

bool FAF_Impl::WriteSerialPort(char * Buffer, unsigned int BufSize)
{
	return false;
}

bool FAF_Impl::ArduinoUsbConnect()
{
	Handler = CreateFileA(static_cast<LPCSTR>(TCHAR_TO_ANSI(*ArduinoPortName)),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);


	if (Handler == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			UE_LOG(LogTemp, Error, TEXT(">> ERROR: Handle was not attached. Reason: %s not available"), *ArduinoPortName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unknown error, port %s"), *ArduinoPortName);
		}
	}
	else
	{
		DCB DCBSerialParameters = { 0 };
		if (!GetCommState(Handler, &DCBSerialParameters))
		{
			UE_LOG(LogTemp, Error, TEXT("failed to get current serial parameters"));
		}
		else
		{
			DCBSerialParameters.BaudRate = CBR_9600;
			DCBSerialParameters.ByteSize = 8;
			DCBSerialParameters.StopBits = ONESTOPBIT;
			DCBSerialParameters.Parity = NOPARITY;
			DCBSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(Handler, &DCBSerialParameters))
			{
				UE_LOG(LogTemp, Error, TEXT("ALERT: could not set Serial port parameters"));
			}
			else
			{
				bIsConnected = true;
				PurgeComm(Handler, PURGE_RXCLEAR | PURGE_TXCLEAR);

				UE_LOG(LogTemp, Warning, TEXT("Arduino connected"));
			}
		}
	}

	return bIsConnected;
}

bool FAF_Impl::ArduinoUsbDisconnect()
{
	if (bIsConnected)
	{
		bIsConnected = false;
		CloseHandle(Handler);
	}

	return true;
}
