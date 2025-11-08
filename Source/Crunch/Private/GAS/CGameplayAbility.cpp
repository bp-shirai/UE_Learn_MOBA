// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CGameplayAbility.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
    USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
    if (OwnerSkeletalMeshComp)
    {
        return OwnerSkeletalMeshComp->GetAnimInstance();
    }

    // GetCurrentActorInfo()->GetAnimInstance();

    return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam, bool bIgnoreSelf, bool bDrawDebug)
{
    TArray<FHitResult> OutResults;
    TSet<AActor*> HitActors; // Prevent adding the same Actor

    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<const IGenericTeamAgentInterface>(AvatarActor);

    for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
    {
        const FVector StartLoc = TargetData->GetOrigin().GetTranslation();
        const FVector EndLoc   = TargetData->GetEndPoint();

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

        TArray<AActor*> ActorsToIgnore;
        if (bIgnoreSelf)
        {
            ActorsToIgnore.Add(AvatarActor);
        }

        EDrawDebugTrace::Type DrawDebugTraceType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        TArray<FHitResult> Results;
        UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTraceType, Results, false);

        for (const FHitResult& Result : Results)
        {
            if (HitActors.Contains(Result.GetActor())) continue;

            // Confirm hostile relationship
            if (OwnerTeamInterface)
            {
                ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor());
                if (OtherActorTeamAttitude != TargetTeam)
                {
                    continue;
                }
            }


            HitActors.Add(Result.GetActor());
            OutResults.Add(Result);
        }
    }

    return OutResults;
}
