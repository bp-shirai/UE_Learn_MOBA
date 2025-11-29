// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/PA_ShopItem.h"
#include "UObject/PrimaryAssetId.h"

FPrimaryAssetId UPA_ShopItem::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UPA_ShopItem::GetShopItemAssetType()
{
    return FPrimaryAssetType(TEXT("ShopItem"));
}

UTexture2D* UPA_ShopItem::GetIcon() const
{
    return Icon.IsValid() ? Icon.Get() : Icon.LoadSynchronous();
}

void FCItemCollection::AddItem(const UPA_ShopItem* NewItem, bool bAddUnique)
{
    if (bAddUnique && Contains(NewItem)) return;

    Items.Add(NewItem);
}

bool FCItemCollection::Contains(const UPA_ShopItem* Item) const
{
    return Items.Contains(Item);
}
