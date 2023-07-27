// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CrawlCommonTypes.h"
#include "FastArrayTagCounter.generated.h"

/**
 *  1. ammunition item contains many diff items of instance, ammo for pistol, can contain 
 *  add ammunition tag for pistol ammunition 30 times, adding 3 tags 30 ammo items, => 90 ammunition
 * everytime shoot, remove one item, reduce tag counter'
 *  tldr, tag count == ammo count
 */

/* ARRAY ELEMENT */

USTRUCT(BlueprintType)
struct FFastArrayTagCounterRecord : public FFastArraySerializerItem {

	GENERATED_BODY()

public:

	UPROPERTY()
		FGameplayTag Tag;

	UPROPERTY()
		int32 Count = 0;

};



/* ARRAY SERLIAZER*/
USTRUCT(BlueprintType)
struct  FFastArrayTagCounter : public FFastArraySerializer{
	GENERATED_BODY()

public:

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams) {
		return FFastArraySerializer::FastArrayDeltaSerialize<FFastArrayTagCounterRecord, FFastArrayTagCounter>(TagArray, DeltaParams, *this);
	}

	int32 GetTagCount(FGameplayTag InTag) const;

	void AddTagCount(FGameplayTag InTag, int32 Delta);

	const TArray<FFastArrayTagCounterRecord>& GetTagArray() const;

protected:

	UPROPERTY()
	 TArray<FFastArrayTagCounterRecord> TagArray;

};

template<>
struct TStructOpsTypeTraits<FFastArrayTagCounter> : public TStructOpsTypeTraitsBase2<FFastArrayTagCounter> {

	enum { WithNetDeltaSerializer = true };

};