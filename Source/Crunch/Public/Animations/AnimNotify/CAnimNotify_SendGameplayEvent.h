// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "CAnimNotify_SendGameplayEvent.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API UCAnimNotify_SendGameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
    FGameplayTag EventTag;

    virtual FString GetNotifyName_Implementation() const override;
};
