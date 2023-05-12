// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.generated.h"

/**
 * 
 */

class UAbilitySystemComponent;


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnhealthChangeDelegate, float, Health, float, MaxHealth);



UCLASS()
class LASDC_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
public:

	UAttributeSetBase();


	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health)

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth)

		UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
		FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Stamina)

		UPROPERTY(BlueprintReadOnly, Category = "MaxStamina", ReplicatedUsing = OnRep_MaxStamina)
		FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxStamina)

		UPROPERTY(BlueprintReadOnly, Category = "MovementSpeed", ReplicatedUsing = OnRep_MaxMovementSpeed)
		FGameplayAttributeData MaxMovementSpeed;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxMovementSpeed)



/**
* These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
**/

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
		virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
		virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);


	UFUNCTION()
		virtual void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed);


	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


//	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) override;

	//RepNotifys
	//UFUNCTION()
	//virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	//CreateDelegates
	//FOnhealthChangeDelegate OnHealthChange;
};
