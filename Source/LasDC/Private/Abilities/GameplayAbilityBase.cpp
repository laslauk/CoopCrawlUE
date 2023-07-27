// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbilityBase.h"
#include "CharacterBaseGAS.h"
#include "AbilitySystemComponent.h"

	/** Actually activate ability, do not call this directly */
 void UGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	 const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) {

	 Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	 FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();


	 for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart) {
		 if (!GameplayEffect.Get()) continue;


		 if(UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get()) {

		 FGameplayEffectSpecHandle specHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

		 if (specHandle.IsValid()) {

			 FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());

			 if (!ActiveGEHandle.IsValid() || !ActiveGEHandle.WasSuccessfullyApplied()) {

				

			 }
		 }
		 }

	 }


	 /* Instantiated abilities ?? mitä ne on*/
	 if (IsInstantiated()) {
		 for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd) {


			 if (!GameplayEffect.Get()) continue;

			 if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get()) {

				 FGameplayEffectSpecHandle specHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

				 if (specHandle.IsValid()) {

					 FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());

					 if (!ActiveGEHandle.IsValid() || !ActiveGEHandle.WasSuccessfullyApplied()) {



					 }
					 else {
						 RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					 }
				 }
			 }
		 }
	 }


}




 ACharacterBaseGAS* UGameplayAbilityBase::GetCharacterBaseFromActorInfo() const {


	 return Cast<ACharacterBaseGAS>(GetAvatarActorFromActorInfo());

 }



/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
 void UGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled) {

	 if (IsInstantiated()) {

		 for (auto ActiveEffectHandle : RemoveOnEndEffectHandles) {

			 if (ActiveEffectHandle.IsValid()) {

				 ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			 }
		 }

		 RemoveOnEndEffectHandles.Empty();

	 }

	 Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);




}
