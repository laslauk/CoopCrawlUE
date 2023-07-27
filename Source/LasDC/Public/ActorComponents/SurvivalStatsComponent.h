// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalStatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASDC_API USurvivalStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USurvivalStatsComponent();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHunger)
		float Hunger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated = OnRep_CurrentThirst)
		float Thirst;

	virtual void PostInitializeComponents();

	UFUNCTION()
		void OnRep_CurrentHunger();

	UFUNCTION()
		void OnRep_CurrentThirst();


	/** Response to stats  being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnHungerAndThirstUpdate();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;




	FTimerHandle StatsTimerHandle;

	void HandleHungerAndThirst();


	/*
	For a player action on a client machine (eg. shooting) there'll often be a 'server RPC' call to let the server know the client is trying to do that action, 
	so the server can verify (eg. check ammo, hit rego, etc) and send back the results to all relevant clients.

	With characters for example, a copy exists for each character on each machine. So in a 3 player game (with it set so the server is a player too) there would 
	be 9 total characters in the whole system, 3 on each player's machine.

	So of the 3 characters on its machine, a client only owns one. So only that character (as per the chart), the client can call a 'server RPC',
	which will trigger code on the copy of that character that exists on the server.!

	That might be the ammo/rego code. Then the server wants to let clients know about the results. 
	So (again on the server's copy of the shooting client's character) it calls a 'client RPC',
	which triggers code back on the owning client's copy of the character, updating it.

	Maybe for ammo updates only the owning client need to know, so a 'client RPC' is used.
	But for shooting animations or hit results maybe every client needs to know, so 'multicast RPC' is used.


	*/
public: 
	//run on server RCP,  RPC that will be called on the client, but executed on the server is very similar but uses the Server keyword:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRCPLowerHunger(float Value); //tälle ei tehä definition
	//bool ServerRPCLowerHunger_Validate(float Value);
	//void ServerRPCLowerHunger_Implementation(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRCPLowerThirst(float Value);
	//bool ServerRPCLowerThirstValidate(float Value);
	//void ServerRPCLowerThirst_Implementation(float Value);


public:	


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	void ReduceHunger(float Value);
	void ReduceThirst(float Value);

		
};
