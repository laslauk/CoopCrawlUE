// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"


/**
 *  Meant to be added CrawlerHUD - and accessed from player controller??? or Perhaps just state
 */
UCLASS()
class LASDC_API UCharacterOverlay : public UUserWidget
{

	GENERATED_BODY()

public:



	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HealthTextValue;

	class ACrawlHUD* CrawlHUD;


	virtual bool Initialize() override;

};
