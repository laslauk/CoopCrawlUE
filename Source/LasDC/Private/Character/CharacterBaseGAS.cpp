// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBaseGAS.h"
#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "Net/UnrealNetwork.h"
#include "InventoryItemInstance.h"
#include "InventoryComp.h"
#include "Components/CapsuleComponent.h"
#include "ActorComponents/SurvivalStatsComponent.h"
#include "ItemActorBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActorComponents/MotionWarpingComponentBase.h"
#include "InventoryList.h"
#include "Attributes/AttributeSetAiming.h"

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

ACharacterBaseGAS::ACharacterBaseGAS()
{

}

ACharacterBaseGAS::ACharacterBaseGAS(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponentBase>
		(CharacterMovementComponentName)) {

	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 66.f; //Common values
	MinNetUpdateFrequency = 33.f;//Common value for fast pace shooter, also server tickrate in DefaultEngine.ini, onlinesubsystem utils

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 560.f);

	//bReplicates = true;
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
	TurningInPlace = ETurningInPlace::NotTurning;
	
}

void ACharacterBaseGAS::SimProxiesTurn()
{

	if (IsEquippingItem()) 
	{
		bRotateRootBone = false;
	}
}

void ACharacterBaseGAS::MulticastDamageReact_Implementation()
{
	PlayHitReactMontage();
}

//void ACharacterBaseGAS::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {



//}

void ACharacterBaseGAS::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


	if(GetLocalRole() > ENetRole:: ROLE_SimulatedProxy)  // > simproxy, on actively locally controlled or authority.
	{ //lasketaan autonomnousproxy, authority, max
		AimOffset(DeltaTime);

	}
	else {
		//on sim proxy, eli toiset actorit jotka jokuj c lient näkee jota ei ite ohjaa
		SimProxiesTurn();
	}
	
}


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

APlayerControllerBase* ACharacterBaseGAS::GetPlayerControllerBase()
{
	return PlayerControllerBase;
}



void ACharacterBaseGAS::PostInitializeComponents() {

	Super::PostInitializeComponents();

	


}


void ACharacterBaseGAS::ResposneToLasseDelegate(float a, float b) {
	UE_LOG(LogTemp, Warning, TEXT("Hello: %f, %f"), a, b);
}

void ACharacterBaseGAS::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	if (AbilitySystemComp) {
		AbilitySystemComp->RemoveActiveEffectsWithTags(InAirTags);
		AbilitySystemComp->RemoveActiveGameplayEffectBySourceEffect(FallingGameplayEffect, AbilitySystemComp);
	}
}


void ACharacterBaseGAS::TryJump() {


	if(CharacterMovementComponent) {
		UE_LOG(LogTemp, Warning, TEXT("> [1/2]ACharacterBase] Try Jump ump.."));
	CharacterMovementComponent->TryTraversal(AbilitySystemComp);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("> [ERROR /2]ACharacterBase] CharacterMovementComponent nullptr.."));
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
			FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComp->GiveAbility(FGameplayAbilitySpec(DefaultAbility));


		
			if (FGameplayAbilitySpec* AbilitySpec =AbilitySystemComp->FindAbilitySpecFromHandle(AbilitySpecHandle)) {
				if (AbilitySpec->Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Combat.Aim"))))
				{
					UE_LOG(LogTemp, Warning, TEXT("IS AIMING ABIITY!! %s"), *AbilitySpec->GetDebugString());
					GetPlayerController()->AimAbility = AbilitySpecHandle;
				}
			}
			
			

			

		
		}
	}

}




