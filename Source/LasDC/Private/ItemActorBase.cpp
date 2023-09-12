// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActorBase.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStateBase.h"
#include "InventoryItemInstance.h"
#include "CharacterBaseGAS.h"
#include "InventoryList.h"
#include "CrawlCommonTypes.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CrawlCommonTypes.h"
#include "Engine/ActorChannel.h"

/* HUOM, TÄMÄ ACTOR VAIN DROPATAAN KUN UNEQUIP JA */

// Sets default values
AItemActorBase::AItemActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	

	if (HasAuthority()) {

		SetReplicatingMovement(true);
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("USphereComponent"));
		SetRootComponent(SphereComponent);
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActorBase::OnPickupCollisionOverlapStart);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItemActorBase::OnPickupCollisionOverlapEnd);
		SphereComponent->SetSphereRadius(64.f);

	}


	PickupTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget")); 
	PickupTextWidget->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale); 
	PickupTextWidget->RegisterComponent();


	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AItemActorBase::BeginPlay()
{
	Super::BeginPlay();


	if (PickupTextWidget) {
		PickupTextWidget->SetVisibility(false);
	
	}


	if (HasAuthority()) {
	
		//initially placed in the worl
		if(!IsValid(ItemInstance) && IsValid(ItemStaticDataClass )) 
		{
			ItemInstance = NewObject<UInventoryItemInstance>();
			ItemInstance->Init(ItemStaticDataClass, nullptr, Quantity);

			UE_LOG(LogTemp,Warning,TEXT(">>>>>>>>TEHTIIN UUS ITEM ACTOR: %s  , instance name: %s--- STATIOC DATA  BASE:: %s"), *GetName(), *ItemInstance->GetName(), *ItemStaticDataClass->GetName());
			
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SphereComponent->SetGenerateOverlapEvents(true);

			InitInternal(); //placed in editor for exapmple
		}
	}
	
}

// Called every frame
void AItemActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



