// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "CharacterMovementComponentBase.h"
#include "Attributes/AttributeSetAiming.h"
#include "PlayerControllerBase.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "HUD/CrawlHUD.h"
#include "HUD/CharacterOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "CharacterBaseGAS.h"




APlayerStateBase::APlayerStateBase() {

	// Create ability system component, and set it to be explicitly replicated
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);


	 

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	NetUpdateFrequency = 30.0f;

	if (ASC) {
		UE_LOG(LogTemp, Warning, TEXT("ABI constructor toimii ainakin"));
	}

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent

	AttributeSetBaseComp = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSetBase"));
	AttributeSetAiming = CreateDefaultSubobject<UAttributeSetAiming>(TEXT("AttributeSetAiming"));


	//Inventory comp
	InventoryComp = CreateDefaultSubobject<UInventoryComp>(TEXT("InventoryComp"));
	InventoryComp->RegisterComponent();
	InventoryComp->SetIsReplicated(true);

	NetUpdateFrequency = 100.0f;



}
  
void APlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps ) const  {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStateBase, InventoryComp);
	DOREPLIFETIME(APlayerStateBase, playerCharacterRef);
	DOREPLIFETIME(APlayerStateBase, PlayerController);
	
}




void APlayerStateBase::BeginPlay()  {
	Super::BeginPlay();


	if (ASC) {
		UE_LOG(LogTemp, Warning, TEXT("Begin playssä ainakin abi system comp"));


		if (GetPawn())
		{
			UE_LOG(LogTemp, Warning, TEXT("PLAYER STATE CONTROLLING PAWN: %s"), *GetPawn()->GetName());
			Cast<ACharacterBaseGAS>(GetPawn())->CharacterMovementComponent->SetupAbilitySystem(ASC);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAYESSÄ EI GETPAWN()"));
		}


		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(TEXT("Movement.EnforcedStrafe"),
				EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &APlayerStateBase::OnEnforcedStrafeTagChanged);

		//getgameplaatytirbute palauttaa delegaten johon voi bindata, kutsutaan automatic kun attribute changes.

		//antaa  FOnAttributeChangeData parametrin missä on ,new, old vale ja gameplay effect mod callback data.

		/*
		HealthChangedDelegateHandle = ASC->
			GetGameplayAttributeValueChangeDelegate(
				AttributeSetBaseComp->GetHealthAttribute()
			).AddUObject(
				this, &APlayerStateBase::HealthChanged);
			*/
	
	

		// Attribute change callbacks
		HealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetHealthAttribute()).AddUObject(this, &APlayerStateBase::HealthChanged);
		MaxHealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetMaxHealthAttribute()).AddUObject(this, &APlayerStateBase::MaxHealthChanged);


		UpdateHUD();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("VITTU EI BEGIN PALY"));
	}
}

void APlayerStateBase::UpdateHUD() {
	UE_LOG(LogTemp, Warning, TEXT("Updating HUD"));
	SetHUDHealth(GetHealth(), GetMaxHealth());
}


void APlayerStateBase::SetHUDHealth(float Health, float MaxHealth)
{

	if (!PlayerController) {
		UE_LOG(LogTemp, Warning, TEXT("------ Returning from SetHUD"));
		return;
	}

	CrawlHUD = CrawlHUD == nullptr ? Cast<ACrawlHUD>(PlayerController->GetHUD()) : CrawlHUD;

	
	bool bHUDValid = CrawlHUD &&
		CrawlHUD->CharacterOverlay &&
		CrawlHUD->CharacterOverlay->HealthBar &&
		CrawlHUD->CharacterOverlay->HealthTextValue;

		UE_LOG(LogTemp, Warning, TEXT("------ Returning from SetHUD"));

	if (bHUDValid) 
	{
		const float HealthPercentage = Health / MaxHealth;
		CrawlHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);



		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		CrawlHUD->CharacterOverlay->HealthTextValue->SetText(FText::FromString(HealthText));
		UE_LOG(LogTemp, Warning, TEXT("------ SET HUD HELTH"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("------ bHUDVALID not valid :("));
	}


}

void APlayerStateBase::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (HasAuthority())
	{
		
		Cast<ACharacterBaseGAS>(playerCharacterRef)->CharacterMovementComponent->OnEnforcedStrafeTagChanged(CallbackTag, NewCount);

	}
	
}


void APlayerStateBase::OnGagaGigeli(float a, float b) {
	UE_LOG(LogTemp, Warning, TEXT("Hello: %f, %f"), a, b);
}


APawn* APlayerStateBase::GetControlledPawn()
{
	return playerCharacterRef;
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const {
	return ASC;
}

UAttributeSetBase* APlayerStateBase::GetAttributeSetBase() const {

	return AttributeSetBaseComp;
}



float APlayerStateBase::GetHealth() const
{
	return AttributeSetBaseComp->GetHealth();
}

void APlayerStateBase::HealthChanged(const FOnAttributeChangeData& Data) {


//	ACharacterBaseGAS* actor = (ACharacterBaseGAS*)(ASC->GetAvatarActor());
//	actor->BP_HealthChanged(Data.NewValue, GetMaxHealth());


	SetHUDHealth(Data.NewValue, GetMaxHealth());

	/*

	if (GetHealth() <= 0.0) {
		Cast<ACharacterBaseGAS>(GetPawn())->Die();
	}
	*/


}


void APlayerStateBase::MaxHealthChanged(const FOnAttributeChangeData& Data) {
	ACharacterBaseGAS* actor = (ACharacterBaseGAS*)(ASC->GetAvatarActor());
	//actor->BP_HealthChanged(Data.NewValue);


}


float APlayerStateBase::GetMaxHealth() const
{
	return AttributeSetBaseComp->GetMaxHealth();
}


/*
void APlayerStateBase::AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire) {

	if (AbilitySystemComp) {

		if (HasAuthority() && AbilityToAcquire) {
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(AbilityToAcquire, 1, 0));
		}


		AbilitySystemComp->InitAbilityActorInfo(this, this);

	}
}
*/