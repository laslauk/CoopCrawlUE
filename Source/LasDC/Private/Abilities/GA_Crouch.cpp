// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Crouch.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"


UGA_Crouch::UGA_Crouch() {
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; //ability only with c++, blueprint require object instance
}


void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//Call Unreal's implementation of Crouch and actually Crouch.
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

	//Crouch does NOT Direclty set the state, put the INTENTION to crouch
	Character->Crouch(); 

 }


void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) {


	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->UnCrouch();   //NOT Direclty set the state, put the INTENTION to crouch



	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

 }


bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const {

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) {
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);

	// UE's Character's existing canCrouch function
	if (Character->CanCrouch())
	{

	}
	return Character->CanCrouch();

 }
