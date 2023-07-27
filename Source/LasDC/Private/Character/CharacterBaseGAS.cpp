// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBaseGAS.h"
#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "InventoryItemInstance.h"
#include "InventoryComp.h"
#include "Components/CapsuleComponent.h"
#include "ActorComponents/SurvivalStatsComponent.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "ActorComponents/MotionWarpingComponentBase.h"
#include "InventoryList.h"
#include "CrawlCommonTypes.h"
#include "CharacterMovementComponentBase.h"
#include "PlayerControllerBase.h"
#include "Camera/CameraComponent.h"
#include "FootstepsComponent.h"
#include "CharacterDataAsset.h"
#include "FootstepsComponent.h"
#include "AttributeSetBase.h"


/*
// Sets default values
ACharacterBaseGAS::ACharacterBaseGAS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bAlwaysRelevant = true;
	//AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	//AbilitySystemComp->SetIsReplicated(true);

	//Subscribe to Delegates
	//AttributeSetBaseComponent->OnHealthChange.AddDynamic(this, &ACharacterBaseGAS::OnHealthChanged);

	FootstepsCom = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsCom"));

	UE_LOG(LogTemp, Warning, TEXT(">>CharacterMovementComponent constructor"));
	CharacterMovementComponent = Cast< UCharacterMovementComponentBase>(GetCharacterMovement());
	if (!CharacterMovementComponent) {
		UE_LOG(LogTemp, Warning, TEXT(">>CharacterMovementComponent constructor NULL"));
	}


	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponentBase>(TEXT("MotionWarpingComponent"));

}
*/

ACharacterBaseGAS::ACharacterBaseGAS(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponentBase>
		(ACharacter::CharacterMovementComponentName)) {

	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bAlwaysRelevant = true;
	//AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	//AbilitySystemComp->SetIsReplicated(true);

	//Subscribe to Delegates
	//AttributeSetBaseComponent->OnHealthChange.AddDynamic(this, &ACharacterBaseGAS::OnHealthChanged);

	FootstepsCom = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsCom"));

	UE_LOG(LogTemp, Warning, TEXT(">>CharacterMovementComponent constructor"));
	CharacterMovementComponent = Cast< UCharacterMovementComponentBase>(GetCharacterMovement());
	if (!CharacterMovementComponent) {
		UE_LOG(LogTemp, Warning, TEXT(">>CharacterMovementComponent constructor NULL"));
	}


	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponentBase>(TEXT("MotionWarpingComponent"));
	///SurvivalStatsComp = CreateDefaultSubobject<USurvivalStatsComponent>(TEXT("Survival Stats Component"));
//	SurvivalStatsComp->RegisterComponent();
	
	
}

//void ACharacterBaseGAS::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {



//}


void ACharacterBaseGAS::Die() {
	// Only runs on Server
//	RemoveCharacterAbilities();
//	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetHiddenInGame(true);

}
/*
 void ACharacterBaseGAS::PawnClientRestart()  {

}
 */

UFootstepsComponent* ACharacterBaseGAS::GetFootStepsComponent() const {

	return FootstepsCom;
}

void ACharacterBaseGAS::ResposneToLasseDelegate(float a, float b) {
	UE_LOG(LogTemp, Warning, TEXT("Hello: %f, %f"), a, b);
}

void ACharacterBaseGAS::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	if (AbilitySystemComp) {

		AbilitySystemComp->RemoveActiveEffectsWithTags(InAirTags);



	}
}


void ACharacterBaseGAS::TryJump() {


	if(CharacterMovementComponent) {
	CharacterMovementComponent->TryTraversal(AbilitySystemComp);
	}
}


//Called from outside, like controller
void ACharacterBaseGAS::TryCrouch(bool SetCrouch) {


	if (SetCrouch) {
		if (AbilitySystemComp) {
			UE_LOG(LogTemp, Warning, TEXT(">>TryCrouch got ASC"));
			AbilitySystemComp->TryActivateAbilitiesByTag(CrouchTags, true);
		}

	}
	else {
		if (AbilitySystemComp) {
			UE_LOG(LogTemp, Warning, TEXT(">>TryCrouch cancelled ASC"));
			AbilitySystemComp->CancelAbilities(&CrouchTags);
		}
	}
		

}


void ACharacterBaseGAS::InitPlayer() {



}

