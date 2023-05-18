// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrawlCommonTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootStep.generated.h"

/**
 * 
 */
UCLASS()
class LASDC_API UAnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		EFoot Foot;


		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
