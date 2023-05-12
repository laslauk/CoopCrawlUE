// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AICharacterBaseGAS.generated.h"


class UAttributeSetBase;

UCLASS()
class LASDC_API AAICharacterBaseGAS : public ACharacter, public IAbilitySystemInterface

{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBaseGAS();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category="AICharacterBase")
		UAbilitySystemComponent* ASC;


		UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(Transient)
		UAttributeSetBase* AttributeSet;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
