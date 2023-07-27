// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemPhysicsVolume.generated.h"



class UGameplayEffect;
class UGameplayAbility;


//struct, data for actor inside volume
USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_USTRUCT_BODY()

		TArray<FGameplayAbilitySpecHandle> AppliedAbilities;
		TArray<FActiveGameplayEffectHandle> AppliedEffects;

};



/**
 *  PhysicsVolume + AbilitySystem
 * - Able to apply GAS effects when actor inside volume
 * - Sends TAGS to the entering / exiting actor via SendGameplayEvent, then AbilitySystem will check those tags
 */
UCLASS()
class LASDC_API AAbilitySystemPhysicsVolume : public APhysicsVolume
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug = false;


	UPROPERTY(EditANywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventTagsToSendOnEnter;

	UPROPERTY(EditANywhere, BlueprintReadWrite)
		TArray<FGameplayTag> GameplayEventTagsToSendOnExit;
	
	// Give ABILITY when inside volume, removed when leave
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> OnGoingAbilitiesToGiveOnEnter;

	// Give ABILITY when inside volume, PEREMMANTENT
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGiveOnEnter;

	//Using unreal map
	TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;

public:

	AAbilitySystemPhysicsVolume();

	/* Overrides */
	virtual void ActorEnteredVolume(class AActor* Other) override;
	virtual void ActorLeavingVolume(class AActor* Other) override;
	virtual void Tick(float DeltaSeconds) override;
};
