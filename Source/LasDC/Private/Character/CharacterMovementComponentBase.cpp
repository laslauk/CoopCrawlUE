// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponentBase.h"

#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"



static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"), 0, TEXT("Draws debug info about traversal") TEXT("  0: off/n") TEXT("  1: on/n"), ECVF_Cheat
);



bool UCharacterMovementComponentBase::TryTraversal(UAbilitySystemComponent* ASC) {



	UE_LOG(LogTemp, Warning, TEXT("1/3 TryTraversal..."));
	for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true)) {
			FGameplayAbilitySpec* Spec;
			UE_LOG(LogTemp, Warning, TEXT("2/3 TryACtivate ability..."));
			Spec = ASC->FindAbilitySpecFromClass(AbilityClass); //nyt k‰ytet‰‰n ability classeja checkaa mit‰ k‰ytet‰‰

			if (Spec && Spec->IsActive()) {
				UE_LOG(LogTemp, Warning, TEXT("3/3 True activate ..."));
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
		2. ja siihen mik‰ funktio kutsutaan .AddUObject(this, &AGDPlayerState::StunTagChanged);

	*/
	//item instance handlaa kun equippaa niin lis‰‰ sen efektin



 }


void UCharacterMovementComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterMovementComponentBase, MovementDirectionType);


}


void UCharacterMovementComponentBase::SetupAbilitySystem(UAbilitySystemComponent* ASC)

{
	/*
	if (ASC)
	{

		UE_LOG(LogTemp, Warning, TEXT("---------------- HANDLE MOVEMENT: "), *ASC->GetOwner()->GetName());

		ASC->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(TEXT("Movement.EnforcedStrafe"),
				EGameplayTagEventType::NewOrRemoved))
			.AddUObject(this, &UCharacterMovementComponentBase::OnEnforcedStrafeTagChanged);

	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("-------XXXXXXXXXX--------- BeginPlay CharacterMoivement ocpmonent FAILED{FAILED}}: "));



	}
	*/
}




EMovementDirectionType UCharacterMovementComponentBase::GetMovementDirectionType() const {
	return MovementDirectionType;
}

void UCharacterMovementComponentBase::SetMovementDirectionType(EMovementDirectionType NewType) {
	
	
	//UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> setting movement type to: %d with role { %d }"), NewType, GetOwner()->GetRemoteRole());

	MovementDirectionType = NewType;
	//UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> movement type now  %d "), MovementDirectionType);

	HandleMovementDirection();
}




void UCharacterMovementComponentBase::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount) {


	if (NewCount)
	{
		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> OnEnforcedStrafeTagChanged: SETTING TO STRAFE "));
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else 
	
	{
		SetMovementDirectionType(EMovementDirectionType::None);
	}

}


void UCharacterMovementComponentBase::HandleMovementDirection() {


	if (GetOwner())
	{
		
		if (GetOwner()->GetRemoteRole() == ROLE_Authority)
		{
			// This code is running on the server
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> This code is running on the server "));
		}
		else if (GetOwner()->GetRemoteRole() == ROLE_AutonomousProxy)
		{
			// This code is running on the client that owns the owning actor
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> This code is running on the client that owns the owning actor "));
		}
		else if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
		{
			// This code is running on a remote client (not the owner)
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> This code is running on a remote client (not the owner)"));
		}

	}
	else {

		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>  DIDNT GET OWNER <<<<<<<<"));
	}


	UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> SETTING MOVEMENT DIR TYPE FOR SWITCH:  %d and Character %s"), MovementDirectionType, *GetOwner()->GetName());
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Orange, FString::Printf(TEXT(">>>>>>>> SETTING MOVEMENT DIR TYPE FOR SWITCH:  %d "), MovementDirectionType));

	switch (MovementDirectionType) 
	{
	case EMovementDirectionType::None: 
	//	bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;


	case EMovementDirectionType::OrientRotationToMovement:
		UE_LOG(LogTemp, Warning, TEXT("Setting  Movement Direction: >OrientRotationToMovement "));
	//	bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		//bUseControllerRotationYaw = true;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;

	case EMovementDirectionType::Strafe: {
		UE_LOG(LogTemp, Warning, TEXT("Setting  Movement Direction: >Strafe "));
	//	bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
	}
		break;

	default:
	//	bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;

	}

}

void UCharacterMovementComponentBase::OnRep_MovementDirectionType()
{
	HandleMovementDirection();
}
