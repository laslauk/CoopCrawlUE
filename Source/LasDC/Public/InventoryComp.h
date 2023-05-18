// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryList.h"
#include "CrawlCommonTypes.h"
#include "InventoryComp.generated.h"





UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASDC_API UInventoryComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComp();

	virtual void InitializeComponent() override;


	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		AActor* GetControlledAvatar();


	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	virtual bool ReplicateSubobjects(class UActorChannel* channel, class FOutBunch*, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;



	UFUNCTION(BlueprintCallable)
		void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
		void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);


	//TODO, parempi select system ei Class
	UFUNCTION(BlueprintCallable)
		void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);


	UFUNCTION(BlueprintCallable)
		void UnEquipItem();

	UFUNCTION(BlueprintCallable)
		class ACharacter* GetControlledCharacter() const;


	UFUNCTION(BlueprintCallable)
		void AddDefaultItem_DEBUG();

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void AddItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
		void EquipItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
		void EquipNext();

	virtual void GameplayEventCallback(const FGameplayEventData* payload);

	static FGameplayTag EquipItemActorTag;
	static FGameplayTag DropItemTag;
	static FGameplayTag EquipNextTag;
	static FGameplayTag UnequipTag;

	UFUNCTION()

		void AddInventoryTags();

	UFUNCTION(BlueprintCallable)
		UInventoryItemInstance* GetEquippedItem() const;

	UPROPERTY(VisibleAnywhere, Replicated)
		UInventoryItemInstance* CurrentEquippedItem = nullptr;
	

	void HandleGameplayEventInternal(FGameplayEventData payload);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(FGameplayEventData payload);

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class UItemStaticData>> DefaultItems;
		
};
