// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/AmmoItemActor.h"
#include "CrawlCommonTypes.h"
#include "InventoryItemInstance.h"


const UAmmoItemStaticData* AAmmoItemActor::GetAmmoItemStaticData() const
{
	return ItemInstance ? Cast<UAmmoItemStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AAmmoItemActor::InitInternal()
{
	Super::InitInternal();


	if (const UAmmoItemStaticData* AmmoData = GetAmmoItemStaticData()) {

		if (AmmoData->StaticMesh) {

			//Create static mesh component
			UStaticMeshComponent* StaticMeshComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("MeshComponent"));
			StaticMeshComp->RegisterComponent();

			StaticMeshComp->SetStaticMesh(AmmoData->StaticMesh);
			StaticMeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		}
	}
}
