// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "InventoryList.h"
#include "Components/SphereComponent.h"
#include "CrawlCommonTypes.h"
#include "ItemActorBase.generated.h"

/* Physical representation of a item in world that can be dropped, references information about its instance data*/

/* KUN EQUIPOATAAN - INVENTORY ITEM INSTANCESSA ONEQUIPPED Handlaa uuden spawnaamisen */

UCLASS()
class LASDC_API AItemActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActorBase();


	virtual bool ReplicateSubobjects(UActorChannel* channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;



	void Init(class UInventoryItemInstance* InInstance, int32 Quantity = 1);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInstance)
	class UInventoryItemInstance* ItemInstance = nullptr;

	UFUNCTION()
	void OnRep_ItemInstance(UInventoryItemInstance* OldItemInstanceValue); //track when required to init on client

	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void OnDropped();

	UFUNCTION()
	void OnRep_ItemState();

	UFUNCTION()
	class AActor* GetOwningPlayerState();


	UPROPERTY(ReplicatedUsing = OnRep_ItemState)
	EItemState ItemState = EItemState::None;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USphereComponent* SphereComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(EditAnywhere, Replicated)
	int32 Quantity = 1;



	/* EVENTS */
	UFUNCTION()
		void OnPickupCollisionOverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
			 int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
		void OnPickupCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* otherActor,
			UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	/** Delegate for notification when a wake event is fired by physics*/


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category  =" Item properties")
	class UWidgetComponent* PickupTextWidget;


public:

	virtual void InitInternal();

	void ShowPickupWidget(bool bShowWidget);

private:
	UPROPERTY(EditAnywhere, Category = "Item Properties")
		bool bOverlapPicksup = false;

};
