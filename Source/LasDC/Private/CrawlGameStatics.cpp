// Fill out your copyright notice in the Description page of Project Settings.


#include "CrawlGameStatics.h"

 const UItemStaticData* UCrawlGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass) {


	 // Unreal creates default object for every Uobject, exists server and client, available basic item parameters
	 if (IsValid(ItemDataClass)) {

		 return GetDefault<UItemStaticData>(ItemDataClass);

	 }

	 return nullptr;

}