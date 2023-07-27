// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemActorBase.h"
#include "AmmoItemActor.generated.h"

class UAmmoItemStaticData;
/**
 * 
 */
UCLASS()
class LASDC_API AAmmoItemActor : public AItemActorBase
{
	GENERATED_BODY()


public:

	const UAmmoItemStaticData* GetAmmoItemStaticData() const;


protected:

	UPROPERTY()
		UMeshComponent* MeshComponent = nullptr;


	virtual void InitInternal() override;

	
};
