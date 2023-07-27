// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	if (HasAuthority()) {
		bReplicates = true;
	
	}
}

void AMovingPlatform::AddActiveTrigger()
{
	if (HasAuthority()) {
		ActiveTriggers++;
	}
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (HasAuthority()) {
		if (ActiveTriggers > 0) {

			ActiveTriggers--;
		}
	}
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);

	if (HasAuthority()) {
		SetReplicateMovement(true);

	}
	
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//liikutetaa servulla, replikointi updates clients

	if (ActiveTriggers > 0) {

		if (HasAuthority()) {
			//TargetLocation is set by widget, but is read as local space

			//basic Transform goes from Local -> Global
			// Inverse: Global -> local


			FVector Location = GetActorLocation();
			float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
			float JourneyTravelled = (Location - GlobalStartLocation).Size();


			if (JourneyTravelled >= JourneyLength) {
				FVector Swap = GlobalStartLocation;
				GlobalStartLocation = GlobalTargetLocation;
				GlobalTargetLocation = Swap;
			}

			FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
			Location += Speed * Direction * DeltaTime;
			SetActorLocation(Location);

		}

	}

}

