// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlatformTrigger.h"
#include "Actors/MovingPlatform.h"
#include "Components/BoxComponent.h"

// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	if (BoxComponent) {
		RootComponent = BoxComponent;
	}

	BoxComponent->RegisterComponent();

	/* Register callbacks */
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapEnd);


}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/* Callback functions*/
void APlatformTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("Activated"));

		for (auto Platform : PlatformsToTrigger) {
			Platform->AddActiveTrigger();
		}

	}

}

void APlatformTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority()) {
		for (auto Platform : PlatformsToTrigger) {
			Platform->RemoveActiveTrigger();
		}

	}


}

