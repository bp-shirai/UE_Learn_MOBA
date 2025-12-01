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
