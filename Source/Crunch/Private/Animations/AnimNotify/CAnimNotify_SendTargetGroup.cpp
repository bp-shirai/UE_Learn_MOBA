// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/AnimNotify/CAnimNotify_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "Containers/EnumAsByte.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayCueManager.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetTraceUtils.h"

void UCAnimNotify_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (TargetSocketNames.Num() <= 1) return;

    AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (OwnerActor && OwnerActor->Implements<UAbilitySystemInterface>())
    {

        FGameplayEventData Data;
        TSet<AActor*> HitActors;
        const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(OwnerActor);

        for (int i = 1; i < TargetSocketNames.Num(); i++)
        {
            const FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
            const FVector EndLoc   = MeshComp->GetSocketLocation(TargetSocketNames[i]);

            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
            ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
            TArray<AActor*> ActorsToIgnore;
            if (bIgnoreOwner)
            {
                ActorsToIgnore.Add(OwnerActor);
            }
            EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
            TArray<FHitResult> HitResults;

            UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLoc, EndLoc, ShapeSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, HitResults, false);

            for (const FHitResult& HitResult : HitResults)
            {
                AActor* HitActor = HitResult.GetActor();

                if (HitActors.Contains(HitActor))
                {
                    continue;
                }

                if (OwnerTeam)
                {
                    if (OwnerTeam->GetTeamAttitudeTowards(*HitActor) != TargetTeam)
                    {
                        continue;
                    }
                }

                FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
                Data.TargetData.Add(TargetHit);
                HitActors.Add(HitActor);

                SendLocalGameplayCue(HitResult);
            }
        }

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Data);
    }

#if WITH_EDITORONLY_DATA

    if (bDrawDebug)
    {
        DrawDebug(MeshComp);
    }

#endif
}

FString UCAnimNotify_SendTargetGroup::GetNotifyName_Implementation() const
{
    if (EventTag.IsValid())
    {
        return EventTag.GetTagLeafName().ToString();
    }

    return TEXT("None");
}

void UCAnimNotify_SendTargetGroup::DrawDebug(USkeletalMeshComponent* MeshComp)
{
#if WITH_EDITORONLY_DATA
    UWorld* World                       = MeshComp->GetWorld();
    EDrawDebugTrace::Type DrawDebugType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
    TArray<FHitResult> OutHits;
    bool bHit                  = false;
    FLinearColor TraceColor    = FLinearColor::Red;
    FLinearColor TraceHitColor = FLinearColor::Green;

    if (World && World->WorldType == EWorldType::EditorPreview)
    {
        if (MeshComp->IsPlaying())
        {
            for (int i = 1; i < TargetSocketNames.Num(); i++)
            {
                FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
                FVector EndLoc   = MeshComp->GetSocketLocation(TargetSocketNames[i]);

                DrawDebugSphereTraceMulti(World, StartLoc, EndLoc, ShapeSweepRadius, DrawDebugType, bHit, OutHits, TraceColor, TraceHitColor, DebugDrawTime);
            }
        }
    }
#endif
}

// #if WITH_EDITORONLY_DATA
// 		UWorld* World = MeshComp->GetWorld();
// 		if (bPreviewIgnoreAttenuation && World && World->WorldType == EWorldType::EditorPreview)
// 		{
// 			if (MeshComp->IsPlaying())
// 			{
// 				UGameplayStatics::PlaySound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
// 			}
// 		}
// 		else
// #endif

void UCAnimNotify_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
    FGameplayCueParameters CueParams;
    CueParams.Location = HitResult.ImpactPoint;
    CueParams.Normal   = HitResult.ImpactNormal;

    for (const FGameplayTag& GameplayCueTag : GameplayCueTags)
    {
        UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(HitResult.GetActor(), GameplayCueTag, EGameplayCueEvent::Executed, CueParams);
    }
}
