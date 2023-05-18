// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FootStep.h"
#include "CharacterbaseGAS.h"
#include "FootstepsComponent.h"

void UAnimNotify_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	Super::Notify(MeshComp, Animation);

	check(MeshComp);
	ACharacterBaseGAS* character = MeshComp ? Cast<ACharacterBaseGAS>(MeshComp->GetOwner()) : nullptr;

	if (character) {

		if (UFootstepsComponent* FootstepsComponent = character->GetFootStepsComponent()) {
			FootstepsComponent->HandleFootstep(Foot);
		}
	
	}
 }