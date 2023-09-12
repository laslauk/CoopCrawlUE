// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Character/CharacterMovementComponentBase.h"
#include "CrawlCommonTypes.h"
#include "ItemActorBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"



ABlasterCharacter::ABlasterCharacter(const FObjectInitializer& ObjectInitializer): ACharacterBaseGAS(ObjectInitializer)
{


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	/* Laitetaan nyt MESHin alle, jos menee kyykkyyn niin spring arm liikkuu mukana*/
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = CameraTargetArmLength;
	CameraBoom->bUsePawnControlRotation = true;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);


	bUseControllerRotationYaw = false;
	//GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;



	if (FollowCamera) {
		DefaultFOV = GetFollowCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
	 
	CameraHideCharTreshold = 200.f;

	
}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HideCharacterIfCameraClose();
	const float InterpSpeed = 15.f;


		if (bIsAiming) 
		{

			CurrentFOV = FMath::FInterpTo(CurrentFOV, 80.f, DeltaTime, ZoomInterpSpeed);
			FVector SocketOffsetCurrent = FMath::VInterpTo(CameraBoom->SocketOffset, FVector(0.f, 150.f, -25.f), DeltaTime, InterpSpeed);
			float  TargetArmLengthCurrent = FMath::FInterpTo(CameraBoom->TargetArmLength, 400.f, DeltaTime, InterpSpeed);
			CameraBoom->SocketOffset = SocketOffsetCurrent;
			CameraBoom->TargetArmLength = TargetArmLengthCurrent;
		}
		else 
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
			FVector SocketOffsetCurrent = FMath::VInterpTo(CameraBoom->SocketOffset, FVector(0.f,0.f,0.f), DeltaTime, InterpSpeed);
			float  TargetArmLengthCurrent = FMath::FInterpTo(CameraBoom->TargetArmLength,700.f, DeltaTime, InterpSpeed);
			CameraBoom->SocketOffset = SocketOffsetCurrent;
			CameraBoom->TargetArmLength = TargetArmLengthCurrent;
		}

		if (FollowCamera) 
		{
			FollowCamera->SetFieldOfView(CurrentFOV);
		}
	

}

void ABlasterCharacter::SetCameraAimMode(ECameraAimMode AimMode) {
	switch (AimMode) {

	case ECameraAimMode::None:
	{
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);
		CameraBoom->TargetArmLength = 700.f;
		break;
	}
	case ECameraAimMode::CarryingWeapon:
	{
		CameraBoom->SocketOffset = FVector(0.f, 150.f, -25);
		CameraBoom->TargetArmLength = 550;


		break;
	}

	case ECameraAimMode::Aiming:
	{
		CameraBoom->SocketOffset = FVector(0.f, 75.f, -25.f);
		CameraBoom->TargetArmLength = 400.f;
		break;
	}

	default:
		break;



	}
}

void ABlasterCharacter::HideCharacterIfCameraClose()
{


	if (!IsLocallyControlled()) {
		return;
	}

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraHideCharTreshold) {
		GetMesh()->SetVisibility(false);
		if (GetEquippedItemActor()) {
			GetEquippedWeaponMeshComponent()->bOwnerNoSee = true;
		}
	}
	else {
		GetMesh()->SetVisibility(true);
		if (GetEquippedItemActor()) {
			GetEquippedWeaponMeshComponent()->bOwnerNoSee = false;
		}

	}
}
