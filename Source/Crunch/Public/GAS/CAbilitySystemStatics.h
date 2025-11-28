// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemStatics.generated.h"

class UGameplayAbility;
struct FScalableFloat;
struct FGameplayAbilitySpec;

/**
 *
 */
UCLASS()
class CRUNCH_API UCAbilitySystemStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "GAS|Utils")
    int32 GetGameplayTagID(const FGameplayTag& Tag);

    UFUNCTION(BlueprintCallable, Category = "GAS|Utils")
    static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

    UFUNCTION(BlueprintCallable, Category = "GAS|Utils")
    static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

    static bool DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck);

    UFUNCTION(BlueprintCallable, Category = "GAS|Utils", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
    static void BP_DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck, ECConfirmType& OutConfirmType);

    UFUNCTION(BlueprintPure, Category = "GAS|Utils")
    static bool IsTargetPawnHostile(const APawn* QueryPawn, const APawn* TargetPawn);

    UFUNCTION(BlueprintPure, Category = "GAS|Utils", meta = (CompactNodeTitle = "Get Value At Level"))
    static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);

    static float GetStaticColldownDurationForAbility(const UGameplayAbility* Ability);
    static float GetStaticCostForAbility(const UGameplayAbility* Ability);
    static bool IsHero(const AActor* ActorToCheck);
    static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec);
};
