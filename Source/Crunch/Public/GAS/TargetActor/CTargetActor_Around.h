// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "CTargetActor_Around.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ACTargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ACTargetActor_Around();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
    FORCEINLINE virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }

    void ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag& InLocalGameplayCueTag);

private:
    UPROPERTY(Replicated)
    FGenericTeamId TeamId;

    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    class USceneComponent* RootComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    class USphereComponent* DetectionSphere;

    UPROPERTY(ReplicatedUsing = OnRep_TargetDetectionRadius)
    float TargetDetectionRadius;

    UFUNCTION()
    void OnRep_TargetDetectionRadius();

    UPROPERTY(Replicated)
    FGameplayTag LocalGameplayCueTag;

    UFUNCTION()
    void ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
