// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBaseGAS.h"
#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CharacterMovementComponentBase.h"
#include "CharacterDataAsset.h"
#include "AttributeSetBase.h"



// Sets default values
ACharacterBaseGAS::ACharacterBaseGAS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//	bReplicates = true;
	//bAlwaysRelevant = true;
	//AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	//AbilitySystemComp->SetIsReplicated(true);

	//Subscribe to Delegates
	//AttributeSetBaseComponent->OnHealthChange.AddDynamic(this, &ACharacterBaseGAS::OnHealthChanged);



}

ACharacterBaseGAS::ACharacterBaseGAS(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponentBase>
		(ACharacter::CharacterMovementComponentName)) {




}


void ACharacterBaseGAS::Die() {
	// Only runs on Server
//	RemoveCharacterAbilities();
//	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetHiddenInGame(true);

}

void ACharacterBaseGAS::ResposneToLasseDelegate(float a, float b) {
	UE_LOG(LogTemp, Warning, TEXT("Hello: %f, %f"), a, b);
}

void ACharacterBaseGAS::InitPlayer() {



}

void ACharacterBaseGAS::GiveAbilities() {
	
	if (HasAuthority() && AbilitySystemComponent) {
		for (auto DefaultAbility : CharacterData.Abilities) {
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}

}

void ACharacterBaseGAS::ApplyStartupEffects() {

	if (GetLocalRole() == ROLE_Authority) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharEffect : CharacterData.Effects) {
			ApplyGameplayEffectToSelf(CharEffect, EffectContext);

		}

	
	}

}

bool ACharacterBaseGAS::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> gameplayEffect, FGameplayEffectContextHandle IneffectContext) {

	if (!gameplayEffect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(gameplayEffect, 1, IneffectContext);
	if (SpecHandle.IsValid()) {

		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return ActiveGEHandle.WasSuccessfullyApplied();


	}

	return false;
}



void ACharacterBaseGAS::OnDelegated(float a, float b) {

}



//Server Only


void ACharacterBaseGAS::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("bbbbbbbb"));

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (PS)
	{

		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		if(AbilitySystemComponent) {
		UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaa"));
		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetBaseComp = PS->GetAttributeSetBase();
		lassegate.AddUniqueDynamic(PS, &APlayerStateBase::OnGagaGigeli);

		//InitAttributes();
	
		GiveAbilities();
		ApplyStartupEffects();

		BP_AfterPossessedInit();
		lassegate.Broadcast(666, 777);

		}
		else {

			UE_LOG(LogTemp, Warning, TEXT("NOT VALID ABI SYSTEM"));
		}
	}
}

// Client only
void ACharacterBaseGAS::OnRep_PlayerState() {

	Super::OnRep_PlayerState();

	if (AbilitySystemComponent == nullptr) {

		APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
		if (PS) {

			//cachce the ASC in th server
				// Set the ASC for clients. Server does this in PossessedBy.
			AbilitySystemComponent = Cast<UAbilitySystemComponent>(PS->GetAbilitySystemComponent());

			//init server side part of the ASC
			// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
			AttributeSetBaseComp = PS->GetAttributeSetBase();
			

			//grant abilities
			//init other components of the character tha tuse ASC here
		}

	}
	//else {
	//	AbilitySystemComponent->RefreshAbilityActorInfo();

	//}


}



/*
void APlayerCharacterBase::OnRep_Controller() {


}

*/









// Called when the game starts or when spawned
void ACharacterBaseGAS::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterBaseGAS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
// Called to bind functionality to input
void ACharacterBaseGAS::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
*/


UAbilitySystemComponent* ACharacterBaseGAS::GetAbilitySystemComponent() const {
	return AbilitySystemComponent;
}
/*
void ACharacterBaseGAS::AcquireAbility(TSubclassOf<UGameplayAbility> AbilityToAcquire) {

	if (AbilitySystemComponent) {

		if (HasAuthority() && AbilityToAcquire) {
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityToAcquire, 1, 0));
		}


		AbilitySystemComponent->InitAbilityActorInfo(this, this);

	}
}
*/

/*
//t‰m‰ kutsutaan, classin sis‰ll‰, constructorissa saubscribetn‰ˆ‰ attribute setin delegateen
void ACharacterBaseGAS::OnHealthChanged(float Health, float MaxHealth) {
	BP_OnHealthChanged(Health, MaxHealth); //kutsutaan myˆs BP event, voi t‰nnekkin lis‰t‰

}

*/



FCharacterData ACharacterBaseGAS::GetCharacterData() const {

	return CharacterData;
}


void ACharacterBaseGAS::SetCharacterData(const FCharacterData& InCharacterData) {

	CharacterData = InCharacterData;
	InitFromCharacterData(CharacterData);

}




void ACharacterBaseGAS::OnRep_CharacterData() {
	InitFromCharacterData(CharacterData, true);

}


void ACharacterBaseGAS::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication) {


}


void ACharacterBaseGAS::PostInitializeComponents() {
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("Nyt inititaan components vcoi pojat"));

	if (IsValid(CharacterDataAsset)) {
		UE_LOG(LogTemp, Warning, TEXT("nyt ollaan char asset isvalid"));
		SetCharacterData(CharacterDataAsset->CharacterData); 
	}
}

void ACharacterBaseGAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	//jos on replicated propertry ni pit‰‰ teh‰ t‰ll‰ jotain??
	DOREPLIFETIME(ACharacterBaseGAS, CharacterData);
}