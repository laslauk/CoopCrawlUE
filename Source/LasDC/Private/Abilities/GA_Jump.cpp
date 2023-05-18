// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GA_Jump.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UGA_Jump::UGA_Jump() {
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

/** Returns true if this ability can be activated right now. Has no side effects */
 bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	 const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	 OUT FGameplayTagContainer* OptionalRelevantTags) const
{

	 if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) {
		 return false;
	 }

	 const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	 return Character->CanJump();

	

 }


/** Actually activate ability, do not call this directly */
void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) {

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) {

		if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
			return;
		}


		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

		 ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		 Character->Jump();

	
		/*
		if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get()) {
			FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

			FGameplayEffectSpecHandle specHandle = AbilityComponent->MakeOutgoingSpec(JumpEffect, 1, EffectContext);

			if (specHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());

				if (!ActiveGEHandle.IsValid() || !ActiveGEHandle.WasSuccessfullyApplied()) {

					UE_LOG(LogTemp, Error, TEXT("FAILED TO APPLY JUMP EFFECT! %s"), *GetNameSafe(Character));

					}
			}
		}
		*/
	}

 }
