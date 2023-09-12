// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifys/AnimNotify_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {

	Super::Notify(MeshComp, Animation);
	UE_LOG(LogTemp, Warning, TEXT("Notify Event owner to send: %s"), *MeshComp->GetOwner()->GetName());
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), Payload.EventTag, Payload);


 }