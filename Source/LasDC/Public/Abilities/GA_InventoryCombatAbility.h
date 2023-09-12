// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GA_InventoryAbility.h"
#include "GA_InventoryCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UGA_InventoryCombatAbility : public UGA_InventoryAbility
{
	GENERATED_BODY()



public:
	virtual bool  CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags) override;


	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);

	/* F*/
	UFUNCTION(BlueprintPure)
	const bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool HasEnoughAmmo() const;

	UFUNCTION(BlueprintCallable)
	void DecreaseAmmo();

	UFUNCTION(BlueprintCallable)
	FTransform GetFireDirection(const FHitResult& TraceHitResult);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool UseAmmo = true;

	//Trace from center of the screen! not from character like WeaponToFocusTrace
	UFUNCTION(BlueprintCallable)
		void TraceUnderCrosshairs(FHitResult& TraceHitResult, FTransform& HitDirectionRotation );


};
