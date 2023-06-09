// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrawlCommonTypes.h"
#include "CrawlGameStatics.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UCrawlGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()



public: 


	UFUNCTION(BlueprintCallable, BlueprintPure)
		static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
	
};
