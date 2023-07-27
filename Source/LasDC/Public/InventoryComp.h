// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InventoryList.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "FastArrayTagCounter.h"
#include "InventoryComp.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipItemActorTag)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(DropItemTag)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipNextTag)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(UnequipTag)

/*

	notes: Stacking items use gameplay FGameplayTag for COUNTING stackable items such as ammo. 
	And declaring what the item is.


*/

/* Delegate declarations */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdatedDelegate, bool, Updated);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASDC_API UInventoryComp : public UActorComponent
{
	GENERATED_BODY()

public:	
//	static FGameplayTag EquipItemActorTag;
//	static FGameplayTag DropItemTag;
//	static FGameplayTag EquipNextTag;
//	static FGameplayTag UnequipTag;


	// Sets default values for this component's properties
	UInventoryComp();

	virtual void InitializeComponent() override;


	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		AActor* GetControlledAvatar();


	UPROPERTY(ReplicatedUsing = OnRep_InventoryList)
	FInventoryList InventoryList;

	virtual bool ReplicateSubobjects(class UActorChannel* channel, class FOutBunch*, FReplicationFlags* RepFlags) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable)
	TArray<UInventoryItemInstance*> GetInventoryItemInstances();


	UFUNCTION(BlueprintCallable)
		void AddItem(TSubclassOf<class UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
		void RemoveItem(TSubclassOf<class UItemStaticData> InItemStaticDataClass);

	UFUNCTION(BlueprintCallable)
		void RemoveItemInstance(class UInventoryItemInstance* InItemInstance);



	UFUNCTION(BlueprintCallable)
		void RemoveItemWithInventoryTag(FGameplayTag InTag, int32 Count = 1);

	UFUNCTION()
	void OnRep_InventoryList();


	//TODO, parempi select system ei Class
	UFUNCTION(BlueprintCallable)
	void EquipItem(TSubclassOf<class UItemStaticData> InItemStaticDataClass);


	UFUNCTION(BlueprintCallable)
		void UnEquipItem();

	UFUNCTION(BlueprintCallable)
		class ACharacter* GetControlledCharacter() const;


	UFUNCTION(BlueprintCallable)
		void AddDefaultItem_DEBUG();

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void AddItemInstance(class UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
		void EquipItemInstance(class UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
		void EquipNext();

	virtual void GameplayEventCallback(const FGameplayEventData* payload);

	TArray<UInventoryItemInstance*> GetAllInstancesWIthTag(FGameplayTag InTag);

	UFUNCTION()
	void AddInventoryTags();

	UFUNCTION(BlueprintCallable)
	int32 GetInventoryTagCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void AddInventoryTagCount(FGameplayTag InTag, int32 Count);


	UFUNCTION(BlueprintCallable)
	class UInventoryItemInstance* GetEquippedItem() const;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentEquippedItem)
	class UInventoryItemInstance* CurrentEquippedItem = nullptr;
	

	void HandleGameplayEventInternal(FGameplayEventData payload);

	UFUNCTION(Server, Reliable)
	void ServerHandleGameplayEvent(FGameplayEventData payload);

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class UItemStaticData>> DefaultItems;


	UPROPERTY(BlueprintAssignable, Category = "InventoryComponent")
	FInventoryUpdatedDelegate InventoryUpdateDelegate;

	UFUNCTION()
		void OnRep_CurrentEquippedItem();


protected:

	/* COUNTER: Inventory tags to keep up with the counts for ammo etc.*/
	UPROPERTY(Replicated)
	FFastArrayTagCounter InventoryTags; //actually a counter for tags == items


		
};
