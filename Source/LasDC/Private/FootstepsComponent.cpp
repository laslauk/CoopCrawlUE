// Fill out your copyright notice in the Description page of Project Settings.

#include "FootstepsComponent.h"
#include "PhysicalMaterialBase.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBaseGAS.h"
// Sets default values for this component's properties

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootsteps"), 
	0,
	TEXT("Draws debug info about footstepts")
	TEXT("  0: off/n")
	TEXT("  1:on/n"),
	ECVF_Cheat);

UFootstepsComponent::UFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UFootstepsComponent::HandleFootstep(EFoot foot) {
	if (ACharacterBaseGAS* Character = Cast<ACharacterBaseGAS>(GetOwner())) {


		const int32 DebugShowFootSteps = CVarShowFootsteps.GetValueOnAnyThread();


		if (USkeletalMeshComponent* mesh = Character->GetMesh()) {
			FHitResult HitResult;
			const FVector SocketLocation = mesh->GetSocketLocation(foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector location = SocketLocation + FVector::UpVector * 20;

			 



			// Line trace from socket location to down
			FCollisionQueryParams queryParams;
			queryParams.bReturnPhysicalMaterial = true;
			queryParams.AddIgnoredActor(Character);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, location, location + FVector::UpVector * -50.f, ECollisionChannel::ECC_WorldStatic, queryParams)) {

				if (HitResult.bBlockingHit) {
					if (HitResult.PhysMaterial.Get()) {

						UPhysicalMaterialBase* PhysMat = Cast<UPhysicalMaterialBase>(HitResult.PhysMaterial.Get());

						if (PhysMat) {

							UGameplayStatics::PlaySoundAtLocation(this, PhysMat->FootstepSound, location, 1.f);

							if (DebugShowFootSteps > 0) {

								DrawDebugString(GetWorld(), location, PhysMat->GetName(), nullptr, FColor::White, 4.f);
							}
						}

						if (DebugShowFootSteps > 0) {
							DrawDebugSphere(GetWorld(), location, 16, 16, FColor::Red, false, 4.0f);
						}
					}

					else {

						if (DebugShowFootSteps > 0) {
							DrawDebugLine(GetWorld(), location, location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
						}
					}

				}
				else {
					if (DebugShowFootSteps > 0) {
						DrawDebugLine(GetWorld(), location, location + FVector::UpVector * -50.f, FColor::Red, false, 4, 0, 1);
						DrawDebugSphere(GetWorld(), location, 16, 16, FColor::Red, false, 4.0f);
					}
				}
			}
		}
	}
}