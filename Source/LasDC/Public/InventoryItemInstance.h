// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"

class UInventoryComponent;
class AItemActorBase;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LASDC_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:

		virtual void Init(TSubclassOf<class UItemStaticData> ItemStaticDataClass, AActor* owner, int32 InQuantity = 1);

		virtual bool IsSupportedForNetworking() const override { return true;}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData*GetItemStaticData() const; //static datat

	UPROPERTY(Replicated)
	TSubclassOf<class UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnequipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);


	UFUNCTION(BlueprintPure)
	AItemActorBase* GetItemActor() const;


	UPROPERTY()
	class AActor* OwningPlayerState;


	int32 GetQuantity() const { return Quantity; }

	void AddItems(int32 Count);


protected:
	UPROPERTY(Replicated)
	AItemActorBase* ItemActor;


public:

	//single instance of item can have multiple items' stack ammo etc
	UPROPERTY(Replicated)
		int Quantity = 1;




	/* add abilities on equip */
	void TryGrantAbilities(AActor* InOwner);
	void TryRemoveAbilities(AActor* inOwner);


	/* add Effects on equip */
	void TryApplyEffects(AActor* InOwner);
	void TryRemoveEffects(AActor* inOwner);

	//effect handles to be saved for use
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<FActiveGameplayEffectHandle> OnGoingItemAddedEffectHandles;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};
