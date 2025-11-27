// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CAbilitySystemGenerics.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 *
 */
UCLASS()
class CRUNCH_API UCAbilitySystemGenerics : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const { return FullStatEffect; }
    FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }
    FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const { return InitialEffects; }
    FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const { return PassiveAbilities; }
    FORCEINLINE const UDataTable* GetBaseStatsDataTable() const { return BaseStatsDataTable; }
	FRealCurve* GetExperienceCurve() const;


private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> FullStatEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> DeathEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
    UDataTable* BaseStatsDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName ExperienceRowName{TEXT("ExperienceNeededToReachLevel")};

    UPROPERTY(EditDefaultsOnly, Category = "Level")
	UCurveTable* ExperienceCurveTable;

};
