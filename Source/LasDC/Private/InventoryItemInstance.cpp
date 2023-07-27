// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"
#include "CrawlGameStatics.h"
#include "CrawlCommonTypes.h"
#include "Character/CharacterBaseGAS.h"
#include "AbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ItemActorBase.h"
#include "InventoryList.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::OnRep_Equipped() {
	//server spawns, do additional stuff, static mesh can replicate out of box
	//do stuff that not out of box
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass, AActor* owner, int32 InQuantity) {
	
	OwningPlayerState = owner;
	Quantity = InQuantity;

	UE_LOG(LogTemp, Warning, TEXT("<<<<<<<<This item: %s, inited with ammo count: %d"), *GetName(), Quantity);
	ItemStaticDataClass = InItemStaticDataClass;

}



const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const{

	return (UCrawlGameStatics::GetItemStaticData(ItemStaticDataClass));


 }

AItemActorBase* UInventoryItemInstance::GetItemActor() const {


	return ItemActor;
}



void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped)
	DOREPLIFETIME(UInventoryItemInstance, ItemActor)
	DOREPLIFETIME(UInventoryItemInstance, Quantity)

}

void UInventoryItemInstance::OnDropped(AActor* InOwner) {
	if (ItemActor) {

		ItemActor->OnDropped();
		OwningPlayerState = nullptr;
		bEquipped = false;

		TryRemoveAbilities(InOwner);
		TryRemoveEffects(InOwner);
	}
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner) {

	//lets just spawn item for some reason

	if (!InOwner) return;
	//spawn itm

	if (UWorld* World = InOwner->GetWorld()) {

		const UItemStaticData* StaticData = GetItemStaticData();

		if (StaticData) {
			FTransform Transform;
			ItemActor = World->SpawnActorDeferred<AItemActorBase>(StaticData->ItemActorClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			ItemActor->Init(this);
			ItemActor->OnEquipped();
			ItemActor->FinishSpawning(Transform);
			if (IsValid(ItemActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!! Spawnattiin item actor: %s "), *ItemActor->GetName());
			}

			if (!(StaticData->ItemActorClass))
			{
				UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!! ON EQUIP ITEM ACTOR CLASS EMPTY IN INVENTORY ITEM INSTANCE"));
			}

			if( ACharacter* Character = Cast <ACharacter>(InOwner))
			{

				if (USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
				{
					ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->WeaponAttachmentSocketName);
					UE_LOG(LogTemp, Warning, TEXT("Succesfulyl attached item for itemactor: %s"), *ItemActor->GetName());
				}
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!!!!!!!!!!!!!!!!!! EI STATIC DATA EQUIP ITEM INSTANCE"));
		}
		TryGrantAbilities(InOwner);
		TryApplyEffects(InOwner);
	}
	

	bEquipped = true;


 }


void UInventoryItemInstance::OnUnequipped(AActor* InOwner) {

	if (IsValid(ItemActor)) {

		ItemActor->Destroy();
		ItemActor = nullptr;

	}
	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
	bEquipped = false;
 }


void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner) {

	if (InOwner && InOwner->HasAuthority()) {
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {
			const UItemStaticData* StaticData = GetItemStaticData();
			for (auto AbilityHandle : GrantedAbilityHandles) {
				AbilityComponent->ClearAbility(AbilityHandle);
			}

			GrantedAbilityHandles.Empty();
		}
	}





}


void UInventoryItemInstance::AddItems(int32 Count)
{
	Quantity += Count;

	if (Quantity < 0) {
		Quantity = 0;
	}


}



void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner) {

	
	if (InOwner && InOwner->HasAuthority()) {
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {
			const UItemStaticData* StaticData = GetItemStaticData();
			for (auto ItemAbility : StaticData->GrantedAbilities) {
				GrantedAbilityHandles.Add(AbilityComponent->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
			}
		}
	}
	
}


/* add Effects on equip */
void UInventoryItemInstance::TryApplyEffects(AActor* InOwner) {

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {

		//item static datassa on effektit mitä lisätään owneriin ku equippataan
		const UItemStaticData* ItemStaticData = GetItemStaticData();


		/* The GameplayEffectContextHandle tells us who created this GameplayEffectSpec. */
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();

		for (TSubclassOf<UGameplayEffect> GameplayEffect : ItemStaticData->OngoingEffects) {
				
			if (GameplayEffect.Get() == nullptr) continue; //tyhjä effekti

			//Tehään handle specille, speco handle nn iiku isntance handle
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

			if (SpecHandle.IsValid()) {
				//Applytaan effekti ja otetaan activeGEhandle talteen myöhempää vemppaamista varten
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				if(ActiveGEHandle.WasSuccessfullyApplied()) {
					OnGoingItemAddedEffectHandles.Add(ActiveGEHandle);
				}
			}
		}
	}
}


void UInventoryItemInstance::TryRemoveEffects(AActor* inOwner) {

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(inOwner)) {
		for (FActiveGameplayEffectHandle ActiveEffectHandle : OnGoingItemAddedEffectHandles) {
			if (ActiveEffectHandle.IsValid()) {
				ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
		}
	}

	OnGoingItemAddedEffectHandles.Empty();

}
