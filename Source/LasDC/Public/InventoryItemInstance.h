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

	/* UOBJECT ALWAYS returns nullptr GetWorld() joten tehää oma override*/
	virtual UWorld* GetWorld() const override final;

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnequipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);


	UFUNCTION(BlueprintPure)
	AItemActorBase* GetItemActor() const;

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	UFUNCTION(BlueprintPure)
		float GetTimeSinceLastInteractedWith() const;

	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;


	/* HUOM, TÄTÄ EI NYT REPLIKOIDA; PITÄISI VARMAAN REPLIKOIDA*/
	UPROPERTY()
	class AActor* OwningPlayerState;

	UFUNCTION()
		void AddSpread();


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



	/* --------- RANGD WEAPON SPECIFICS, MAYBE SUBCLASS TO OWN IINVENTORY ITEM INSTANCEN ??????? ----------*/


	// Time since this weapon was last fired (relative to world time)
	double LastFireTime = 0.0;

	// The current heat
	float CurrentHeat = 0.0f;

	// The current spread angle (in degrees, diametrical)
	float CurrentSpreadAngle = 0.0f;

	// Do we currently have first shot accuracy?
	bool bHasFirstShotAccuracy = false;

	// The current *combined* spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;

	// The current standing still multiplier
	float StandingStillMultiplier = 1.0f;

	// The current jumping/falling multiplier
	float JumpFallMultiplier = 1.0f;

	// The current crouching multiplier
	float CrouchingMultiplier = 1.0f;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
	float BulletTraceSweepRadius = 0.0f;

	// Spread exponent, affects how tightly shots will cluster around the center line
// when the weapon has spread (non-perfect accuracy). Higher values will cause shots
// to be closer to the center (default is 1.0 which means uniformly within the spread range)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.1), Category = "Spread|Fire Params")
		float SpreadExponent = 1.0f;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
		float MaxDamageRange = 25000.0f;


	void ComputeSpreadRange(float& MinSpread, float& MaxSpread);
	void ComputeHeatRange(float& MinHeat, float& MaxHeat);


	inline float ClampHeat(float NewHeat)
	{
		float MinHeat;
		float MaxHeat;
		ComputeHeatRange(/*out*/ MinHeat, /*out*/ MaxHeat);

		return FMath::Clamp(NewHeat, MinHeat, MaxHeat);
	}

	// Updates the spread and returns true if the spread is at minimum
	bool UpdateSpread(float DeltaSeconds);

	// Updates the multipliers and returns true if they are at minimum
	bool UpdateMultipliers(float DeltaSeconds);

	// Time since firing before spread cooldown recovery begins (in seconds)
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = s))
	float SpreadRecoveryCooldownDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToSpreadCurve;

	int32 GetBulletsPerCartridge() const
	{
		return BulletsPerCartridge;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config")
	int32 BulletsPerCartridge = 1;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
	FRuntimeFloatCurve DistanceDamageFalloff;


	float GetCalculatedSpreadAngle() const
	{
		return CurrentSpreadAngle;
	}

	float GetCalculatedSpreadAngleMultiplier() const
	{
		return bHasFirstShotAccuracy ? 0.0f : CurrentSpreadAngleMultiplier;
	}

	float GetSpreadExponent() const
	{
		return SpreadExponent;
	}

	float GetMaxDamageRange() const
	{
		return MaxDamageRange;
	}

	float GetBulletTraceSweepRadius() const
	{
		return BulletTraceSweepRadius;
	}

};
