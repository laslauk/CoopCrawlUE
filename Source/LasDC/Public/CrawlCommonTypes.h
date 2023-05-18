// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrawlCommonTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
		TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
		TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
		class UBlendSpace* MovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UAnimSequenceBase* IdleAnimationAsset = nullptr;


};

UENUM(BlueprintType)
enum class EFoot : uint8 {
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};




UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject {

	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AItemActorBase> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponAttachmentSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bCanBeEquipped = true;



};