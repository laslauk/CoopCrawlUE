// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceBase.h"
#include "CrawlCommonTypes.h"
#include "CharacterBaseGAS.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Animation/AnimSequenceBase.h"





UBlendSpace* UAnimInstanceBase::GetLocomotionBlendspace() const {

	ACharacterBaseGAS* character = Cast<ACharacterBaseGAS>(GetOwningActor());
	
	if (character) {
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

		FCharacterData data = character->GetCharacterData();
		if (data.CharacterAnimDataAsset) {

			return data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}

	}

	return  DefaultCharacterAnimDataAasset ? DefaultCharacterAnimDataAasset->CharacterAnimationData.IdleAnimationAsset : nullptr;


 }