// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CrawlCommonTypes.h"
#include "CharacterAnimDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class LASDC_API UCharacterAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		FCharacterAnimationData CharacterAnimationData;



};
