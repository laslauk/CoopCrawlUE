// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameInstanceCrawl.h"

#include "OnlineSessionSettings.h"
#include "Actors/PlatformTrigger.h"
#include "AbilitySystemGlobals.h"
#include "UObject/ConstructorHelpers.h" //Class finder for finding blueprint types
#include "OnlineSubsystem.h"

const static FName SESSION_NAME = TEXT("MyGame");

void UGameInstanceCrawl::Init()
{
	Super::Init();
	//has to be done for targetdata


	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UGameInstanceCrawl::JoinServer(const FString& IpAddress)
{

	UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>joining ipaddress"));

	Join(IpAddress);
}



UGameInstanceCrawl::UGameInstanceCrawl(const FObjectInitializer& ObjectInitializer)
{

	/* TEST Uclass finder*/
	/* ConstructorHelpers::FClassFinder<APlatformTrigger> PlatformTriggerBPClass(TEXT("/Game/_Main/Actors/Level/BP_PlatformTrigger"));
	if (PlatformTriggerBPClass.Class) {

		UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx FOund class: %s "), *PlatformTriggerBPClass.Class->GetName())

	} */

	/*
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/_Main/UIWidgets/OnlineMenu/WBP_MainMenu"));
	if (MenuBPClass.Class) {

		MenuClass = MenuBPClass.Class;
	
		UE_LOG(LogTemp, Warning, TEXT("xxxxxxxxx FOund WIDGET class: %s "), *MenuClass->GetName())

	} */



	IOnlineSubsystem* OnlineSS = IOnlineSubsystem::Get();

	if (OnlineSS){
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *OnlineSS->GetSubsystemName().ToString());
		 SessionInterface =  OnlineSS->GetSessionInterface(); //InolienSessionPtri s session ptr,

		if (SessionInterface.IsValid()) 
		{
			//PlayerNumber, text name, settings struct FOnlineSettings
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGameInstanceCrawl::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGameInstanceCrawl::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGameInstanceCrawl::OnSessionSearchComplete);


	

			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>SESSION INTERFACE CREATED"));




		

		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>No SessionInterface found"));
		}


	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>No subystem found"));
	}



}

void UGameInstanceCrawl::LoadMenu()
{

	
}


void UGameInstanceCrawl::FindSessions() {
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid()) {

		SessionSearch->MaxSearchResults = 10;
		SessionSearch->bIsLanQuery = true;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		//	SessionSearch->bIsLanQuery = true; //this uses ONLY lan
			//SessionSearch->QuerySettings.Set()

		UE_LOG(LogTemp, Warning, TEXT("--- Starting to find Session _----"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UGameInstanceCrawl::Host()
{
	if (SessionInterface.IsValid()) 
	{

		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (ExistingSession != nullptr) {
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}


	}
}

void UGameInstanceCrawl::Join(const FString& IpAddress)
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Joining: %s"), *IpAddress));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	PlayerController->ClientTravel(IpAddress, ETravelType::TRAVEL_Absolute);


}

void UGameInstanceCrawl::Search()
{


}

void UGameInstanceCrawl::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (Success) {
		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>ONNISTUI TEHDÄ SESION: %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>Failed onsessioncreate"));
		return;
	}

//	UEngine* Engine = GetEngine();
	//Engine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (World) {
		// Siirtää playerControllerit uuteen mappiin

		/*
			Content/_Main/Levels/lvl_Level0

			/Content/_Main/Levels/LobbyLevel/level_lobby
		*/
	
		/* huom ?listen on parametri, tekee listenserverin*/
		UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>trying serverTravel"));
		/* Siirtää kaikki playerControllerit mappiin, client travel vaan yhe*/
		bool SuccessTravel = World->ServerTravel("/Game/_Main/Levels/lvl_Level0?listen");
		if (SuccessTravel) {
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>Success serverTravel"));
			
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>[FAIL] Server travel serverTravel"));


		}
	}


}

void UGameInstanceCrawl::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success) {
		CreateSession();
	}

}

void UGameInstanceCrawl::CreateSession()
{
	if (SessionInterface.IsValid()) {
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		//SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bUsesPresence = true;
		//SessionSettings.NumPrivateConnections = 0;
		/*

		SessionSettings->NumPrivateConnections = 0;
		SessionSettings->NumPublicConnections = NumPublicConnections;
		SessionSettings->bAllowInvites = true;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
		SessionSettings->bIsDedicated = false;
		SessionSettings->bUsesPresence = true;
		SessionSettings->bIsLANMatch = IsLANMatch;
		SessionSettings->bShouldAdvertise = true;
		

		*/
 UE_LOG(LogTemp, Warning, TEXT("---- [x] TEHTIIN GAME SESSION:"));
		SessionSettings.Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineService);
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}

}

void UGameInstanceCrawl::OpenMainMenu()
{

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	PlayerController->ClientTravel("/Game/_Main/Levels/LobbyLevel/level_lobby", ETravelType::TRAVEL_Absolute);

}

void UGameInstanceCrawl::OnSessionSearchComplete(bool Success) {
	if (Success) {
		//UE_LOG(LogTemp, Warning, TEXT("---- [X] Found Game Session!:"));

		if (SessionSearch.IsValid()) {
			for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults) {

				UE_LOG(LogTemp, Warning, TEXT("---- [X] Found Game Session!: %s"), *SearchResult.GetSessionIdStr());

			}
		}


	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("---- [ERROR]  Game Session  ERRO VITTU"));
	}

}