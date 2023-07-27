// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameInstanceCrawl.h"

void UGameInstanceCrawl::Init()
{

}

UGameInstanceCrawl::UGameInstanceCrawl(const FObjectInitializer& ObjectInitializer)
{


}

void UGameInstanceCrawl::Host()
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (World) {


		// Siirtää playerControllerit uuteen mappiin
		World->ServerTravel
	}

}

void UGameInstanceCrawl::Join(const FString& IpAddress)
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Joining: %s"), *IpAddress));

}
