// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_InventoryAbility.h"
#include "InventoryComp.h"
#include "CrawlCommonTypes.h"
#include "ItemActorBase.h"
#include "InventoryItemInstance.h"

#include "Inventory/Items/WeaponItemActor.h"

 void UGA_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) {
	 Super::OnGiveAbility(ActorInfo, Spec);

	 //abilitiy instanced from actor so has inventory component,
	 // IF inventory compo is given runtime, get on activate ability function

	 InventoryComp = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComp>();
	 UE_LOG(LogTemp, Warning, TEXT("<<<<<<<<OnGiveAiblity: owner actor name : %s"), *InventoryComp->GetName());
}



 UInventoryComp* UGA_InventoryAbility::GetInventoryComponent()  const {
	
	 if (!InventoryComp) {
		 return  GetOwningActorFromActorInfo()->FindComponentByClass<UInventoryComp>();
		
	 }
	return InventoryComp;
}


UInventoryItemInstance* UGA_InventoryAbility::GetEquippedItemInstance() const {
	
	return GetInventoryComponent() ? GetInventoryComponent()->GetEquippedItem() : nullptr;
}



const UItemStaticData* UGA_InventoryAbility::GetEquippedItemStaticData() const  {


	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();



	if (!EquippedItem) {
		UE_LOG(LogTemp, Warning, TEXT("<<<<<<<<GetEquippedItemStaticData()  GetEquippedItemInstance------ NULL"));
	}
	return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
}




const UWeaponStaticData* UGA_InventoryAbility::GetEquippedItemWeaponStaticData() const  {


	if (!GetEquippedItemStaticData()) {
		UE_LOG(LogTemp, Warning, TEXT("<<<<<<<<GetEquippedItemWeaponStaticData() ------ NULL"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("<<<<<<<<GetEquippedItemWeaponStaticData() ------ %s"), *(GetEquippedItemStaticData()->Name.ToString()));
	}
	

	return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}



AItemActorBase* UGA_InventoryAbility::GetEquippedItemActor()  {
	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance(); //tää löytyy

	return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}




AWeaponItemActor* UGA_InventoryAbility::GetEquippedWeaponItemActor()  {

	if (GetEquippedItemActor() == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT(" --------  UGA_InventoryAbility::GetEquippedWeaponItemActor() NULL !!!!"));
	}
	return Cast<AWeaponItemActor>(GetEquippedItemInstance()->GetItemActor());
}

