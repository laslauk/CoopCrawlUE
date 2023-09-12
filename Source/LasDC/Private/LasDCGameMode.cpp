// Copyright Epic Games, Inc. All Rights Reserved.

#include "LasDCGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "HUD/CrawlHUD.h"
#include "GameFramework/HUD.h"
#include "PlayerControllerBase.h"


ALasDCGameMode::ALasDCGameMode()
{

	PlayerControllerClass = APlayerControllerBase::StaticClass();
	//HUDClass = ACrawlHUD::StaticClass();
	
}

void ALasDCGameMode::NotifyPlayerDied(APlayerControllerBase* PlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("HEIIIII KUOLEMA!!!!!!!!!!!!!!!!!!!!!!"));
	if (PlayerController) {
		PlayerController->RestartPlayerIn(2.0f);
	}
}
