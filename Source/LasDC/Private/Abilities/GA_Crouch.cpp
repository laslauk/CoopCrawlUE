// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Crouch.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"


UGA_Crouch::UGA_Crouch() {
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; //ability only with c++, blueprint require object instance
}

	/** Actually activate ability, do not call this directly */
void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//here we actually call crouch, unreals implementation ready character whatever
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->Crouch(); //NOT Direclty set the state, put the INTENTION to crouch

	UE_LOG(LogTemp, Warning, TEXT("CROUCH ACTIVATE ABILITY"));

 }

/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) {


	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->UnCrouch();   //NOT Direclty set the state, put the INTENTION to crouch



	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

 }



//check if can activate ability
bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const {

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) {
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);

	//use unreal character's existing canCrouch function
	if (Character->CanCrouch()) {

	}
	return Character->CanCrouch();

 }
