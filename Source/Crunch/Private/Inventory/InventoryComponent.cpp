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

    if (GetGold() < ItemToPurchase->GetPrice()) return;

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

    UInventoryItem* InventoryItem  = NewObject<UInventoryItem>();
    FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
    InventoryItem->InitItem(NewHandle, NewItem);
    InventoryMap.Add(NewHandle, InventoryItem);
    OnItemAdded.Broadcast(InventoryItem);

    UE_LOG(LogTemp, Warning, TEXT("[Server] UInventoryComponent::GrantItem : ItemName : %s HandleId = %d"), *NewItem->GetItemName().ToString(), NewHandle.GetHandleId());

    Client_ItemAdded(NewHandle, NewItem);

    InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent.Get());
}

#pragma endregion

#pragma region--------------- Client ---------------------------------------------

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem)
{
    if(GetOwner()->HasAuthority()) return;

    UInventoryItem* InventoryItem  = NewObject<UInventoryItem>();
    InventoryItem->InitItem(AssignedHandle, NewItem);
    InventoryMap.Add(AssignedHandle, InventoryItem);
    OnItemAdded.Broadcast(InventoryItem);

    UE_LOG(LogTemp, Warning, TEXT("[Client] UInventoryComponent::ItemAdded : ItemName : %s HandleId = %d"), *NewItem->GetItemName().ToString(), AssignedHandle.GetHandleId());

}

#pragma endregion

