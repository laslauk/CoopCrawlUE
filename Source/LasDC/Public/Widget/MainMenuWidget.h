// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/MenuBaseWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UMainMenuWidget : public UMenuBaseWidget
{
	GENERATED_BODY()
	

public:

	void SetMainMenuInterface(IMainMenuInterface* MenuInterface);



	virtual bool Initialize() override;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* HostButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* BackButton;
	

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* ConnectToIP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* JoinServerButton;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UEditableTextBox* EditableTextBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* FindSessionButton;

	
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UWidget* JoinByIPMenu;

	UFUNCTION()
		void HostServer();

	UFUNCTION()
		void ShowJoinMenu();

	UFUNCTION()
		void OnJoinServerButtonClicked();


	UFUNCTION()
		void OnFindSessionsClicked();

	UFUNCTION()
		void OnBackButtonClicked();




};
