// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryWidget.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"

#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryComponent.h"
#include "Logging/LogVerbosity.h"
#include "Widgets/InventoryItemWidget.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ItemWidgetClass) UE_LOG(LogTemp, Warning, TEXT("%s : ItemWidgetClass is nullptr"), *GetName());

    if (APawn* OwnerPawn = GetOwningPlayerPawn())
    {
        InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
        if (InventoryComponent)
        {
            InventoryComponent->OnItemAdded.AddUObject(this, &ThisClass::ItemAdded);
            InventoryComponent->OnItemStackCountChanged.AddUObject(this, &ThisClass::ItemStackCountChanged);

            int Capacity = InventoryComponent->GetCapacity();

            ItemList->ClearChildren();
            ItemWidgets.Empty();

            for (int i = 0; i < Capacity; i++)
            {
                UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
                if (NewEmptyWidget)
                {
                    NewEmptyWidget->SetSlotNumber(i);
                    UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
                    if (NewItemSlot)
                    {
                        NewItemSlot->SetPadding(FMargin(2.f));
                    }

                    ItemWidgets.Add(NewEmptyWidget);

                    NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &ThisClass::HandleItemDragDrop);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s : Owner[%s] InventoryComponent not found"), *GetName(), *OwnerPawn->GetName());
        }
    }
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
    if (!InventoryItem) return;

    if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
    {
        NextAvailableSlot->UpdateInventoryItem(InventoryItem);

        PopulatedItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);
        if (InventoryComponent)
        {
            InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
        }
    }
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
    for (UInventoryItemWidget* Widget : ItemWidgets)
    {
        if (Widget->IsEmpty())
        {
            return Widget;
        }
    }

    return nullptr;
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
    if (UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(Handle))
    {
        (*FoundWidget)->UpdateStackCount();
    }
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget)
{
    if (!DestinationWidget || !SourceWidget) return;

    const UInventoryItem* SourceItem      = SourceWidget->GetInventoryItem();
    const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem(); // If the slot is empty, it is nullptr.
    

    DestinationWidget->UpdateInventoryItem(SourceItem);
    SourceWidget->UpdateInventoryItem(DestinationItem);

    //UE_LOG(LogTemp, Warning, TEXT("HandleItemDragDrop : Src[%s] Dest[%s]"), *GetNameSafe(SourceItem), *GetNameSafe(DestinationItem));

    PopulatedItemEntryWidgets[DestinationWidget->GetItemHandle()] = DestinationWidget;

    if (InventoryComponent)
    {
        InventoryComponent->ItemSlotChanged(DestinationWidget->GetItemHandle(), DestinationWidget->GetSlotNumber());
    }

    if (!SourceWidget->IsEmpty())
    {
        PopulatedItemEntryWidgets[SourceWidget->GetItemHandle()] = SourceWidget;
        if (InventoryComponent)
        {
            InventoryComponent->ItemSlotChanged(SourceWidget->GetItemHandle(), SourceWidget->GetSlotNumber());
        }
    }
}
