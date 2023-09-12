// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/WeaponItemActor.h"
#include "PhysicalMaterialBase.h"
#include "InventoryItemInstance.h"
#include "Actors/BulletCasing.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/SkeletalMeshSocket.h"
#include "NiagaraFunctionLibrary.h"
#include "CrawlCommonTypes.h"




AWeaponItemActor::AWeaponItemActor():AItemActorBase(){


}


FVector AWeaponItemActor::GetMuzzleLocation() const {

	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation();

}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const {
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}



void AWeaponItemActor::InitInternal() {
	Super::InitInternal();

	//Haetaan weaponin static data ja lis‰t‰‰n se WEAPON ACTORIIN mik‰ on siis maailmassa, 


	if (const UWeaponStaticData* WeaponData = GetWeaponStaticData()) {

		UE_LOG(LogTemp, Warning, TEXT("AWeaponItemActor InitInternal"));

		//WEAPON HAS SKELETAL MESH COMPONENT DATA
		if (WeaponData->SkeletalMesh) {

			
			USkeletalMeshComponent* SkeletalComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("MeshComponent"));
			if (SkeletalComp) {
				UE_LOG(LogTemp, Warning, TEXT("WEAPON DATA > SKELETAL MESH"));
				SkeletalComp->RegisterComponent();
				SkeletalComp->SetSkeletalMesh(WeaponData->SkeletalMesh);
				SkeletalComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				this->AddInstanceComponent(SkeletalComp);
				MeshComponent = SkeletalComp;
			}
		}

		//WEAPON HAS ONLY STATIC MESH DATA
		else if (WeaponData->StaticMesh) { //ei o skeletal mesh component weapon datalla mut on static mesh


			UStaticMeshComponent* StaticMeshComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("MeshComponent"));

			StaticMeshComp->RegisterComponent();
			StaticMeshComp->SetStaticMesh(WeaponData->StaticMesh);
			StaticMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			this->AddInstanceComponent(StaticMeshComp);
			MeshComponent = StaticMeshComp;

		}

	}

}



/* RPCss‰ pit‰‰ lis‰‰ "implementation" suffix*/
void AWeaponItemActor::ServerPlayWeaponFireEffects_Implementation()
{

	MultiCastPlayWeaponFireEffects();
	//MultiCastPlayWeaponFireEffects();

}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& InHitResult)
{
	if (InHitResult.PhysMaterial.Get()) {

		UPhysicalMaterialBase* PhysicalMaterial = Cast<UPhysicalMaterialBase>(InHitResult.PhysMaterial.Get());
		if (PhysicalMaterial) {

			UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->PointImpactSound, InHitResult.Location, 1.0f);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactParticleFX, InHitResult.ImpactPoint);

		}

	
	}
	if (const UWeaponStaticData* WeaponData = GetWeaponStaticData()) {
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->AttackSound, GetActorLocation(), 1.0f);

		if (WeaponData->FXActorOnPrimaryAction) {
			USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(MeshComponent);
			const USkeletalMeshSocket* AmmoEjectSocket = Comp->GetSocketByName(FName("AmmoEject"));
			if (AmmoEjectSocket)
			{
				FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(Comp);
			

				UWorld* World = GetWorld();
				if (World) 
				{
				
					World->SpawnActor<ABulletCasing>(
						WeaponData->FXActorOnPrimaryAction,
						SocketTransform.GetLocation(),
						SocketTransform.GetRotation().Rotator()
						);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("GetWorld null"));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("AmmoEjectSocket null"));

			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WeaponData FXActorOnPrimaryAction"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("WeaponData nullptr"));
	}

}


void AWeaponItemActor::SpawnBulletCasing() {


	if (const UWeaponStaticData* WeaponData = GetWeaponStaticData()) {
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->AttackSound, GetActorLocation(), 1.0f);

		if (WeaponData->FXActorOnPrimaryAction) {
			USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(MeshComponent);
			const USkeletalMeshSocket* AmmoEjectSocket = Comp->GetSocketByName(FName("AmmoEject"));
			if (AmmoEjectSocket)
			{
				FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(Comp);


				UWorld* World = GetWorld();
				if (World)
				{

					World->SpawnActor<ABulletCasing>(
						WeaponData->FXActorOnPrimaryAction,
						SocketTransform.GetLocation(),
						SocketTransform.GetRotation().Rotator()
						);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("GetWorld null"));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("AmmoEjectSocket null"));

			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WeaponData FXActorOnPrimaryAction null"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("WeaponData null"));
	}

}

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& InHitResult)
{

	if (HasAuthority()) { //hasAuthority on server
		MulticastPlayWeaponEffects(InHitResult);
	}
	else {
		PlayWeaponEffectsInternal(InHitResult);
	}

}



