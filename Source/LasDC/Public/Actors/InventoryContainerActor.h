// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "GameFramework/Actor.h"
#include "PlayerControllerBase.h"
#include "InventoryList.h"
#include "NativeGameplayTags.h"

#include "InventoryContainerActor.generated.h"


class UInventoryContainerComponent;
class UUserWidget;
class UItemInstance;



UE_DECLARE_GAMEPLAY_TAG_EXTERN(OpenInventoryContainerEventTag)

UCLASS()
class LASDC_API AInventoryContainerActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryContainerActor();

	UInventoryContainerComponent* InventoryContainerComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UUserWidget* OpenedInventoryWidget;

public:	

	

	UFUNCTION(Server, Reliable)
	void ServerHandleInventoryTakeAll(APlayerControllerBase* OtherController);



	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_InventoryList, Category = "Inventory List")
	FInventoryList InventoryList;

	UFUNCTION(BlueprintCallable)
	TArray<UInventoryItemInstance*> GetInventoryItemInstances();




	/* Aluksi vain servulla, kun client koittaa ottaa, tekee sitten requestin ett‰ siaisko t‰n itemin, servu checkaa saako, antaa ja poistaa t‰‰lt‰*/
	/* Tai oikeastaan n‰iden perusteella tehd‰‰n instanssit ?*/
	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<UItemStaticData>, int32> ItemStaticDataToContain;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ItemInstancesContainedUpdated)
	TArray<UInventoryItemInstance*> ItemInstancesContained;


	UFUNCTION()
	void OnRep_InventoryList();

	UFUNCTION()
	void OnRep_ItemInstancesContainedUpdated();

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryMenu(bool NewOpen);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* INTERFACE OVERRIDES */
	UFUNCTION()
	virtual void ReactToInteractPure(APlayerControllerBase* OtherPlayerController) override;

	UFUNCTION()
	void ReactToInteract_Implementation() override; //This is a blueprint implemented functrion. DOnt impolement in C++

	 void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const ;

	 void TakeAllItemsInternal(APlayerControllerBase* OtherController);

	 UFUNCTION(Server, Reliable)
	 void HandleTakingAllItems();



	 UFUNCTION(BlueprintCallable)
	 void HandleTakeAllItems(APlayerControllerBase* OtherController);

//	 void TakeAllItems(APlayerControllerBase* OtherController);


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryContainerWidgetClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebugPrintContents = false;


	TArray<APlayerControllerBase*> OpenedCharacters;

};
