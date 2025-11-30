// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemToolTip.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/PA_ShopItem.h"

void UItemToolTip::SetItem(const UPA_ShopItem* Item)
{
    ItemTitle->SetText(Item->GetItemName());
    ItemDescription->SetText(Item->GetItemDescription());
    Icon->SetBrushFromTexture(Item->GetIcon());
}

void UItemToolTip::SetPrice(float NewPrice)
{
    ItemPrice->SetText(FText::AsNumber(static_cast<int>(NewPrice)));
}
