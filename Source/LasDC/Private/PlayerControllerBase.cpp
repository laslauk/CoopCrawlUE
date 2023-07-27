// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedPlayerInput.h"
#include "InventoryComp.h"
#include "InputActionValue.h"
#include "LasDCGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "InventoryComp.h"
#include "PlayerStateBase.h"
#include "EnhancedInputComponent.h"
#include "InteractInterface.h"
#include "GameFramework/HUD.h"
#include "Actors/InventoryContainerActor.h"
#include "AttributeSetBase.h"
#include "PlayerStateBase.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "CharacterBaseGAS.h"




void APlayerControllerBase::OnPossess(APawn* pawn) {

    Super::OnPossess(pawn);
    if (pawn) {
        PlayerCharacter_Ref = Cast<ACharacterBaseGAS>(pawn);
        PlayerState_Ref = GetPlayerState<APlayerStateBase>();



        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: %s, Possessed pawn: %s"), *GetName(), *GetPawn()->GetName());

        // Set the EnhancedPlayerInputComponent as the input component for the controller
        SetAutonomousProxy(true);

    }



    //register to tag

    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(pawn)) {

        DeathStateTagDelegate = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &APlayerControllerBase::OnPawnDeathStateChanged);
    //    ASC->GetGameplayAttributeValueChangeDelegate(GetPlayerState<APlayerStateBase>()->AttributeSetBaseComp->GetHealthAttribute()).AddUObject(PlayerCharacter_Ref, &ACharacterBaseGAS::OnHealthAttributeChanged);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("--- ERROR ON Aplayercontrollerbase Onpossess, catn find ASC"));
    }

  

}


void APlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(APlayerControllerBase, PlayerCharacter_Ref, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(APlayerControllerBase, PlayerState_Ref);



}


void APlayerControllerBase::UnregisterDeathEventTag() {

    if (DeathStateTagDelegate.IsValid()) {

        if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
        {
            AbilityComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
        }
    }
}

void APlayerControllerBase::OnUnPossess()
{

    Super::OnUnPossess();

    UnregisterDeathEventTag();

}

void APlayerControllerBase::OnPawnDeathStateChanged(const FGameplayTag callbackTag, int32 NewCount)
{

    //something has applied to death state stack one dead.tag, so its dead
    if (NewCount > 0) {

        //Notify GAME Mmode on server (exists only on server) that pawn has been kiled

        UWorld* World = GetWorld();
        ALasDCGameMode* GameMode = World ? Cast<ALasDCGameMode>(World->GetAuthGameMode()) : nullptr;
        BP_OnDeath(); //blueprint death notify

        if (GameMode) {
          
            GameMode->NotifyPlayerDied(this);
            
        }


        UnregisterDeathEventTag();
    }

}

void APlayerControllerBase::RestartPlayer()
{
    UWorld* World = GetWorld();
    ALasDCGameMode* GameMode = World ? Cast<ALasDCGameMode>(World->GetAuthGameMode()) : nullptr;


    if (GameMode) {
        UE_LOG(LogTemp, Warning, TEXT("RESTARITNTGG RESTARITNTGGRESTARITNTGGRESTARITNTGGRESTARITNTGGRESTARITNTGGRESTARITNTGGRESTARITNTGG" ));
        GameMode->RestartPlayer(this);

    }

}

void APlayerControllerBase::RestartPlayerIn(float InTime)
{

    //set
    ChangeState(NAME_Spectating);
    GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle, this, &APlayerControllerBase::RestartPlayer, InTime, false);
}






