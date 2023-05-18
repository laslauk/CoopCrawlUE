// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryList.h"
#include "InventoryItemInstance.h"
#include "CrawlCommonTypes.h"
#include "CrawlGameStatics.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass, AActor* owner) {

	//new item INSTANCE
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = NewObject<UInventoryItemInstance>();
	Item.ItemInstance->Init(InItemStaticDataClass, owner);


	//IMPORTANT, ARRAYS WORKING, MARK DIRTY
	MarkItemDirty(Item);


}
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

		if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass)) {
			ItemIter.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}

}