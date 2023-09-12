// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "HUD/CrawlHUD.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "PlayerControllerBase.generated.h"


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

	virtual void Tick(float DeltaTime) override;

	APlayerControllerBase();

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnDeathStateChanged(const FGameplayTag callbackTag, int32 NewCount);

	void RestartPlayer();
	void RestartPlayerIn(float InTime);

	virtual void OnPossess(APawn* aPawn) override;

	void OnAimSpreadAttributeChanged(const FOnAttributeChangeData& Data);

	virtual void OnUnPossess() override;
	void SetHUDCrosshairs(float DeltaTime);




	virtual void SetupInputComponent() override;


	FTimerHandle RestartPlayerTimerHandle;
	FDelegateHandle DeathStateTagDelegate;

	UPROPERTY(VisibleAnywhere)
	bool bTickCrossHair;


	FHitResult HitTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController", ReplicatedUsing = OnRep_PlayerCharacter)
	 ACharacterBaseGAS* PlayerCharacter_Ref;


	UPROPERTY(EditAnywhere, Replicated)
	APlayerStateBase* PlayerState_Ref = nullptr;

	UFUNCTION()
	FORCEINLINE APlayerStateBase* GetPlayerStateBase() { return PlayerState_Ref; }

	
	UFUNCTION()
	void OnRep_PlayerCharacter();

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase")
	FORCEINLINE void CameraLocked(bool locked);


	UPROPERTY(EditAnywhere)
	class ACrawlHUD* HUD;


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

	UPROPERTY(EditDefaultsOnly)
		class UInputAction* OpenMainMenuInputAction;

	UPROPERTY(EditDefaultsOnly)
		class UInputAction* OpenGameMenuInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* LookRightInputAction;


	UPROPERTY(EditDefaultsOnly)
	class UInputAction* LookUpInputAction;

	UPROPERTY(EditDefaultsOnly)
		class UInputAction* PickupInputAction;

	void ClearUIPointers();

	/* Input Action Callbacks */

	void OnLookUpInputAction(const struct FInputActionValue& val);
	void OnPickupInputAction(const struct FInputActionValue& val);
	void OnLookRightInputAction(const struct FInputActionValue& val);
	void OnInteractInputActionStarted(const struct FInputActionValue& val);
	void OnOpenGameMenuInputActionStarted(const struct FInputActionValue& val);
	void OnOpenMainMenuInputActionStarted(const struct FInputActionValue& val);
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


	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;



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

	UPROPERTY(Replicated, EditAnywhere, Category = "Activatable Abilities")
	FGameplayAbilitySpecHandle ShootAbility;

	UPROPERTY(Replicated, EditAnywhere, Category = "Activatable Abilities")
	FGameplayAbilitySpecHandle AimAbility;

private:





public:
	void UnregisterDeathEventTag();

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerController")
	void BP_OnDeath();


	/* UI Widgets */

	UPROPERTY()
	UUserWidget* Menu;

	TSubclassOf< UUserWidget> MenuClass;
	class UMainMenuWidget* MainMenuPtr = nullptr;

	TSubclassOf<UUserWidget> InGameMenuClass;
	class UMenuBaseWidget* InGameMenuPtr = nullptr;



	bool bIsMenuOpen = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> AimSpreadEffect;



	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> MovementGEStateClass;

	UPROPERTY()
		FGameplayEffectSpecHandle MovementGESpecHandle;
	FActiveGameplayEffectHandle MovementActiveGEHandle;

	UUserWidget* InventoryWidget = nullptr;

	float SpreadCurrent;

	/* SERVER RPC*/
	UFUNCTION(Server, Reliable)
	void ServerEquipItemRequested();

	bool IsAbilityActive(FGameplayTagContainer* WithTags, FGameplayTagContainer* WithoutTags, UGameplayAbility* Ignore);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPCTrace();

	UFUNCTION()
	void TraceUnderCrossHair(FHitResult& TraceHitResult);

	UPROPERTY()
	float CrosshairVelocityFactor;
	UPROPERTY()
	float CrosshairInAirFactor;

	UPROPERTY()
	float CrosshairAimFactor;

	UPROPERTY()
	 FCrossHairData CrosshairData;

//	void OnPrimaryAction(const  FInputActionValue& Value);
};
