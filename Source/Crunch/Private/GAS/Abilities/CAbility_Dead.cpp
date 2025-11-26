// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Dead.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/OverlapResult.h"

#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"

UCAbility_Dead::UCAbility_Dead()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag    = Tags::Stats::Dead;

    AbilityTriggers.Add(TriggerData);

    ActivationBlockedTags.RemoveTag(Tags::Stats::Stun);
}

void UCAbility_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (K2_HasAuthority())
    {
        AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
        if (Killer)
        {
        }

        TArray<AActor*> RewardTargets = GetRewardTargets();
        for (const AActor* RewardTarget : RewardTargets)
        {
        }
    }
}

TArray<AActor*> UCAbility_Dead::GetRewardTargets() const
{
    TSet<AActor*> OutActors;

    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor || !GetWorld()) return OutActors.Array();

    TArray<FOverlapResult> OverlapResults;
    const FVector Pos = AvatarActor->GetActorLocation();
    FCollisionObjectQueryParams ObjectQueryParams(ECC_Pawn);
    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(RewardRange);

    if (GetWorld()->OverlapMultiByObjectType(OverlapResults, Pos, FQuat::Identity, ObjectQueryParams, CollisionShape))
    {
        for (const FOverlapResult& OverlapResult : OverlapResults)
        {
            AActor* OtherActor                          = OverlapResult.GetActor();
            const IGenericTeamAgentInterface* OtherTeam = Cast<IGenericTeamAgentInterface>(OtherActor);
            if (!OtherTeam || OtherTeam->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
            {
                continue;
            }

            if (!UCAbilitySystemStatics::IsHero(OtherActor))
            {
                continue;
            }

            OutActors.Add(OtherActor);
        }
    }

    return OutActors.Array();
}
