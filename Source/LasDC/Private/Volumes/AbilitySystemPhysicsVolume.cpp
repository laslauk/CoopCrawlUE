// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/AbilitySystemPhysicsVolume.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"



AAbilitySystemPhysicsVolume::AAbilitySystemPhysicsVolume() {

	PrimaryActorTick.bCanEverTick = true;


}


void AAbilitySystemPhysicsVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	//Check for server authority, not on client. Skip if client
	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other)) {


		/* Give Permanent Abilities to the entering actor*/
		for (auto Ability : PermanentAbilitiesToGiveOnEnter) {
			ASC->GiveAbility(FGameplayAbilitySpec(Ability));

		}

		//Add entered actor to the map of this volume for bookkeepiing and handles
		EnteredActorsInfoMap.Add(Other);


		/* Give Temporary abilities to the entering actor */
		for (auto Ability : OnGoingAbilitiesToGiveOnEnter) {
			FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(Ability));
			EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilityHandle);
		}


		/* Apply Temporal Effects when entering the volume, save GE handle to the struct */
		for (auto GameplayEffect : OnGoingEffectsToApply) {

			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddInstigator(Other, Other);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			
			if (SpecHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (ActiveGEHandle.WasSuccessfullyApplied()) {
					EnteredActorsInfoMap[Other].AppliedEffects.Add(ActiveGEHandle);
				}
			}
		}
		

		/* Send Gameplayevent to the actor that has entered the volume with given Tags to activate wanted Gameplay Abiltiy */
		for (auto EventTag : GameplayEventTagsToSendOnEnter) {
			FGameplayEventData EventPayload;
			EventPayload.EventTag = EventTag;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
		}

	}
}

void AAbilitySystemPhysicsVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);


	if (!HasAuthority()) return;


	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other)) {

		if (EnteredActorsInfoMap.Find(Other))
		{
			/* Remove effects from actor  given by this volume */
			for (auto GameplayEffectHandle : EnteredActorsInfoMap[Other].AppliedEffects)
			{

				ASC->RemoveActiveGameplayEffect(GameplayEffectHandle);
			}

			/* Remoe abilities aswell */
			for (auto GameplayAbilityHandle : EnteredActorsInfoMap[Other].AppliedAbilities) 
			{
				ASC->ClearAbility(GameplayAbilityHandle);
			}
			EnteredActorsInfoMap.Remove(Other);
		}
	



		/* Apply Temporal Effects when entering the volume, save GE handle to the struct */
		for (auto GameplayEffect : OnExitEffectsToApply) {

			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddInstigator(Other, Other);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

			if (SpecHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}


		for (auto EventTag : GameplayEventTagsToSendOnExit) {
			FGameplayEventData EventPayload;
			EventPayload.EventTag = EventTag;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
		}

	}

}

void AAbilitySystemPhysicsVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDrawDebug) {
		DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0, 0, 5);
	}
}
