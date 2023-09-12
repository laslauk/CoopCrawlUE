// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"
#include "CrawlCommonTypes.h"
#include "CharacterBaseGAS.h"
#include "Animation/BlendSpace.h"
#include "Character/CharacterMovementComponentBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "ItemActorBase.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Animation/AnimSequenceBase.h"
#include "CrawlCommonTypes.h"
#include "PlayerControllerBase.h"

#include "InventoryItemInstance.h"
#include "InventoryComp.h"


void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CharacterRef = Cast<ACharacterBaseGAS>(TryGetPawnOwner());
}

void UAnimInstanceBase::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterRef == nullptr) {
		CharacterRef = Cast<ACharacterBaseGAS>(TryGetPawnOwner());
	}

	/* UPDATE VARIABLES for AnimBp*/

	/* Stop if no character ref*/
	if (CharacterRef == nullptr) { return; }

	FVector Velocity = CharacterRef->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();
	bIsInAir = CharacterRef->GetCharacterMovement()->IsFalling();

	/* Jos inputtia että halutaan liikuttaa*/
	bIsAccelerating = CharacterRef->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
	bItemEquipped = CharacterRef->IsEquippingItem();
	bIsCrouched = CharacterRef->bIsCrouched;
	bIsAiming = CharacterRef->bIsAiming;
	EquippedItem = CharacterRef->GetEquippedItemActor();
	TurningInPlace = CharacterRef->TurningInPlace;
	bRotateRootBone = CharacterRef->bRotateRootBone;


	/* Aim Rotation = 0, jos kattoo suoraan Z axis of WORLD, */
	FRotator AimRotation = CharacterRef->GetBaseAimRotation(); /*AIm Rotation antaa -180 ja 180 välillä, jotain kamera, GLOBAALI EI local to character */
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(CharacterRef->GetVelocity()); //rotation from X Vector
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation);
	//UE_LOG(LogTemp, Warning, TEXT("AimRotation Yaw: %f, MovementRotation: %f"), AimRotation.Yaw, MovementRotation.Yaw);


	/* Takes shortest path -180 -> 0 -> 180 vaan shjortest*/
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
	YawOffset = DeltaRotation.Yaw*-1;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = CharacterRef->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = CharacterRef->GetAO_Yaw();
	AO_Pitch = CharacterRef->GetAO_Pitch();

	if (bItemEquipped && EquippedItem) 
	{
		USkeletalMeshComponent* ItemMesh = CharacterRef->GetEquippedWeaponMeshComponent();
		if (ItemMesh)
		{
			
			LeftHandTransform = ItemMesh->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World); //world space transform of socket
		
			//Transform to bone space of mesh
			//weapon socket is hand_r bone,
			FVector OutPosition;
			FRotator OutRotation;

			ItemMesh->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			FTransform MuzzleTipTransform = ItemMesh->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			//Get Muzzletip X factor == Direction its pointing, get it from sockets rotation
			//GetRotation -> Returns FQUat (quaternion)
			//transform it to FRotator (unreal engine thing) with ROtator()
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

		//	DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);

			//Draw line to hit target

			FTransform RightHandTransform =  CharacterRef->GetMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World); //world space transform of socket

	
			

			if (CharacterRef->GetPlayerControllerBase()) {
	

			//		RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - CharacterRef->GetPlayerControllerBase()->HitTarget.Location));
			//		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation()- CharacterRef->GetPlayerControllerBase()->HitTarget.Location), FColor::Orange);
		
	

			}



		}
		
	}
}

const UItemStaticData* UAnimInstanceBase::GetEquippedItemStaticData() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());
	UInventoryComp* InventoryComponent = character ? character->GetInventoryComponent() : nullptr;
	UInventoryItemInstance* InventoryItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;

	return InventoryItemInstance ? InventoryItemInstance->GetItemStaticData() : nullptr;
	
}


// HUOM!!! TÄÄT KUTSUTAAN NYT BLUEPRINTEISTÄ JOKA TICK :S?? huono: TODO: FIX IT!!

UBlendSpace* UAnimInstanceBase::GetLocomotionBlendspace() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());
	
	if (character) {

		if (const UItemStaticData* ItemData = GetEquippedItemStaticData()) 
		{

			if (ItemData->CharacterAnimationData.MovementBlendspace)
			{
				return ItemData->CharacterAnimationData.MovementBlendspace;
			}
		}

		//DEFAULT - NO EQUIPPED
		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) 
		{
			return data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}

	}

	return DefaultCharacterAnimDataAasset ? DefaultCharacterAnimDataAasset->CharacterAnimationData.MovementBlendspace : nullptr ;
 }


UAnimSequenceBase* UAnimInstanceBase::GetIdleAnimation() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());

	if (character) {


		if (const UItemStaticData* ItemData = GetEquippedItemStaticData()) {


			if (ItemData->CharacterAnimationData.IdleAnimationAsset)
				return ItemData->CharacterAnimationData.IdleAnimationAsset;

		}

		//DEFAULT
		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) {

			return data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}

	}

	return  DefaultCharacterAnimDataAasset ? DefaultCharacterAnimDataAasset->CharacterAnimationData.IdleAnimationAsset : nullptr;


 }




UBlendSpace* UAnimInstanceBase::GetCrouchLocomotionBlendspace() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());

	if (character) {

		if (const UItemStaticData* ItemData = GetEquippedItemStaticData()) {


			if (ItemData->CharacterAnimationData.CrouchMovementBlendSpace)
				return ItemData->CharacterAnimationData.CrouchMovementBlendSpace;

		}

		//DEFAULT
		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) {

			return data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendSpace;

		}

	}

	return DefaultCharacterAnimDataAasset ? DefaultCharacterAnimDataAasset->CharacterAnimationData.CrouchMovementBlendSpace : nullptr;
}


UAnimSequenceBase* UAnimInstanceBase::GetCrouchIdleAnimation() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());

	if (character) {


		if (const UItemStaticData* ItemData = GetEquippedItemStaticData()) {


			if (ItemData->CharacterAnimationData.CrouchIdleAnimationAsset)
				return ItemData->CharacterAnimationData.CrouchIdleAnimationAsset;

		}

		//DEFAULT
		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) {

			return data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}

	}

	return  DefaultCharacterAnimDataAasset ? DefaultCharacterAnimDataAasset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;


}