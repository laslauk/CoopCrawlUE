// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "CharacterBaseGAS.h"
#include "PlayerStateBase.h"
#include "CharacterMovementComponentBase.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"

UAttributeSetBase::UAttributeSetBase(){

}


void UAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) {

	Super::PostGameplayEffectExecute(Data);


	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle health  changes.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		ACharacterBaseGAS* OwningAvatarChar = Cast<ACharacterBaseGAS>(GetOwningAbilitySystemComponent()->GetAvatarActor());

		if (OwningAvatarChar) {

			OwningAvatarChar->MulticastDamageReact();

		}
		/* PLAY HIT REACTION TÄÄLLÄ ?*/

	}

	else if (Data.EvaluatedData.Attribute == GetMaxMovementSpeedAttribute()) {

		ACharacter* OwningAvatarChar = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());

		if (!OwningAvatarChar) return;
		UE_LOG(LogTemp, Warning, TEXT("Hello OwningAvatarChar"));
		UCharacterMovementComponent* CharacterMovement = OwningAvatarChar ? OwningAvatarChar->GetCharacterMovement() : nullptr;


		if (!CharacterMovement) return;
		UE_LOG(LogTemp, Warning, TEXT("Hello CharacterMovement"));
	

		if (CharacterMovement) {
			const float MaxSpeed = GetMaxMovementSpeed();
			UE_LOG(LogTemp, Warning, TEXT("Hello CharacterMovement: %s , %f"), *OwningAvatarChar->GetName(), MaxSpeed);
			CharacterMovement->MaxWalkSpeed = MaxSpeed;
			SetMaxMovementSpeed(MaxSpeed);
		}


	}
}




// All replicated attributes must always notify about their replication, even if they didnt' change, because GAS requirers this to handle client side prediction!
void UAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const  {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//REPNOTIFY_Always tells the OnRep function to trigger if the local value is already equal to the value being repped down from the Server (due to prediction). 

	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxMovementSpeed, COND_None, REPNOTIFY_Always);
	//TÄNNE JOS TULEE LISÄÄ ATTRIBUTJEA NI NIILLE OMA MACRO
}






/* ON REP */
void UAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Health, OldHealth);
}

void UAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Stamina, OldStamina);
}

void UAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxStamina, OldMaxStamina);
}


void UAttributeSetBase::OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT ON REP MAX MOVEMENT SPED: %f, %f"), MaxMovementSpeed.GetCurrentValue(), OldMaxMovementSpeed.GetCurrentValue());

	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxMovementSpeed, OldMaxMovementSpeed);
	ACharacter* chararcter = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());


	AActor* owner = (GetOwningAbilitySystemComponent()->GetOwner());
	if (owner) {

		UE_LOG(LogTemp, Warning, TEXT("ON SENTÄÄ OWNER %s"), *owner->GetName());
		if (ACharacter* playerCharref = Cast<APlayerStateBase>(owner)->playerCharacterRef) {
			UE_LOG(LogTemp, Warning, TEXT("ON SENTÄÄ OWNER CHARACTER %s"), *playerCharref->GetName());

		}

	}
	else {

	}
	if (chararcter) {
		chararcter->GetCharacterMovement()->MaxWalkSpeed =  MaxMovementSpeed.GetCurrentValue();
		UE_LOG(LogTemp, Warning, TEXT("Max moveent speed set for: %s, AS:  %f"), *chararcter->GetName(), chararcter->GetCharacterMovement()->MaxWalkSpeed);
	}
	else {


		if (ACharacter* playerCharref = Cast<APlayerStateBase>(owner)->playerCharacterRef) {

			UE_LOG(LogTemp, Warning, TEXT("------- On sentään PlayerCharacter Ref olemassa"));
			playerCharref->GetCharacterMovement()->MaxWalkSpeed = MaxMovementSpeed.GetCurrentValue();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("CHARACTERON VITTU NULL SAATANA"));
		}
	
	}



	
}