void ACharacterBaseGAS::GiveAbilities() {
	
	if (HasAuthority() && AbilitySystemComp) {
		for (auto DefaultAbility : CharacterData.Abilities) {
			AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}

}

void ACharacterBaseGAS::ApplyStartupEffects() {

	if (GetLocalRole() == ROLE_Authority) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		for (auto CharEffect : CharacterData.Effects) {
			ApplyGameplayEffectToSelf(CharEffect, EffectContext);
		}
	}

}

bool ACharacterBaseGAS::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> gameplayEffect, FGameplayEffectContextHandle IneffectContext) {

	if (!gameplayEffect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(gameplayEffect, 1, IneffectContext);
	if (SpecHandle.IsValid()) {

		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return ActiveGEHandle.WasSuccessfullyApplied();


	}

	return false;
}



void ACharacterBaseGAS::OnDelegated(float a, float b) {

}



//PossessedBy - Server Only
void ACharacterBaseGAS::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("bbbbbbbb"));

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	SetAutonomousProxy(true);
	if (PS)
	{
		PS_ref = PS;


		


		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComp = Cast<UAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		if(AbilitySystemComp) {

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		PS->playerCharacterRef = this;
		AttributeSetBaseComp = PS->GetAttributeSetBase();
		lassegate.AddUniqueDynamic(PS, &APlayerStateBase::OnGagaGigeli);


		//Bind delegates to attribute stat changes, at the end is the function to bne run, Middle is what attribute changed
		if (!AttributeSetBaseComp) {

			UE_LOG(LogTemp, Warning, TEXT("!!!!!!!! CANT FIND AATRIBURE SET BVASE COMP FOR CAHRACTE R!!!!!!!!"));
		}
		 
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetMaxMovementSpeedAttribute()).AddUObject(this, &ACharacterBaseGAS::OnMaxMovementSpeedChanged);
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseComp->GetHealthAttribute()).AddUObject(this, &ACharacterBaseGAS::OnHealthAttributeChanged);


		//register is delegate for  Responding to Changes in Gameplay Tags, when gameplay tags are added or removed
		//eli ability system componentti huutaa tolle onragdollstatetagchanged delegatelle että kukkuu kun tämä kyseinen tag muuttuu
		AbilitySystemComp->RegisterGameplayTagEvent(
				FGameplayTag::RequestGameplayTag(TEXT("State.Ragdoll"),
				EGameplayTagEventType::NewOrRemoved)
		).AddUObject(this, &ACharacterBaseGAS::OnRagdollStateTagChanged);


		CameraLockedDelegate.AddUniqueDynamic(Cast<APlayerControllerBase>(GetController()), &APlayerControllerBase::CameraLocked);

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

	UE_LOG(LogTemp, Warning, TEXT("!!!!!!!! CHARACTER OWNER %s!!!!!!!!"),*GetNetOwner()->GetName());
}

// Client only
void ACharacterBaseGAS::OnRep_PlayerState() {

	Super::OnRep_PlayerState();

	if (AbilitySystemComp == nullptr) {

		APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
		if (PS) {
			PS_ref = PS;
			//cachce the ASC in th server
				// Set the ASC for clients. Server does this in PossessedBy.
			AbilitySystemComp = Cast<UAbilitySystemComponent>(PS->GetAbilitySystemComponent());

			if (!AbilitySystemComp) {
				UE_LOG(LogTemp, Warning, TEXT("Vittu"));
				return;
			}
			//init server side part of the ASC
			// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
			AbilitySystemComp->InitAbilityActorInfo(PS, this);
			AttributeSetBaseComp = PS->GetAttributeSetBase();
			
			PS->playerCharacterRef = this;
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



UInventoryComp* ACharacterBaseGAS::GetInventoryComponent() const {

	if (PS_ref) {

		return PS_ref->InventoryComp;
	}
	else {

		return nullptr;
	}
}


void ACharacterBaseGAS::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;

}

//Rekisteröity rakentajassa
void ACharacterBaseGAS::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	
	//death
	if (Data.NewValue <= 0 && Data.OldValue > 0) {
		ACharacterBaseGAS* DamageInstigatorCharacter = nullptr; //damage dealer
		
		UE_LOG(LogTemp, Warning, TEXT("%%%%%%% DEATH %%%%%%%"));

		//send gameplay event
		FGameplayEventData EventPayload;
		EventPayload.EventTag = ZeroHealthEventTag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ZeroHealthEventTag, EventPayload);
		

		//not used but this is how you get from the attribute change effect the instigator  such as atacker
		if (Data.GEModData) {
			const FGameplayEffectContextHandle& EffectContext = Data.GEModData->EffectSpec.GetEffectContext();
			DamageInstigatorCharacter = Cast<ACharacterBaseGAS>(EffectContext.GetInstigator());
		}

		

	}

}





