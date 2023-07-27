// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CrawlCommonTypes.h"
#include "GameplayTagContainer.h"
#include "CharacterMovementComponentBase.generated.h"


class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class LASDC_API UCharacterMovementComponentBase : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	bool TryTraversal(UAbilitySystemComponent* ASC);
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintPure)
		EMovementDirectionType GetMovementDirectionType() const;

	UFUNCTION(BlueprintCallable)
	void SetMovementDirectionType(EMovementDirectionType InMovementDirectionType);



	/* Use gameplay effect to handle rotation setting, We have specific tag that we apply to character from that effect */
	/* from movement component,we subscribe to the tag, when all the sum of applied tags is > 0, we will use strafe movement*/
	/* when 0 count on arrayt, use basic movement */
	// Callback when tag changed
	UFUNCTION(BlueprintCallable)
	void OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount);




protected:

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;


	UPROPERTY(EditAnywhere)
	EMovementDirectionType MovementDirectionType;
	
	void HandleMovementDirection();
};
