// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponentBase.h"

#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"



static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"), 0, TEXT("Draws debug info about traversal") TEXT("  0: off/n") TEXT("  1: on/n"), ECVF_Cheat
);



bool UCharacterMovementComponentBase::TryTraversal(UAbilitySystemComponent* ASC) {



	
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true)) {
			FGameplayAbilitySpec* Spec;

			Spec = ASC->FindAbilitySpecFromClass(AbilityClass); //nyt käytetään ability classeja checkaa mitä käytetää

			if (Spec && Spec->IsActive()) {
				return true;
			}
		}
	}

	

	return false;

}

void UCharacterMovementComponentBase::BeginPlay() {
	Super::BeginPlay();

	HandleMovementDirection();

	//subscribe to tag changed event
	// use RegisterGameplayTagEvent for a delegate when gameplay tags added or removed.
	// params: 
	/*
		1. EGameplayTagEvent Type - specife when to fire, added/removed, both
		2. ja siihen mikä funktio kutsutaan .AddUObject(this, &AGDPlayerState::StunTagChanged);

	*/
	//item instance handlaa kun equippaa niin lisää sen efektin

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())) {
		ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.EnforcedStrafe"), EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UCharacterMovementComponentBase::OnEnforcedStrafeTagChanged);

	}


 }


EMovementDirectionType UCharacterMovementComponentBase::GetMovementDirectionType() const {
	return MovementDirectionType;
}

void UCharacterMovementComponentBase::SetMovementDirectionType(EMovementDirectionType NewType) {
	MovementDirectionType = NewType;
	HandleMovementDirection();
}


void UCharacterMovementComponentBase::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount) {


	if (NewCount > 0) {
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else {
		SetMovementDirectionType(EMovementDirectionType::OrientRotationToMovement);
	}

}


void UCharacterMovementComponentBase::HandleMovementDirection() {

	switch (MovementDirectionType) {
	case EMovementDirectionType::None: 
		break;


	case EMovementDirectionType::OrientRotationToMovement:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;

	case EMovementDirectionType::Strafe: {
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
	}
		break;

	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;

	}

}