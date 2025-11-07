// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

#include "CCharacter.generated.h"

class UAbilitySystemComponent;
class UCAbilitySystemComponent;
class UAttributeSet;
class UCAttributeSet;
class UWidgetComponent;

/**
 *
 */

UCLASS(Abstract)
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ACCharacter();
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
private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    UAnimMontage* DeathMontage;

    void StartDeathSequence();
    void Respawn();

    void PlayDeathAnimation();

    virtual void OnDead();
    virtual void OnRespawn();
#pragma endregion
};
