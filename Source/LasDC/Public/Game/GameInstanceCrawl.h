// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h" //UUserwidget
#include "Interfaces/MainMenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameInstanceCrawl.generated.h"

/**
 * 
 */


UCLASS()
class LASDC_API UGameInstanceCrawl : public UGameInstance, public IMainMenuInterface
{
	GENERATED_BODY()

public:

	virtual void Init() override;


	UGameInstanceCrawl(const FObjectInitializer& ObjectInitializer);


	/* Exec toimii: GameInstance, PlayerController, Possessed Pawns, HUSD, CheatManagers, GameModes*/

	
	UFUNCTION( BlueprintCallable)
	void LoadMenu();



	UFUNCTION(Exec)
	void Join(const FString& IpAddress);

	UFUNCTION(Exec)
	void Search();

	/* Delegate callbacks */
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnSessionSearchComplete(bool Success);

	void FindSessions();

	void CreateSession();
	IOnlineSessionPtr SessionInterface;


	TSubclassOf< UUserWidget> MenuClass; 

	UPROPERTY()
	UUserWidget* Menu;

private:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;


public: 

	/* Inherited via IMainMenuInterface */
	virtual void OpenMainMenu() override;

	UFUNCTION(Exec)
	virtual void Host() override;

	virtual void JoinServer(const FString& IpAddress) override;

};
