// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_GameplayAbility_RangedWeapon.h"
#include "AbilitySystemComponent.h"
#include "Character/CharacterBaseGAS.h"
#include "Inventory/Items/WeaponItemActor.h"
#include "CrawlGameStatics.h"
#include "PlayerStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/ProjectileBase.h"
#include "InventoryItemInstance.h"
#include "CrawlCollisionChannels.h"


UInventoryItemInstance* UGA_GameplayAbility_RangedWeapon::GetWeaponInstance() const
{
	

	if (GetEquippedItemInstance()) {

		return GetEquippedItemInstance();

	}
	else {
	
		return nullptr;
	}
	



}

int32 UGA_GameplayAbility_RangedWeapon::FindFirstPawnHitResult(const TArray<FHitResult>& HitResults)
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];

		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass())) {
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();
			//katotaan osuko johonki mik‰ on kiinni jossaki
			if (
					(HitActor != nullptr)
					&& (HitActor->GetAttachParentActor() != nullptr) 
					&& (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr)
				) 
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}

FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent)
{
	if (ConeHalfAngleRad > 0.f)
	{
		const float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

		// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
		// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
		const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
		const float AngleFromCenter = FromCenter * ConeHalfAngleDegrees;
		const float AngleAround = FMath::FRand() * 360.0f;

		FRotator Rot = Dir.Rotation();
		FQuat DirQuat(Rot);
		FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
		FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
		FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;
		FinalDirectionQuat.Normalize();

		return FinalDirectionQuat.RotateVector(FVector::ForwardVector);
	}
	else
	{
		return Dir.GetSafeNormal();
	}
}

void UGA_GameplayAbility_RangedWeapon::TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits)
{


	UInventoryItemInstance* ItemInstance = InputData.WeaponData;
	const int32 BulletsPerCartridge = ItemInstance->GetBulletsPerCartridge();

	for (int32 BulletIdx = 0; BulletIdx < BulletsPerCartridge; ++BulletIdx) {

		const float BaseSpreadAngle = ItemInstance->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = ItemInstance->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir,
			HalfSpreadAngleInRadians,
			ItemInstance->GetSpreadExponent());

		const FVector EndTrace = InputData.StartTrace + (BulletDir * ItemInstance->GetMaxDamageRange());

		FVector HitLocation = EndTrace;

		TArray<FHitResult> AllImpacts;

		FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace,
			ItemInstance->GetBulletTraceSweepRadius(), /*bIsSimulated=*/ false,
			/*out*/ AllImpacts);


		DrawDebugSphere(GetWorld(), Impact.Location, 42, 16, FColor::Green, false, 3);
		const AActor* HitActor = Impact.GetActor();

		if (HitActor)
		{
			if (AllImpacts.Num() > 0)
			{
				OutHits.Append(AllImpacts);
			}
			HitLocation = Impact.ImpactPoint;

		}

		//Jos ei osunu mihink‰‰, esim amutaan tyhj‰‰n
		// Make sure there's always an entry in OutHits so the direction can be used for tracers, etc...
		if (OutHits.Num() == 0)
		{
			if (!Impact.bBlockingHit)
			{
				// Locate the fake 'impact' at the end of the trace
				//sama ku teht‰is elsess‰ jonnekki kauas traceus
				Impact.Location = EndTrace;
				Impact.ImpactPoint = EndTrace;
			}

			OutHits.Add(Impact);
		}



	}



}

void UGA_GameplayAbility_RangedWeapon::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo()) {

		TArray<AActor*> AttachedActors;
		Avatar->GetAttachedActors(/*out*/ AttachedActors);
		TraceParams.AddIgnoredActors(AttachedActors);
	}
}



ECollisionChannel UGA_GameplayAbility_RangedWeapon::DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const
{
	return Crawl_TraceChannel_Weapon;
}

