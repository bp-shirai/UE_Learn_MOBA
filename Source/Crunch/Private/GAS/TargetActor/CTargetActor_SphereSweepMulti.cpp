// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor/CTargetActor_SphereSweepMulti.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

#//include "GameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


ACTargetActor_SphereSweepMulti::ACTargetActor_SphereSweepMulti()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACTargetActor_SphereSweepMulti::StartTargeting(UGameplayAbility* Ability)
{
    Super::StartTargeting(Ability);

    StartLocation = OwningAbility->GetAvatarActorFromActorInfo()->GetActorLocation();
}

void ACTargetActor_SphereSweepMulti::ConfirmTargetingAndContinue()
{
    if (OwningAbility && OwningAbility->GetCurrentActorInfo()->IsNetAuthority())
    {
        FHitResult HitResult;
        

        

        // Create Target Data
        FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
        HitData->HitResult = HitResult;

        FGameplayAbilityTargetDataHandle Handle(HitData);

        // Broadcast Target Data
        TargetDataReadyDelegate.Broadcast(Handle);
    }
}