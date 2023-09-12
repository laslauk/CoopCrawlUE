
// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CrawlHUD.h"
#include "HUD/CharacterOverlay.h"
#include "PlayerStateBase.h"
#include "PlayerControllerBase.h"

void ACrawlHUD::DrawHUD() 
{

	Super::DrawHUD();


	FVector2D ViewportSize;
	if (GEngine) {

		GEngine->GameViewport->GetViewportSize(ViewportSize);

		const FVector2D ViewPortCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * CrossHairData.CrosshairSpread;
		//UE_LOG(LogTemp, Warning, TEXT("Crosshair spread: %f"), CrossHairData.CrosshairSpread);

		/* Draw all crosshair parts */
		if (CrossHairData.CrossHairsBottom) {
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(CrossHairData.CrossHairsBottom, ViewPortCenter, Spread, CrossHairData.CrosshairsColor);
		}
		if (CrossHairData.CrossHairsTop)  {
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(CrossHairData.CrossHairsTop, ViewPortCenter, Spread, CrossHairData.CrosshairsColor);

		}
		if (CrossHairData.CrossHairsLeft) { 
			FVector2D Spread( -SpreadScaled, 0.f);
			DrawCrosshair(CrossHairData.CrossHairsLeft, ViewPortCenter, Spread, CrossHairData.CrosshairsColor); }

		if (CrossHairData.CrossHairsRight) { 
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(CrossHairData.CrossHairsRight, ViewPortCenter, Spread, CrossHairData.CrosshairsColor);
		
		}

		if (CrossHairData.CrossHairsCenter) { 
			FVector2D Spread(0.f, 0.f); 
			DrawCrosshair(CrossHairData.CrossHairsCenter, ViewPortCenter, Spread, CrossHairData.CrosshairsColor);
		}

	}


}

void ACrawlHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{

	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	//HUD's Draw function for Textures
	
	if (!Texture) {

		UE_LOG(LogTemp, Warning, TEXT("TEXTURE NULL S:SS rteturning......."));
		return;
	}
	DrawTexture
	(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,

		TextureWidth,
		TextureHeight,
		0.f, 0.f, //UV coords
		1.f, 1.f, // texture space Widht, height

		CrosshairColor //White == original texture color
		//rest are defaulted
	);




}


void ACrawlHUD::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterOverlay();


}

void ACrawlHUD::SetCharacterOverlay()
{
	APlayerControllerBase* PlayerController = Cast< APlayerControllerBase>(GetOwningPlayerController());
	if (PlayerController && CharacterOverlayClass ) {

		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
		CharacterOverlay->CrawlHUD = this;
		
		UE_LOG(LogTemp, Warning, TEXT("ACrawlHUD::SetCharacterOverlay()"));

		if (PlayerController->PlayerState_Ref) {

			PlayerController->PlayerState_Ref->UpdateHUD();
		}
		
	}
}
