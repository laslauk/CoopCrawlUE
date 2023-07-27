// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformTrigger.generated.h"

class AMovingPlatform;
class UBoxComponent;

UCLASS()
class LASDC_API APlatformTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformTrigger();


		

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//EditAnywhere --> AIHEUTTAA ETT� PTR ON EDIT!! EI content, Halutaan ett� Visible anywhere
	UPROPERTY(VisibleAnywhere, Category = "PlatformTrigger")
		UBoxComponent* BoxComponent = nullptr;

	UFUNCTION()
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;


	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/* Linked moving paltforms*/
	UPROPERTY(EditAnywhere)
	TArray<AMovingPlatform*> PlatformsToTrigger;

};