APlayerControllerBase* ACharacterBaseGAS::GetPlayerController() {

	return PlayerControllerBase;
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

void ACharacterBaseGAS::Falling()
{
	if (!AbilitySystemComp) { 
		UE_LOG(LogTemp, Warning, TEXT("EI APBILTIY SYMST!"));
		return; }
	

	FGameplayEffectContextHandle EffectContext = AbilitySystemComp->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(FallingGameplayEffect, 1, EffectContext);
	
	if (SpecHandle.IsValid()) {

		FActiveGameplayEffectHandle ActiveGEHandle  = AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (ActiveGEHandle.WasSuccessfullyApplied()) {
			UE_LOG(LogTemp, Warning, TEXT("APPLIED ON FALLING"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("EI APBILTIY Falling SYMST!"));


		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("EI O VALID SPEC HADLe"));

	}
	


}

void ACharacterBaseGAS::ServerSetAimingStatus_Implementation(bool isAiming) {
	bIsAiming = isAiming;
}


//PossessedBy - Server Only
void ACharacterBaseGAS::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("bbbbbbbb"));

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	SetAutonomousProxy(true);

	// ASC MixedMode replication requires that the ASC Owner's Owner be the Controller.
	SetOwner(NewController);

	if (PS)
	{
		PS_ref = PS;
		PlayerControllerBase = Cast<APlayerControllerBase>(PS_ref->GetPlayerController());


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

bool ACharacterBaseGAS::IsEquippingItem() const {
	if (PS_ref) {
		if (PS_ref->InventoryComp) {
			return PS_ref->InventoryComp->IsOwnerEquippingItem();
		}
	} 
	return false;
}

AItemActorBase* ACharacterBaseGAS::GetEquippedItemActor() const
{
	if (PS_ref == nullptr) return nullptr;
	if (PS_ref->InventoryComp == nullptr) return nullptr;
	if (PS_ref->InventoryComp->GetEquippedItem() == nullptr) return nullptr;
	return PS_ref->InventoryComp->GetEquippedItem()->GetItemActor();
}

USkeletalMeshComponent* ACharacterBaseGAS::GetEquippedWeaponMeshComponent() const
{

	AItemActorBase* Item = GetEquippedItemActor();
	if (Item) {

		return	Item->FindComponentByClass<USkeletalMeshComponent>();
	
	}
	return nullptr;
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

void ACharacterBaseGAS::PlayHitReactMontage()
{
	if (HitReactMontage) {

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance) 
		{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("Front");
		AnimInstance->Montage_JumpToSection(SectionName);
		}
	
	}
}

void ACharacterBaseGAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//jos on replicated propertry ni pitää tehä tällä jotain??
	DOREPLIFETIME(ACharacterBaseGAS, CharacterData);
	DOREPLIFETIME(ACharacterBaseGAS, bIsAiming);
	//DOREPLIFETIME(ACharacterBaseGAS, OverlappingItem);
	DOREPLIFETIME(ACharacterBaseGAS, Test);
	DOREPLIFETIME_CONDITION(ACharacterBaseGAS, OverlappingItem, COND_OwnerOnly); //OverlappingItem will replicate to only playerClients who owns this CharacteRBaseGAS pawn
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
				 UE_LOG(LogTemp, Warning, TEXT("Ability %s Succesfully applied crouch effect %s"), *GetName(), *GetNameSafe(CrouchStateEffect));
			 }
		 }

	 }

 }

 bool ACharacterBaseGAS::IsAiming() const {
	
	 /* checkataan onko ability systemillä effect tag? */
	 return bIsAiming;
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

 void ACharacterBaseGAS::AimOffset(float DeltaTime)
 {
	 
	 if (!PS_ref) { return; }
	 if (!PS_ref->InventoryComp) { return; }
	 if (PS_ref->InventoryComp->CurrentEquippedItem == nullptr) { return; }
	
	 FVector Velocity = GetVelocity();
	 Velocity.Z = 0.f;
	 float Speed = Velocity.Size();
	 bool bIsInAir = GetCharacterMovement()->IsFalling();

	 if (!bIsInAir && Speed == 0.f) 
	 { //standing still not jumping
		 bRotateRootBone = true;
		 FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		 FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		// bUseControllerRotationYaw = true;

		 if (TurningInPlace == ETurningInPlace::NotTurning) {
			 InterpAO_Yaw = AO_Yaw;
		 }

		 AO_Yaw = DeltaAimRotation.Yaw;
		 TurnInPlace(DeltaTime);

	 }
	 if (Speed > 0.f || bIsInAir) { //running or jumping
		 StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		// bUseControllerRotationYaw = true;
		 TurningInPlace = ETurningInPlace::NotTurning;
		 AO_Yaw = 0.f;
		 bRotateRootBone = false;
	 }

	 /* ONGELMANA: LIITTYY UNREALIN ROTATION PACKAGAUKSEEN KUN LÄHETTÄÄ OVER NETWORK, koska compressaa. rotation compressed down to 5 bytes, 
	 jos on negative, niin se compressetaan unsigned valuee range [0,360). Joten ei pysty noin, pitää converttaa, */
	 AO_Pitch = GetBaseAimRotation().Pitch;

	 if (AO_Pitch > 90.f && !IsLocallyControlled()) { //tehdään vaan clienteille jotka ei o local, vaan joku toinen hahmo toisen näöäkökulmasta
		 //mapataan pitch from [270,360) to [-90,0)
		 FVector2D InRange(270.f, 360.f);
		 FVector2D OutRange(-90.f, 0.);
		 AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	 }
	


	
	 /* > !HasAuthority ja IsLocallyControlled ---- Character joka on Clientillä ja se mitä siinä ohjataan* /
	 /* > HasAuthority() ja !isLocallyControlled --- Character joka exists servulla, mutta ei controlloitu
	 * 
	 ** */
	 /*
	 * 
	 * 
	 if (HasAuthority() && !IsLocallyControlled()) {
		 UE_LOG(LogTemp, Warning, TEXT("AO Pithc: %f"), AO_Pitch);
		 

	 }

	 */

 }

 void ACharacterBaseGAS::TurnInPlace(float DeltaTime) {

	 if (AO_Yaw > 90.f) { //jos kattoo oikealle niin --> 90 kohti
		 TurningInPlace = ETurningInPlace::Right;
	 }
	 else if (AO_Yaw < -90.f) { //Vasemmallepäin, jos reachaa ni sit pitäisi turnaa, eli kamerna mukana
		 TurningInPlace = ETurningInPlace::Left;
	 }

	 if (TurningInPlace != ETurningInPlace::NotTurning) {
		 InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 10.f);
		 AO_Yaw = InterpAO_Yaw;


		 if (FMath::Abs(AO_Yaw) < 15.f) {
			 TurningInPlace = ETurningInPlace::NotTurning;
			 StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		 }

	 }

 }

 void ACharacterBaseGAS::OnRep_OverlappingItem(AItemActorBase* LastValue)
 {
	// GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item: %s,  %s"), *GetOwner()->GetName(), *ItemStaticData->Name.ToString()));
	 GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString::Printf(TEXT("On_Rep Overlapping Item Called")));
	 UE_LOG(LogTemp, Warning, TEXT("OnRep_OverlappingItem called"));
	 if (OverlappingItem) {
		 OverlappingItem->ShowPickupWidget(true);
	 }
	
	 if(LastValue)
	 {
		 LastValue->ShowPickupWidget(false);
	 }
 }

 void ACharacterBaseGAS::SetOverlappingItem(AItemActorBase* Item)
 {
	 GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString::Printf(TEXT("SetOverlappingItem set")));
	 UE_LOG(LogTemp, Warning, TEXT("SetOverlappingItem set"));



	 if (OverlappingItem) {

		 OverlappingItem->ShowPickupWidget(false);

	 }


	 OverlappingItem = Item;

	 /* If local , not on server*/
	 if (IsLocallyControlled())  /* for server to showw*/
	 {
		 if (OverlappingItem)
		 {
			 OverlappingItem->ShowPickupWidget(true);
		 }
	 }
 } 

 void ACharacterBaseGAS::OnRep_Test()
 {
	 UE_LOG(LogTemp, Warning, TEXT("On TEST TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST!"));

 }

