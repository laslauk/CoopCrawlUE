// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CrawlCommonTypes.generated.h"


class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class AItemActorBase;
class UNiagaraSystem;




/* CHARACTER DATA*/

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





/* CHARACTER ANIMATION DATA*/

USTRUCT(BlueprintType)
struct FCharacterAnimationData {

	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
		class UBlendSpace* MovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UBlendSpace* CrouchMovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;


};

/* PROJECTILE STATIC DATA*/
UCLASS(BlueprintType, Blueprintable)
class UProjectileStaticData : public UObject {
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bHasSplash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float DamageRadius;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float GravityMultiplier = 1.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float InitialSpeed = 3000.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxSpeed = 3000.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSubclassOf<UGameplayEffect>> EffectsToApplyOnHit;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* OnStopVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* OnStopSFX = nullptr;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* TracerParticleSystem;


};

/* ITEM BASE STATIC DATA*/

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

	UPROPERTY(EditDefaultsOnly)
	FCharacterAnimationData CharacterAnimationData;

	//how to use weapons == weapons grant an USE (such as melee, shoot) ability to use the weapon
	UPROPERTY(EditDefaultsOnly, BLueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities; //item grants abilities

	//effects we apply to character when we equip it
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> InventoryTags;

	//1 item instance can have multiple items
	UPROPERTY(EditDefaultsOnly, Blueprintable)
		int32 MaxStackCount = 1;

};



USTRUCT(BlueprintType, Blueprintable)
struct FCrossHairData {

	GENERATED_BODY()



public:

		UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrossHairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrossHairsBottom;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrossHairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrossHairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrossHairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	float CrosshairSpread;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	FLinearColor CrosshairsColor;
};


/* WEAPON ITEM STATIC DATA*/

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData {

	GENERATED_BODY()

public:

	//effect to apply damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimationAsset* WeapoFireAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* PrimaryActionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float FireDelayTime = 1.0f;
	

	UPROPERTY(EditDefaultsOnly, BLueprintReadOnly)
		float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AActor> FXActorOnPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AProjectileBase> WeaponFireProjectileClass;


	/* NOTE, even it is named AMmoTag, it is COUNT tag, TODO fix names, can be used for other stacking items such as potion etc.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		 FGameplayTag AmmoTag;


	/* NOTE: N‰m‰ soitetaan Animaatiosta nyt Notifyill‰ koska parempi ajottaa siell‰,  MUTTA TODO Ett‰ data tulisi t‰‰lt‰ eik‰ m‰‰ritet‰ vaan animaatioissa eirkseen, parempi
	olla yhdess‰ paikassa kaikki m‰‰ritetty
	
	*/

	//Attack soundit on played by itemActor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UNiagaraSystem* MuzzleFireEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		 FCrossHairData CrosshairData;

};



/* Ammunition item */
UCLASS(BlueprintType, Blueprintable)
class UAmmoItemStaticData : public UItemStaticData {

	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh = nullptr;
};


/********* GENERAL ENUMS ***********/

UENUM(BlueprintType)
enum class EItemState : uint8
{
	None UMETA(DIsplayName = "None"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};


UENUM(BlueprintType)
enum class EFoot : uint8 {
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};


UENUM(BlueprintType)
enum class ECameraAimMode : uint8 {
	None UMETA(DisplayName = "FreeCameraRotation"),
	CarryingWeapon UMETA(DisplayName = "CarryingWeapon"),
	Aiming UMETA(DisplayName = "Aiming")
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8 {
	
	None UMETA(DisplayName = "None"),
	OrientRotationToMovement UMETA(DisplayName = "OrientRotationToMovement"),
	Strafe UMETA(DisplayName = "Strafe")


};


UENUM(BlueprintType)
enum class ETurningInPlace : uint8 {
	NotTurning UMETA(DisplayName = "Not Turning"),
	Left UMETA(DisplayName = "Turning Left"),
	Right UMETA(DisplayName = "Turning Right"),
	MAX UMETA(DisplayName = "DefaultMax")
};