// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicalMaterialBase.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class LASDC_API UPhysicalMaterialBase : public UPhysicalMaterial
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
		class  USoundBase* FootstepSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
		class  USoundBase* PointImpactSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
		UNiagaraSystem* PointImpactParticleFX = nullptr;
};