void APlayerControllerBase::SetupInputComponent() {
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());


    if (Subsystem) {

        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("EI OLE LOCAL SUBSYSTEM <<<<<<<<<<<<<<<<<<"));
    }

    if (UEnhancedInputComponent* EI = Cast<UEnhancedInputComponent>(InputComponent)) {
       

        //BINDINGS FOR CALLBACKS

        if (PrimaryInputAction) {
            EI->BindAction(PrimaryInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnPrimaryInputActionStarted);
            EI->BindAction(PrimaryInputAction, ETriggerEvent::Completed, this, &APlayerControllerBase::OnPrimaryInputActionEnded);
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

        if (InteractInputAction) {
            EI->BindAction(InteractInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnInteractInputActionStarted);
        }

        if (EquipItemInputAction) {
            EI->BindAction(EquipItemInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnEquipNextItemTriggered);
        }
        if (DropItemInputAction) {
            EI->BindAction(DropItemInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnDropItemTriggered);
        }
        if (UnequipItemInputAction) {
            EI->BindAction(UnequipItemInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnUnequipItemTriggered);
        }

        if (OpenInventoryInputAction) {
            EI->BindAction(OpenInventoryInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnOpenInventoryActionInputActionStarted);
        }


        if (CrouchInputAction) {
            EI->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnCrouchInputActionStarted);
            EI->BindAction(CrouchInputAction, ETriggerEvent::Completed, this, &APlayerControllerBase::OnCrouchInputActionEnded);
        }


        if (SprintInputAction)
        {
            EI->BindAction(SprintInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnSprintInputActionStarted);
            EI->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &APlayerControllerBase::OnSprintInputActionEnded);
        }
  
        if (AimInputAction) {
            EI->BindAction(AimInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnAimInputActionStarted);
            EI->BindAction(AimInputAction, ETriggerEvent::Completed, this, &APlayerControllerBase::OnAimInputActionEnded);
        }
         
         

    }
    else {
        UE_LOG(LogTemp, Warning, TEXT(">>>>>>> EI CINPUT COMPONENTTIA "));
    }
}



void APlayerControllerBase::OnRep_PlayerCharacter()
{

    if (IsLocalPlayerController())
    {
        // This code will only execute on the local client
        if (PlayerCharacter_Ref != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("LocalPawn updated: %s"), *PlayerCharacter_Ref->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("LocalPawn is null."));
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Hello, on rep ref saatu kai: ref name: %s"), *PlayerCharacter_Ref->GetName());

}






/*
void APlayerControllerBase::OnPrimaryAction(const FInputActionValue& Value) {


}
*/


void APlayerControllerBase::OnDropItemTriggered(const struct FInputActionValue& val) {
    FGameplayEventData EventPayload;
    EventPayload.EventTag = DropItemTag;

    UE_LOG(LogTemp, Warning, TEXT("OnDropItemTriggered!!!!!!!!!!!!! by %s for character %s"), *GetName(), *PlayerCharacter_Ref->GetName());
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, DropItemTag, EventPayload);

}
void APlayerControllerBase::OnEquipNextItemTriggered(const struct FInputActionValue& val) {

    UE_LOG(LogTemp, Warning, TEXT("OnEquipNextItemTriggered!!!!!!!!!!!!! by %s for character %s"), *GetName(), *PlayerCharacter_Ref->GetName());
    FGameplayEventData EventPayload;
    EventPayload.EventTag = EquipNextTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, EquipNextTag, EventPayload);

}
void APlayerControllerBase::OnUnequipItemTriggered(const struct FInputActionValue& val) {
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UnequipTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, UnequipTag, EventPayload);
}


void APlayerControllerBase::OnCrouchInputActionStarted(const struct FInputActionValue& val) {

 //   FGameplayEventData EventPayload;
 //   EventPayload.EventTag = UnequipTag;
 //   UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, UnequipTag, EventPayload);

    if (PlayerCharacter_Ref) {
        PlayerCharacter_Ref->TryCrouch(true);
    }
}


void APlayerControllerBase::OnCrouchInputActionEnded(const struct FInputActionValue& val) {

    //   FGameplayEventData EventPayload;
    //   EventPayload.EventTag = UnequipTag;
    //   UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, UnequipTag, EventPayload);
    if (PlayerCharacter_Ref) {
        PlayerCharacter_Ref->TryCrouch(false);
    }
}


