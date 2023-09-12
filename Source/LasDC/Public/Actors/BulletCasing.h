// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCasing.generated.h"

UCLASS()
class LASDC_API ABulletCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletCasing();

	void OnTimerEnded();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* 
			OtherActor, UPrimitiveComponent* OtherComp, 
			FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
		float ShellEjectionImpulse;

	bool bTriggered;

	UPROPERTY(EditAnywhere)
		class USoundCue* ShellHitGroundSound;


	FTimerHandle DestroyTimerHandle;


	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CapsuleComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CasingMesh;




};
