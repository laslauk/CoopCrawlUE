// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/AttributeSetAiming.h"
#include "Net/UnrealNetwork.h"
#include "CharacterBaseGAS.h"
#include "PlayerStateBase.h"
#include "CharacterMovementComponentBase.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"

void UAttributeSetAiming::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//REPNOTIFY_Always tells the OnRep function to trigger if the local value is already equal to the
	//value being repped down from the Server (due to prediction)

	/*TODO: TESTATAA JOS SPREADII EI TARVIS REPLIKOIDA ENTISSÄ*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetAiming, AimSpread, COND_None, REPNOTIFY_Always);
}

void UAttributeSetAiming::OnRep_AimSpread(const FGameplayAttributeData& OldAimSpread)
{
	//For prediction system
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetAiming, AimSpread, OldAimSpread);

} 
