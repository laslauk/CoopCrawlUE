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
#include "ItemActorBase.h"
#include "CharacterBaseGAS.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterMovementComponentBase.h"
#include "EnhancedInputComponent.h"
#include "GameplayEffect.h"

#include "InteractInterface.h"
#include "Game/GameInstanceCrawl.h"
#include "GameFramework/HUD.h"
#include "Actors/InventoryContainerActor.h"
#include "InventoryItemInstance.h"
#include "Attributes/AttributeSetAiming.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Character/BlasterCharacter.h"
#include "AttributeSetBase.h"
#include "PlayerStateBase.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"


#include "HUD/CrawlHUD.h"
#include "HUD/CharacterOverlay.h"

#include "Widget/MenuBaseWidget.h"
#include "Widget/InGameMenu.h"
#include "Widget/MainMenuWidget.h"

#include "DrawDebugHelpers.h" /* Draw Debug sphere */


#define TRACE_LENGTH 8000.f




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

        if (LookRightInputAction) {

            EI->BindAction(LookRightInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnLookRightInputAction);

        }

        if (LookUpInputAction) {
            EI->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::OnLookUpInputAction);
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

        if (OpenGameMenuInputAction) {
            EI->BindAction(OpenGameMenuInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnOpenGameMenuInputActionStarted);
        }


        if (OpenMainMenuInputAction) {
            EI->BindAction(OpenMainMenuInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnOpenMainMenuInputActionStarted);
        }

        if (PickupInputAction) {
            EI->BindAction(PickupInputAction, ETriggerEvent::Started, this, &APlayerControllerBase::OnPickupInputAction);
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



void APlayerControllerBase::OnPossess(APawn* pawn) {
    if (!HasAuthority()) return;
    Super::OnPossess(pawn);
   
    if (pawn) 
    {
        PlayerCharacter_Ref = Cast<ACharacterBaseGAS>(pawn);
        PlayerState_Ref = GetPlayerState<APlayerStateBase>();
    
        //  ClientSetHUD(ACrawlHUD::StaticClass());

        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: %s, Possessed pawn: %s"), *GetName(), *GetPawn()->GetName());

        // Set the EnhancedPlayerInputComponent as the input component for the controller
        SetAutonomousProxy(true);

        UAbilitySystemComponent* ASC = PlayerCharacter_Ref->GetAbilitySystemComponent();

        if (PlayerState_Ref) {
            PlayerState_Ref->PlayerController = this;
            PlayerState_Ref->CrawlHUD = Cast<ACrawlHUD>(GetHUD());

            UE_LOG(LogTemp, Warning, TEXT("CRAWL HUD ptr SET!"));
        }

        if (PlayerCharacter_Ref && ASC)
        {

            FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            MovementGESpecHandle = ASC->MakeOutgoingSpec(MovementGEStateClass, 1, EffectContext);
            //  SpecHandle.Data.Get()->SetByCallerTagMagnitudes.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Change.AimSpread")), 5.f);

            if (MovementGESpecHandle.IsValid())
            {
                MovementActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*MovementGESpecHandle.Data.Get());
                if (MovementActiveGEHandle.WasSuccessfullyApplied())
                {
                    UE_LOG(LogTemp, Warning, TEXT("APPLIED MOVEMENT JUTTU"));
                }
            }
        }
    }



    //register to tag

    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(pawn))
    {

        DeathStateTagDelegate = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &APlayerControllerBase::OnPawnDeathStateChanged);
    //    ASC->GetGameplayAttributeValueChangeDelegate(GetPlayerState<APlayerStateBase>()->AttributeSetBaseComp->GetHealthAttribute()).AddUObject(PlayerCharacter_Ref, &ACharacterBaseGAS::OnHealthAttributeChanged);
        if (PlayerState_Ref) 
        {
            ASC->GetGameplayAttributeValueChangeDelegate(
                PlayerState_Ref->AttributeSetAiming->GetAimSpreadAttribute())
                .AddUObject(this, &APlayerControllerBase::OnAimSpreadAttributeChanged);

        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("--- ERROR ON Aplayercontrollerbase Onpossess, catn find ASC"));
    }

 
}



