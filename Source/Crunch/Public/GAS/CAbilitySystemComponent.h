// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
	UCAbilitySystemComponent();

    void ApplyInitialEffects();



private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
};
