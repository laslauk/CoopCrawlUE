// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BulletCasing.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
#include "Sound/SoundCue.h" //sound cuye
#include "Components/CapsuleComponent.h"

// Sets default values
ABulletCasing::ABulletCasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	
	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));

	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);

	/* Phyiscs simulation needs to set OnHit Events, We using Phyiscs- physics simulation geenrates hitevents*/
	CasingMesh->SetNotifyRigidBodyCollision(true); //== in Blueprints =Simulation Generates Hit Events

	ShellEjectionImpulse = 1000.f;
	bTriggered = false;

}

void ABulletCasing::OnTimerEnded() {

	Destroy();

}

void ABulletCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// do only once
	if (bTriggered) return;

	UE_LOG(LogTemp, Warning, TEXT("ON HUIT shell soudn"));
	if (ShellHitGroundSound) {
		UE_LOG(LogTemp, Warning, TEXT("Playing shell soudn"));
		UGameplayStatics::PlaySoundAtLocation(this, ShellHitGroundSound, GetActorLocation());
	}

	if (DestroyTimerHandle.IsValid()) {
		DestroyTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABulletCasing::OnTimerEnded, 3.0f, false);
	}
	
	bTriggered = true;
	CasingMesh->OnComponentHit.RemoveDynamic(this, &ABulletCasing::OnHit);





}

// Called when the game starts or when spawned
void ABulletCasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->AddImpulse(GetActorRightVector() * ShellEjectionImpulse);
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABulletCasing::OnTimerEnded, 6.0f, false);
	CasingMesh->OnComponentHit.AddDynamic(this, &ABulletCasing::OnHit);

	float RandomRotation = FMath::RandRange(-100.f, 100.f);
	SetActorRotation(GetActorRotation() + FRotator(RandomRotation), ETeleportType::None);

}

