// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceCrawl.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UGameInstanceCrawl : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UGameInstanceCrawl(const FObjectInitializer& ObjectInitializer);


	/* Exec toimii: GameInstance, PlayerController, Possessed Pawns, HUSD, CheatManagers, GameModes*/

	
	UFUNCTION(Exec)
	void Host();

	UFUNCTION(Exec)
	void Join(const FString& IpAddress);

};
