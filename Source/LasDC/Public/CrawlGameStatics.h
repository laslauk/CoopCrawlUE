// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrawlCommonTypes.h"
#include "CrawlGameStatics.generated.h"

class UGameplayEffect;
class AProjectileBase;
class UProjectileStaticData;
/**
 * 
 */
UCLASS()
class LASDC_API UCrawlGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()



public: 


	UFUNCTION(BlueprintCallable, BlueprintPure)
		static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);


	//global function ni kaikki ketk‰ haluaa teh‰ radial damage ni voi k‰ytt‰‰ t‰t‰

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius, float DamageAmount, TArray <TSubclassOf< UGameplayEffect>> DamageEffects,
			const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType, TArray<AActor*> ActorsToIgnore);
	

	//LaucnHprojectile
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static AProjectileBase* LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass, FTransform Transform, AActor* Owner, APawn* Instigator);
};
