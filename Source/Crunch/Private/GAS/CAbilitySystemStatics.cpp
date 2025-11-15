// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Misc/AssertionMacros.h"


int32 UCAbilitySystemStatics::GetGameplayTagID(const FGameplayTag& Tag)
{
    if (!Tag.IsValid()) return -1;
    return Tag.GetTagName().GetDisplayIndex().ToUnstableInt();
}


void UCAbilitySystemStatics::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
	ensure(ASC);

	if (ASC && ASC->HasMatchingGameplayTag(TagToAdd) == false)
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UCAbilitySystemStatics::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
	ensure(ASC);

	if (ASC && ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UCAbilitySystemStatics::DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck)
{
	const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
	ensureMsgf(ASC, TEXT("Actor %s does not have an AbilitySystemComponent"), *GetNameSafe(InActor));

	return ASC ? ASC->HasMatchingGameplayTag(TagToCheck) : false;
}

void UCAbilitySystemStatics::BP_DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck, ECConfirmType& OutConfirmType)
{
	if (!ensure(InActor))
	{
		OutConfirmType = ECConfirmType::No;
		return;
	}
	OutConfirmType = DoesActorHaveTag(InActor, TagToCheck) ? ECConfirmType::Yes : ECConfirmType::No;
}