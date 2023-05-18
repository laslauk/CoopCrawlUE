// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrawlCommonTypes.h"
#include "FootstepsComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASDC_API UFootstepsComponent : public UActorComponent 
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootstepsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="FootstepComp")
		FName LeftFootSocketName = TEXT("foot_l");


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FootstepComp")
		FName RightFootSocketName = TEXT("foot_r");

public:	


	void HandleFootstep(EFoot Foot);
		
};