// Called when the game starts or when spawned
void ACharacterBaseGAS::BeginPlay()
{
	Super::BeginPlay();




	
}


UCameraComponent* ACharacterBaseGAS::GetFollowCamera()
{
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	return Camera;
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

	return AbilitySystemComp;
}

void ACharacterBaseGAS::StartRagdoll()
{
	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	
	if(SkeletalMeshComp) {
	UE_LOG(LogTemp, Warning, TEXT("-------------- RAGDOLL StART ---------"));

		UE_LOG(LogTemp, Warning, TEXT("-------------- RAGDOLL Is simulatin physics ---------"));
		SkeletalMeshComp->SetSimulatePhysics(true);
		SkeletalMeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		SkeletalMeshComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMeshComp->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		SkeletalMeshComp->WakeAllRigidBodies();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		

	}
}

void ACharacterBaseGAS::OnRagdollStateTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	//checkataan muutos, jos +1 count niin on ragdoll state
	UE_LOG(LogTemp, Warning, TEXT("--------- ON RAGDOLL STATE CHANGED ON CHAR: %s"), *this->GetName());
	if (NewCount > 0) {
		UE_LOG(LogTemp, Warning, TEXT("--------- [x] CALLING RAGDOLL START  ON CHAR: %s"), *this->GetName());
		StartRagdoll();
	}
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
//tämä kutsutaan, classin sisällä, constructorissa saubscribetnäöä attribute setin delegateen
void ACharacterBaseGAS::OnHealthChanged(float Health, float MaxHealth) {
	BP_OnHealthChanged(Health, MaxHealth); //kutsutaan myös BP event, voi tännekkin lisätä

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


void ACharacterBaseGAS::PostLoad() {
	Super::PostLoad();
	UE_LOG(LogTemp, Warning, TEXT("Nyt inititaan components vcoi pojat"));

	if (IsValid(CharacterDataAsset)) {
		UE_LOG(LogTemp, Warning, TEXT("nyt ollaan char asset isvalid"));
		SetCharacterData(CharacterDataAsset->CharacterData); 
	}
}

void ACharacterBaseGAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	//jos on replicated propertry ni pitää tehä tällä jotain??
	DOREPLIFETIME(ACharacterBaseGAS, CharacterData);
}


 void ACharacterBaseGAS::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {


	 Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	 //ei olla crouchaamassa, ok! SUper hoitaa
	 if (!CrouchStateEffect.Get()) return;

	 if (AbilitySystemComp) {


		 //The GameplayEffectSpec (GESpec) can be thought of as the instantiations of GameplayEffects.
		 //tehään  GA_Effect handle Abiltiysysemcomponentilla
		 FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();

		 //this is used to create spec, Jostakin efektistä, ja efekti on asetettu hard set by designer to "CrouchStateEffect" variable ja sieltä otetaaan.
		 FGameplayEffectSpecHandle CrouchSpecHandle = AbilitySystemComp->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContext); 

		 if (CrouchSpecHandle.IsValid()) {
			 //The ASC holds its current active GameplayEffects in FActiveGameplayEffectsContainer ActiveGameplayEffects
			 FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*CrouchSpecHandle.Data.Get());


			 //Debug
			 if (ActiveGEHandle.WasSuccessfullyApplied()) {
				 UE_LOG(LogTemp, Warning, TEXT("Ability %s failed to apply crouch effect %s"), *GetName(), *GetNameSafe(CrouchStateEffect));
			 }
		 }

	 }




 }


 void ACharacterBaseGAS::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {


	 //poistetaan efekti, sillä efekti loppuisi heti kun ei paina nappulaa, mutta nytten OnEndCrouch unrealin oma implementraatio kutsuu sitä ja sitten vasta poistetaa nefekti
	 if (AbilitySystemComp && CrouchStateEffect.Get()) {
		 AbilitySystemComp->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComp);
	 }
	 Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

 }


 UMotionWarpingComponentBase* ACharacterBaseGAS::GetMotionWarpingComponent() const {

	 return MotionWarpingComponent;
 }