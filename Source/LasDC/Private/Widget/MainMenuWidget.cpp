// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Game/GameInstanceCrawl.h"
#include "PlayerControllerBase.h"
#include "Components/EditableTextBox.h"
#include "Interfaces/MainMenuInterface.h"





bool UMainMenuWidget::Initialize()
{

	 if (Super::Initialize())
	 {

		 if (HostButton) 
		 {
			HostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HostServer);

		 }

		 if (ConnectToIP) {
			 ConnectToIP->OnClicked.AddDynamic(this, &UMainMenuWidget::ShowJoinMenu);
		 }

		 if (FindSessionButton) {
			 FindSessionButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnFindSessionsClicked);
		 }

		if (BackButton)
		{
			BackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackButtonClicked);
		}

		if (JoinServerButton) {
			JoinServerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinServerButtonClicked);
		}
		return true;
	}
	 else 
	 {
		 return false;
	 }

	return false;
}


 void UMainMenuWidget::HostServer()
 {
	 UE_LOG(LogTemp, Warning, TEXT("HOSTING SERVER!!"));

	 if (MainMenuInterface) {
		 Cast<APlayerControllerBase>(GetOwningPlayer())->MainMenuPtr = nullptr;
		 MainMenuInterface->Host();
	
	 }
 }


 void UMainMenuWidget::ShowJoinMenu(){

	 if (MenuSwitcher) {
		 UE_LOG(LogTemp, Warning, TEXT("HOMNO"));
		 MenuSwitcher->SetActiveWidget(JoinByIPMenu);

	 }

 }

 void UMainMenuWidget::OnFindSessionsClicked() {


	 Cast<UGameInstanceCrawl>(GetGameInstance())->FindSessions();

 }

 void UMainMenuWidget::OnJoinServerButtonClicked()
 {
	 FString healthMessage = FString::Printf(TEXT(" Trying to join"));
	 GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

	 FString IpAddress = EditableTextBox->GetText().ToString();
	 if (MainMenuInterface)
	 {
		 MainMenuInterface->JoinServer(IpAddress);
		  healthMessage = FString::Printf(TEXT(" Joining: %s"));
		 GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

	 }
	


 }

 void UMainMenuWidget::OnBackButtonClicked()
 {

	 if (MenuSwitcher) {
		 MenuSwitcher->SetActiveWidget(MainMenu);

	 }
 }