void APlayerControllerBase::OnAimSpreadAttributeChanged(const FOnAttributeChangeData& Data) {
  //  UE_LOG(LogTemp, Warning, TEXT("AIM SPREAD VALUIE: %f"), Data.NewValue);
}

void APlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(APlayerControllerBase, PlayerCharacter_Ref, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(APlayerControllerBase, PlayerState_Ref);
    DOREPLIFETIME(APlayerControllerBase, ShootAbility);
    DOREPLIFETIME(APlayerControllerBase, AimAbility);
    
    

}


void APlayerControllerBase::UnregisterDeathEventTag() {

    if (DeathStateTagDelegate.IsValid()) {

        if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
        {
            AbilityComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
        }
    }
}

bool APlayerControllerBase::IsAbilityActive(FGameplayTagContainer* WithTags, FGameplayTagContainer* WithoutTags, UGameplayAbility* Ignore)
{

   //lokataan aina kun iteroidaan

        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
        if (!ASC) { return false; }


        //ABILITYLIST_SCOPE_LOCK() EI TOIMI COMPILER
        for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
        {
            /* skipataan tämä ability jos tyhjä tai ei ole active, etsitään activea*/
            if (!Spec.IsActive() || Spec.Ability == nullptr || Spec.Ability == Ignore)
            {
                continue;
            }

            /* Verrataan tageja*/
            const bool WithTagPass = (!WithTags || Spec.Ability->AbilityTags.HasAny(*WithTags)); //näitä etitään
            const bool WithoutTagsPass = (!WithoutTags || !Spec.Ability->AbilityTags.HasAny(*WithoutTags)); //näitä ei saa olla, jos joku olöytyy niin Withouttagpass = 0 ja failaa seuraava check

            if (WithTagPass && WithoutTagsPass)
            {
                return true;
            }
        }
       
    return false;
}



void APlayerControllerBase::OnUnPossess()
{
    Super::OnUnPossess();

    UnregisterDeathEventTag();

}

