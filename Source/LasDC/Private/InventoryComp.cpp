// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComp.h"
#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "CrawlGameStatics.h"
#include "Engine/ActorChannel.h"
#include "InventoryList.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "InventoryItemInstance.h"

#include "PlayerStateBase.h"
#include "Components/ActorComponent.h"
#include "ItemActorBase.h"
#include "CrawlCommonTypes.h"

//static / global vars?
// 
	//DEPCRIATED, KORVATTU MACROILLA TOP OF THE FILE
//FGameplayTag UInventoryComp::EquipItemActorTag;
//FGameplayTag UInventoryComp::DropItemTag;
//FGameplayTag UInventoryComp::EquipNextTag;
//FGameplayTag UInventoryComp::UnequipTag;

UE_DEFINE_GAMEPLAY_TAG(EquipItemActorTag, "Event.Inventory.EquipItemActor")
UE_DEFINE_GAMEPLAY_TAG(DropItemTag, "Event.Inventory.DropItem")
UE_DEFINE_GAMEPLAY_TAG(EquipNextTag, "Event.Inventory.EquipNext")
UE_DEFINE_GAMEPLAY_TAG(UnequipTag, "Event.Inventory.Unequip")



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

	// ...
}


// Called when the game starts
void UInventoryComp::BeginPlay()
{
	Super::BeginPlay();


	// ...
	
}

AActor* UInventoryComp::GetControlledAvatar() {
	return  Cast<APlayerStateBase>(GetOwner())->GetPawn();

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
	DOREPLIFETIME(UInventoryComp, InventoryTags);
}



void UInventoryComp::InitializeComponent() {

	Super::InitializeComponent();

	//ONLY SERVER CAN MAKE CHANGES TO INVENTORY

	if (GetOwner()->HasAuthority()) {
		for (auto ItemClass : DefaultItems) {
			InventoryList.AddItem(ItemClass, GetOwner());
		}

	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())) {

		ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipItemActorTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(DropItemTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipNextTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(UnequipTag).AddUObject(this, &UInventoryComp::GameplayEventCallback);
	}


}


// Called every frame
void UInventoryComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	/* DEBUG */
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
	/* DEBUH END */

	 const TArray<FFastArrayTagCounterRecord>& InventoryTagArray = InventoryTags.GetTagArray();


	for (const auto& TagRecord : InventoryTagArray) {

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Purple, FString::Printf(TEXT("Tag: %s, %d"), *TagRecord.Tag.ToString(), TagRecord.Count));
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

			if (!(CurrentEquippedItem->ItemStaticDataClass)) {
				UE_LOG(LogTemp, Warning, TEXT("DROPITEM----------NO ITEM STATIC DATA CLASS"));
				return;
			}
	
				RemoveItem(CurrentEquippedItem->ItemStaticDataClass);
				CurrentEquippedItem->OnDropped(GetControlledAvatar());
				CurrentEquippedItem = nullptr;
		}
	}

}


