// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WallRun.generated.h"

/**
 *  AbilityTasks are like tick functions for abilities
 * 
 * - Use Delegates -> way how to communicate with ability tasks, how to get information from them
 */


 /*

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDetermenedDelegate, bool, bLeftSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate); //this is used to finished job, we have to do somethinf aterwards




class ACharacter;

class UCharacterMovementComponent;

UCLASS()
class LASDC_API UAbilityTask_WallRun : public UAbilityTask
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintAssignable)
	FOnWallRunFinishedDelegate OnFinished;

	UPROPERTY(BlueprintAssignable)
	FOnWallRunWallSideDetermenedDelegate OnWallSideDetermened;

	// IMPORTANT: creates instance of the task, every task must have this, static, 
	UFUNCTION(BlueprintCallable, Category ="Abiltiy|Tasks", meta= (HiddenPin = "OwningAbility", DefaultToSelf ="OwningAbility"))
	static UAbilityTask_TickWallRun* CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
		UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes);

	virtual bool Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	virtual void TickTask(float DeltaTime) override;

protected:


	UCharacterMovementComponent* ChracterMovement = nullptr;

	ACharacter* CharacterOwner = nullptr;
	TArray<TEnumAsByte<EObjectTypeQuery>> WallRun_TraceObjectTypes;

	bool FindRunnableWall(FHitResult& OnWallHit);


	bool ISWallOnTheLeft(const FHitResult& InWallHit) const;

};


*/