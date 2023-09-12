// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityBase.h"
#include "GA_Crouch.generated.h"

/**
*  Uses tags at game character to activate ability by granting them on button action input press
 * 
 */
UCLASS()
class LASDC_API UGA_Crouch : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UGA_Crouch();

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility
	(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData
	) override;


	/*
	Native function, called if an ability ends normally or abnormally. 
	If bReplicate is set to true, try to replicate the ending to the client/server
	*/

	virtual void EndAbility
	(	
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled
	) override;


	virtual bool CanActivateAbility
	(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags, 
		const FGameplayTagContainer* TargetTags,
		OUT FGameplayTagContainer* OptionalRelevantTags
	) const override;
	
};