void APlayerControllerBase::SetHUDCrosshairs(float DeltaTime)
{
    if (!IsLocalController()) return;
    if (!PlayerCharacter_Ref) { 
        UE_LOG(LogTemp, Warning, TEXT("CHAR REF NBULL"));  return;
    }

    if (!PlayerState_Ref) {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER STATE REF NULL")); 
      return;
      }
   

    HUD = HUD == nullptr ? Cast<ACrawlHUD>(GetHUD()): HUD;
    

    if (HUD)
    {

        if (UInventoryItemInstance* EquippedItem = PlayerState_Ref->InventoryComp->GetEquippedItem())
        {
            const UWeaponStaticData* StaticData = Cast<UWeaponStaticData>(EquippedItem->GetItemStaticData());
            if (StaticData)
            {

              
                /* Mapataan max walk speed [0,600] to [0,1] */
                const UAttributeSetBase* asd = PlayerCharacter_Ref->GetAbilitySystemComponent()->GetSet<UAttributeSetBase>();
                 
            
       //   UE_LOG(LogTemp, Warning, TEXT("SPEED: %f"),  asd->GetMaxMovementSpeed());
            //    MaxMovementSpeed.GetCurrentValue()
               
       
                FVector2D MaxWalkSpeedRange(0.f, 600.f);
                FVector2D VelocityMultiplierRange(0.f, 1.f);
                FVector Velocity = PlayerCharacter_Ref->GetCharacterMovement()->Velocity;
                Velocity.Z = 0.f;

                CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(MaxWalkSpeedRange, VelocityMultiplierRange, Velocity.Size());


                    if (MovementGESpecHandle.IsValid())
                    {
                     //   UE_LOG(LogTemp, Warning, TEXT("homo"));
                     //   UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(MovementGESpecHandle, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Change.AimSpread")), 5.f);
                       // MovementGESpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Change.AimSpread")), 5.f);
                       
             
                          /* TODO: Tälel joku parempi tapa varmaankin laskea kuin updatee joka tick täällä ?*/
                  //      PlayerCharacter_Ref->GetAbilitySystemComponent()->UpdateActiveGameplayEffectSetByCallerMagnitude(
                   //     MovementActiveGEHandle,FGameplayTag::RequestGameplayTag(TEXT("Attribute.Change.AimSpread")), CrosshairVelocityFactor);

                      //  PlayerCharacter_Ref->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*MovementGESpecHandle.Data.Get());
                    }
                    else {

                      
                    }
                   
             
              

                if (PlayerCharacter_Ref->GetCharacterMovement()->IsFalling()) 
                {
                  //  CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25, DeltaTime, 2.25f);
              
                    /*
                    FGameplayEffectContextHandle EffectContext = PlayerCharacter_Ref->GetAbilitySystemComponent()->MakeEffectContext();
                    FGameplayEffectSpecHandle SpecHandle = PlayerCharacter_Ref->GetAbilitySystemComponent()->MakeOutgoingSpec(AimSpreadEffect, 1, EffectContext);
                    SpecHandle.Data.Get()->SetByCallerTagMagnitudes.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Change.AimSpread")), 5.f);
                    FActiveGameplayEffectHandle ActiveGEHandle = PlayerCharacter_Ref->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());


                    if (SpecHandle.IsValid()) {}
                       */
                     
                    
                   
                }
                else
                {
                 //   CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 50.f);
                }

                if (PlayerCharacter_Ref->bIsAiming)
                {
                    CrosshairAimFactor = 0.5f; // FMath::FInterpTo(CrosshairInAirFactor, 0.60f, DeltaTime, 33.f);
                    UE_LOG(LogTemp, Warning, TEXT("CrosshairAimFactor: %f"), CrosshairAimFactor);
                }
                else 
                {
                    CrosshairAimFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 33.f);
                }

                SpreadCurrent = FMath::FInterpTo(SpreadCurrent, PlayerState_Ref->AttributeSetAiming->GetAimSpread(), DeltaTime, 33.f);  // 0.5f + CrosshairVelocityFactor + 
                SpreadCurrent = FMath::Clamp(SpreadCurrent, 0, 100);

                /* TODO: tää siirtää ASCille? */
                CrosshairData.CrossHairsTop = StaticData->CrosshairData.CrossHairsTop;
                CrosshairData.CrossHairsBottom = StaticData->CrosshairData.CrossHairsBottom;
                CrosshairData.CrossHairsLeft = StaticData->CrosshairData.CrossHairsLeft;
                CrosshairData.CrossHairsRight = StaticData->CrosshairData.CrossHairsRight;
                CrosshairData.CrossHairsCenter = StaticData->CrosshairData.CrossHairsCenter;
                CrosshairData.CrosshairSpread = SpreadCurrent;
              
              //  UE_LOG(LogTemp, Warning, TEXT("CrosshairSpread: %f"), CrosshairData.CrosshairSpread);
                HUD->SetCrosshairData(CrosshairData);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("StaticData WEAPONILLE BAD!"));

            }
            
          //  const UWeaponStaticData* WeaponData = Cast<UWeaponStaticData>(PlayerState_Ref->InventoryComp->GetEquippedItem()->GetItemStaticData());
        }
        else 
        {
            if (HUD) 
            {
                HUD->ClearCrossHairData();
            }
        }
  
    }
    else
        {

      //  UE_LOG(LogTemp, Warning, TEXT("FAILED HUD CAST"));
    }
}

void APlayerControllerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SetHUDCrosshairs(DeltaTime);

    //only meaningful for autonomyes proxys or servers' local character, not the srver

    if (IsLocalController()) {
        FHitResult HitResult;
        TraceUnderCrossHair(HitResult);
       // HitTarget = HitResult;

    }


}

