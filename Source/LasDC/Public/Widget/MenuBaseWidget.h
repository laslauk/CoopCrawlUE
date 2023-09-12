// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/MainMenuInterface.h"
#include "MenuBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UMenuBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void Setup();
	void TearDown();

	void SetMenuInterface(IMainMenuInterface* MenuInterface);

protected:

	//t‰‰t kautta kutsutaan interfacejuttuja, mainmenu interface nimi olisi parempi olla joku Onlinestuff interface
	IMainMenuInterface* MainMenuInterface;
};
