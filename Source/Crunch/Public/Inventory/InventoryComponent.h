// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;
class UInventoryItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /*NewItem*/);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    void TryPurchase(const UPA_ShopItem* ItemToPurchase);

    float GetGold() const;
    FORCEINLINE int GetCapacity() const { return Capacity; }

    void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);

    UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;
    bool IsAllSlotOccupied() const;
    UInventoryItem* GetAvailableStackForItem(const UPA_ShopItem* Item) const;
    bool IsFullFor(const UPA_ShopItem* Item) const;

    FOnItemAddedDelegate OnItemAdded;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

    UPROPERTY()
    TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int Capacity{6};

#pragma region--------------- Server ---------------------------------------------

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

    void GrantItem(const UPA_ShopItem* NewItem);
#pragma endregion

#pragma region--------------- Client ---------------------------------------------
private:
    UFUNCTION(Client, Reliable)
    void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem);

#pragma endregion
};
