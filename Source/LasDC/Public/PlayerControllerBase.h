// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void OnPossess( APawn* pawn) override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController", ReplicatedUsing = OnRep_PlayerCharacter)
	class ACharacterBaseGAS * PlayerCharacterRef;
	
	UFUNCTION()
	void OnRep_PlayerCharacter();



	UPROPERTY(EditDefaultsOnly)
	class UInputAction* PrimaryInputAction;


	UPROPERTY(EditDefaultsOnly)
	class UInputAction* SecondaryInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveForwardInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveSideInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* JumpInputAction;


	UPROPERTY(EditDefaultsOnly)
	class UInputAction* DropItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* EquipItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* UnequipItemInputAction;



	void OnPrimaryInputAction(const struct FInputActionValue& val);
	void OnSecondaryInputAction(const struct FInputActionValue& val);
	void OnMoveForwardInputAction(const struct FInputActionValue& val);
	void OnMoveSideInputAction(const struct FInputActionValue& val);
	void OnJumpInputAction(const struct FInputActionValue& val);
	void OnDropItemTriggered(const struct FInputActionValue& val);
	void OnEquipItemTriggered(const struct FInputActionValue& val);
	void OnUnequipItemTriggered(const struct FInputActionValue& val);


//	void OnPrimaryAction(const  FInputActionValue& Value);
};
