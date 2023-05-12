// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterBaseGAS.h"
#include "AttributeSetBase.h"

// Sets default values
AAICharacterBaseGAS::AAICharacterBaseGAS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSet"));

}

// Called when the game starts or when spawned
void AAICharacterBaseGAS::BeginPlay()
{
	Super::BeginPlay();
	ASC->InitAbilityActorInfo(this, this); //owner is this actor, and avatar is the same

}

// Called every frame
void AAICharacterBaseGAS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


UAbilitySystemComponent* AAICharacterBaseGAS::GetAbilitySystemComponent() const  {

	return ASC;

}

/*
// Called to bind functionality to input
void AAICharacterBaseGAS::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

*/

