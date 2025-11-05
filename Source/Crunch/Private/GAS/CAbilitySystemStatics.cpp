// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemStatics.h"

int32 UCAbilitySystemStatics::GetGameplayTagID(const FGameplayTag& Tag)
{
    if (!Tag.IsValid()) return -1;
    return Tag.GetTagName().GetDisplayIndex().ToUnstableInt();
}
