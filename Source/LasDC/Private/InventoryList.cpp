// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryList.h"
#include "InventoryItemInstance.h"
#include "CrawlCommonTypes.h"
#include "CrawlGameStatics.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass, AActor* owner) {

	//new item INSTANCE
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = NewObject<UInventoryItemInstance>();
	Item.ItemInstance->Init(InItemStaticDataClass, owner, UCrawlGameStatics::GetItemStaticData(InItemStaticDataClass)->MaxStackCount);


	//IMPORTANT, ARRAYS WORKING, MARK DIRTY
	MarkItemDirty(Item);


}

// Ei käytetä
void FInventoryList::AddItem(UInventoryItemInstance* InItemInstance) {


	//new item INSTANCE
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = InItemInstance;
	

	//IMPORTANT, ARRAYS WORKING, MARK DIRTY
	MarkItemDirty(Item);


}




void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass) {


	
	for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter) {
		FInventoryListItem Item = *ItemIter;

		if ((Item.ItemInstance)) {
		//	UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>> HOMO :DDDDDDDDDDD %s"), *Item.ItemInstance->GetName());
		}

		if ((Item.ItemInstance->ItemStaticDataClass)) {
		//	UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>> HOMO STATIC DATA :DDDDDDDDDDD %s"), *Item.ItemInstance->ItemStaticDataClass.GetDefaultObject()->Name.ToString());
		}

		if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass)) {
		//	UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>> REMOVED FROM ARRAY <<<<<<<<<<<<"));
			ItemIter.RemoveCurrent();
			MarkArrayDirty();
	
			break;
		}
	}
	
}

void FInventoryList::RemoveItem(UInventoryItemInstance* InItemInstance)
{
	for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter) {

		UE_LOG(LogTemp, Warning, TEXT(": REMOVED ITREM FROM INVENTORYLIST: %s"), *ItemIter->ItemInstance->GetItemStaticData()->Name.ToString());

		FString msg = FString::Printf(TEXT(": REMOVED ITREM FROM INVENTORYLIST: %s"), *ItemIter->ItemInstance->GetItemStaticData()->Name.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, msg);

		FInventoryListItem& Item = *ItemIter;
		if (IsValid(InItemInstance) && InItemInstance == Item.ItemInstance) {


			ItemIter.RemoveCurrent();
			MarkArrayDirty();
			break;

		}

	}
	
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllInstancesWithTag(FGameplayTag InTag)
{
	TArray<UInventoryItemInstance*> OutInstances;
	for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;
		if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->InventoryTags.Contains(InTag))
		{
			OutInstances.Add(Item.ItemInstance);
		}
	}
	return OutInstances;
}



TArray<UInventoryItemInstance*> FInventoryList::GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	TArray<UInventoryItemInstance*> OutInstances;
	for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;
		if (!Item.ItemInstance)
		{
			continue;
		}
		const UItemStaticData* ItemData = Item.ItemInstance->GetItemStaticData();
		const bool bSameType = ItemData->IsA(InItemStaticDataClass);
		const bool bHasCapacity = ItemData->MaxStackCount > Item.ItemInstance->GetQuantity();
		if (bSameType && bHasCapacity)
		{
			OutInstances.Add(Item.ItemInstance);
		}
	}
	return OutInstances;
}
