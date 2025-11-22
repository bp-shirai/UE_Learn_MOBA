// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "CTargetActor_GroundPick.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API ACTargetActor_GroundPick : public AGameplayAbilityTargetActor
{
    GENERATED_BODY()

public:
    ACTargetActor_GroundPick();

    virtual void ConfirmTargetingAndContinue() override;

    void SetTargetAreaRadius(float NewRadius);

    void SetTargetOptions(bool bTargetEnemy, bool bTargetFriendly);

protected:
    bool bShouldTargetEnemy{true};
    bool bShouldTargetFriendly{false};

    virtual void Tick(float DeltaTime) override;

    FVector GetTargetPoint() const;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetAreaRadius{300.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetTraceRange{2000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Visibility};
};
