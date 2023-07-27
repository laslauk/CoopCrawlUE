// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"
#include "CrawlCommonTypes.h"
#include "CharacterBaseGAS.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Animation/AnimSequenceBase.h"
#include "CrawlCommonTypes.h"
#include "InventoryItemInstance.h"
#include "InventoryComp.h"


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

		if (const UItemStaticData* ItemData = GetEquippedItemStaticData()) {


			if (ItemData->CharacterAnimationData.MovementBlendspace)
				return ItemData->CharacterAnimationData.MovementBlendspace;

		}

		//DEFAULT
		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) {

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