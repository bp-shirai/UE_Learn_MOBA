// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopWidget.h"

#include "Framework/CAssetManager.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/ShopItemWidget.h"

#include "Components/TileView.h"

void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
    
    LoadShopItems();

    ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::ShopItemWidgetGenerated);

    if (APawn* OwnerPawn = GetOwningPlayerPawn())
    {
        OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
        if (!OwnerInventoryComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s : Owner[%s] InventoryComponent not found"), *GetName(), *OwnerPawn->GetName());
        }
    }
}

void UShopWidget::LoadShopItems()
{
    UCAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished));
}

void UShopWidget::ShopItemLoadFinished()
{
    TArray<const UPA_ShopItem*> ShopItems;
    UCAssetManager::Get().GetLoadedShopItems(ShopItems);

    for (const UPA_ShopItem* ShopItem : ShopItems)
    {
        ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
    }
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
    UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
    if (ItemWidget)
    {
        if (OwnerInventoryComponent)
        {
            ItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
            // ItemWidget->OnShopItemClicked.AddUObject(this, &ThisClass::ShopItemClicked);
        }
        ItemMap.Add(ItemWidget->GetShopItem(), ItemWidget);
    }
}