/* TODO: SELVITÄ TÄMÄN TOIMINTA*/
bool AItemActorBase::ReplicateSubobjects(UActorChannel* channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) {

	bool WroteSomething = Super::ReplicateSubobjects(channel, Bunch, RepFlags);
	WroteSomething |= channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
	return WroteSomething;
}



 void AItemActorBase::OnEquipped() {
	 ItemState = EItemState::Equipped;
	 SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 SphereComponent->SetGenerateOverlapEvents(false);
 }

 void AItemActorBase::OnUnequipped() {
	 ItemState = EItemState::None;
	 SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 SphereComponent->SetGenerateOverlapEvents(false);
 }

 void AItemActorBase::OnDropped() {

	 GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);


	 //NOTE, DUMB WAY TO DO
	
	
	 if (GetOwningPlayerState()) 
	 {
		 UE_LOG(LogTemp, Warning, TEXT("ON DROP - Onko player state owner set?? %s"), *GetOwningPlayerState()->GetName());

		 const FVector loc = GetActorLocation();

		 UE_LOG(LogTemp, Warning, TEXT("ITEM ACTOR LOC: %f , %f ,%f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);


		 constexpr float dropItemDist = 300.f;
		 constexpr float dropItemTraceDist = 1000.f;

		 if (ACharacter* character = Cast<APlayerStateBase>(GetOwningPlayerState())->playerCharacterRef) 
		 {
			 const FVector forward = character->GetActorForwardVector();
			 const FVector TraceStart = loc + forward * dropItemDist;
			 const FVector TraceEnd = TraceStart - FVector::UpVector * dropItemTraceDist;

			 FCollisionQueryParams QueryParams;
			 QueryParams.AddIgnoredActor(this);
			 FHitResult Hit;


			 GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, QueryParams);
			 DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);

			 if (Hit.bBlockingHit)
			 {
				 SetActorLocation(Hit.Location+FVector(.0f,.0f,50));

			//	 PickupTextWidget->SetWorldLocation(GetActorLocation());
				 UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!! Dropattiin item actor to location: %s "), *PickupTextWidget->GetComponentLocation().ToString());


				 ItemState = EItemState::Dropped;
				 SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				 UE_LOG(LogTemp, Warning, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
				 SphereComponent->SetGenerateOverlapEvents(true);
				 ItemInstance->OwningPlayerState = nullptr;
			 }
		 }
	 }
 }

 AActor* AItemActorBase::GetOwningPlayerState() {
	 if (ItemInstance) {
		return ItemInstance->OwningPlayerState;
	 }
	 return nullptr;
 }


 void AItemActorBase::Init(class UInventoryItemInstance* InInstance, int32 InQuantity) {

	 ItemInstance = InInstance;
	 Quantity = InQuantity;
	 InitInternal();

 }


 //Clientside stuff for actor properties when State changes at server
 void AItemActorBase::OnRep_ItemState() {
	 switch (ItemState) {

	 case EItemState::None: {
		 SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		 SphereComponent->SetGenerateOverlapEvents(true);
		 break;
	 }
	 case EItemState::Equipped: {
		 SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		 SphereComponent->SetGenerateOverlapEvents(false);
		 break;
	 }

	 case EItemState::Dropped: {
		 SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		 SphereComponent->SetGenerateOverlapEvents(true);
	break;
	
	}



	 default:break;
	 }
 }

 void AItemActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	 Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	 //jos on replicated propertry ni pitää tehä tällä jotain??
	 DOREPLIFETIME(AItemActorBase, ItemInstance);
	 DOREPLIFETIME(AItemActorBase, ItemState);
	 DOREPLIFETIME(AItemActorBase, Quantity);
	
 }

 void AItemActorBase::OnPickupCollisionOverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
	 int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) {

		
	 ACharacterBaseGAS* OverlappingCharacter = Cast<ACharacterBaseGAS>(otherActor);
	 if (!OverlappingCharacter) return;
	 UE_LOG(LogTemp, Warning, TEXT("OVERLAPPING ITEM : +   VLAUE Character %s!!!"), *OverlappingCharacter->GetName());

	 if (HasAuthority()) {
	
		OverlappingCharacter->SetOverlappingItem(this);
		UE_LOG(LogTemp, Warning, TEXT("OVERLAPPING ITEM : + SETTING TEST VLAUE Character %s!!!"), *OverlappingCharacter->GetName());

	 }
	 /* pickup item if collision overlap, send gameplay event to actor [EquipItemActorTag]*/
	 if (bOverlapPicksup) {
		 if (HasAuthority()) {
			 UE_LOG(LogTemp, Warning, TEXT("OVERLAPPING  WITH AUTHORITY!!!"));
				FGameplayEventData EventPayload;
				EventPayload.Instigator = this;
				EventPayload.OptionalObject = ItemInstance;
				EventPayload.EventTag = EquipItemActorTag; /* Tämä menee Characterin InventoryCompille*/
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(otherActor, EquipItemActorTag, EventPayload);
		 }
	 }
 }



 void AItemActorBase::OnPickupCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* otherActor,
	 UPrimitiveComponent* otherComp, int32 otherBodyIndex)
 {
	 ACharacterBaseGAS* OverlappingCharacter = Cast<ACharacterBaseGAS>(otherActor);
	 if (OverlappingCharacter) {
		 OverlappingCharacter->SetOverlappingItem(nullptr);
	 }
	 /*
	// if (HasAuthority()) {
	//	 Cast<ACharacterBaseGAS>(otherActor)->Test = this;
	// }
//	 UE_LOG(LogTemp, Warning, TEXT(">>OnPickupCollisionOverlapEnd"));


	 ACharacterBaseGAS* OverlappingCharacter = Cast<ACharacterBaseGAS>(otherActor);
	 if (OverlappingCharacter)
	 {
		 if (HasAuthority())
		 {
			 OverlappingCharacter->SetOverlappingItem(this);
		 }
	 }
	 */
 }


 void AItemActorBase::OnRep_ItemInstance(UInventoryItemInstance* OldItemInstanceValue) {
	 UE_LOG(LogTemp, Warning, TEXT("xxxxxx -> calling from onrep_itemInstance init interal"));

	 if (IsValid(ItemInstance) && !IsValid(OldItemInstanceValue)) //first time received this intsance 
	 {
		 InitInternal();
		 UE_LOG(LogTemp, Warning, TEXT("xxxxxx -> calling from onrep_itemInstance init interal"));
	 }
 }

 void AItemActorBase::InitInternal() {

	 UE_LOG(LogTemp, Warning, TEXT("AItemActorBase InitInternal"));


 }

 void AItemActorBase::ShowPickupWidget(bool bShowWidget)
 {
	 UE_LOG(LogTemp, Warning, TEXT("ShowPickupWidget called [1/2] "));
	 if (PickupTextWidget) 
	 {
		 UE_LOG(LogTemp, Warning, TEXT("ShowPickupWidget called [2/2]  Success!"));
		 PickupTextWidget->SetVisibility(bShowWidget);
	 }
 }

