// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Dead.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Logging/LogVerbosity.h"

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
        if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
        {
            Killer = nullptr;
        }

        UE_LOG(LogTemp, Warning, TEXT("Killer = %s"), *GetNameSafe(Killer));

        TArray<AActor*> RewardTargets = GetRewardTargets();
        if (RewardTargets.Num() == 0 && !Killer)
        {
            UE_LOG(LogTemp, Warning, TEXT("No Reward Targets Found"));
            K2_EndAbility();
            return;
        }

        if (Killer && !RewardTargets.Contains(Killer))
        {
            RewardTargets.Add(Killer);
        }

        float SelfExperience        = GetAbilitySystemComponentFromActorInfo_Ensured()->GetNumericAttribute(UCHeroAttributeSet::GetExperienceAttribute());
        float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
        float TotalGoldReward       = BaseGoldReward + GoldRewardPerExperience * SelfExperience;

        if (Killer)
        {
            float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
            float KillerGoldReward       = TotalGoldReward * KillerRewardPortion;

            FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
            if (EffectSpec.IsValid())
            {
                EffectSpec.Data->SetSetByCallerMagnitude(Tags::Attribute::Experience, KillerExperienceReward);
                EffectSpec.Data->SetSetByCallerMagnitude(Tags::Attribute::Gold, KillerGoldReward);

                UE_LOG(LogTemp, Warning, TEXT("Killer Experience Reward = %f"), KillerExperienceReward);
                UE_LOG(LogTemp, Warning, TEXT("Killer Gold Reward = %f"), KillerGoldReward);

                K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
            }

            TotalExperienceReward -= KillerExperienceReward;
            TotalGoldReward -= KillerGoldReward;
        }

        float ExperiencePerTarget            = TotalExperienceReward / RewardTargets.Num();
        float GoldPerTarget                  = TotalGoldReward / RewardTargets.Num();
        FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
        if (EffectSpec.IsValid())
        {
            EffectSpec.Data->SetSetByCallerMagnitude(Tags::Attribute::Experience, ExperiencePerTarget);
            EffectSpec.Data->SetSetByCallerMagnitude(Tags::Attribute::Gold, GoldPerTarget);

            K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
        }

        if (RewardTargets.Num() > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Targets Experience Reward = %f"), ExperiencePerTarget);
            UE_LOG(LogTemp, Warning, TEXT("Targets Gold Reward = %f"), GoldPerTarget);
        }

        K2_EndAbility();
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
            AActor* OtherActor = OverlapResult.GetActor();

            // UE_LOG(LogTemp, Warning, TEXT("OtherActor = %s"), *GetNameSafe(OtherActor));

            const IGenericTeamAgentInterface* OtherTeam = Cast<IGenericTeamAgentInterface>(OtherActor);
            if (OtherTeam)
            {
                // UE_LOG(LogTemp, Warning, TEXT("OtherTeam = %d"), OtherTeam->GetGenericTeamId().GetId());
            }

            if (!OtherTeam || OtherTeam->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
            {
                // auto attr = OtherTeam->GetTeamAttitudeTowards(*AvatarActor);
                // UE_LOG(LogTemp, Warning, TEXT("OtherTeam = %d"), attr);
                continue;
            }

            if (!UCAbilitySystemStatics::IsHero(OtherActor))
            {
                // UE_LOG(LogTemp, Warning, TEXT("!IsHero"));
                continue;
            }

            OutActors.Add(OtherActor);
        }
    }

    if (ShouldDrawDebug())
    {
    }

    return OutActors.Array();
}
