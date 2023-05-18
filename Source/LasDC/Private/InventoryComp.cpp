// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComp.h"
#include "InventoryItemInstance.h"
#include "CrawlCommonTypes.h"
#include "Net/UnrealNetwork.h"
#include "CrawlGameStatics.h"
#include "Engine/ActorChannel.h"
#include "InventoryItemInstance.h"
#include "InventoryList.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "PlayerStateBase.h"
#include "InventoryItemInstance.h"

#include "InventoryList.h"

//static / global vars?
FGameplayTag UInventoryComp::EquipItemActorTag;
FGameplayTag UInventoryComp::DropItemTag;
FGameplayTag UInventoryComp::EquipNextTag;
FGameplayTag UInventoryComp::UnequipTag;


static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"), 0, TEXT("Draws debug info about inventory") TEXT("  0: off/n") TEXT("  1: on/n"), ECVF_Cheat
);


// Sets default values for this component's properties
UInventoryComp::UInventoryComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this, &UInventoryComp::AddInventoryTags);
	  

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())) {

		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComp::EquipItemActorTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComp::DropItemTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComp::EquipNextTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComp::UnequipTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
	}



	// ...
}


// Called when the game starts
void UInventoryComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

AActor* UInventoryComp::GetControlledAvatar() {
	AActor* actor = Cast<APlayerStateBase>(GetOwner())->GetPawn();

	return actor;

}

bool UInventoryComp::ReplicateSubobjects( UActorChannel* channel,  FOutBunch* Bunch, FReplicationFlags* RepFlags) {


	bool WroteSomething = Super::ReplicateSubobjects(channel, Bunch, RepFlags);



	for (FInventoryListItem& item : InventoryList.GetItemsRef()) {

		UInventoryItemInstance* ItemInstance = item.ItemInstance;
		if (IsValid(ItemInstance)) {
			WroteSomething |= channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}



UInventoryItemInstance*  UInventoryComp::GetEquippedItem() const {
	return CurrentEquippedItem;
}

void UInventoryComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComp, InventoryList);
	DOREPLIFETIME(UInventoryComp, CurrentEquippedItem);
	
}



void UInventoryComp::InitializeComponent() {

	Super::InitializeComponent();

	//ONLY SERVER CAN MAKE CHANGES TO INVENTORY

	if (GetOwner()->HasAuthority()) {
		for (auto ItemClass : DefaultItems) {
			InventoryList.AddItem(ItemClass, GetOwner());
		}



	}
}


// Called every frame
void UInventoryComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;
	if (bShowDebug) {
	
		for (FInventoryListItem& Item : InventoryList.GetItemsRef()) {

			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();

			if (IsValid(ItemInstance) && IsValid(ItemStaticData)) {

				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item: %s,  %s"), *GetOwner()->GetName(), *ItemStaticData->Name.ToString()));
			}
		}
	}

	// ...
}


ACharacter* UInventoryComp::GetControlledCharacter() const {
	ACharacter* Character = nullptr;

/*
	if (APlayerStateBase* PS = Cast<APlayerStateBase>(GetOwner())) {
		//if (Character = PS->playerCharacterRef) {
		return PS->playerCharacterRef;
		//}
	}
	*/

	return Character;
}


void UInventoryComp::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass) {

	if (GetOwner()->HasAuthority()) {
	InventoryList.AddItem(InItemStaticDataClass, GetOwner());
	}
}



void UInventoryComp::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass) {

	if (GetOwner()->HasAuthority()) {
	InventoryList.RemoveItem(InItemStaticDataClass);
}
}



void UInventoryComp::AddDefaultItem_DEBUG() {

	EquipItem(InventoryList.GetItemsRef()[0].ItemInstance->ItemStaticDataClass);

	
}

void UInventoryComp::DropItem() {

	if (GetOwner()->HasAuthority())
	{

		if (IsValid(CurrentEquippedItem)) {
			CurrentEquippedItem->OnDropped();
			RemoveItem(CurrentEquippedItem->ItemStaticDataClass);
			CurrentEquippedItem = nullptr;
		}
	}

}


