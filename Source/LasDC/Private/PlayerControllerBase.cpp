// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedPlayerInput.h"
#include "InventoryComp.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
 
#include "EnhancedInputComponent.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "CharacterBaseGAS.h"




void APlayerControllerBase::OnPossess(APawn* pawn) {

    Super::OnPossess(pawn);
    if (pawn) {
        PlayerCharacterRef = Cast<ACharacterBaseGAS>(pawn);

    

        // Set the EnhancedPlayerInputComponent as the input component for the controller
     

    }
}


void APlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(APlayerControllerBase, PlayerCharacterRef, COND_None, REPNOTIFY_Always);
}



void APlayerControllerBase::SetupInputComponent() {
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());


    if (Subsystem) {
        UE_LOG(LogTemp, Warning, TEXT("SUB SYSTEM"));
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }


    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent)) {
        UE_LOG(LogTemp, Warning, TEXT("HomoHomoHomoHomoHomoHomoHomo"));

        


        if (PrimaryInputAction) {
            EI->BindAction(PrimaryInputAction, ETriggerEvent::Completed, this, &APlayerControllerBase::OnPrimaryInputAction);
        }

        if (JumpInputAction) {
            EI->BindAction(JumpInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnJumpInputAction);
        }

        if (MoveForwardInputAction) {
            EI->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnMoveForwardInputAction);
        }

        if (MoveSideInputAction) {
            EI->BindAction(MoveSideInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnMoveSideInputAction);
        }



    }
    else {
        UE_LOG(LogTemp, Warning, TEXT(">>>>>>> EI CINPUT COMPONENTTIA "));
    }
}

void APlayerControllerBase::OnPrimaryInputAction(const FInputActionValue& val) {

    UE_LOG(LogTemp, Warning, TEXT("++++++PRIMARY ACTION "));
    PlayerCharacterRef->BP_OnPrimaryInputAction();
}



void APlayerControllerBase::OnSecondaryInputAction(const struct FInputActionValue& val) {


    if (PlayerCharacterRef) {
        PlayerCharacterRef->BP_OnSecondaryInputAction();
    }
}


void APlayerControllerBase::OnJumpInputAction(const struct FInputActionValue& val) {

    if (PlayerCharacterRef) {
        PlayerCharacterRef->TryJump();
    }
}


void APlayerControllerBase::OnMoveForwardInputAction(const struct FInputActionValue& val) {

    if (!PlayerCharacterRef) return;

  //  UE_LOG(LogTemp, Warning, TEXT(">OnMoveForwardInputAction."));
    //Huom, input movement suunta top down pelistä, Z ylöspäin kartassa TODO paremmaksi jos kameraa voi kääntää yms
    PlayerCharacterRef->AddMovementInput(FVector3d(1.0f, 0.0f, 0.f), val.GetMagnitude(), false);



}


void APlayerControllerBase::OnMoveSideInputAction(const struct FInputActionValue& val) {
    if (!PlayerCharacterRef) return;
   // UE_LOG(LogTemp, Warning, TEXT(">OnMoveSideInputAction."));
    PlayerCharacterRef->AddMovementInput(FVector3d(0.f, 1.0f, 0.f), val.GetMagnitude(), false);

}



void APlayerControllerBase::OnRep_PlayerCharacter()
{

    if (IsLocalPlayerController())
    {
        // This code will only execute on the local client
        if (PlayerCharacterRef != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("LocalPawn updated: %s"), *PlayerCharacterRef->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LocalPawn is null."));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Hello, on rep ref saatu kai: ref name: %s"), *PlayerCharacterRef->GetName());

}






/*
void APlayerControllerBase::OnPrimaryAction(const FInputActionValue& Value) {


}
*/


void APlayerControllerBase::OnDropItemTriggered(const struct FInputActionValue& val) {
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComp::DropItemTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComp::DropItemTag, EventPayload);

}
void APlayerControllerBase::OnEquipItemTriggered(const struct FInputActionValue& val) {

    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComp::EquipItemActorTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComp::EquipItemActorTag, EventPayload);

}
void APlayerControllerBase::OnUnequipItemTriggered(const struct FInputActionValue& val) {
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComp::UnequipTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComp::UnequipTag, EventPayload);
}