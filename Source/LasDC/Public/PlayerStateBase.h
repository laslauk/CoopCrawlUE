// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "InventoryComp.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"

/**
 * 
 */


//DECLARE_MULTICAST_DELEGATE_TwoParams(DelNameLasseTwoFloatsIn, float, float)
//DECLARE_MULTICAST_DELEGATE_RetVal_TwoParams(float, DelNameRetValTwoParams, float,float)


//https://www.youtube.com/watch?v=WyyUPqdZQfU



UCLASS()
class LASDC_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerStateBase();

	virtual void BeginPlay() override;
	
	//	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	//	void AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire);
		UPROPERTY()
		UAbilitySystemComponent* ASC;

		UPROPERTY()
		class UAttributeSetBase* AttributeSetBaseComp;

		UPROPERTY(EditAnywhere, Replicated)
		ACharacter* playerCharacterRef;


		UAbilitySystemComponent* GetAbilitySystemComponent() const override;
		class UAttributeSetBase* GetAttributeSetBase() const;
	

/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

		UFUNCTION(BlueprintCallable, Category = "PlayerStateBase")
			float GetHealth() const;

		UFUNCTION(BlueprintCallable, Category = "PlayerStateBase")
			float GetMaxHealth() const;


		UFUNCTION()
		void OnGagaGigeli(float a, float b);

		FDelegateHandle HealthChangedDelegateHandle;
		FDelegateHandle MaxHealthChangedDelegateHandle;

		//atribute change callbacks
		virtual void HealthChanged(const FOnAttributeChangeData& Data);
		virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);


public:

		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

		UPROPERTY(EditAnywhere, Replicated)
		UInventoryComp* InventoryComp = nullptr;

		UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetOpenedInventoryContainerActor(AActor* Other) { OpenedInventoryContainerActor = Other; }

		UFUNCTION(BlueprintCallable, Category = "Inventory")
		AActor* GetOpenedInventoryContainerActor() { return OpenedInventoryContainerActor;  }


private:

		//TODO Maybe replicate?
		UPROPERTY(EditAnywhere)
		AActor* OpenedInventoryContainerActor = nullptr;


	//	UFUNCTION(BlueprintCallable, Category = "PlayerStateBase")
	//		void LasseNoRetuFloats(float a, float b);

	//	UFUNCTION(BlueprintCallable, Category = "PlayerStateBase")
	//		float LasseFloatsMultiply(float a, float b);

	//	DelNameLasseTwoFloatsIn LNoRetTwoParams;
	//	DelNameRetValTwoParams LFloatRetTwoparams;

	//	FDelegateName 

	//	FDelegateHandle HealthChangedDelegateHandle;
	//	FDelegateHandle MaxHealthChangedDelegateHandle;




//		virtual void HealthChanged(const FOnAttributeChangeData& Data);
	//	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
private:
};
