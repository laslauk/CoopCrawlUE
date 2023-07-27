// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LasDCGameMode.generated.h"

class APlayerControllerBase;


UCLASS(minimalapi)
class ALasDCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALasDCGameMode();

	void NotifyPlayerDied(APlayerControllerBase* PlayerController);
};



