// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/SurvivalStatsComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USurvivalStatsComponent::USurvivalStatsComponent()
{
	//SetIsReplicated(true);

	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	Hunger = 100.f;
	Thirst = 100.f;

	
}

void USurvivalStatsComponent::PostInitializeComponents() {

	


}
void USurvivalStatsComponent::OnRep_CurrentHunger()
{
	//Kun servulla hunger muuttuu, ajetaan tämä 
	//Kun servulla hunger muuttuu, ajetaan tämä funtkio, kutsutaan on hugner joka ei o replikoitutekemään update

	FString healthMessage = FString::Printf(TEXT(" ONREP : You now have %f HUNGER remaining."), Hunger);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	OnHungerAndThirstUpdate();
}

void USurvivalStatsComponent::OnRep_CurrentThirst()
{
	//Kun servulla hunger muuttuu, ajetaan tämä funtkio, kutsutaan on hugner joka ei o replikoitutekemään update
	OnHungerAndThirstUpdate();
}

// OnStatsUpdate ei o replikoitu joten nmanually call it on all devices
void USurvivalStatsComponent::OnHungerAndThirstUpdate()
{
	//Client-specific functionality
	if (GetOwnerRole() != ROLE_Authority) {

		FString healthMessage = FString::Printf(TEXT(" CLIENT : You now have %f thirst remaining."), Thirst);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

	}

	if (GetOwnerRole() == ROLE_Authority) {
		FString healthMessage = FString::Printf(TEXT("SERVER You now have %f thirst remaining."), Thirst);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
	/* tähän mitkä occuraa kaikilla masiinoilla */

}

// Called when the game starts
void USurvivalStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	//Handle, , owner, callback,, time, loop?
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &USurvivalStatsComponent::HandleHungerAndThirst, 1.0f, true);

	// ...
	
}


/* Runb on server */
void USurvivalStatsComponent::HandleHungerAndThirst()
{

	ReduceThirst(5);
	ReduceHunger(5);
	/*if (GetOwner()->HasAuthority()) {
		
	}
	else {
		ReduceThirst(10);
		ReduceHunger(10);

	}
	
	*/
}



void USurvivalStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USurvivalStatsComponent, Hunger);
	DOREPLIFETIME(USurvivalStatsComponent, Thirst);
}

void USurvivalStatsComponent::ReduceHunger(float Value)
{

	if (GetOwnerRole() == ROLE_Authority) {

		Hunger -= Value;

		OnHungerAndThirstUpdate();

	}

	/*
	//cant do ROLE == ? because its actor component not actor
	if (GetOwnerRole() != ROLE_Authority) {

		//not authority, run server function
		ServerRCPLowerHunger(Value);
	}
	else {

		//we got authorrity
		Hunger -= Value;
	}
	*/
}

void USurvivalStatsComponent::ReduceThirst(float Value)
{
	if (GetOwnerRole() == ROLE_Authority) {

		Thirst -= Value;
	
	}
	/*
	//cant do ROLE == ? because its actor component not actor
	if (GetOwnerRole() != ROLE_Authority) {
		ServerRCPLowerThirst(Value);
		//not authority, run server function

	}
	else {
		//we got authorrity


	} */
}




bool USurvivalStatsComponent::ServerRCPLowerHunger_Validate(float Value)
{
	return true;
}

void USurvivalStatsComponent::ServerRCPLowerHunger_Implementation(float Value)
{
	//run on server, but check anyway
	if (GetOwnerRole() == ROLE_Authority) //hakee tän comnponentin ownerin 
	{
		//nyt ajetaan vain servulla tämä

		ReduceHunger(Value);

	}

}

bool USurvivalStatsComponent::ServerRCPLowerThirst_Validate(float Value)
{
	return true;
}

void USurvivalStatsComponent::ServerRCPLowerThirst_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority) //hakee tän comnponentin ownerin 
	{
		ReduceThirst(Value);


	}

}