// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActorBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "WeaponItemActor.generated.h"

/*
NETWORKING Reminder:


- Serverillä one true AUTHORITATIVE game state.

jokanen client remote control PAWNS jotka ne owns on the server. ja lähettää sille procedure calls to perform ingame actions.
- server ei streamaa visuals directly to clients monitors. sen sijaan, Server REPLICATES information about the game state to each tclient.
- esim mitkä actorit existaa, miten actorit behavee ja mitä value different variablet should have. Clientit uses infoa että ne simuloi aprroximately mitä tapahtuu servulla


//Declaration of Server RPC MyFunction.
UFUNCTION(Server, Reliable, WithValidation)
void MyFunction(int myInt);


//Implementation of Server RPC MyFunction.
void AExampleClass::MyFunction_Implementation(int myInt)
{
	//Gameplay code goes here.
}


RPCs (Remote Procedure Calls) are functions that are called locally, but executed remotely on another machine (separate from the calling machine).


he primary use case for these features are to do UNRELIABLE gameplay events that are transient or cosmetic in nature.

> Sounds, FX, spawn particles,

!! Tärkeä konsepti, how ownership works 
https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/Actors/OwningConnections/


						OWNERSHIP:
						each connection has Playercontroller, owned by A connection.

*/


UCLASS()
class LASDC_API AWeaponItemActor : public AItemActorBase
{
	GENERATED_BODY()


public:

	AWeaponItemActor();

	const UWeaponStaticData* GetWeaponStaticData() const;

	UFUNCTION()
	void SpawnBulletCasing();

	UFUNCTION(BlueprintCallable)
	void PlayWeaponEffects(const FHitResult& InHitResult);

	UFUNCTION()
	void PlayWeaponFiringEffectsInternal();




	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	UFUNCTION()
		FORCEINLINE UMeshComponent* GetMeshComponent() { return MeshComponent; }


protected:


	UPROPERTY() //visuals only local
		UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;

	//soundFX saatetaan executee owning client tai server, multicast function.
	//executetaan serverillä ja kaikilla connect clients 
	// NetMulticast on RPCs - replicated function, can be called from any machine
	// mutta direct their implementation to happend onm speciifc machine joka on connected to netwrok session
	// netmulticast, called on all clients that are connected tot the server + server itse.
	// 
	// 
	//Multicast RPCs are designed to be called from the server,  and then executed on the server as well as all currently connected clients. 
	// !! Multicast RPCs can also be called from clients but will only execute locally in this case.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayWeaponEffects(const FHitResult& InHitResult);


	/* Effektien logiikka*/
	/*
		kutsuaan abilitystä blueprint PlayWeaponFiringEffects()

		PlayWeaponFiringEffects() -> ServerPlayWeaponFireEffects()  -> MultiCastPlayWeaponFireEffects() == Actual koodi

		toinen tapa miten Pelkkä effectit thety:

		Blueprinteistä: PlayWeaponEffects()
						-SERVU HasAuthority() -> MULTICAST_Play_WeaponEffects() ==  (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy) ->   PlayWeaponEffectsInternal() actual koodi
						-EI SERVU: No Authority -> PlayWeaponEffectsInternal() Actual koodi
	
	*/


	/* Eka kutsutaan lokaalisti play weapon firing*/
	UFUNCTION(BlueprintCallable)
	void PlayWeaponFiringEffects();



	/* Play kutsuu tätä joka suoritetaan servulla*/
	UFUNCTION(Server, Reliable)
	void ServerPlayWeaponFireEffects();

	/* Server play kutsuu tätä joka menee servulle JA clienteille Servulla kutsutaan, menee clienteillä*/
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastPlayWeaponFireEffects();


	//This does the execution, no communication, just play
	void PlayWeaponEffectsInternal(const FHitResult& InHitResult);
	
public:

	UFUNCTION(BlueprintCallable)
		FTransform GetFireDirection(const FHitResult& HitTarget);
};
