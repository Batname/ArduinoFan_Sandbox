// Fill out your copyright notice in the Description page of Project Settings.

#include "AF_Controller.h"
#include "AF_Impl.h"


// Sets default values for this component's properties
UAF_Controller::UAF_Controller(const FObjectInitializer &ObjectInitializer)
	: UActorComponent(ObjectInitializer)
	, AF_Impl(MakeShareable(new FAF_Impl()))
	, ArduinoMotorVoltageDefault(150)
	, CurrentArduinoMotorVoltage(ArduinoMotorVoltageDefault)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::UAF_Controller"));
}

UAF_Controller::~UAF_Controller()
{
	if (AF_Impl.IsValid())
	{
		AF_Impl.Reset();
	}

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::~UAF_Controller"));
}


// Called when the game starts
void UAF_Controller::BeginPlay()
{
	Super::BeginPlay();

	AF_Impl->ArduinoInit();

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::BeginPlay"));
}

void UAF_Controller::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ArduinoDisconnect();

	UE_LOG(LogTemp, Warning, TEXT("UAF_Controller::EndPlay"));
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
}

void UAF_Controller::OnUnregister()
{
	Super::OnUnregister();
}

bool UAF_Controller::IsConnected() const
{
	return false;
}

bool UAF_Controller::ArduinoInit()
{
	return false;
}

bool UAF_Controller::ArduinoDisconnect()
{
	AF_Impl->ArduinoDisconnect();

	return false;
}

bool UAF_Controller::ArduinoMotorForvard()
{
	return false;
}

bool UAF_Controller::ArduinoMotorStop()
{
	return false;
}

bool UAF_Controller::SetArduinoMotorVoltage(uint8 RelativeVoltage)
{
	return false;
}

void UAF_Controller::InterfaceEventTick(float DeltaTime)
{
}

