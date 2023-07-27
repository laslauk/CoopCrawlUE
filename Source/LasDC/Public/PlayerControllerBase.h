// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "PlayerControllerBase.generated.h"

class AHUD;
class APlayerStateBase;
class ACharacterBaseGAS;
class UUserWidget;
/**
 * 
 */
UCLASS()
class LASDC_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void OnPawnDeathStateChanged(const FGameplayTag callbackTag, int32 NewCount);

	void RestartPlayer();
	void RestartPlayerIn(float InTime);

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;


	FTimerHandle RestartPlayerTimerHandle;
	FDelegateHandle DeathStateTagDelegate;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController", ReplicatedUsing = OnRep_PlayerCharacter)
	 ACharacterBaseGAS* PlayerCharacter_Ref;


	UPROPERTY(EditAnywhere, Replicated)
	APlayerStateBase* PlayerState_Ref = nullptr;

	
	UFUNCTION()
	void OnRep_PlayerCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase")
	void CameraLocked(bool locked);


	/* Input Action bindings - InputAction defined in the blueprint */

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
	class UInputAction* CrouchInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* DropItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* EquipItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* UnequipItemInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* SprintInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* AimInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* OpenInventoryInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* InteractInputAction;

	/* Input Action Callbacks */

	void OnInteractInputActionStarted(const struct FInputActionValue& val);
	void OnOpenInventoryActionInputActionStarted(const struct FInputActionValue& val);
	void OnPrimaryInputActionStarted(const struct FInputActionValue& val);
	void OnPrimaryInputActionEnded(const struct FInputActionValue& val);
	void OnSecondaryInputAction(const struct FInputActionValue& val);
	void OnMoveForwardInputAction(const struct FInputActionValue& val);
	void OnMoveSideInputAction(const struct FInputActionValue& val);
	void OnJumpInputAction(const struct FInputActionValue& val);
	void OnDropItemTriggered(const struct FInputActionValue& val);
	void OnEquipNextItemTriggered(const struct FInputActionValue& val);
	void OnUnequipItemTriggered(const struct FInputActionValue& val);
	void OnCrouchInputActionStarted(const struct FInputActionValue& val);
	void OnCrouchInputActionEnded(const struct FInputActionValue& val);
	void OnSprintInputActionStarted(const struct FInputActionValue& val);
	void OnSprintInputActionEnded(const struct FInputActionValue& val);
	void OnAimInputActionStarted(const struct FInputActionValue& val);
	void OnAimInputActionEnded(const struct FInputActionValue& val);




	/* Tags to send on event*/
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PrimaryActionStartedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PrimaryActionEndedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimStartedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimEndedEventTag;

	/* UI Classes */

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget")
	TSubclassOf<UUserWidget> InventoryWidgetClass;


	/* Tämä tulee avatulta actorilta */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory Widget")
	UUserWidget* OpenedInventoryContainerActorWidget = nullptr;


	UFUNCTION(BlueprintCallable)
	void OpenInventoryContainerActor(AInventoryContainerActor* InventoryContainerActor);




private:
	void UnregisterDeathEventTag();
	UUserWidget* InventoryWidget = nullptr;


public:
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerController")
	void BP_OnDeath();

//	void OnPrimaryAction(const  FInputActionValue& Value);
};
