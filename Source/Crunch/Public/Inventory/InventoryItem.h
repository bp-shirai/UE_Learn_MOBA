// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryItem.generated.h"

class UPA_ShopItem;
class UInventoryComponent;
class UAbilitySystemComponent;

/**
 *
 */
UCLASS()
class CRUNCH_API UInventoryItem : public UObject
{
    GENERATED_BODY()
public:
    void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
    const UPA_ShopItem* GetShopItem() const { return ShopItem; }
    FInventoryItemHandle GetHandle() const { return Handle; }

    void ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent);

private:
    UPROPERTY()
    const UPA_ShopItem* ShopItem;

    FInventoryItemHandle Handle;

    FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
    FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
