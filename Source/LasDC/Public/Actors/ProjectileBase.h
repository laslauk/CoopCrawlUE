// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrawlCommonTypes.h"
#include "ProjectileBase.generated.h"

UCLASS()
class LASDC_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();


	const UProjectileStaticData* GetProjectileStaticData() const;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	TSubclassOf<UProjectileStaticData> ProjectileDataClass;

	UFUNCTION()
 void OnBoxCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);


	
	/*	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

			*/
	/*
	(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult);
		*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); 


	UPROPERTY(BlueprintReadWrite)
		class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	void DebugDrawPath() const;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent = nullptr;

	UFUNCTION(BlueprintCallable)
	void OnProjectileStop(const FHitResult& ImpactResult);

	class UParticleSystemComponent* TracerParticleSystemComponent;

};