APlayerControllerBase::APlayerControllerBase()
{
    /* FIND Blueprint Class */

    bTickCrossHair = false;
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/_Main/UIWidgets/OnlineMenu/WBP_MainMenu"));
    if (MenuBPClass.Class) {

        MenuClass = MenuBPClass.Class;
        UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx FOund WIDGET class: %s "), *MenuClass->GetName())
    }

    ConstructorHelpers::FClassFinder<UInGameMenu> InGameMenuBPClass(TEXT("/Game/_Main/UIWidgets/InGameMenu/WBP_InGameMenu"));
        if (InGameMenuBPClass.Class) {
            InGameMenuClass = InGameMenuBPClass.Class;
            UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx FOund WIDGET class: %s "), *InGameMenuClass->GetName())
        }
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


    if (GetLocalPlayer()) {

        UE_LOG(LogTemp, Warning, TEXT(" ======= GET LOCAL PLAYER [yes] %s =========="), *GetLocalPlayer()->GetName());
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT(" ======= GET LOCAL PLAYER [NULLPTR!!!!] =========="));
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

    FGameplayAbilityTargetingLocationInfo Info;
    FHitResult HitResult;
    TraceUnderCrossHair(HitResult);
    FVector_NetQuantize Quantized(HitResult.Location);
    HitResult.Location = Quantized;
   



    if (IsLocalController()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("PC %s: ---- ON LOCAL CONTROLLER!! [yes], Controlled Pawn: %s"), *this->GetName(), *GetPawn()->GetName());
    }
    else 
    {

        UE_LOG(LogTemp, Warning, TEXT("PC %s: ---- ei ole! LOCAL CONTROLLER!! [NO]"), *this->GetName());
    }

    /*
    
    return PC->IsLocalController();
	}
	else if (const APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		return OwnerPawn->IsLocallyControlled();
    */

    if (!HasAuthority()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("EI O PCLLA AUTHORITYA JATKAA SHOOTABILITY ACTIVATE"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("PC:lla on authority "));

    }

    if (PlayerState_Ref == nullptr)
    {
       UE_LOG(LogTemp, Warning,TEXT("Pc state on null"));
        return;
    }
    

    if (ShootAbility.IsValid()) 
    {
        UE_LOG(LogTemp, Warning, TEXT("PC ShootAbility ability spec handle on valid??:S"));
        UAbilitySystemComponent* ASC = PlayerState_Ref->GetAbilitySystemComponent();
        bool isInInstance;
        const UGameplayAbility *Abili = UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(ASC, ShootAbility, isInInstance);
       
        UE_LOG(LogTemp, Warning, TEXT("PC ShootAbility onkin"), *Abili->GetName());
    }
    else {

        UE_LOG(LogTemp, Warning, TEXT("PC ShootAbility ability spec handle ei o valid :S"));

    }

    PlayerState_Ref->GetAbilitySystemComponent()->TryActivateAbility(ShootAbility);
    
  //  EventPayload.TargetData = Info.MakeTargetDataHandleFromHitResult(nullptr, HitResult);
 //   UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, PrimaryActionStartedEventTag, EventPayload);


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

    UE_LOG(LogTemp, Warning, TEXT("> 1/2 PlayerCOntrollerBase OnInputActionJump.."));
    if (PlayerCharacter_Ref) {
        UE_LOG(LogTemp, Warning, TEXT("> 2/2 PlayerCOntrollerBase Call Try Jumpo.."));
        PlayerCharacter_Ref->TryJump();
    }
}


void APlayerControllerBase::OnMoveForwardInputAction(const struct FInputActionValue& val) {

    if (!PlayerCharacter_Ref) return;

    //  UE_LOG(LogTemp, Warning, TEXT(">OnMoveForwardInputAction."));
      //Huom, input movement suunta top down pelistä, Z ylöspäin kartassa TODO paremmaksi jos kameraa voi kääntää yms
 //   PlayerCharacter_Ref->AddMovementInput(FVector3d(1.0f, 0.0f, 0.f), val.GetMagnitude(), false);

    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)); //yaw rotations vector representation

    PlayerCharacter_Ref->AddMovementInput(Direction, val.GetMagnitude(), false);


}


