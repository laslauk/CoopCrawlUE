// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/InGameMenu.h"
#include "PlayerControllerBase.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{

    if (Super::Initialize()) {

        UE_LOG(LogTemp, Warning, TEXT("INIT UiNGameMenu"));
        if (CancelButton) {
            CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::OnCancelButtonClicked);
        }

        if (MainMenuButton) {
            MainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::OnMainMenuButtonClicked);
        }

        return true;
    }
    else {

        return false;
    }

 

    return false;
}


void UInGameMenu::OnCancelButtonClicked() {
    UE_LOG(LogTemp, Warning, TEXT("Tearing down OnCancelButtonClicked!!"));
    this->TearDown();

    Cast<APlayerControllerBase>(GetOwningPlayer())->ClearUIPointers();
}

void UInGameMenu::OnMainMenuButtonClicked() {
    UE_LOG(LogTemp, Warning, TEXT("OnMainMenuButtonClicked!!"));
    if (MainMenuInterface) {
        TearDown();
        MainMenuInterface->OpenMainMenu();
        Cast<APlayerControllerBase>(GetOwningPlayer())->ClearUIPointers();
    }


}