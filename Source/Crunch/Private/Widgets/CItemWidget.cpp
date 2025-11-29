// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
}

void UCItemWidget::SetIcon(UTexture2D* IconTexture)
{
    Icon->SetBrushFromTexture(IconTexture);
}

FReply UCItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

FReply UCItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
   
    if(HasAnyUserFocus())
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

void UCItemWidget::RightButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("RightButtonClicked"));
}

void UCItemWidget::LeftButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("LeftButtonClicked"));
}
