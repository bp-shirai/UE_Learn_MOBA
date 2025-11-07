// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/AnimNotify/CAnimNotify_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"

void UCAnimNotify_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (TargetSocketNames.Num() == 0) return;

    AActor* Actor = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (Actor && Actor->Implements<UAbilitySystemInterface>())
    {
        FGameplayEventData Data;
        for (int i = 1; i < TargetSocketNames.Num(); i++)
        {
            FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();

            FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
            FVector EndLoc   = MeshComp->GetSocketLocation(TargetSocketNames[i]);

            LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLoc);
            LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLoc);

            Data.TargetData.Add(LocationInfo);
        }

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventTag, Data);
    }
}

FString UCAnimNotify_SendTargetGroup::GetNotifyName_Implementation() const
{
    if (EventTag.IsValid())
    {
        return EventTag.GetTagLeafName().ToString();
    }

    return TEXT("None");
}

void UCAnimNotify_SendTargetGroup::DrawDebug()
{
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
}
