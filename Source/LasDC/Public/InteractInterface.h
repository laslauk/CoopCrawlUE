// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractInterface.generated.h"

/**
 *  https://www.youtube.com/watch?v=wNDrCcjtLdA
 */
UINTERFACE(MinimalAPI)
class  UInteractInterface : public UInterface
{
	GENERATED_BODY()
	
};


class IInteractInterface
{
	GENERATED_BODY()

public:
	/** Add interface function declarations here */

	/* PURE c++, can be only called inside C++*/
	UFUNCTION(Category = "Interact Reaction")
	 virtual void ReactToInteractPure(APlayerControllerBase* OtherPlayerController) = 0;


	UFUNCTION(BlueprintNativeEvent, Category = "Interact Reaction")
	 void ReactToInteract();   




};