void APlayerControllerBase::OnOpenInventoryActionInputActionStarted(const FInputActionValue& val)
{
 
    if (!InventoryWidgetClass) return;

    //on jo
    if (InventoryWidget) {

        UE_LOG(LogTemp, Warning, TEXT("Suljetaan inventory comp"));
        InventoryWidget->RemoveFromParent();
        bShowMouseCursor = false;
        InventoryWidget = nullptr;
    }
    else
    
    {
        UE_LOG(LogTemp, Warning, TEXT("Avataan inventory comp"));
        InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
        if (InventoryWidget) {
            InventoryWidget->AddToViewport();
            bShowMouseCursor = true;
        }

    }

    if (AActor * OpenedContainer  = PlayerState_Ref->GetOpenedInventoryContainerActor())
    {
       
    }

}

void APlayerControllerBase::OnPrimaryInputActionStarted(const FInputActionValue& val) {

    FGameplayEventData EventPayload;
    EventPayload.EventTag = PrimaryActionStartedEventTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, PrimaryActionStartedEventTag, EventPayload);

  //  PlayerCharacter_Ref->BP_OnPrimaryInputAction();
}

void APlayerControllerBase::OnPrimaryInputActionEnded(const FInputActionValue& val) {


    FGameplayEventData EventPayload;
    EventPayload.EventTag = PrimaryActionEndedEventTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, PrimaryActionEndedEventTag, EventPayload);

    //  PlayerCharacter_Ref->BP_OnPrimaryInputAction();
}



void APlayerControllerBase::OnSecondaryInputAction(const struct FInputActionValue& val) {


    if (PlayerCharacter_Ref) {
        PlayerCharacter_Ref->BP_OnSecondaryInputAction();
    }
}


void APlayerControllerBase::OnJumpInputAction(const struct FInputActionValue& val) {

    if (PlayerCharacter_Ref) {
        PlayerCharacter_Ref->TryJump();
    }
}


void APlayerControllerBase::OnMoveForwardInputAction(const struct FInputActionValue& val) {

    if (!PlayerCharacter_Ref) return;

    //  UE_LOG(LogTemp, Warning, TEXT(">OnMoveForwardInputAction."));
      //Huom, input movement suunta top down pelistä, Z ylöspäin kartassa TODO paremmaksi jos kameraa voi kääntää yms
    PlayerCharacter_Ref->AddMovementInput(FVector3d(1.0f, 0.0f, 0.f), val.GetMagnitude(), false);

}

void APlayerControllerBase::OnInteractInputActionStarted(const FInputActionValue& val)
{
    ACharacterBaseGAS* PlayerChar = Cast<ACharacterBaseGAS>(GetPawn());

    //if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, 
   // TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true)) 
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
    ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
    FHitResult OutHit;
    float TraceRadius = 300.0f;

    UKismetSystemLibrary::SphereTraceSingleForObjects(this, PlayerChar->GetActorLocation(), PlayerChar->GetActorLocation(), TraceRadius, ObjectTypesArray, false, {}, EDrawDebugTrace::ForDuration, OutHit, true);

    if (OutHit.bBlockingHit) {

        // bIsImplemented will be true if OriginalObject implements UReactToTriggerInterface.
        bool bIsImplemented = OutHit.GetActor()->GetClass()->ImplementsInterface(UInteractInterface::StaticClass());


        if (OutHit.GetActor()->Implements<UInteractInterface>())
        {
            /*
            Cast<IInteractInterface>(OutHit.GetActor())->ReactToInteractPure(); // C++ only implementation
            IInteractInterface::Execute_ReactToInteract(OutHit.GetActor()); //Calling Blueprint Implementation
            */

            /* PROPER WAY */
            IInteractInterface* Interface = Cast<IInteractInterface>(OutHit.GetActor());
            if (Interface) {

                Interface->ReactToInteractPure(this); // C++
                Interface->Execute_ReactToInteract(OutHit.GetActor());


                /* actor sendaa gameplay eventin itsestään ptr ja täällä hoidetaa haetaa sen pitämä data*/

               // if (PlayerState_Ref) {
                //    GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, FString::Printf(TEXT("ON INTERACTISA PLAYERSTATE:  %s"), *PlayerState->GetName()));
                //    PlayerState_Ref->SetOpenedInventoryContainerActor(OutHit.GetActor());

              //  }

            }


            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, FString::Printf(TEXT("Hello %s"), *OutHit.GetActor()->GetName()));

           // IInteractInterface::Execute_ReactToInteract(OutHit.GetActor());
        }

        //alternative:  bIsImplemented will be true if OriginalObject implements UReactToTriggerInterfacce.
        // bIsImplemented = OriginalObject->Implements<UReactToTriggerInterface>(); 
     

      
        
      
    }
}


