// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActorBase.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStateBase.h"
#include "InventoryItemInstance.h"
#include "CharacterBaseGAS.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CrawlCommonTypes.h"
#include "Engine/ActorChannel.h"

// Sets default values
AItemActorBase::AItemActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("USphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActorBase::OnPickupCollisionOverlap);
	SphereComponent->SetSphereRadius(64.f);



	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AItemActorBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		if(!IsValid(ItemInstance) && IsValid(ItemStaticDataClass )) 
		{
			ItemInstance = NewObject<UInventoryItemInstance>();
			ItemInstance->Init(ItemStaticDataClass, nullptr )
		}
	}
	
}

// Called every frame
void AItemActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

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

	 //NOTE, DUMB WAY TO DO
	  
	 GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	 if (OwningPlayerState) {


		 UE_LOG(LogTemp, Warning, TEXT("ITEM ACTOR BASE OWENR: %s"), *OwningPlayerState->GetName());

		
		 const FVector loc = GetActorLocation();

		 UE_LOG(LogTemp, Warning, TEXT("ITEM ACTOR LOC: %f , %f ,%f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);


		 constexpr float dropItemDist = 300.f;
		 constexpr float dropItemTraceDist = 1000.f;

		 if (!OwningPlayerState->playerCharacterRef) {

			 UE_LOG(LogTemp, Warning, TEXT(">>>>>>>> EI PLAYER CHAR REF"));
			 return;
		 }

		 const FVector forward = OwningPlayerState->playerCharacterRef->GetActorForwardVector();
		 const FVector TraceStart = loc + forward * dropItemDist;
		 const FVector TraceEnd = TraceStart - FVector::UpVector * dropItemTraceDist;

		 FCollisionQueryParams QueryParams;
		 QueryParams.AddIgnoredActor(this);
		 FHitResult Hit;

		 DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);
		 GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, QueryParams);

		 if (Hit.bBlockingHit)
		 {
			 SetActorLocation(Hit.Location+FVector(.0f,.0f,50));
			 ItemState = EItemState::Dropped;
			 SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			 UE_LOG(LogTemp, Log, TEXT("Trace hit actor: %s"), *Hit.GetActor()->GetName());
			 OwningPlayerState = nullptr;
			 SphereComponent->SetGenerateOverlapEvents(true);
		 }
		 else {

		 }
	 }
	 else {
		 UE_LOG(LogTemp, Warning, TEXT("ITEM ACTOR  TYHJÄ BASE OWNING %s"));

	 }

 }

 void AItemActorBase::Init(class UInventoryItemInstance* InInstance) {


	 ItemInstance = InInstance;
	 OwningPlayerState = Cast<APlayerStateBase>(ItemInstance->OwningPlayerState);
	 

 }

 void AItemActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	 Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	 //jos on replicated propertry ni pitää tehä tällä jotain??
	 DOREPLIFETIME(AItemActorBase, ItemInstance);
	 DOREPLIFETIME(AItemActorBase, ItemState);
 }

 void AItemActorBase::OnPickupCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
	 int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult) {

	 if (HasAuthority()) {

			FGameplayEventData EventPayload;
			EventPayload.Instigator = this;
			EventPayload.OptionalObject = ItemInstance;
			EventPayload.EventTag = UInventoryComp::EquipItemActorTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(otherActor, OverlapEventTag, EventPayload);
	 }
 }