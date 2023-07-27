// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityBase.h"
#include "GA_Vault.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class LASDC_API UGA_Vault : public UGameplayAbilityBase
{
	GENERATED_BODY()

		UGA_Vault();
	

public: 
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;


	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;


	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;


protected:
	UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
		float HorizontalTraceRadius = 30.0f;
		UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
			float HorizontalTraceLength = 500.0f;
		UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
			float HorizontalTraceStep = 30.0f;


		UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
			int HorizontalTraceCount = 5;


		UPROPERTY(EditDefaultsOnly, Category = "VerticalTrace")
			float VerticalTraceRadius = 30.0f;

		UPROPERTY(EditDefaultsOnly, Category = "VerticalTrace")
			float VerticalTraceLength = 30.0f;

		UPROPERTY(EditDefaultsOnly, Category = "VerticalTrace")
			float VerticalTraceStep = 30.0f;

		UPROPERTY(EditDefaultsOnly)
			TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

		UPROPERTY(EditDefaultsOnly)
			UAnimMontage* VaultMontage = nullptr;

		FVector JumpToLocation;
		FVector JumpOverLocation;

		UPROPERTY(EditDefaultsOnly)
			TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;

		UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;
};
