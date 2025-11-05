// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/AnimNotify/CAnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

void UCAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
    
    if (ASC)
    {
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
    }
}

FString UCAnimNotify_SendGameplayEvent::GetNotifyName_Implementation() const
{
    if (EventTag.IsValid())
    {
        // TArray<FName> TagNames;
        // UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
        // return TagNames.Last().ToString();
        return EventTag.GetTagLeafName().ToString();
    }

    return TEXT("None");
}
