// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_InventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "InventoryComp.h"
#include "Inventory/Items/WeaponItemActor.h"




#include "PlayerStateBase.h"
#include "PlayerControllerBase.h"


#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"


 //Traces, line,sphere
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "CharacterBaseGAS.h"
#include "CrawlCommonTypes.h"



bool UGA_InventoryCombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{

	if (UseAmmo) {
		return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
	} else {
		return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
	}

}

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult) {

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData()) {

			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle OutSpec = ASC->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -(WeaponStaticData->BaseDamage));
		
			return OutSpec;
		}


	}
	return FGameplayEffectSpecHandle();
}



const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult)
{


	/* Setup */

	FHitResult FocusHit;
	TArray<AActor*> ActorsToIgnore = { GetAvatarActorFromActorInfo() };
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();
	ACharacterBaseGAS* Character = GetCharacterBaseFromActorInfo();

	const FTransform& CameraTransform = Character->GetFollowCamera()->GetComponentTransform();
	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;



	/* Perform Trace */

	//
	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, FocusHit, true);

	if (!WeaponItemActor) {
		UE_LOG(LogTemp, Warning, TEXT("--------- NULL: WeaponItemActor on GetWeaponToFocusTraceResult "));
		return false;
	}
	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;
	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true);

	

	return OutHitResult.bBlockingHit;

}

bool UGA_InventoryCombatAbility::HasEnoughAmmo() const 
{

	//get static data for wepaon ammunition tag so we can request for inventory with this tag how many we got

	if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
	{
		if (UInventoryComp* InventoryComponent = GetInventoryComponent()) 
		{
			//Inventory tag count can be used for everything with counts se 
			return !WeaponStaticData->AmmoTag.IsValid() || InventoryComponent->GetInventoryTagCount(WeaponStaticData->AmmoTag) > 0;
		}
	}

	return false;
}

void UGA_InventoryCombatAbility::DecreaseAmmo()
{

	if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
	{
		if (!WeaponStaticData->AmmoTag.IsValid()) return;

		if (UInventoryComp* Inventory = GetInventoryComponent()) {

			Inventory->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
		}

	}

}

FTransform UGA_InventoryCombatAbility::GetFireDirection(const FHitResult& TraceHitResult) {
	APlayerStateBase* OwningPlayerState = Cast<APlayerStateBase>(GetOwningActorFromActorInfo());

	if (OwningPlayerState) {

		AWeaponItemActor* weapon = Cast<AWeaponItemActor>(GetEquippedItemActor());
		if (weapon)
		{
			return weapon->GetFireDirection(TraceHitResult);
		}
	}
	return FTransform();

}

void UGA_InventoryCombatAbility::TraceUnderCrosshairs(FHitResult& TraceHitResult, FTransform& HitDirectionRotation)
{

	APlayerStateBase* OwningPlayerState = Cast<APlayerStateBase>(GetOwningActorFromActorInfo());
	if (OwningPlayerState) {
	
		Cast<APlayerControllerBase>(OwningPlayerState->GetPlayerController())->TraceUnderCrossHair(TraceHitResult);

		AWeaponItemActor* weapon = Cast<AWeaponItemActor>(GetEquippedItemActor());
	
		if (weapon)
		{
			HitDirectionRotation = weapon->GetFireDirection(TraceHitResult);
		}
	}
}