void APlayerControllerBase::PostProcessInput(const float DeltaTime, const bool bGamePaused) {

  //  if (UAbilitySystemComponent* ASC = PlayerState_Ref->GetAbilitySystemComponent())
   // {
      //  ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
     //   static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
     //   AbilitiesToActivate.Reset();

        //TryActivateAbility(ShootAbility);

  //  }


    Super::PostProcessInput(DeltaTime, bGamePaused);

}

void APlayerControllerBase::BeginPlay()
{
    Super::BeginPlay();



}




void APlayerControllerBase::OnLookUpInputAction(const FInputActionValue& val)
{
    if (!PlayerCharacter_Ref)return;

    PlayerCharacter_Ref->AddControllerPitchInput(-val.GetMagnitude());
    
}

void APlayerControllerBase::OnPickupInputAction(const FInputActionValue& val)
{
    if (PlayerCharacter_Ref) 
    {
        if (PlayerCharacter_Ref->OverlappingItem) 
        {
            /* Calling from server */
            if (HasAuthority())
            {
                UE_LOG(LogTemp, Warning, TEXT("OVERLAPPING  WITH AUTHORITY!!!"));
                FGameplayEventData EventPayload;
                EventPayload.Instigator = PlayerCharacter_Ref->OverlappingItem;
                EventPayload.OptionalObject = PlayerCharacter_Ref->OverlappingItem->ItemInstance;
                EventPayload.EventTag = EquipItemActorTag;
                UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, EquipItemActorTag, EventPayload);
            }
            /* Called from client, send RPC to server */
            else 
            {
                ServerEquipItemRequested();
            }
        }
    }
}

void APlayerControllerBase::OnLookRightInputAction(const FInputActionValue& val)
{
    if (!PlayerCharacter_Ref) return;

    PlayerCharacter_Ref->AddControllerYawInput(val.GetMagnitude());
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

void APlayerControllerBase::ClearUIPointers() {

    InGameMenuPtr = nullptr;
    MainMenuPtr = nullptr;
}

void APlayerControllerBase::OnOpenGameMenuInputActionStarted(const FInputActionValue& val)
{
    if (IsValid(InGameMenuClass) && InGameMenuPtr == nullptr) {
        InGameMenuPtr = CreateWidget<UInGameMenu>(this, InGameMenuClass);
        InGameMenuPtr->Setup();
        UE_LOG(LogTemp, Warning, TEXT(">>> tehtiin ingaem menu"));
    }
    else {
        InGameMenuPtr->TearDown();
        InGameMenuPtr = nullptr;
        UE_LOG(LogTemp, Warning, TEXT(">>> EI OLE INGAME MENU CLASS VALID?."));
    }
  

}


void APlayerControllerBase::OnMoveSideInputAction(const struct FInputActionValue& val) {
    if (!PlayerCharacter_Ref) return;
    // UE_LOG(LogTemp, Warning, TEXT(">OnMoveSideInputAction."));
   // PlayerCharacter_Ref->AddMovementInput(FVector3d(0.f, 1.0f, 0.f), val.GetMagnitude(), false);

    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
    const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)); //yaw rotations vector representation

    PlayerCharacter_Ref->AddMovementInput(Direction, val.GetMagnitude(), false);



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
   // UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, AimStartedEventTag, EventPayLoad);


    PlayerState_Ref->GetAbilitySystemComponent()->TryActivateAbility(AimAbility);

}

void APlayerControllerBase::OnAimInputActionEnded(const FInputActionValue& val)
{
    FGameplayEventData EventPayLoad;
    EventPayLoad.EventTag = AimEndedEventTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, AimEndedEventTag, EventPayLoad);
 //   PlayerState_Ref->GetAbilitySystemComponent()->EndAbility()


  //  PlayerState_Ref->GetAbilitySystemComponent()->TryActivateAbility(ShootAbility);

}


