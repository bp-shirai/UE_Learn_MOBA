// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "CAnimNotify_SendTargetGroup.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API UCAnimNotify_SendTargetGroup : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
    FGameplayTagContainer GameplayCueTags;

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
    TEnumAsByte<ETeamAttitude::Type> TargetTeam{ETeamAttitude::Hostile};

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities", meta=(Categories = "GameplayCue"))
    FGameplayTag EventTag;

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
    TArray<FName> TargetSocketNames;

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
    float ShapeSweepRadius{60.f};

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
    bool bIgnoreOwner{true};

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities|Debug")
    bool bDrawDebug{true};

    UPROPERTY(EditAnywhere, Category = "Gameplay Abilities|Debug", meta = (EditCondition = "bDrawDebug"))
    float DebugDrawTime{1.f};

    virtual FString GetNotifyName_Implementation() const override;

    void DrawDebug(USkeletalMeshComponent* MeshComp);

    void SendLocalGameplayCue(const FHitResult& HitResult) const;
};
