// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h" //spawnEMitterATtached()

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"

/* Particles for tracer */
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

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

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));


	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);



	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;

	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(CollisionBox);
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	//StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName("Projectile");
	StaticMeshComponent->bReceivesDecals = false;


	/* TODO, check onko tää performance hit olla myös */
//	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectileBase::OnProjectileStop);

	

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData && ProjectileMovementComponent) {
		if (ProjectileData->StaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(ProjectileData->StaticMesh);
		}
		
		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileData->MaxSpeed;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.0f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileData->GravityMultiplier;


		ProjectileMovementComponent->Velocity = ProjectileData->InitialSpeed * GetActorForwardVector(); //assume proper transofrmation appleid before BeginPlay()

		if (ProjectileData->TracerParticleSystem) 
		{
			TracerParticleSystemComponent  = UGameplayStatics::SpawnEmitterAttached(
				ProjectileData->TracerParticleSystem,
				StaticMeshComponent,
				FName(), //For attaching to bones
				GetActorLocation(),
				GetActorRotation(),
				EAttachLocation::KeepWorldPosition
				
			);

		}
	}

	//Debug
	const int32 DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();
	if (DebugShowProjectile) DebugDrawPath();

	/* server handles Onhit stuff like dm,g */
	if (HasAuthority()) 
	{
	//	CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnBoxCollisionBeginOverlap);

	}

}

const UProjectileStaticData* AProjectileBase::GetProjectileStaticData() const {
	if (IsValid(ProjectileDataClass)) {

		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}
	return nullptr;
}



/* CALLED WHEN DESTROYED, SPAWN SHIT AT LOCATION */
void AProjectileBase::EndPlay(const EEndPlayReason::Type EndPLayReason) {

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();

	if (ProjectileData) {
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->OnStopSFX, GetActorLocation(), 1.f);    
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->OnStopVFX, GetActorLocation());

	}

	Super::EndPlay(EndPLayReason);

}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("On HIT! projectile"));
	
	/* Play sound, add particle fx*/
	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	if (ProjectileData)
	{
		if (ProjectileData->bHasSplash) {
			UCrawlGameStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
				ProjectileData->DamageRadius,
				ProjectileData->BaseDamage,
				ProjectileData->EffectsToApplyOnHit,
				ProjectileData->RadialDamageQueryTypes,
				ProjectileData->RadialDamageTraceType,
				{} //< Actors To Ignore
			);

		}
		else {

			//ENDISSÄ PLAYTAAN SOUNDFX JA FX
			UCrawlGameStatics::ApplyDirectDamage(
				this,
				GetOwner(),
				OtherActor,
				ProjectileData->BaseDamage,
				ProjectileData->EffectsToApplyOnHit
			);

		}
	}
	Destroy();
	UE_LOG(LogTemp, Warning, TEXT("HIT TARGET: !!!!!!! %s"), *OtherActor->GetName());
}



void AProjectileBase::OnProjectileStop(const FHitResult& ImpactResult) {

	const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
	UE_LOG(LogTemp, Warning, TEXT("On Projectile Stop! projectile"));
	if(ProjectileData) {

		if (ProjectileData->bHasSplash) {
			UE_LOG(LogTemp, Warning, TEXT("On Projectile Stop! has splasH+"));
			UCrawlGameStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
				ProjectileData->DamageRadius,
				ProjectileData->BaseDamage,
				ProjectileData->EffectsToApplyOnHit,
				ProjectileData->RadialDamageQueryTypes,
				ProjectileData->RadialDamageTraceType,
				{} //< Actors To Ignore
			);
		}
		else
		{


		}


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

void AProjectileBase::OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator() != OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginOverlap Instigator: %s ---> Other actor: %s"), *GetInstigator()->GetName(), *OtherActor->GetName());
		OnHit(OverlappedComp, OtherActor, OtherComp, FVector::ZeroVector, SweepResult);
	}

}
