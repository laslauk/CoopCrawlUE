// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECrawlAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UENUM(BlueprintType)
enum class ECrawlAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};


UCLASS()
class LASDC_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()


public:
	/* TODO: !!!!*/
	/* Getters Crawl castatuille jutuille, esim get ASC, player controller, controller, character, FROM ACTOR INFO*/


	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);


	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);


	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "Crawl|Ability")
	void SetCameraMode(ECameraAimMode CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "Crawl|Ability")
	void ClearCameraMode();


	UFUNCTION(BlueprintCallable, Category = "Crawl|Ability")
	AController* GetControllerFromActorInfo() const;


protected:

	//Applying effects on start, ability wont manage their lifeitme
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToRemoveOnEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToJustApplyOnStart;

	TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class ACharacterBaseGAS* GetCharacterBaseFromActorInfo() const;


	UFUNCTION(Server, Reliable, BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void LaunchProjectileActorServer(UObject* WorldContextObject, TSubclassOf<class AProjectileBase> ProjectileActorClass, FTransform Transform, AActor* Owner, APawn* Instigator);

	//~UGameplayAbility interface overridet kuten canactivate, etc.


	// Current camera mode set by the ability.
	ECameraAimMode ActiveCameraMode;
	
};
