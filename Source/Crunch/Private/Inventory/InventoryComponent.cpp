// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "Inventory/PA_ShopItem.h"
#include "Inventory/InventoryItem.h"
#include "GAS/CHeroAttributeSet.h"
#include "Widgets/ItemToolTip.h"

UInventoryComponent::UInventoryComponent()
{

    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
    if (!OwnerASC) return;

	Server_Purchase(ItemToPurchase);
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerASC) return;

	if (GetGold() < ItemToPurchase->GetPrice()) return;

	OwnerASC->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
}
bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

float UInventoryComponent::GetGold() const
{
    return OwnerASC ? OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetGoldAttribute()) : 0.f;
}
