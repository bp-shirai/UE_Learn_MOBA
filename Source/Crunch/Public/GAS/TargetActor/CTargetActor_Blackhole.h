// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "CTargetActor_Blackhole.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ACTargetActor_Blackhole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
    GENERATED_BODY()
public:
    ACTargetActor_Blackhole();
    void ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }
    virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    class USceneComponent* RootComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    class USphereComponent* DetectionSphere;

    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    class UParticleSystemComponent* BlackholeVFX;

    UPROPERTY(Replicated)
    FGenericTeamId TeamId;

    float PullSpeed;

    UPROPERTY(ReplicatedUsing = OnRep_BlackholeRange)
    float BlackholeRange;

    float BlackholeDuration;
	
	UFUNCTION()
    void OnRep_BlackholeRange();


    UFUNCTION()
    void ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
