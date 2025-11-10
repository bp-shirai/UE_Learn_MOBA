// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"

#include "CCharacter.generated.h"

class UAbilitySystemComponent;
class UCAbilitySystemComponent;
class UAttributeSet;
class UCAttributeSet;
class UWidgetComponent;
class UAIPerceptionStimuliSourceComponent;
/**
 *
 */

UCLASS(Abstract)
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    ACCharacter();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void ServerSideInit();
    void ClientSideInit();
    bool IsLocallyControlledByPlayer() const;

    // Only called on the server
    virtual void PossessedBy(AController* NewController) override;

    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region------ Ability System ---------------------------------------------
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
    void BindGASChangeDelegates();
    void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);

    UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Abilities")
    UCAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UCAttributeSet* AttributeSet;

#pragma endregion
#pragma region----- UI ---------------------------------------------------------

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "UI")
    UWidgetComponent* OverHeadWidgetComponent;

    void ConfigureOverHeadWidget();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    float StatsGaugeVisibilityCheckUpdateGap{1.f};

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    float StatsGaugeVisibilityRangeSquared{10000000.f};

    FTimerHandle StatsGaugeVisibilityUpdateTimerHandle;
    void UpdateStatsGaugeVisibility();
    void SetStatsGaugeEnable(bool bIsEnable);

#pragma endregion
#pragma region----- Death and Respawn ------------------------------------------
public:
    bool IsDead() const;
    void Respawn_Immediately();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    UAnimMontage* DeathMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    float DeathMontageFinishedTimeShift{-0.8f};

    FTimerHandle DeathMontageTimerHandle;

    void DeathMontageFinished();
    void SetRagdollEnable(bool bIsEnable);

    FTransform MeshRelativeTransform;

    void StartDeathSequence();
    void Respawn();

    void PlayDeathAnimation();

    virtual void OnDead();
    virtual void OnRespawn();
#pragma endregion

#pragma region----- Team  ------------------------------------------
public:
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    virtual FGenericTeamId GetGenericTeamId() const override;

private:
    UPROPERTY(ReplicatedUsing = OnRep_TeamID)
    FGenericTeamId TeamID;

    UFUNCTION()
    virtual void OnRep_TeamID();

#pragma endregion

#pragma region----- AI  ------------------------------------------
public:
    UPROPERTY(VisibleAnywhere, Category = "AI Perception")
    UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource;

    void AIPerceptionStimuliSourceEnable(bool bIsEnable);

#pragma endregion
};
