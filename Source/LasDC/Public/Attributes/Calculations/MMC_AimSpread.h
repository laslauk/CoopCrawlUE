// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AimSpread.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UMMC_AimSpread : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
};
