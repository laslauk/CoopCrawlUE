// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CrawlCommonTypes.h"
#include "AbilitySystemComponent.h"
#include "CharacterBaseGAS.generated.h"


class UAttributeSetBase;
class APlayerStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLasseDelegate, float, a, float, b);

UCLASS()
class LASDC_API ACharacterBaseGAS : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBaseGAS();
	ACharacterBaseGAS(const FObjectInitializer& ObjectInitializer);


	virtual void PostInitializeComponents() override;

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


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
	APlayerStateBase* PS_ref;
	
	UFUNCTION()
	void ResposneToLasseDelegate(float a, float b);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;




	virtual void Die();
	//UFUNCTION(BlueprintCallable, Category = "CharacterBase")
//	void AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire);




//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterBase")
//	UAttributeSetBase* AttributeSetBaseComponent;

	//UFUNCTION()
	//	void OnHealthChanged(float Health, float MaxHealth);

	//UFUNCTION(BlueprintImplementableEvent, Category = "CharacterBase", meta = (DisplayName = "OnHealthChanged"))
	//	void BP_OnHealthChanged(float Health, float MaxHealth);

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



	//Gameplay events


	protected:
		UPROPERTY(EditDefaultsOnly)
		FGameplayTag JumpEventTag;

		UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer InAirTags;

		//Gameplay tags



	



};
