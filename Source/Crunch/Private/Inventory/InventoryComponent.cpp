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

    OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

float UInventoryComponent::GetGold() const
{
    const UAbilitySystemComponent* OwnerASC = OwnerAbilitySystemComponent.Get();
    return OwnerASC ? OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetGoldAttribute()) : 0.f;
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
    Server_Purchase(ItemToPurchase);
}

#pragma region--------------- Server ---------------------------------------------

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
    UAbilitySystemComponent* OwnerASC = OwnerAbilitySystemComponent.Get();
    if (!OwnerASC) return;

    if (GetGold() < ItemToPurchase->GetPrice())
    {
        return; // Don't have enough Gold.
    }

    if (IsFullFor(ItemToPurchase))
    {
        return; // There is not enough space.
    }

    OwnerASC->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());

    GrantItem(ItemToPurchase);
}
bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
    return true;
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
    if (!GetOwner()->HasAuthority()) return;

    if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
    {
        StackItem->AddStackCount();
        OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());

        Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
    }
    else
    {
        UInventoryItem* InventoryItem  = NewObject<UInventoryItem>();
        FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
        InventoryItem->InitItem(NewHandle, NewItem);
        InventoryMap.Add(NewHandle, InventoryItem);

        OnItemAdded.Broadcast(InventoryItem);
    	UE_LOG(LogTemp, Warning, TEXT("[Server] Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());

        Client_ItemAdded(NewHandle, NewItem);

        InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent.Get());
    }
}

#pragma endregion

#pragma region--------------- Client ---------------------------------------------

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem)
{
    if (GetOwner()->HasAuthority()) return;

    UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
    InventoryItem->InitItem(AssignedHandle, NewItem);
    InventoryMap.Add(AssignedHandle, InventoryItem);

    OnItemAdded.Broadcast(InventoryItem);
    UE_LOG(LogTemp, Warning, TEXT("[Client] UInventoryComponent::ItemAdded : ItemName : %s HandleId = %d"), *NewItem->GetItemName().ToString(), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
    if (GetOwner()->HasAuthority()) return;

    UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
    if (FoundItem)
    {
        FoundItem->SetStackCount(NewCount);
        OnItemStackCountChanged.Broadcast(Handle, NewCount);
    }
}



#pragma endregion


#pragma region -------------------- Checking Functions -------------------------------------------------
void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber)
{
    if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
    {
        FoundItem->SetSlot(NewSlotNumber);
    }
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
    UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
    if (FoundItem)
    {
        return *FoundItem;
    }

    return nullptr;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
    return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPA_ShopItem* Item) const
{
    if (!Item || !Item->IsStackable()) return nullptr;

    for (const auto& [Handle, InventoryItem] : InventoryMap)
    {
        if (InventoryItem && InventoryItem->IsForItem(Item) && !InventoryItem->IsStackFull())
        {
            return InventoryItem;
        }
    }

    return nullptr;
}

bool UInventoryComponent::IsFullFor(const UPA_ShopItem* Item) const
{
    if (!Item) return false;

    if (IsAllSlotOccupied())
    {
        return GetAvailableStackForItem(Item) == nullptr;
    }

    return false;
}

#pragma endregion
