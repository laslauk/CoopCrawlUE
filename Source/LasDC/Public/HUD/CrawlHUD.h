// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrawlCommonTypes.h"
#include "CrawlHUD.generated.h"

/**
 * 
 */

class UCharacterOverlay;

UCLASS()
class LASDC_API ACrawlHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;


private:
	FCrossHairData CrossHairData;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 32.f;
public:

	UPROPERTY(EditAnywhere, Category  = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY(EditAnywhere)
	 UCharacterOverlay* CharacterOverlay;

	 UCharacterOverlay* GetCharacterOverlay() { return CharacterOverlay; }

	 virtual void BeginPlay() override;

	 void SetCharacterOverlay();

	FORCEINLINE void ClearCrossHairData() 
	{
		CrossHairData.CrossHairsBottom = nullptr;
		CrossHairData.CrossHairsTop = nullptr;
		CrossHairData.CrossHairsLeft = nullptr;
		CrossHairData.CrossHairsRight = nullptr;
		CrossHairData.CrossHairsCenter = nullptr;
	
	}

	FORCEINLINE void SetCrosshairData(const FCrossHairData& Data) {


		CrossHairData.CrossHairsBottom = Data.CrossHairsBottom;
		CrossHairData.CrossHairsTop = Data.CrossHairsTop;
		CrossHairData.CrossHairsLeft = Data.CrossHairsLeft;
		CrossHairData.CrossHairsRight = Data.CrossHairsRight;
		CrossHairData.CrossHairsCenter = Data.CrossHairsCenter;
		CrossHairData = Data;


	//	UE_LOG(LogTemp, Warning, TEXT("PISTETTIIN DATA<-- %s "), *CrossHairData.CrossHairsBottom->GetName());
	}

};
