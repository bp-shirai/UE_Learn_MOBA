// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "CProjectileActor.generated.h"

UCLASS()
class ACProjectileActor : public AActor, public IGenericTeamAgentInterface
{
    GENERATED_BODY()
public:
    // Sets default values for this actor's properties
    ACProjectileActor();
    void ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);

    /** Retrieve team identifier in form of FGenericTeamId */
    virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue")
    FGameplayTag HitGameplayCueTag;

    UPROPERTY(Replicated)
    FGenericTeamId TeamId;

    UPROPERTY(Replicated)
    FVector MoveDir;

    UPROPERTY(Replicated)
    float ProjectileSpeed;

    UPROPERTY()
    const AActor* Target;

    FGameplayEffectSpecHandle HitEffectSpecHandle;
    FTimerHandle ShootTimerHandle;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    void TravelMaxDIstanceReached();
    void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult);
};