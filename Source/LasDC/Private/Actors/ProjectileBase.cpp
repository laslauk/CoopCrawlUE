// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "CrawlGameStatics.h"
#include "NiagaraFunctionLibrary.h"


static TAutoConsoleVariable<int32> CVarShowProjectiles(
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("draws debug info aboutr projectile")
	TEXT(" 0: off/n")
	TEXT("0 on/n"),
	ECVF_Cheat
);

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(true);
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectileBase::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName("Projectile");
	StaticMeshComponent->bReceivesDecals = false;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData && ProjectileMovementComponent) {
		if (ProjectileData->StaticMesh) {
			StaticMeshComponent->SetStaticMesh(ProjectileData->StaticMesh);
		}
		
		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileData->MaxSpeed;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.0f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileData->GravityMultiplier;


		ProjectileMovementComponent->Velocity = ProjectileData->InitialSpeed * GetActorForwardVector(); //assume proper transofrmation appleid before BeginPlay()
	}

	//Debug
	const int32 DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();
	if (DebugShowProjectile) DebugDrawPath();
	
}

const UProjectileStaticData* AProjectileBase::GetProjectileStaticData() const {
	if (IsValid(ProjectileDataClass)) {

		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}
	return nullptr;
}




void AProjectileBase::EndPlay(const EEndPlayReason::Type EndPLayReason) {

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();

	if (ProjectileData) {
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->OnStopSFX, GetActorLocation(), 1.f);    
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->OnStopVFX, GetActorLocation());

	}

	Super::EndPlay(EndPLayReason);

}



void AProjectileBase::OnProjectileStop(const FHitResult& ImpactResult) {

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();

	if(ProjectileData) {

		UCrawlGameStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
			ProjectileData->DamageRadius,
			ProjectileData->BaseDamage,
			ProjectileData->EffectsToApplyOnHit,
			ProjectileData->RadialDamageQueryTypes,
			ProjectileData->RadialDamageTraceType,
			{} //< Actors To Ignore
			);

	}

	Destroy();
}


void AProjectileBase::DebugDrawPath() const {
	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();

	if (ProjectileData) {

		FPredictProjectilePathParams PredictProjectilePathParams;
		PredictProjectilePathParams.StartLocation = GetActorLocation();
		PredictProjectilePathParams.LaunchVelocity = ProjectileData->InitialSpeed * GetActorForwardVector();
		PredictProjectilePathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
		PredictProjectilePathParams.bTraceComplex = true;
		PredictProjectilePathParams.bTraceWithCollision = true;
		PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictProjectilePathParams.DrawDebugTime = 3.0f;
		PredictProjectilePathParams.OverrideGravityZ = ProjectileData->GravityMultiplier == 0.f ? 0.0001 : ProjectileData->GravityMultiplier;

		FPredictProjectilePathResult PredictProjectilePathResult;

		//Do a prediction
		if (UGameplayStatics::PredictProjectilePath(this, PredictProjectilePathParams, PredictProjectilePathResult)) {

		}


	}

}

//GetLifeTimeReplicatedProps ei tarvi declaree headerissä tulee  "Net/UnrealNetWorkh."
/*



*/
void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AProjectileBase, ProjectileDataClass);

}