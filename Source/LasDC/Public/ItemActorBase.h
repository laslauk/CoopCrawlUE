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

UCLASS()
class LASDC_API AItemActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActorBase();

	virtual bool ReplicateSubobjects(UActorChannel* channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;





	void Init(class UInventoryItemInstance* InInstance);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	class UInventoryItemInstance* ItemInstance = nullptr;

	UPROPERTY()
	class APlayerStateBase* OwningPlayerState;

	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void OnDropped();


	UPROPERTY(Replicated)
	TEnumAsByte<EItemState> ItemState = EItemState::None;

	UPROPERTY()
	class USphereComponent* SphereComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UFUNCTION()
		void OnPickupCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
			 int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FGameplayTag OverlapEventTag;
};