void UInventoryComp::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass) {


	if (GetOwner()->HasAuthority()) {
		for (auto Item : InventoryList.GetItemsRef()) {

			if (Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass) {
				AActor* actor = GetControlledAvatar();
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

			CurrentEquippedItem->OnUnequipped(GetControlledAvatar());
			CurrentEquippedItem = nullptr;
		}
	}



}



void UInventoryComp::AddItemInstance(UInventoryItemInstance* InItemInstance) {

	
		if (!GetOwner()->HasAuthority())
		{
			return;
		}


		TArray<UInventoryItemInstance*> Items = InventoryList.GetAllAvailableInstancesOfType(InItemInstance->ItemStaticDataClass);

		Algo::Sort(Items, [](auto* InA, auto* InB)
			{
				return InA->GetQuantity() < InB->GetQuantity();
			});

		const int32 MaxItemStackCount = InItemInstance->GetItemStaticData()->MaxStackCount;
		int32 ItemsLeft = InItemInstance->GetQuantity();

	

		for (auto* Item : Items)
		{
			const int32 EmptySlots = MaxItemStackCount - Item->GetQuantity();
			const int32 SlotsToAdd = FMath::Max(EmptySlots, ItemsLeft);
			ItemsLeft -= SlotsToAdd;
			Item->AddItems(SlotsToAdd);
			InItemInstance->AddItems(-SlotsToAdd);


			for (FGameplayTag InventoryTag : Item->GetItemStaticData()->InventoryTags)
			{
			
				InventoryTags.AddTagCount(InventoryTag, SlotsToAdd);
				//UE_LOG(LogTemp, Warning, TEXT(">>>Nyt InventoryTags koko on:   --  %d "), SlotsToAdd);
			}

			if (ItemsLeft <= 0)
			{

				return;
			}
		}

		while (ItemsLeft > MaxItemStackCount)
		{
			AddItem(InItemInstance->GetItemStaticData()->GetClass());
			for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
			{
				InventoryTags.AddTagCount(InventoryTag, MaxItemStackCount);
			}
			ItemsLeft -= MaxItemStackCount;
			InItemInstance->AddItems(-MaxItemStackCount);
		}

		InventoryList.AddItem(InItemInstance);
		for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
		{
			InventoryTags.AddTagCount(InventoryTag, InItemInstance->GetQuantity());
		}
	


}

void UInventoryComp::EquipItemInstance(UInventoryItemInstance* InItemInstance) {
	if (GetOwner()->HasAuthority()) {
		for (auto item : InventoryList.GetItemsRef()) {

			if (item.ItemInstance == InItemInstance) {


				item.ItemInstance->OnEquipped(GetControlledAvatar());
				CurrentEquippedItem = item.ItemInstance;

			

				
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
		//oli alunperin tässä??? todo what even
		ServerHandleGameplayEvent(*payload);

	}
	else if (NetRole == ROLE_SimulatedProxy) {

		ServerHandleGameplayEvent(*payload);
		
	}






}


void UInventoryComp::AddInventoryTags() {
	UGameplayTagsManager& tagsManager = UGameplayTagsManager::Get();



	//DEPCRIATED, KORVATTU MACROILLA TOP OF THE FILE
//	UInventoryComp::EquipItemActorTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Equip item from actro event"));
//	UInventoryComp::DropItemTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"), TEXT("drop item "));
//	UInventoryComp::EquipNextTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"), TEXT("equip enext"));
//	UInventoryComp::UnequipTag = tagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.Unequip"), TEXT("uneqiup itme"));

	tagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
}



void UInventoryComp::HandleGameplayEventInternal(FGameplayEventData payload) {



	if (!GetOwner()->HasAuthority())
	{
		return;
	}


	FGameplayTag eventTag = payload.EventTag;

		if (eventTag == EquipItemActorTag) {

			if ( UInventoryItemInstance* iteminstance = Cast<UInventoryItemInstance>(payload.OptionalObject)) {

				AddItemInstance(const_cast<UInventoryItemInstance*>(iteminstance));
				iteminstance->OwningPlayerState = GetOwner();

				if (payload.Instigator) {
					const_cast<AActor*>(payload.Instigator.Get())->Destroy();

				}
			}
		}


		else if (eventTag == EquipNextTag) {
	
			EquipNext();
		}
		else if (eventTag ==DropItemTag) {
		
			DropItem();
		}
		else if (eventTag ==UnequipTag) {
		
			UnEquipItem();

		}
		
		//tää on vaa servulle
		InventoryUpdateDelegate.Broadcast(true);

}

void UInventoryComp::OnRep_CurrentEquippedItem()
{
	InventoryUpdateDelegate.Broadcast(true);
}

void UInventoryComp::ServerHandleGameplayEvent_Implementation(FGameplayEventData payload) {

	HandleGameplayEventInternal(payload);

}

int32 UInventoryComp::GetInventoryTagCount(FGameplayTag Tag) const
{
	return InventoryTags.GetTagCount(Tag);
}


void UInventoryComp::RemoveItemInstance(UInventoryItemInstance* InItemInstance)
{
	if (GetOwner()->HasAuthority()) {

		InventoryList.RemoveItem(InItemInstance);

		for (FGameplayTag InvTag : InItemInstance->GetItemStaticData()->InventoryTags) {

			InventoryTags.AddTagCount(InvTag, -InItemInstance->GetQuantity());

		}


	}

}

void UInventoryComp::RemoveItemWithInventoryTag(FGameplayTag InTag, int32 Count)
{
	if (GetOwner()->HasAuthority()) {

		int32 CountLeft = Count;

		TArray<UInventoryItemInstance*> Items = GetAllInstancesWIthTag(InTag);

		Algo::Sort(Items,
			[](UInventoryItemInstance* InA, UInventoryItemInstance* InB)
			{
				return InA->GetQuantity() < InB->GetQuantity();
			});

		for (auto Item : Items) {

			int32 AvailableCount = Item->GetQuantity();
			int32 ItemsToRemove = CountLeft;

			if (ItemsToRemove >= AvailableCount) {
				RemoveItemInstance(Item);
			}
			else {

				Item->AddItems(-ItemsToRemove);
				for (FGameplayTag InvTag : Item->GetItemStaticData()->InventoryTags) {


					InventoryTags.AddTagCount(InvTag, -ItemsToRemove);
				}
			}
			CountLeft -= ItemsToRemove;
		}

	}
	InventoryUpdateDelegate.Broadcast(true);
}
void UInventoryComp::OnRep_InventoryList()
{
	InventoryUpdateDelegate.Broadcast(true);
}

TArray<UInventoryItemInstance*>  UInventoryComp::GetAllInstancesWIthTag(FGameplayTag InTag) {

	return InventoryList.GetAllInstancesWithTag(InTag);

}


void UInventoryComp::AddInventoryTagCount(FGameplayTag Tag, int32 Count)
{
	InventoryTags.AddTagCount(Tag, Count);
}

TArray<UInventoryItemInstance*> UInventoryComp::GetInventoryItemInstances()
{
	TArray<UInventoryItemInstance*> Items;

	for (auto Item : InventoryList.GetItemsRef()) {
		Items.Add(Item.ItemInstance);
	}

	return Items;
}
