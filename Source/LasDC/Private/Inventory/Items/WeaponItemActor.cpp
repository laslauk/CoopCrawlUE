// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/WeaponItemActor.h"
#include "PhysicalMaterialBase.h"
#include "InventoryItemInstance.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "CrawlCommonTypes.h"




AWeaponItemActor::AWeaponItemActor() {


}


FVector AWeaponItemActor::GetMuzzleLocation() const {

	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation();

}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const {
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}



void AWeaponItemActor::InitInternal() {
	Super::InitInternal();

	//Haetaan weaponin static data ja lisätään se WEAPON ACTORIIN mikä on siis maailmassa, 


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

/* Multicastfunction checkaa onko kyseessä Autonomys proxy == eli owning client kuka ampuu abilityn, joten checkataan
	että ei executeta uudelleen multicastin kautta, katotaan onko kyseessä owning client. koska ampuu, execute owning clientillä internal (ei authority)
	ja sitten multicast kaikille event
*/
void AWeaponItemActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy) {
		PlayWeaponEffectsInternal(InHitResult);
	}
}
