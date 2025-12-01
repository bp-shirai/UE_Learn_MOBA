// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Widgets/ItemToolTip.h"
#include "Inventory/PA_ShopItem.h"

void UItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
    Icon->SetBrushFromTexture(IconTexture);
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
        return FReply::Handled().SetUserFocus(TakeWidget());
    }

    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }

    return SuperReply;
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

    if (HasAnyUserFocus())
    {
        if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
        {
            RightButtonClicked();
            return FReply::Handled();
        }
        else if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            LeftButtonClicked();
            return FReply::Handled();
        }
    }

    return SuperReply;
}

void UItemWidget::RightButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("RightButtonClicked"));
}

void UItemWidget::LeftButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("LeftButtonClicked"));
}

UItemToolTip* UItemWidget::SetToolTipWidget(const UPA_ShopItem* Item)
{
    if (GetOwningPlayer() && ItemToolTipClass && Item)
    {
        UItemToolTip* NewToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ItemToolTipClass);
        if (NewToolTip)
        {
            NewToolTip->SetItem(Item);
            SetToolTip(NewToolTip);
            
            return NewToolTip;
        }
    }
 
    return nullptr;
}

// void UItemWidget::ReplaceToolTip(UItemToolTip* NewToolTip)
// {
//     if (UWidget* CurrentToolTip = GetToolTip())
//     {
//         CurrentToolTip->RemoveFromParent();
//         CurrentToolTip->ConditionalBeginDestroy(); // If necessary
//     }

//     if (NewToolTip)
//     {
//         SetToolTip(NewToolTip);
//     }
//     else
//     {
//         SetToolTip(nullptr);
//     }
// }
