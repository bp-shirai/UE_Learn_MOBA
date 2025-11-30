// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "CItemWidget.generated.h"

class UImage;
class UTextBlock;
class UItemToolTip;
class UPA_ShopItem;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void SetIcon(UTexture2D* IconTexture);

protected:
    UItemToolTip* SetToolTipWidget(const UPA_ShopItem* Item);

private:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void RightButtonClicked();
    virtual void LeftButtonClicked();

    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(EditDefaultsOnly, Category = "ToolTip")
    TSubclassOf<UItemToolTip> ItemToolTipClass;
};
