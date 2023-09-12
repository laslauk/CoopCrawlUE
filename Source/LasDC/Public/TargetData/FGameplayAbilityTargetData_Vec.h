// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/*
#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "GameplayPrediction.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FGameplayAbilityTargetData_Vec.generated.h"
*/
/**
 * 
 */
 /*
USTRUCT(BlueprintType)
struct LASDC_API FGameplayAbilityTargetData_Vec : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName MyName;

	UPROPERTY()
	FPredictionKey MyPredictionKey;
		
	// This is required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Vec::StaticStruct();
	}

	// This is required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		// The engine already defined NetSerialize for FName & FPredictionKey, thanks Epic!
		MyName.NetSerialize(Ar, Map, bOutSuccess);
		MyPredictionKey.NetSerialize(Ar, Map, bOutSuccess);
		bOutSuccess = true;
		return true;
	}

};


struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Vec> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Vec>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};


*/