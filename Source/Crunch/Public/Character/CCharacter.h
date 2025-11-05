// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

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

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Ability System ---------------------------------------------

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Crunch|Gameplay Ability")
    UCAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UCAttributeSet* AttributeSet;

#pragma endregion
#pragma region UI ---------------------------------------------------------

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "Crunch|UI")
    UWidgetComponent* OverHeadWidgetComponent;

    void ConfigureOverHeadStatsWidget();

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|UI")
    float OverHeadVisibilityCheckUpdateGap{1.f};

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|UI")
    float OverHeadVisibilityRangeSquared{10000000.f};

    FTimerHandle OverHeadVisibilityUpdateTimerHandle;
    void UpdateOverHeadVisibility();

#pragma endregion
};
