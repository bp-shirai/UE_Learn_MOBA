// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopItemWidget.h"

#include "Blueprint/IUserListEntry.h"
#include "Components/ListView.h"

#include "Inventory/PA_ShopItem.h"
#include "Framework/CAssetManager.h"
#include "Widgets/TreeNodeInterface.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    InitWithShopItem(Cast<UPA_ShopItem>(ListItemObject));

    ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());
}

void UShopItemWidget::RightButtonClicked()
{
    OnItemPurchaseIssued.Broadcast(ShopItem);
}

void UShopItemWidget::LeftButtonClicked()
{
    OnShopItemClicked.Broadcast(this);
}

//
//-------------- TreeNodeInterface Implement -----------------------------------
//
UUserWidget* UShopItemWidget::GetWidget() const
{
    UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass());
    Copy->CopyFromOther(this);
    return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
    const FItemCollection* Collection = UCAssetManager::Get().GetCombinationForItem(ShopItem);
    if (Collection)
    {
        return ItemsToInterfaces(Collection->GetItems());
    }

    return TArray<const ITreeNodeInterface*>{};
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
    const FItemCollection* Collection = UCAssetManager::Get().GetIngredientsForItem(ShopItem);
    if (Collection)
    {
        return ItemsToInterfaces(Collection->GetItems());
    }

    return TArray<const ITreeNodeInterface*>{};
}

const UObject* UShopItemWidget::GetItemObject() const
{
    return ShopItem;
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* OtherWidget)
{
    OnItemPurchaseIssued = OtherWidget->OnItemPurchaseIssued;
    OnShopItemClicked    = OtherWidget->OnShopItemClicked;
    InitWithShopItem(OtherWidget->GetShopItem());
    ParentListView = OtherWidget->ParentListView;
}

void UShopItemWidget::InitWithShopItem(const UPA_ShopItem* NewShopItem)
{
    ShopItem = NewShopItem;
    if (!ShopItem) return;

    SetIcon(ShopItem->GetIcon());

    SetToolTipWidget(ShopItem);
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const
{

    TArray<const ITreeNodeInterface*> Interfaces;
    if (!ParentListView) return Interfaces;

    for (const UPA_ShopItem* Item : Items)
    {
        const UShopItemWidget* ItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(Item);
        if (ItemWidget)
        {
            Interfaces.Add(ItemWidget);
        }
    }

    return Interfaces;
}