void APlayerControllerBase::OnMoveSideInputAction(const struct FInputActionValue& val) {
    if (!PlayerCharacter_Ref) return;
    // UE_LOG(LogTemp, Warning, TEXT(">OnMoveSideInputAction."));
    PlayerCharacter_Ref->AddMovementInput(FVector3d(0.f, 1.0f, 0.f), val.GetMagnitude(), false);

}


void APlayerControllerBase::OnSprintInputActionStarted(const struct FInputActionValue& val) {
    if (PlayerCharacter_Ref) {
        if (UAbilitySystemComponent* ASC = PlayerCharacter_Ref->GetAbilitySystemComponent())
        {
            ASC->TryActivateAbilitiesByTag(PlayerCharacter_Ref->SprintTags, true);
        }
    }

}


void APlayerControllerBase::OnSprintInputActionEnded(const struct FInputActionValue& val) {

    if (PlayerCharacter_Ref) {
        if (UAbilitySystemComponent* ASC = PlayerCharacter_Ref->GetAbilitySystemComponent())
        {
            ASC->CancelAbilities(&(PlayerCharacter_Ref->SprintTags));
        }
    }
}

void APlayerControllerBase::OpenInventoryContainerActor(AInventoryContainerActor* InventoryContainerActor) {

    /* On jo auki ikkuna*/
    if (OpenedInventoryContainerActorWidget) {
        OpenedInventoryContainerActorWidget->RemoveFromParent();
        OpenedInventoryContainerActorWidget = nullptr;
        bShowMouseCursor = false;
        return;
    }

    OpenedInventoryContainerActorWidget = CreateWidget<UUserWidget>(this, InventoryContainerActor->InventoryContainerWidgetClass);

    if (OpenedInventoryContainerActorWidget) {
        PlayerState_Ref->SetOpenedInventoryContainerActor(InventoryContainerActor);
        OpenedInventoryContainerActorWidget->AddToViewport();

        bShowMouseCursor = true;




        IInteractInterface* Interface = Cast<IInteractInterface>(OpenedInventoryContainerActorWidget);
        Interface->Execute_ReactToInteract(OpenedInventoryContainerActorWidget);

    }

}



void APlayerControllerBase::OnAimInputActionStarted(const FInputActionValue& val)
{
    FGameplayEventData EventPayLoad;
    EventPayLoad.EventTag = AimStartedEventTag;
    UE_LOG(LogTemp, Warning, TEXT("Started aim input action"));
    if (PlayerCharacter_Ref) {
        UE_LOG(LogTemp, Warning, TEXT("does have playercharacter ref, sent to there"));
    }
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, AimStartedEventTag, EventPayLoad);

}

void APlayerControllerBase::OnAimInputActionEnded(const FInputActionValue& val)
{
    FGameplayEventData EventPayLoad;
    EventPayLoad.EventTag = AimEndedEventTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, AimEndedEventTag, EventPayLoad);


}
