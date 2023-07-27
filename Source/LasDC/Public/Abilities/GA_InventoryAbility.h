// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityBase.h"
#include "GA_InventoryAbility.generated.h"

class UInventoryComp;
class UInventoryItemInstance;
class UWeaponStaticData;
class UItemStaticData;
class AItemActorBase;
class AWeaponItemActor;


UCLASS()
class LASDC_API UGA_InventoryAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure)
		UInventoryComp* GetInventoryComponent()const;


	UFUNCTION(BlueprintPure)
		UInventoryItemInstance* GetEquippedItemInstance() const;


	UFUNCTION(BlueprintPure)
		const UItemStaticData* GetEquippedItemStaticData() const;


	UFUNCTION(BlueprintPure)
		const UWeaponStaticData* GetEquippedItemWeaponStaticData() const ;

	UFUNCTION(BlueprintPure)
		AItemActorBase* GetEquippedItemActor() ;


	UFUNCTION(BlueprintPure)
		AWeaponItemActor* GetEquippedWeaponItemActor() ;

protected:
	UPROPERTY()
	UInventoryComp* InventoryComp = nullptr;




	
};
