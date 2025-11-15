// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "CTargetActor_SphereSweepMulti.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API ACTargetActor_SphereSweepMulti : public AGameplayAbilityTargetActor
{
    GENERATED_BODY()

public:
    ACTargetActor_SphereSweepMulti();

    virtual void StartTargeting(UGameplayAbility* Ability) override;

    virtual void ConfirmTargetingAndContinue() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float TraceRange{1000.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Pawn};

    FVector StartLocation;
};
