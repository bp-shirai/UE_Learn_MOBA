// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CShopWidget.h"

#include "Framework/CAssetManager.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/CShopItemWidget.h"

#include "Components/TileView.h"

void UCShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
    LoadShopItems();

    ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::ShopItemWidgetGenerated);

    if (APawn* OwnerPawn = GetOwningPlayerPawn())
    {
        OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
    }
}

void UCShopWidget::LoadShopItems()
{
    UCAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished));
}

void UCShopWidget::ShopItemLoadFinished()
{
    TArray<const UPA_ShopItem*> ShopItems;
    UCAssetManager::Get().GetLoadedShopItems(ShopItems);

    for (const UPA_ShopItem* ShopItem : ShopItems)
    {
        ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
    }
}

void UCShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
    UCShopItemWidget* ItemWidget = Cast<UCShopItemWidget>(&NewWidget);
    if (ItemWidget)
    {
        if (OwnerInventoryComponent)
        {
            ItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
            //ItemWidget->OnShopItemClicked.AddUObject(this, &ThisClass::ShopItemClicked);
        }
        ItemMap.Add(ItemWidget->GetShopItem(), ItemWidget);
    }
}
