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

	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<UProjectileStaticData> ProjectileDataClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UPROPERTY()
		class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	void DebugDrawPath() const;

	UPROPERTY()

		class UStaticMeshComponent* StaticMeshComponent = nullptr;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);



};
