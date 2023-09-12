// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"
#include "PlayerStateBase.h"
#include "HUD/CrawlHUD.h"

bool UCharacterOverlay::Initialize()
{
	Super::Initialize();



	if (GetOwningPlayerState() && Cast<APlayerStateBase>(GetOwningPlayerState())) {

		UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!PostLoad Characveroverlay"));
		Cast<APlayerStateBase>(GetOwningPlayerState())->UpdateHUD();
	}


	return false;
}
