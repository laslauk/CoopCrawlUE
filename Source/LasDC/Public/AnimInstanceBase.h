// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
		class UBlendSpace* GetLocomotionBlendspace() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
		class UAnimSequenceBase* GetIdleAnimation() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		class UCharacterAnimDataAsset* DefaultCharacterAnimDataAasset;
};
