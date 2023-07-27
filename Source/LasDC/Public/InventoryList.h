// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryList.generated.h"
//fast array initialized
class UInventoryIteminstance;
class UItemStaticData;


USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem {

	GENERATED_BODY()


public:
	
	//inventory record is managing this instance
	UPROPERTY()
	class UInventoryItemInstance* ItemInstance = nullptr;

};



USTRUCT(BluePrintType)
struct FInventoryList : public FFastArraySerializer {

	GENERATED_BODY()

		bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams) 
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>(Items, DeltaParams, *this);
	}

	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass, AActor* owner);
	void AddItem(UInventoryItemInstance* InItemInstance);
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	void RemoveItem(UInventoryItemInstance* InItemInstance);

	TArray<FInventoryListItem>& GetItemsRef() { return Items; }
	TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag InTag);
	TArray< UInventoryItemInstance*> GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticData); //return all available instances, 

protected:


	UPROPERTY()
	TArray<FInventoryListItem> Items;
};


template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum {WithNetDeltaSerializer = true};
};