FHitResult UGA_GameplayAbility_RangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHitResults) const
{


	TArray<FHitResult> HitResults;
	FCollisionQueryParams TraceParams(
		SCENE_QUERY_STAT(WeaponTrace),
		/*bTraceComplex=*/ true,
		/*IgnoreActor=*/ GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;
	AddAdditionalTraceIgnoreActors(TraceParams); //lis‰t‰‰n parametreihin ampuja actorin kiinnitetyt paskat jos on tiell‰

	//haetaan ase trace channel
	const ECollisionChannel TraceChannel = DetermineTraceChannel(TraceParams, bIsSimulated);

	if (SweepRadius > 0.0f)
	{

		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{

		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);


		
		
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Cyan, false, 5);
	}

	FHitResult Hit(ForceInit);

	if (HitResults.Num() > 0)
	{


		// Filter the output list to prevent multiple hits on the same actor;
		// this is to prevent a single bullet dealing damage multiple times to
		// a single actor if using an overlap trace
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
			{
				return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
			};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		Hit = OutHitResults.Last();
	}
	else
	{
	
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	
	 

	return Hit;

}

FVector UGA_GameplayAbility_RangedWeapon::GetWeaponTargetingSourceLocation() const
{
	// Use Pawn's location as a base
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();
	const FQuat SourceRot = AvatarPawn->GetActorQuat();

	FVector TargetingSourceLocation = SourceLoc;

	//@TODO: Add an offset from the weapon instance and adjust based on pawn crouch/aiming/etc...

	return TargetingSourceLocation;
}


FTransform UGA_GameplayAbility_RangedWeapon::GetTargetingTransform(APawn* SourcePawn, ECrawlAbilityTargetingSource Source)  {

	/* AIM DIRECTION + AIM START */
	FHitResult HitResult;
	FTransform Trans;
	TraceUnderCrosshairs(HitResult, Trans);
	return Trans;

}
FHitResult UGA_GameplayAbility_RangedWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const
{

	static float DebugThickness = 1.0f;
//	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 3.f, 0, DebugThickness);


	FHitResult Impact;

// Trace and process instant hit if something was hit
// First trace without using sweep radius

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		
		Impact = WeaponTrace(StartTrace, EndTrace, /*SweepRadius=*/ 0.0f, bIsSimulated, /*out*/ OutHits); 
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
	
		// If this weapon didn't hit anything with a line trace and supports a sweep radius, try that
		if (SweepRadius > 0.0f)
		{
		
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, /*out*/ SweepHits);

			// If the trace with sweep radius enabled hit a pawn, check if we should use its hit results
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// If we had a blocking hit in our line trace that occurs in SweepHits before our
				// hit pawn, we should just use our initial hit results since the Pawn hit should be blocked
				bool bUseSweepHits = true;
				for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
				{
					const FHitResult& CurHitResult = SweepHits[Idx];

					auto Pred = [&CurHitResult](const FHitResult& Other)
					{
						return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
					};
					if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
					{
						bUseSweepHits = false;
						break;
					}
				}

				if (bUseSweepHits)
				{
					OutHits = SweepHits;
				}
			}
		}
	}


	return Impact;
}


void UGA_GameplayAbility_RangedWeapon::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits) {

	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	UInventoryItemInstance*  WeaponData = GetEquippedItemInstance();




	if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponData)
	{

	
		FRangedWeaponFiringInput InputData;
		InputData.WeaponData = WeaponData;
		InputData.bCanPlayBulletFX = (AvatarPawn->GetNetMode() != NM_DedicatedServer);

		//@TODO: Should do more complicated logic here when the player is close to a wall, etc...
		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, ECrawlAbilityTargetingSource::CameraTowardsFocus);
		InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();

		const float WEAPON_MAX_RANGE = 10000.f; //@TODO: PARAMETRIKSI ASEESEEN
		InputData.EndAim = InputData.StartTrace + InputData.AimDir * WEAPON_MAX_RANGE; /* T*/



		static float DebugThickness = 2.0f;
	//	DrawDebugLine(GetWorld(), InputData.StartTrace, InputData.StartTrace + (InputData.AimDir * 100.0f), FColor::Yellow, false, 3, 0, DebugThickness);
	
	
	
	//T‰h‰n viel‰  
	//	TraceBulletsInCartridge(InputData, /*out*/ OutHits);
		TraceBulletsInCartridge(InputData, /*out*/ OutHits);
	
	}


}



