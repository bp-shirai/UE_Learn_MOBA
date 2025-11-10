// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CMinion.h"
#include "AbilitySystemComponent.h"

#include "GAS/CGameplayTags.h"
#include "GameplayTagContainer.h"

void ACMinion::PickSkinBasedOnTeamID()
{
    USkeletalMesh* Skin = SkinMap.FindRef(GetGenericTeamId());
    if (Skin)
    {
        GetMesh()->SetSkeletalMesh(Skin);
    }
}

void ACMinion::OnRep_TeamID()
{
    PickSkinBasedOnTeamID();
}

void ACMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    Super::SetGenericTeamId(NewTeamID);
    PickSkinBasedOnTeamID();
}

bool ACMinion::IsActive() const
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    return ASC ? !ASC->HasMatchingGameplayTag(Tags::Stats::Dead) : false;
}

void ACMinion::Activate()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(Tags::Stats::Dead));
    }
}
