// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CrawlCommonTypes.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"

#include "GameplayEffectExtension.h"
#include "CharacterBaseGAS.generated.h"


class UCameraComponent;
class UAttributeSetBase;
class USurvivalStatsComponent;
class APlayerStateBase;
class UMotionWarpingComponentBase;
class UCharacterMovementComponentBase;
class UInventoryComp;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLasseDelegate, float, a, float, b);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCameraLockedDelegate, bool, locked);

UCLASS()
class LASDC_API ACharacterBaseGAS : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
//	ACharacterBaseGAS();
	ACharacterBaseGAS(const FObjectInitializer& ObjectInitializer);


	virtual void PostLoad() override;

	UCameraComponent* GetFollowCamera();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;





	void GiveAbilities();
	void ApplyStartupEffects();

	/* PROPERTIES */

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GAS")
		TSubclassOf<UGameplayEffect> DefaultAttributeSet;

	//UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GAS")
	//	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

//	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GAS")
//	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
		UAbilitySystemComponent* AbilitySystemComp;

	UPROPERTY(Transient)
	UAttributeSetBase* AttributeSetBaseComp;


	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> gameplayEffect, FGameplayEffectContextHandle IneffectContext);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnDelegated(float a, float b);


	UPROPERTY(BlueprintAssignable, Category = "gaga")
	FLasseDelegate lassegate;

	UPROPERTY(BlueprintAssignable, Category = "gaga")
	FCameraLockedDelegate CameraLockedDelegate;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	APlayerStateBase* PS_ref;
	
	UFUNCTION()
	void ResposneToLasseDelegate(float a, float b);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	void StartRagdoll();

protected:

	UFUNCTION()
		void OnRagdollStateTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:

	virtual void Die();


	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "AfterPossessedInit"))
	void BP_AfterPossessedInit();


	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
		void InitPlayer();



	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase")
	void BP_HealthChanged(float newHealth, float MaxHealth);



	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterInput")
	void BP_OnPrimaryInputAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "CharacterInput")
	void BP_OnSecondaryInputAction();

	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void TryJump();

	UFUNCTION(BlueprintCallable, Category = "CharacterInput")
	void TryCrouch(bool SetCrouch);

	//UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase")
	//void BP_MaxHealthChanged(float newHealth);

	void Landed(const FHitResult& Hit) override;

	
	// ~ Server - PC and PS Valid
	void PossessedBy(AController* NewController) override;
	// ~ Client - PS Valid  
	void OnRep_PlayerState() override;
	// ~ Client - PC Valid					            
//	void OnRep_Controller() override;

public:
	UFUNCTION(BlueprintCallable)
		FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
		void SetCharacterData(const FCharacterData& InCharacterData);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	 class UFootstepsComponent* GetFootStepsComponent() const;



protected:
	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
		FCharacterData CharacterData;

	UFUNCTION()
		void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

		UPROPERTY(EditDefaultsOnly)
		class UCharacterDataAsset* CharacterDataAsset;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	class UFootstepsComponent* FootstepsCom;

	
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Enhanced input


	//Crouching a bit complicated, you can be at crouching state, dont cancel crouch ability if obstacle on, so lets handle uncrouch ourself
	//apply the effect yourself, not by abiltiy

	//apply effect state tag when still in crouch state even if tried to cancel crouch ability,

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UInventoryComp* GetInventoryComponent() const;


	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData&);

	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

	//Gameplay events


	public:

		//nämä asetetaan editorissa joita abilitiyt kattelee että tekeekö abilityn trigger
		UPROPERTY(EditDefaultsOnly)
		FGameplayTag JumpEventTag;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer InAirTags;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer CrouchTags;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer SprintTags;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTag ZeroHealthEventTag;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTag RagdollStateTag;


		//Gameplay tags

		protected:
		//apply crouch state tag
		UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> CrouchStateEffect;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
		UMotionWarpingComponentBase* MotionWarpingComponent;

		public:

			UMotionWarpingComponentBase* GetMotionWarpingComponent() const;
	//Delegates


	//subscribe to attribute change of max movement speed
			//attribute change pitää vaikuttaa MovementComponent
			//Subscribetään Attributen changene ja sit siitä Movement compo
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SurvivalStats")
		USurvivalStatsComponent* SurvivalStatsComp;

	protected:
		FDelegateHandle MaxMovementSpeedChangedDelegateHandle;

	
		UCharacterMovementComponentBase* CharacterMovementComponent;

	




		/* Survival stuff*/

public:
	

};