void AWeaponItemActor::PlayWeaponFiringEffectsInternal() {

	/* Aseen Firing animaatio == ‰‰ni ja animaatio */
	/* Spawnaa bullet casing */

	const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData();
	if (WeaponStaticData) {
		UAnimationAsset* FireAnimAsset = WeaponStaticData->WeapoFireAnimation;
		const bool bNOLOOP_ANIMATION = false;
		// we have weapon fire animation, play it
		if (FireAnimAsset) {
			USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(GetMeshComponent());
			if (SkeletalMeshComp) {
				SkeletalMeshComp->PlayAnimation(FireAnimAsset, bNOLOOP_ANIMATION);
			}
		}
	}


	SpawnBulletCasing();

}



void AWeaponItemActor::PlayWeaponFiringEffects()
{
	

	/* Kutsutaan blueprintist‰ client ja servulle molemmille */
	/* Client playaa internalin itelleen, servu kutsuu kaikille mutta multicast checkaa et‰t ei ownerille*/
	
	
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("playing firing weapon effecst :HasAuthority()!"));
		MultiCastPlayWeaponFireEffects();
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("playing firing weapon effecst :Calling server weapon fire!"));
		ServerPlayWeaponFireEffects();
	}

	
	// t‰‰ kutsutaan vaan servulla, mutta pit‰‰ myˆs kertoa muille


	
}

//T‰m‰ kutsutaan kaikilla clineteill‰, koska servulla kutssutaan ni kutsuu itelleki
void AWeaponItemActor::MultiCastPlayWeaponFireEffects_Implementation() {
	/* Halutaan playata muille clienteille, jotka ei ole t‰m‰n omistaja, on lokaali ja on joko simulatedproxy tai authority*/

		/* t‰‰ kutsutaan clientilla ja executetaan servulta */
		/* Called to play weapon effects */

	UE_LOG(LogTemp, Warning, TEXT("Multicast at owner: %s"), *Owner->GetName());
	PlayWeaponFiringEffectsInternal();
	/*if (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy) {

		UE_LOG(LogTemp, Warning, TEXT("OWENR: %s"), *Owner.GetName());
		PlayWeaponFiringEffectsInternal();
	}
	*/
	

	/*
	const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData();
	if (WeaponStaticData) {
		UAnimationAsset* FireAnimAsset = WeaponStaticData->WeapoFireAnimation;	
		const bool bNOLOOP_ANIMATION = false;
		// we have weapon fire animation, play it
		if (FireAnimAsset) {
			USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(GetMeshComponent());
			if (SkeletalMeshComp) {
				SkeletalMeshComp->PlayAnimation(FireAnimAsset, bNOLOOP_ANIMATION);
			}
		}
	}
	*/


}


/* Multicastfunction checkaa onko kyseess‰ Autonomys proxy == eli owning client kuka ampuu abilityn, joten checkataan
	ett‰ ei executeta uudelleen multicastin kautta, katotaan onko kyseess‰ owning client. koska ampuu, execute owning clientill‰ internal (ei authority)
	ja sitten multicast kaikille event
*/
void AWeaponItemActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy) {
		PlayWeaponEffectsInternal(InHitResult);
	}
}


/* Uses Weapon (GUN) muzzleFlash socket location to spawn the projectiles direcxtion*/
FTransform AWeaponItemActor::GetFireDirection(const FHitResult& HitTarget) {

	FTransform SocketTransform = MeshComponent->GetSocketTransform(FName("MuzzleFlash"));

	//From muzzle flash socket to hit location from TraceUnderCrosshairs
	FVector ToTarget = HitTarget.ImpactPoint - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation(); //get rotation this vector represents

	FTransform SpawnTransform = FTransform(TargetRotation, SocketTransform.GetLocation(), FVector::OneVector);
	return SpawnTransform;

}