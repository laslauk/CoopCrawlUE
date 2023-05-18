// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"
#include "CrawlGameStatics.h"
#include "CrawlCommonTypes.h"
#include "Character/CharacterBaseGAS.h"

#include "ItemActorBase.h"
#include "InventoryList.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::OnRep_Equipped() {
	//server spawns, do additional stuff, static mesh can replicate out of box
	//do stuff that not out of box
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass, AActor* owner) {
	
	OwningPlayerState = owner;
	ItemStaticDataClass = InItemStaticDataClass;
	
	/*
	UItemStaticData* data = ItemStaticDataClass.GetDefaultObject();
	if (data) {
		UE_LOG(LogTemp, Warning, TEXT("AFTER inventory instacne static data: %s"), *data->Name.ToString());

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("EI SAATU EES DEFAULT OBJ "));
	}
	*/

}



const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const{

	return UCrawlGameStatics::GetItemStaticData(ItemStaticDataClass);


 }



void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped)
	DOREPLIFETIME(UInventoryItemInstance, ItemActor)
		

}

void UInventoryItemInstance::OnDropped() {
	if (ItemActor) {
		ItemActor->OnDropped();
		OwningPlayerState = nullptr;
		bEquipped = false;
	}
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner) {

	//lets just spawn item for some reason


	//spawn itm

	if (UWorld* World = InOwner->GetWorld()) {
		if (InOwner) {
			UE_LOG(LogTemp, Warning, TEXT("Equiping item for actor: %s"), *InOwner->GetName());
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FAILED ONEQUIP"));
			return;
		}


		const UItemStaticData* StaticData = GetItemStaticData();

		if (StaticData) {

		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActorBase>(StaticData->ItemActorClass, Transform);
		ItemActor->Init(this);
		ItemActor->OnEquipped();

		//Check for skeletal mesh


		ItemActor->FinishSpawning(Transform);
		ACharacter* Character = Cast <ACharacter>(InOwner);

		if (USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr) {

			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->WeaponAttachmentSocketName);
			UE_LOG(LogTemp, Warning, TEXT("Succesfulyl attached item for itemactor: %s"), *ItemActor->GetName());

		
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("EI SAAT UWRODL"));
	}

	bEquipped = true;


 }


void UInventoryItemInstance::OnUnequipped() {

	if (ItemActor) {

		ItemActor->Destroy();
		ItemActor = nullptr;

	}
	bEquipped = false;
 }
