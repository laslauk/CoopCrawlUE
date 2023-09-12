// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuBaseWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UInGameMenu : public UMenuBaseWidget
{
	GENERATED_BODY()

public:


	virtual bool Initialize() override;

	UFUNCTION()
	void OnCancelButtonClicked();


	UFUNCTION()
	void OnMainMenuButtonClicked();




	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* CancelButton;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* MainMenuButton;
	
};