void APlayerControllerBase::OnOpenMainMenuInputActionStarted(const struct FInputActionValue& val) {



    if (MainMenuPtr == nullptr) {
        MainMenuPtr = CreateWidget<UMainMenuWidget>(this, MenuClass);
        MainMenuPtr->Setup();
    }
    else
    {
        MainMenuPtr->TearDown();
        MainMenuPtr = nullptr;

    }

  

 


    /*
    UGameInstanceCrawl* GameInstance = Cast<UGameInstanceCrawl>(GetGameInstance());

    if (GameInstance) {
        UE_LOG(LogTemp, Warning, TEXT("Got game instance"));

        if (MenuClass) {

            UMainMenuWidget* MenuPtr = CreateWidget<UMainMenuWidget>(this, MenuClass);

            if (MenuPtr) {
                MenuPtr->AddToViewport();
                FInputModeUIOnly InputModeData;

                InputModeData.SetWidgetToFocus(MenuPtr->TakeWidget());
                InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                bShowMouseCursor = true;
                this->SetInputMode(InputModeData);


                MenuPtr->SetMainMenuInterface(Cast<UGameInstanceCrawl>(GetGameInstance()));
       
            }
            else {
      
            }

        }
        //   GameInstance->LoadMenu();
    }

    */
}



/* Only executed on a server */
void APlayerControllerBase::ServerEquipItemRequested_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("OVERLAPPING  WITH AUTHORITY!!!"));
    FGameplayEventData EventPayload;
    EventPayload.Instigator = PlayerCharacter_Ref->OverlappingItem;
    EventPayload.OptionalObject = PlayerCharacter_Ref->OverlappingItem->ItemInstance;
    EventPayload.EventTag = EquipItemActorTag;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter_Ref, EquipItemActorTag, EventPayload);

}

void APlayerControllerBase::ClientRPCTrace_Implementation() {

}

void APlayerControllerBase::TraceUnderCrossHair(FHitResult& TraceHitResult)
{

    /* get center of screen */
    // 1. get screen size
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f); //middle (X,Y) in VIEWPORT , SCREEN SPACE
    //We need them in WORLD space

    //Deproject 2D Screen space coordinate into 3D world space point and direction

    //Jos halutaan playerController  UGameplayStatics,getplayercontroller(this,0)

    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    ULocalPlayer* const LP = this ? this->GetLocalPlayer() : nullptr;
   
    /*
    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        this,
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection
    );


    */
    bool bScreenToWorld = true;
    /* success deproject */
    if (bScreenToWorld) 
    {
        float DistanceToCharacter = 0;
        if (PlayerCharacter_Ref) {

            CrosshairWorldPosition = Cast<ABlasterCharacter>(PlayerCharacter_Ref)->FollowCamera->GetComponentLocation();
            CrosshairWorldDirection = Cast<ABlasterCharacter>(PlayerCharacter_Ref)->FollowCamera->GetComponentRotation().Vector();

        
            DistanceToCharacter = (PlayerCharacter_Ref->GetActorLocation() - CrosshairWorldPosition).Size();
        }

        FVector Start = CrosshairWorldPosition + CrosshairWorldDirection * DistanceToCharacter;
        FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;


      //  DrawDebugLine(GetWorld(), CrosshairWorldPosition, CrosshairWorldPosition+ CrosshairWorldDirection*1000.f,FColor::White,false,5.f);
      //  DrawDebugSphere(GetWorld(), Start, 16.f, 12, FColor::Red, false);

        //perform linetrace
        GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

         /* Jos ei osu mihinkään, pistetään impactpoint end pointiks mihin tracettiin*/
         if (!TraceHitResult.bBlockingHit) 
         {
             TraceHitResult.ImpactPoint = End;
             TraceHitResult.Location = End;
         //    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.5f);
         }
         else
         {
         //    DrawDebugSphere(GetWorld(), TraceHitResult.Location, 12.f, 12.f, FColor::Yellow, false, 1.0f);
          //   DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.5f);
         }


         if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>()) 
         {
             CrosshairData.CrosshairsColor = FLinearColor::Red;
         }
         else {

             CrosshairData.CrosshairsColor = FLinearColor::White;


         }

      /* NOTE, TEST; AINA SINNE KAUKAISUUTEEN */
      //   TraceHitResult.ImpactPoint = End;

    }

}


