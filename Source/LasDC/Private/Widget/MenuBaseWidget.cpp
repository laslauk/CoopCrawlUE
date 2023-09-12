// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MenuBaseWidget.h"
#include "Game/GameInstanceCrawl.h"

void UMenuBaseWidget::Setup()
{

    AddToViewport();
    UWorld* World = GetWorld();

    APlayerController* PlayerController = World->GetFirstPlayerController();


    FInputModeGameAndUI InputModeData;
    InputModeData.SetWidgetToFocus(TakeWidget());
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    GetOwningPlayer()->bShowMouseCursor = true;
    GetOwningPlayer()->SetInputMode(InputModeData);

    SetMenuInterface(Cast<UGameInstanceCrawl>(GetGameInstance()));


}

void UMenuBaseWidget::TearDown()
{
	if (GetOwningPlayer()) {
		FInputModeGameOnly InputModeData;

		// GetOwningPlayer()->SetInputMode(InputModeData);
		GetOwningPlayer()->bShowMouseCursor = false;
		RemoveFromParent();
        GetOwningPlayer()->SetInputMode(InputModeData);
        UE_LOG(LogTemp, Warning, TEXT("Tearing down in menuBaseWidget!!"));
	}





	UE_LOG(LogTemp, Warning, TEXT("- removing mnu"));
}

void UMenuBaseWidget::SetMenuInterface(IMainMenuInterface* MenuInterface)
{
	this->MainMenuInterface = MenuInterface;

}