void UInventoryComp::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass) {


	if (GetOwner()->HasAuthority()) {
		for (auto Item : InventoryList.GetItemsRef()) {

			if (Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass) {
				AActor* actor = Cast<APlayerStateBase>(GetOwner())->GetPawn();
				if (!actor) {
					UE_LOG(LogTemp, Warning, TEXT("NULLPTR ACTOR"));
						return;
				}
			
				Item.ItemInstance->OnEquipped(actor);
				CurrentEquippedItem = Item.ItemInstance;
				break;
			}
		
		}
	}


}


void UInventoryComp::UnEquipItem() {

	if (GetOwner()->HasAuthority()) {
		if (IsValid(CurrentEquippedItem)) {

			CurrentEquippedItem->OnUnequipped();
			CurrentEquippedItem = nullptr;
		}
	}

}



void UInventoryComp::AddItemInstance(UInventoryItemInstance* InItemInstance) {

	if (GetOwner()->HasAuthority()) {
		InventoryList.AddItem(InItemInstance);
	}

}

void UInventoryComp::EquipItemInstance(UInventoryItemInstance* InItemInstance) {
	if (GetOwner()->HasAuthority()) {
		for (auto item : InventoryList.GetItemsRef()) {

			if (item.ItemInstance == InItemInstance) {


				item.ItemInstance->OnEquipped(GetControlledAvatar());

			}

		}
	}
}


void UInventoryComp::EquipNext() {

	TArray<FInventoryListItem>& items = InventoryList.GetItemsRef();

	const bool bNoItems = items.Num() == 0;
	const bool bOneAndEquipped = items.Num() == 1 && CurrentEquippedItem;

	if (bNoItems || bOneAndEquipped) return;

	UInventoryItemInstance* TargetItem = CurrentEquippedItem;

	for (auto item : items) {

		if (item.ItemInstance->GetItemStaticData()) {

			if (item.ItemInstance != CurrentEquippedItem) {

				TargetItem = item.ItemInstance;
				break;
			}

		}
	}

	if (CurrentEquippedItem) {

		if (TargetItem == CurrentEquippedItem) {
			return;
		}

		UnEquipItem();

	}

	EquipItemInstance(TargetItem);
}




void UInventoryComp::GameplayEventCallback(const FGameplayEventData* payload) {

	ENetRole NetRole = GetOwnerRole();
	if (NetRole == ROLE_Authority) {

		HandleGameplayEventInternal(*payload);
	}
	else if (NetRole == ROLE_AutonomousProxy) {  //autonomys proxy 

		ServerHandleGameplayEvent(*payload);

	}





}







void UInventoryComp::AddInventoryTags() {
	UGameplayTagsManager& tagsManager = UGameplayTagsManager::Get();
	UInventoryComp::EquipItemActorTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Equip item from actro event"));
	UInventoryComp::DropItemTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"), TEXT("drop item "));
	UInventoryComp::EquipNextTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"), TEXT("equip enext"));
	UInventoryComp::UnequipTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.Unequip"), TEXT("uneqiup itme"));

	tagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
}



void UInventoryComp::HandleGameplayEventInternal(FGameplayEventData payload) {


	ENetRole NetRole = GetOwnerRole();
	if (NetRole == ROLE_Authority) {

		FGameplayTag eventTag = payload.EventTag;

		if (eventTag == UInventoryComp::EquipItemActorTag) {
			if (const UInventoryItemInstance* iteminstance = Cast<UInventoryIteminstance>(payload.OptionalObject)) {
				AddItemInstance(const_cast<UInventoryItemInstance*>(iteminstance));

				if (payload.Instigator) {
					const_cast<AActor*>(payload.Instigator)->Destroy();

				}
			}
		}
		else if (eventTag == UInventoryComp::EquipNextTag) {
			EquipNext();
		}
		else if (eventTag == UInventoryComp::DropItemTag) {
			DropItem();
		}
		else if (eventTag == UInventoryComp::EquipItemActorTag) {

			UnEquipItem();

		}
	
	}

}

void UInventoryComp::ServerHandleGameplayEvent(FGameplayEventData payload) {

	HandleGameplayEventInternal(payload);

}