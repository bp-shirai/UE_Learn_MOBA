// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;
class UInventoryItem;
class UGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /*NewItem*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangedDelegate, const FInventoryItemHandle& /*ItemHandle*/, int /*NewCount*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /*ItemHandle*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAbilityCommitted, const FInventoryItemHandle& /*ItemHandle*/, float /*CooldownDuration*/, float /*CooldownTimeRemaining*/);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    void TryPurchase(const UPA_ShopItem* ItemToPurchase);
    void TryActivateItem(const FInventoryItemHandle& ItemHandle);
    void SellItem(const FInventoryItemHandle& ItemHandle);

    float GetGold() const;
    FORCEINLINE int GetCapacity() const { return Capacity; }

    void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);

    UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;
    bool IsAllSlotOccupied() const;
    UInventoryItem* GetAvailableStackForItem(const UPA_ShopItem* Item) const;
    bool IsFullFor(const UPA_ShopItem* Item) const;

    bool FoundIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UPA_ShopItem*>& IgnoreItems = {});
    UInventoryItem* TryGetItemForShopItem(const UPA_ShopItem* Item) const;

    FOnItemAddedDelegate OnItemAdded;
    FOnItemStackCountChangedDelegate OnItemStackCountChanged;
    FOnItemRemovedDelegate OnItemRemoved;
    FOnItemAbilityCommitted OnItemAbilityCommitted;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

    UPROPERTY()
    TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int Capacity{6};

    void AbilityCommitted(UGameplayAbility* CommittedAbility);

#pragma region--------------- Server ---------------------------------------------

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ActivateItem(FInventoryItemHandle ItemHandle);

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SellItem(FInventoryItemHandle ItemHandle);

    void GrantItem(const UPA_ShopItem* NewItem);
    void ComsumeItem(UInventoryItem* Item);
    void RemoveItem(UInventoryItem* Item);

    bool TryItemCombination(const UPA_ShopItem* NewItem);
#pragma endregion

#pragma region--------------- Client ---------------------------------------------
private:
    UFUNCTION(Client, Reliable)
    void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem);

    UFUNCTION(Client, Reliable)
    void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);

    UFUNCTION(Client, Reliable)
    void Client_ItemRemoved(FInventoryItemHandle Handle);

#pragma endregion
};
