// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryItem.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"

#include "Inventory/PA_ShopItem.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"

UInventoryItem::UInventoryItem()
    : StackCount(1)
{
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem, UAbilitySystemComponent* NewOwnerAbilitySystemComponent)
{
    Handle                      = NewHandle;
    ShopItem                    = NewShopItem;
    OwnerAbilitySystemComponent = NewOwnerAbilitySystemComponent;

    if (UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent())
    {
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);
    }

    ApplyGASModifications();
}

void UInventoryItem::ApplyGASModifications()
{
    UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent();

    if (!GetShopItem() || !OwnerASC) return;
    if (!OwnerASC->GetOwner() || !OwnerASC->GetOwner()->HasAuthority()) return;

    TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
    if (EquipEffect)
    {
        AppliedEquippedEffectHandle = OwnerASC->BP_ApplyGameplayEffectToSelf(EquipEffect, 1, OwnerASC->MakeEffectContext());
    }

    TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
    if (GrantedAbility)
    {
        GrantedAbilitySpecHandle = OwnerASC->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
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

bool UInventoryItem::TryActivateGrantedAbility()
{
    UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent();
    if (!GrantedAbilitySpecHandle.IsValid()) return false;

    if (OwnerASC && OwnerASC->TryActivateAbility(GrantedAbilitySpecHandle))
    {
        return true;
    }

    return false;
}

void UInventoryItem::ApplyConsumeEffect()
{
    UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent();
    if (!IsValid() || !OwnerASC) return;

    TSubclassOf<UGameplayEffect> ComsumeEffect = ShopItem->GetConsumeEffect();
    if (ComsumeEffect)
    {
        OwnerASC->BP_ApplyGameplayEffectToSelf(ComsumeEffect, 1, OwnerASC->MakeEffectContext());
    }
}

void UInventoryItem::RemoveGASModifications()
{
    UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent();
    if (!OwnerASC) return;

    OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).RemoveAll(this);

    if (OwnerASC->GetOwner()->HasAuthority())
    {
        if (AppliedEquippedEffectHandle.IsValid())
        {
            OwnerASC->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
        }

        if (GrantedAbilitySpecHandle.IsValid())
        {
            OwnerASC->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
        }
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

float UInventoryItem::GetAbilityCooldownTimeRemaining() const
{
    if (!IsGrantingAnyAbility()) return 0.f;

    return UCAbilitySystemStatics::GetCooldownRemainingFor(ShopItem->GetGrantedAbilityCDO(), GetOwnerAbilitySystemComponent());
}

float UInventoryItem::GetAbilityCooldownDuration() const
{
    if (!IsGrantingAnyAbility()) return 0.f;

    return UCAbilitySystemStatics::GetCooldownDurationFor(ShopItem->GetGrantedAbilityCDO(), GetOwnerAbilitySystemComponent(), 1);
}

float UInventoryItem::GetAbilityManaCost() const
{
    if (!IsGrantingAnyAbility()) return 0.f;

    return UCAbilitySystemStatics::GetManaCostFor(ShopItem->GetGrantedAbilityCDO(), GetOwnerAbilitySystemComponent(), 1);
}

bool UInventoryItem::CanCastAbility() const
{
    UAbilitySystemComponent* OwnerASC = GetOwnerAbilitySystemComponent();

    if (!IsGrantingAnyAbility() || !OwnerASC) return false;

    FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromHandle(GrantedAbilitySpecHandle);
    if (Spec)
    {
        return UCAbilitySystemStatics::CheckAbilityCost(*Spec, OwnerASC);
    }

    return UCAbilitySystemStatics::CheckAbilityCostStatic(ShopItem->GetGrantedAbilityCDO(), OwnerASC);
}

void UInventoryItem::ManaUpdated(const FOnAttributeChangeData& Data)
{
    OnAbilityCanCastUpdated.Broadcast(CanCastAbility());
}
