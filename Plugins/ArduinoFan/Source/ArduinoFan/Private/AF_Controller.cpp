// Fill out your copyright notice in the Description page of Project Settings.

#include "AF_Controller.h"
#include "AF_EventInterface.h"

// Sets default values for this component's properties
UAF_Controller::UAF_Controller(const FObjectInitializer &ObjectInitializer)
	: UActorComponent(ObjectInitializer)
	, ArduinoPortName("\\\\.\\COM3")
	, ArduinoWaitTime(2.f)
	, ArduinoMaxDataLength(255)
	, ArduinoMotorVoltageDefault(0)
	, ArduinoCommunicationDelay(0.1f)
	, CurrentArduinoMotorVoltage(ArduinoMotorVoltageDefault)
	, InterfaceDelegate(nullptr)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::UAF_Controller"));
}

UAF_Controller::~UAF_Controller()
{
	if (AF_Impl != nullptr)
	{
		AF_Impl->ArduinoMotorStop();
		AF_Impl = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::~UAF_Controller"));
}


// Called when the game starts
void UAF_Controller::BeginPlay()
{
	Super::BeginPlay();

	AF_Impl = FArduinoFanModule::Get().GetAF_Impl();

	if (AF_Impl)
	{
		AF_Impl->ArduinoMotorForvard();
	}

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::BeginPlay, %p"), AF_Impl);
}

void UAF_Controller::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

// Called every frame
void UAF_Controller::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InterfaceEventTick(DeltaTime);
}

void UAF_Controller::OnRegister()
{
	Super::OnRegister();

	//Attach the delegate pointer automatically
	SetInterfaceDelegate(GetOwner());
}

void UAF_Controller::OnUnregister()
{
	Super::OnUnregister();
}

void UAF_Controller::SetInterfaceDelegate(UObject * NewDelegate)
{
	//Use this format to support both blueprint and C++ form
	if (NewDelegate->GetClass()->ImplementsInterface(UAF_EventInterface::StaticClass()))
	{
		InterfaceDelegate = NewDelegate;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("ArduinoController Warning: Delegate is NOT set, did you implement AF_EventInterface?"));
	}
}

bool UAF_Controller::IsConnected() const
{
	if (AF_Impl)
	{
		return AF_Impl->IsConnected();
	}

	return false;
}

bool UAF_Controller::ArduinoMotorForvard()
{
	if (AF_Impl)
	{
		return AF_Impl->ArduinoMotorForvard();
	}

	return false;
}

bool UAF_Controller::ArduinoMotorStop()
{
	if (AF_Impl)
	{
		return AF_Impl->ArduinoMotorStop();
	}

	return false;
}

bool UAF_Controller::SetArduinoMotorVoltage(uint8 RelativeVoltage)
{
	if (AF_Impl)
	{
		return AF_Impl->SetArduinoMotorVoltage(RelativeVoltage);
	}

	return false;
}

void UAF_Controller::InterfaceEventTick(float DeltaTime)
{
	if (AF_Impl == nullptr)
	{
		return;
	}

	// Fire arduino message event
	const FString ArduinoMessage = AF_Impl->GetArduinoMessage();
	if (!ArduinoMessage.IsEmpty())
	{
		IAF_EventInterface::Execute_ArduinoMessage(InterfaceDelegate, ArduinoMessage);
	}
}

