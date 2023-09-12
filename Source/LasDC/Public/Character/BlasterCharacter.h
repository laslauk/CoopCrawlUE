// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Character/CharacterBaseGAS.h"
#include "BlasterCharacter.generated.h"

/**
 * 
 */




UCLASS()
class LASDC_API ABlasterCharacter : public ACharacterBaseGAS
{
	GENERATED_BODY()

		ABlasterCharacter(const FObjectInitializer& ObjectInitializer);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCameraAimMode(ECameraAimMode AimMode);

	UPROPERTY(EditAnywhere, Category = "Camera")
		float ZoomedFOV = 60.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float ZoomInterpSpeed  = 60.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float CurrentFOV = 60.f;

	void InterpFOV(float DeltaTime);

	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraHideCharTreshold;

	void HideCharacterIfCameraClose();





public:
	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraTargetArmLength = 700.0f;

protected:

	/* PLAYER CHARACTER  STUFF ONLY */

public: 
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

};
