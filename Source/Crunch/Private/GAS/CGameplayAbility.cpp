// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CGameplayAbility.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/HitResult.h"
#include "Kismet/KismetSystemLibrary.h"

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

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, bool bIgnoreSelf, bool bDrawDebug)
{
    TArray<FHitResult> OutResults;
    TSet<AActor*> HitActors;

    for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
    {
        FVector StartLoc = TargetData->GetOrigin().GetTranslation();
        FVector EndLoc   = TargetData->GetEndPoint();
        
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

        TArray<AActor*> ActorsToIgnore;
        if (bIgnoreSelf)
        {
            ActorsToIgnore.Add(GetOwningActorFromActorInfo());
        }

        EDrawDebugTrace::Type DrawDebugTraceType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        TArray<FHitResult> Results;
        UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTraceType, Results, false);

        for(const FHitResult& Result : Results)
        {
            if (HitActors.Contains(Result.GetActor())) continue;
            
            HitActors.Add(Result.GetActor());
            OutResults.Add(Result);
        }
    }

    return OutResults;
}
