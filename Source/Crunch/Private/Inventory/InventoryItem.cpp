// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"

#include "Inventory/PA_ShopItem.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Templates/SubclassOf.h"

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
    Handle   = NewHandle;
    ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (!GetShopItem() || !AbilitySystemComponent) return;
    if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority()) return;

    TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
    if (EquipEffect)
    {
        AppliedEquippedEffectHandle = AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EquipEffect, 1, AbilitySystemComponent->MakeEffectContext());
    }

    TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
    if (GrantedAbility)
    {
        const FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
        if (FoundSpec)
        {
            GrantedAbilitySpecHandle = FoundSpec->Handle;
        }
        else
        {
            GrantedAbilitySpecHandle = AbilitySystemComponent->GiveAbility(GrantedAbility);
        }
    }

}
