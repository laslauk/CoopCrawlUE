// Fill out your copyright notice in the Description page of Project Settings.


#include "CrawlGameStatics.h"
#include "CrawlCommonTypes.h"
#include "Kismet/KismetSystemLibrary.h" // < For Traces, SphereOverlapActors()
#include "AbilitySystemBlueprintLibrary.h" // < AbilitySystem utilities, SendGameplayEvent
#include "AbilitySystemComponent.h"
#include "Actors/ProjectileBase.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
    TEXT("ShowDebugRadialDamage"),
    0,
    TEXT("draws debug info aboutr radial damage")
    TEXT(" 0: off/n")
    TEXT("0 on/n"),
    ECVF_Cheat
);



 const UItemStaticData* UCrawlGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass) {
    if (!IsValid(ItemDataClass))
    {
        return nullptr;
    }
    return ItemDataClass->GetDefaultObject<UItemStaticData>();
}


 void UCrawlGameStatics::ApplyDirectDamage(UObject* WorldContextObject, AActor* DamageCauser, AActor* DamagedTarget, float DamageAmount,
     TArray<TSubclassOf<UGameplayEffect>> DamageEffects)
 {
     UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamagedTarget);

     /* Halutaan applyy damage effecti kohteeseen */
     if (ASC) 
     {
         FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
         EffectContext.AddInstigator(DamageCauser, DamageCauser);

         for (auto Effect : DamageEffects) 
         {
             FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);

             if (SpecHandle.IsValid()) 
             {
                 UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -DamageAmount);
                 FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

             }
         }
     }

 }


 void UCrawlGameStatics::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius, float DamageAmount, 
     TArray<TSubclassOf<UGameplayEffect>> DamageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType, TArray<AActor*> ActorsToIgnore)
 {

     //Radial damage: 1. Sphere trace all objects, 2. sitten line trace että checkataan onko ne esim seinän takana yms.
     TArray<AActor*> OutActors;
    

     /* Haetaan kaikki jotka on relevant tälle radial damagelle*/
     UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

     const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());

     for(AActor * Actor : OutActors) {

         FHitResult HitResult;

         //Checkataan onko kohteeseen näkyvyyys
         if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true)) {
             AActor* Target = HitResult.GetActor();

             if (Target == Actor) {

                 bool bWasApplied = false;

                 if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target)) {

                     // The GameplayEffectContextHandle tells us who created this GameplayEffectSpec.
                     //  The GameplayEffectSpec (GESpec) can be thought of as the instantiations of GameplayEffects. 
                     //  Niis on reference gameplayefrecti class mitä ne represeenttaa, what lvl, who created.
                     FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
                     EffectContext.AddInstigator(DamageCauser, DamageCauser); //instigaattori on damage causer ja efektin myös


                     /* Lisätään effektit line trace kohteisiin */
                     for (auto Effect : DamageEffects) {

                         //Tarvitaan specHandle niin tehään listasta effekteitä
                         FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);


                         if (SpecHandle.IsValid()) {

                             //!!!! käyttämällä tagsetbycallermang: Pistetään applied effektiin DAMAGE value mikä tähän applyRadialDamageen on syötetty
                             UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -DamageAmount);


                             //Apply effect spec to the target
                             FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                             if (ActiveGEHandle.WasSuccessfullyApplied()) 
                             {

                                 bWasApplied = true;

                             }
                         }
                     }
                 }

                 if (bDebug) {
                     DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
                     DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16,16, bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
                     DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::White, 0, false, 1.0f);
                 }
             }
             else {
                 if (bDebug) {
                     DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
                     DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f, 0, 1);
                     DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::Red, 0, false, 1.0f);
                 }
             }
         }
         else {

             if (bDebug) {
                 DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
                 DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f, 0, 1);
                 DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()), nullptr, FColor::Red, 0, false, 1.0f);
             }

         }

     }
     if (bDebug) {
         DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16, FColor::White, false, 4.f, 0, 1);
     }
 }

 AProjectileBase* UCrawlGameStatics::LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass,
     FTransform Transform, AActor* Owner, APawn* Instigator)
 {
     /* Get the world */
     UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

     /* check if valid AND SERVER, spawn replicated actors ONLY on servers, then they will be automatiocally repliacted to all clients */
     if (World && World->IsServer()) {

         //Deferred, set all data first, then invoke construction manually!!!, FinishSpawningConstruction();
         if (AProjectileBase* Projectile = World->SpawnActorDeferred<AProjectileBase>(AProjectileBase::StaticClass(), Transform, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn)) {
             Projectile->ProjectileDataClass = ProjectileDataClass; // < Set the data of projectile
             Projectile->FinishSpawning(Transform); // < call constructor manually
             return Projectile;
         }
     }


     return nullptr;
 }

 AProjectileBase* UCrawlGameStatics::LaunchProjectileActor(UObject* WorldContextObject, TSubclassOf<AProjectileBase> ProjectileActorClass,
     FTransform Transform, AActor* Owner, APawn* Instigator)
 {
    


     /* Get the world */
     UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

     /* check if valid AND SERVER, spawn replicated actors ONLY on servers, then they will be automatiocally repliacted to all clients */
     if (World && World->IsServer()) {

         //Deferred, set all data first, then invoke construction manually!!!, FinishSpawningConstruction();

         AProjectileBase* Projectile = World->SpawnActorDeferred<AProjectileBase>(ProjectileActorClass, Transform, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

         if (Projectile)
         {
             UE_LOG(LogTemp, Warning, TEXT("Spawning projectile....."));
             Projectile->ProjectileDataClass = ProjectileActorClass->GetDefaultObject<AProjectileBase>()->ProjectileDataClass; // < Set the data of projectile
             Projectile->FinishSpawning(Transform); // < call constructor manually
             return Projectile;
         }
     }


     return nullptr;
 }

