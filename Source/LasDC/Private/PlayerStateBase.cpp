// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
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


	//Inventory comp
	InventoryComp = CreateDefaultSubobject<UInventoryComp>(TEXT("InventoryComp"));
	InventoryComp->SetIsReplicated(true);



}
  
void APlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps ) const  {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStateBase, InventoryComp);
	DOREPLIFETIME(APlayerStateBase, playerCharacterRef);
	
}


void APlayerStateBase::BeginPlay()  {
	Super::BeginPlay();


	if (ASC) {
		UE_LOG(LogTemp, Warning, TEXT("Begin playssä ainakin abi system comp"));
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
		//HealthChangedDelegateHandle = AbilitySystemComp ->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetHealthAttribute()).AddUObject(this, &APlayerStateBase::HealthChanged);
		//MaxHealthChangedDelegateHandle = AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetMaxHealthAttribute()).AddUObject(this, &APlayerStateBase::MaxHealthChanged);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("VITTU EI BEGIN PALY"));
	}
}


void APlayerStateBase::OnGagaGigeli(float a, float b) {
	UE_LOG(LogTemp, Warning, TEXT("Hello: %f, %f"), a, b);
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
	ACharacterBaseGAS* actor = (ACharacterBaseGAS*)(ASC->GetAvatarActor());
	actor->BP_HealthChanged(Data.NewValue, GetMaxHealth());


	if (GetHealth() <= 0.0) {
		Cast<ACharacterBaseGAS>(GetPawn())->Die();
	}



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