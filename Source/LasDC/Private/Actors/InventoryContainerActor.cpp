// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/InventoryContainerActor.h"
#include "PlayerControllerBase.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemInstance.h"
#include "InventoryList.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CrawlGameStatics.h"
#include "PlayerStateBase.h"

#include "Inventory/InventoryContainerComponent.h"


UE_DEFINE_GAMEPLAY_TAG(OpenInventoryContainerEventTag, "Event.Inventory.OpenInventoryContainer")

// Sets default values
AInventoryContainerActor::AInventoryContainerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	InventoryContainerComponent = CreateDefaultSubobject< UInventoryContainerComponent>(TEXT("InventoryContainerComponent"));
	InventoryContainerComponent->RegisterComponent();
}

// Called when the game starts or when spawned
void AInventoryContainerActor::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	
		for (auto& ItemStaticDataPair : ItemStaticDataToContain) {

			InventoryList.AddItem(ItemStaticDataPair.Key, this);


			UInventoryItemInstance* ItemInstance = NewObject<UInventoryItemInstance>();
			ItemInstance->Init(ItemStaticDataPair.Key, this, UCrawlGameStatics::GetItemStaticData(ItemStaticDataPair.Key)->MaxStackCount);
			ItemInstancesContained.Add(ItemInstance);

			//UInventoryItemInstance* ItemInstance = NewObject<UInventoryItemInstance>();
			//ItemInstance->Init(ItemStaticDataPair.Key, nullptr, ItemStaticDataPair.Value);
		//	ItemInstancesContained.Add(ItemInstance);
		}


	
}



// Called every frame
void AInventoryContainerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<UInventoryItemInstance*> AInventoryContainerActor::GetInventoryItemInstances()
{
/*	TArray<UInventoryItemInstance*> Items;
	for (auto InventoryListItem : InventoryList.GetItemsRef()) {

		Items.Add(InventoryListItem.ItemInstance);
	}
	*/
	return ItemInstancesContained;
}

void AInventoryContainerActor::OnRep_InventoryList()
{

}

void AInventoryContainerActor::OnRep_ItemInstancesContainedUpdated()
{
	FString healthMessage = FString::Printf(TEXT("%s OnRep_ItemInstancesContainedUpdated item count: %d !!"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

}

void AInventoryContainerActor::ToggleInventoryMenu(bool NewOpen) {



}

void AInventoryContainerActor::ReactToInteractPure( APlayerControllerBase* OtherPlayerController)
{
	OpenedCharacters.Add(OtherPlayerController);

	OtherPlayerController->OpenInventoryContainerActor(this);



	if (bDebugPrintContents) {

		UE_LOG(LogTemp, Warning, TEXT("Items length: %d"), InventoryList.GetItemsRef().Num());
		
		for (auto InventoryListItem : InventoryList.GetItemsRef()) {

			UE_LOG(LogTemp, Warning, TEXT("Item: %s"), *InventoryListItem.ItemInstance->GetItemStaticData()->Name.ToString());
		}
		
	}



}

void AInventoryContainerActor::ReactToInteract_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("HELLOOOOOO - BP implemented"));
}


void AInventoryContainerActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInventoryContainerActor, ItemInstancesContained);

}

void AInventoryContainerActor::TakeAllItemsInternal(APlayerControllerBase* OtherController)
{


	TArray<UInventoryItemInstance*> ItemInstancesTemp;

	for (auto InventoryListItem : InventoryList.GetItemsRef()) {
		ItemInstancesTemp.Add(InventoryListItem.ItemInstance);
	}

	for (auto InventoryItem: ItemInstancesTemp) {
		OtherController->PlayerState_Ref->InventoryComp->AddItemInstance(InventoryItem);
	}

	for (auto InventoryListItem : InventoryList.GetItemsRef()) {
		InventoryList.RemoveItem(InventoryListItem.ItemInstance);
	}

}

void AInventoryContainerActor::HandleTakingAllItems_Implementation()
{
	ItemInstancesContained.Empty();
}

/* This is called from client to initiiate the item process */
void AInventoryContainerActor::HandleTakeAllItems(APlayerControllerBase* OtherController) {

		FString healthMessage = FString::Printf(TEXT("%s requested "), *GetFName().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (GetNetMode() == ENetMode::NM_Client) {
			SetOwner(OtherController);
		}
		HandleTakingAllItems();
		SetOwner(nullptr);

	


}

/* serverside only*/
void AInventoryContainerActor::ServerHandleInventoryTakeAll_Implementation(APlayerControllerBase* OtherController)
{
	FString healthMessage = FString::Printf(TEXT("%s SERVER HANDLE TAKE LAL ITEMS server !!"), *GetFName().ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

	TakeAllItemsInternal(OtherController);


}

