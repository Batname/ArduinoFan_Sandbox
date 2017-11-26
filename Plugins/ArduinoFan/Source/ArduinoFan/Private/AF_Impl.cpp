// Fill out your copyright notice in the Description page of Project Settings.

#include "AF_Impl.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

#include <string>

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
		// Execute Arduino command
		if (!AF_Impl->ArduinoCommand.IsEmpty())
		{
			std::string InputString(TCHAR_TO_ANSI(*AF_Impl->ArduinoCommand));
			char *c_string = new char[AF_Impl->ArduinoCommand.Len() + 1];
			std::copy(InputString.begin(), InputString.end(), c_string);
			c_string[InputString.size()] = '\n';

			//Writing string to arduino
			AF_Impl->WriteSerialPort(c_string, AF_Impl->ArduinoMaxDataLength);

			delete[] c_string;

			// clear command
			AF_Impl->ArduinoCommand = "";
		}

		// Read data from Arduino
		int ReadResult = AF_Impl->ReadSerialPort(AF_Impl->IncomingDataBuffer, AF_Impl->ArduinoMaxDataLength);
		if (ReadResult)
		{
			AF_Impl->IncomingDataBuffer[ReadResult] = 0;
			UE_LOG(LogTemp, Warning, TEXT("IncomingDataBuffer: %s"), *FString(AF_Impl->IncomingDataBuffer));
			AF_Impl->ArduinoMessage = FString(AF_Impl->IncomingDataBuffer);
		}
		else
		{
			AF_Impl->ArduinoMessage = "";
		}

		//prevent thread from using too many resources
		FPlatformProcess::Sleep(AF_Impl->ArduinoCommunicationDelay);
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

FAF_Impl::FAF_Impl(FString ArduinoPortName, float ArduinoWaitTime, int ArduinoMaxDataLength, int32 ArduinoMotorVoltageDefault, float ArduinoCommunicationDelay)
	: ArduinoWorker(nullptr)
	, ArduinoPortName(ArduinoPortName)
	, ArduinoWaitTime(ArduinoWaitTime)
	, ArduinoMaxDataLength(ArduinoMaxDataLength)
	, ArduinoMotorVoltageDefault(ArduinoMotorVoltageDefault)
	, ArduinoCommunicationDelay(ArduinoCommunicationDelay)
	, ArduinoMessage("")
	, ArduinoCommand("")
	, bIsConnected(false)
	, IncomingDataBuffer(nullptr)
{
	UE_LOG(LogTemp, Warning, TEXT("FAF_Impl::FAF_Impl"));
}


FAF_Impl::~FAF_Impl()
{
	UE_LOG(LogTemp, Warning, TEXT("FAF_Impl::~FAF_Impl()"));

	ArduinoDisconnect();

	if (IncomingDataBuffer != nullptr)
	{
		delete[] IncomingDataBuffer;
	}
}

bool FAF_Impl::IsConnected() const
{
	return true;
}

bool FAF_Impl::ArduinoInit()
{
	// create buffer
	IncomingDataBuffer = new char[ArduinoMaxDataLength];

	// create thread
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

bool FAF_Impl::ArduinoMotorStart()
{
	ArduinoCommand = "2";

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
	DWORD BytesRead;
	unsigned int ToRead;

	ClearCommError(Handler, &Errors, &Status);

	if (Status.cbInQue > 0)
	{
		if (Status.cbInQue > BufSize)
		{
			ToRead = BufSize;
		}
		else
		{
			ToRead = Status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(Handler, Buffer, ToRead, &BytesRead, NULL))
		{
			return BytesRead;
		}
	}

	return 0;
}

bool FAF_Impl::WriteSerialPort(char * Buffer, unsigned int BufSize)
{
	DWORD BytesSend;

	if (!WriteFile(Handler, (void*)Buffer, BufSize, &BytesSend, 0))
	{
		ClearCommError(Handler, &Errors, &Status);
		return false;
	}
	else
	{
		return true;
	}
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
