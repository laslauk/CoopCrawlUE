// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LASDC_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:

		virtual void Init(TSubclassOf<class UItemStaticData> ItemStaticDataClass, AActor* owner);

		virtual bool IsSupportedForNetworking() const override { return true;}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData*GetItemStaticData() const; //static datat

	UPROPERTY(Replicated)
	TSubclassOf<class UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
		bool bEquipped = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner);
	virtual void OnUnequipped();
	virtual void OnDropped();



	UPROPERTY()
		class AActor* OwningPlayerState;

protected:
	UPROPERTY(Replicated)
	class AItemActorBase* ItemActor = nullptr;


};
