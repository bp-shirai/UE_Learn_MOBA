// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemStatics.generated.h"

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

};