void UGA_GameplayAbility_RangedWeapon::FireProjectileFromMuzzleToHitDirection(const FHitResult& HitResult)
{

	AWeaponItemActor* WeaponActor = GetEquippedWeaponItemActor();

	if (!WeaponActor) {
		return;
	}

	ACharacterBaseGAS* OwnerCharacter = GetCharacterBaseFromActorInfo();

	FVector HitLocation = HitResult.Location;
	FVector MuzzleLocation = WeaponActor->GetMuzzleLocation();

	FRotator AimRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, HitLocation);

	if (GetEquippedItemWeaponStaticData()->WeaponFireProjectileClass == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Fire Weapon Projectile: PROJECTIL CLASS NULL ON weapon static data"));
	}

	UCrawlGameStatics::LaunchProjectileActor(
		GetWorld(),													    /* World context object */
		GetEquippedItemWeaponStaticData()->WeaponFireProjectileClass ,  /* Projectile class */
		FTransform(AimRotation, MuzzleLocation, FVector::OneVector),    /* Rotation and SPawn location as trasnform*/
		OwnerCharacter,												    /* Owner */
		OwnerCharacter												    /* Instigator */
	);

	
}

bool UGA_GameplayAbility_RangedWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{


	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	return bResult;
}

	/*if (bResult)
	{
		if (GetEquippedItemInstance() == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon ability %s cannot be activated because there is no associated ranged weapon (equipment instance=%s but needs to be derived from %s)"),
				*GetPathName(),
				*GetPathNameSafe(GetEquippedItemInstance()),
				*UInventoryItemInstance::StaticClass()->GetName());
			bResult = false;
		}
	}
	*/



