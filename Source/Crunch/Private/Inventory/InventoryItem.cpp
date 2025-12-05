// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryItem.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"

#include "Inventory/PA_ShopItem.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"

UInventoryItem::UInventoryItem()
    : StackCount(1)
{
}

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

bool UInventoryItem::IsValid() const
{
    return ShopItem != nullptr;
}

void UInventoryItem::SetSlot(int NewSlot)
{
    Slot = NewSlot;
}

bool UInventoryItem::IsStackFull() const
{
    return StackCount >= ShopItem->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPA_ShopItem* Item) const
{
    if (!Item) return false;

    return ShopItem == Item;
}

bool UInventoryItem::AddStackCount()
{
    if (IsStackFull())
    {
        return false;
    }
    else
    {
        StackCount++;
        return true;
    }
}

bool UInventoryItem::ReduceStackCount()
{
    --StackCount;
    if (StackCount <= 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool UInventoryItem::SetStackCount(int NewStackCount)
{
    if (NewStackCount > 0 && NewStackCount <= ShopItem->GetMaxStackCount())
    {
        StackCount = NewStackCount;
        return true;
    }
    else
    {
        return false;
    }
}

bool UInventoryItem::TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (!GrantedAbilitySpecHandle.IsValid()) return false;

    if (AbilitySystemComponent && AbilitySystemComponent->TryActivateAbility(GrantedAbilitySpecHandle))
    {
        return true;
    }

    return false;
}

void UInventoryItem::ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (!IsValid()) return;

    TSubclassOf<UGameplayEffect> ComsumeEffect = ShopItem->GetConsumeEffect();
    if (ComsumeEffect)
    {
        AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(ComsumeEffect, 1, AbilitySystemComponent->MakeEffectContext());
    }
}

void UInventoryItem::RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
    if (!AbilitySystemComponent) return;

    if (AppliedEquippedEffectHandle.IsValid())
    {
        AbilitySystemComponent->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
    }

    if (GrantedAbilitySpecHandle.IsValid())
    {
        AbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
    }
}

bool UInventoryItem::IsGrantedAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
    if (!IsValid()) return false;

    TSubclassOf<UGameplayAbility> GrantedAbility = ShopItem->GetGrantedAbility();
    return GrantedAbility == AbilityClass;
}

bool UInventoryItem::IsGrantingAnyAbility() const
{
    if (!IsValid()) return false;

    return ShopItem->GetGrantedAbility() != nullptr;
}
