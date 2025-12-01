// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemDragDropOp.h"

#include "Widgets/InventoryItemWidget.h"
#include "Widgets/ItemWidget.h"

void UInventoryItemDragDropOp::SetDraggedItem(UInventoryItemWidget* DraggedItem)
{
    Payload = DraggedItem;
    if (DragVisualClass)
    {
        UItemWidget* DragItemWidget = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
        if (DragItemWidget)
        {
            DragItemWidget->SetIcon(DraggedItem->GetIconTexture());
            DefaultDragVisual = DragItemWidget;
        }
    }
}