void UGA_GameplayAbility_RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	OnTargetDataReadyCallbackDelegateHandle = MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

	// Update the last firing time
	UInventoryItemInstance* WeaponData = GetEquippedItemInstance();


	if (WeaponData) {
		WeaponData->UpdateFiringTime();
	}


	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
 void UGA_GameplayAbility_RangedWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) {
	
	 
	 if (IsEndAbilityValid(Handle, ActorInfo))
	 {
		 if (ScopeLockCount > 0)
		 {
			 WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			 return;
		 }

		 UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
		 check(MyAbilityComponent);


		 // When ability ends, consume target data and remove delegate
		 MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		 MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());


		 Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	 }

}



 void UGA_GameplayAbility_RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) {
	 UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	 check(MyAbilityComponent);

	 if (MyAbilityComponent) {

	
	 }

	 if (const FGameplayAbilitySpec* AbilitySpec = MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	 {
		 FScopedPredictionWindow ScopedPrediction(MyAbilityComponent);





		 // Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		 FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));


		 /* Lokaalisti ohjattu eik‰ ole serveri, niin l‰hetet‰‰n servulla datea*/








		// FString asd2 =GetEquippedItemInstance()->GetPawn()->IsLocallyControlled() ? "true": "false";
	//	 UE_LOG(LogTemp, Warning, TEXT("LOcallyControlled from player pawn: state value is: %s"), *GetEquippedItemInstance()->GetName(), *asd2);
	
		 const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		 if (bShouldNotifyServer)
		 {
			 UE_LOG(LogTemp, Warning, TEXT("Notifying server for data..."));
			 //Jos ShouldProduceTargetDataOnServer  == false
			 //Client will RPC TargetData to the server on confirmation via CallServerSEtReplicatedData in OnTargetDataReadyCallbCk.
			 /* Client to server RPC ->*/
			 MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, 
				 CurrentActivationInfo.GetActivationPredictionKey(),
				 LocalTargetDataHandle,
				 ApplicationTag,
				 MyAbilityComponent->ScopedPredictionKey);
			 /* Confirmaa servuylla ett‰ t‰‰l ois TargetDataa hei*/

		 }

		 const bool bIsTargetDataValid = true;
		 bool bProjectileWeapon = false;



		/* TƒS OLIS JOTAA SERVER CODE?*/
		/* TƒS OLIS JOTAA SERVER CODE?*/
		/* TƒS OLIS JOTAA SERVER CODE?*/
		/* TƒS OLIS JOTAA SERVER CODE?*/
		
				// See if we still have ammo
		 if (bIsTargetDataValid && CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, nullptr))
		 {
			 // We fired the weapon, add spread
			 UInventoryItemInstance* WeaponData = GetEquippedItemInstance();
			 if (WeaponData) {
				 WeaponData->AddSpread();
			 }
	
			

			 // Let the blueprint do stuff like apply effects to the targets
			OnRangedWeaponTargetDataReady(LocalTargetDataHandle); 	 //Blueprint implementable event
	
		 }
		 else
		 {
			 UE_LOG(LogTemp, Warning, TEXT("Weapon ability %s failed to commit (bIsTargetDataValid=%d)"), *GetPathName(), bIsTargetDataValid ? 1 : 0);
			 K2_EndAbility();
		 }


	 }

	 // We've processed the data?? Consume == ?? wat
	 MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

 }

 void UGA_GameplayAbility_RangedWeapon::GetCrosshairHitResultTargeting() {

	 FHitResult Hitres;
	 GetActorsClientCrosshairTraceHit(Hitres);
	 FGameplayAbilityTargetingLocationInfo Info;
	 FGameplayAbilityTargetDataHandle TargetData2 = Info.MakeTargetDataHandleFromHitResult(this, Hitres);

	 OnTargetDataReadyCallback(TargetData2, FGameplayTag());

 }

 /* T‰t‰ kutsutaan BP fire abilityss‰ IsLocalCOntrolledin takana eli vaan ite kontrolleraaja ktusuu*/
 void UGA_GameplayAbility_RangedWeapon::StartRangedWeaponTargeting() {

	
	 check(CurrentActorInfo);
	 AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	 check(AvatarActor);


	 UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	 check(MyAbilityComponent);


	 AController* Controller = GetControllerFromActorInfo();
	 check(Controller);



	 //ULyraWeaponStateComponent* WeaponStateComponent = Controller->FindComponentByClass<ULyraWeaponStateComponent>();
	 FScopedPredictionWindow ScopedPrediction(MyAbilityComponent, CurrentActivationInfo.GetActivationPredictionKey());

	 /* Teh‰‰n LOCAL targeting*/
	 TArray<FHitResult> FoundHits;
	 PerformLocalTargeting(/*out*/ FoundHits);

	 // Fill out the target data from the hit results
	 FGameplayAbilityTargetDataHandle TargetData;

	 if (FoundHits.Num() > 0)
	 {
		 const int32 CartridgeID = FMath::Rand();

		 for (const FHitResult& FoundHit : FoundHits)
		 {
			 FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			 NewTargetData->HitResult = FoundHit;
			// NewTargetData->CartridgeID = CartridgeID;

			 TargetData.Add(NewTargetData);
		 }
	 }

	 // Send hit marker information
	 /* const bool bProjectileWeapon = false;
	 if (!bProjectileWeapon && (WeaponStateComponent != nullptr))
	 {
		 WeaponStateComponent->AddUnconfirmedServerSideHitMarkers(TargetData, FoundHits);
	 } 
	 */
	 /*
	 
	 then the client will RPC its TargetData to the server on 
	 confirmation via CallServerSetReplicatedTargetData() in
	 UAbilityTask_WaitTargetData::OnTargetDataReadyCallback()
	 */
	 /* Target dataan tungetto kaikki mihi osuttu TargetData*/
	 // Process the target data immediately+
	

	 OnTargetDataReadyCallback(TargetData, FGameplayTag());
	// OnTargetDataReadyCallback(TargetData, FGameplayTag());
 }


 void UGA_GameplayAbility_RangedWeapon::GetActorsClientCrosshairTraceHit(FHitResult& HitResult) {


	 FTransform Trans;
	 TraceUnderCrosshairs(HitResult, Trans);

 }