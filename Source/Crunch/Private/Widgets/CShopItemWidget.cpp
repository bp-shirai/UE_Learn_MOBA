// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CShopItemWidget.h"

#include "Inventory/PA_ShopItem.h"


void UCShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    ShopItem = Cast<UPA_ShopItem>(ListItemObject);
    if (ShopItem)
    {
        SetIcon(ShopItem->GetIcon());
    }
}
