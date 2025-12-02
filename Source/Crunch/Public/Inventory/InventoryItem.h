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
    UInventoryItem();

    bool IsValid() const;
    void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
    FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
    FORCEINLINE FInventoryItemHandle GetHandle() const { return Handle; }

    bool TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent);
    void ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent);
    void ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent);
    void RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent);

    FORCEINLINE int GetStackCount() const { return StackCount; }

    void SetSlot(int NewSlot);

    bool IsStackFull() const;
    bool IsForItem(const UPA_ShopItem* Item) const;

    // Return true is was able to add.
    bool AddStackCount();

    // Returns true if the stack is not empty after reducing.
    bool ReduceStackCount();

    // Return true if was able to set.
    bool SetStackCount(int NewStackCount);

private:
    UPROPERTY()
    const UPA_ShopItem* ShopItem;

    FInventoryItemHandle Handle;
    int StackCount;
    int Slot;

    FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
    FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
