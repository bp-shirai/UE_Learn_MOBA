// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"

#include "Inventory/PA_ShopItem.h"
#include "Inventory/InventoryItem.h"
#include "GAS/CHeroAttributeSet.h"
#include "Widgets/ItemToolTip.h"
#include "Framework/CAssetManager.h"

UInventoryComponent::UInventoryComponent()
{

    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
    if (UAbilitySystemComponent* ASC = OwnerAbilitySystemComponent.Get())
    {
        ASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
    }
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

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
    UInventoryItem* FoundItem = GetInventoryItemByHandle(ItemHandle);
    if (!FoundItem) return;

    Server_ActivateItem(ItemHandle);
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

    //    if (IsFullFor(ItemToPurchase))
    //    {
    //        return; // There is not enough space.
    //    }

    if (!IsFullFor(ItemToPurchase))
    {
        OwnerASC->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
        GrantItem(ItemToPurchase);
        return;
    }

    if (TryItemCombination(ItemToPurchase))
    {
        OwnerASC->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
    }
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
        if (TryItemCombination(NewItem))
        {
            return;
        }

        UInventoryItem* InventoryItem  = NewObject<UInventoryItem>();
        FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
        InventoryItem->InitItem(NewHandle, NewItem, GetOwnerAbilitySystemComponent());
        InventoryMap.Add(NewHandle, InventoryItem);

        OnItemAdded.Broadcast(InventoryItem);
        UE_LOG(LogTemp, Warning, TEXT("[Server] Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());

        FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();

        Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle);

        // InventoryItem->ApplyGASModifications();
        // CheckItemCombination(InventoryItem);
    }
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
    UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
    if (!InventoryItem) return;

    InventoryItem->TryActivateGrantedAbility();

    const UPA_ShopItem* Item = InventoryItem->GetShopItem();
    if (Item && Item->IsConsumable())
    {
        ComsumeItem(InventoryItem);
    }
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
    return true;
}

void UInventoryComponent::ComsumeItem(UInventoryItem* Item)
{
    if (!GetOwner()->HasAuthority()) return;
    if (!Item) return;

    Item->ApplyConsumeEffect();
    if (!Item->ReduceStackCount())
    {
        RemoveItem(Item);
    }
    else
    {
        OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
        Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
    }
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
    if (!GetOwner()->HasAuthority()) return;
    if (!Item) return;

    Item->RemoveGASModifications();
    OnItemRemoved.Broadcast(Item->GetHandle());
    InventoryMap.Remove(Item->GetHandle());

    Client_ItemRemoved(Item->GetHandle());
}

#pragma endregion

#pragma region--------------- Client ---------------------------------------------

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem, FGameplayAbilitySpecHandle GrantedAbilitySpecHandle)
{
    if (GetOwner()->HasAuthority()) return;

    UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
    InventoryItem->InitItem(AssignedHandle, NewItem, GetOwnerAbilitySystemComponent());
    InventoryItem->SetGrantedAbilitySpecHandle(GrantedAbilitySpecHandle);
    InventoryMap.Add(AssignedHandle, InventoryItem);

    OnItemAdded.Broadcast(InventoryItem);
    UE_LOG(LogTemp, Warning, TEXT("[Client] UInventoryComponent::ItemAdded : ItemName : %s HandleId = %d"), *NewItem->GetItemName().ToString(), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
    if (GetOwner()->HasAuthority()) return;

    if (UInventoryItem* InventoryItem = GetInventoryItemByHandle(Handle))
    {
        InventoryItem->SetStackCount(NewCount);
    }

    OnItemStackCountChanged.Broadcast(Handle, NewCount);
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle Handle)
{
    if (GetOwner()->HasAuthority()) return;

    if (UInventoryItem* InventoryItem = GetInventoryItemByHandle(Handle))
    {
        InventoryItem->RemoveGASModifications();
    }

    OnItemRemoved.Broadcast(Handle);
    InventoryMap.Remove(Handle);
}

#pragma endregion

#pragma region-------------------- Checking Functions -------------------------------------------------
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

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
    Server_SellItem(ItemHandle);
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
    UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
    if (!InventoryItem || !InventoryItem->IsValid()) return;

    float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();
    if (UAbilitySystemComponent* ASC = OwnerAbilitySystemComponent.Get())
    {
        ASC->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * InventoryItem->GetStackCount());
    }

    RemoveItem(InventoryItem);
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
    return true;
}

bool UInventoryComponent::TryItemCombination(const UPA_ShopItem* NewItem)
{
    if (!GetOwner()->HasAuthority()) return false;
    if (!NewItem) return false;

    const FItemCollection* CombinationItems = UCAssetManager::Get().GetCombinationForItem(NewItem);
    if (!CombinationItems) return false;

    for (const UPA_ShopItem* CombinationItem : CombinationItems->GetItems())
    {
        TArray<UInventoryItem*> Ingredients;
        if (!FoundIngredientForItem(CombinationItem, Ingredients, /*IgnoreItems*/ {NewItem}))
        {
            continue;
        }

        for (UInventoryItem* Ingredient : Ingredients)
        {
            RemoveItem(Ingredient);
        }

        GrantItem(CombinationItem);
        return true;
    }

    return false;
}

bool UInventoryComponent::FoundIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UPA_ShopItem*>& IgnoreItems)
{
    const FItemCollection* IngredientItems = UCAssetManager::Get().GetIngredientsForItem(Item);
    if (!IngredientItems) return false;

    bool bAllFound = true;
    for (const UPA_ShopItem* Ingredient : IngredientItems->GetItems())
    {
        if (IgnoreItems.Contains(Ingredient)) continue;

        if (UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient))
        {
            OutIngredients.Add(FoundItem);
        }
        else
        {
            bAllFound = false;
            break;
        }
    }
    return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UPA_ShopItem* Item) const
{
    if (!Item) return nullptr;

    for (const auto& [Handle, InventoryItem] : InventoryMap)
    {
        if (InventoryItem && InventoryItem->IsForItem(Item))
        {
            return InventoryItem;
        }
    }

    return nullptr;
}

void UInventoryComponent::AbilityCommitted(UGameplayAbility* CommittedAbility)
{
    if (!CommittedAbility) return;

    float CooldownTimeRemaining = 0.f;
    float CooldownDuration      = 0.f;

    CommittedAbility->GetCooldownTimeRemainingAndDuration(CommittedAbility->GetCurrentAbilitySpecHandle(), CommittedAbility->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

    for (const auto& [Handle, InventoryItem] : InventoryMap)
    {
        if (InventoryItem && InventoryItem->IsGrantedAbility(CommittedAbility->GetClass()))
        {
            OnItemAbilityCommitted.Broadcast(Handle, CooldownDuration, CooldownTimeRemaining);
            break;
        }
    }
}

void UInventoryComponent::TryActivateItemInSlot(int SlotNumber)
{
    UE_LOG(LogTemp, Warning, TEXT("TryActivateItemInSlot: %d"), SlotNumber);

    for (const auto& [Handle, InventoryItem] : InventoryMap)
    {
        if (InventoryItem && InventoryItem->GetSlot() == SlotNumber)
        {
            TryActivateItem(Handle);
            break;
        }
    }
}
