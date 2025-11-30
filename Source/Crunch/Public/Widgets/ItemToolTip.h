// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UImage;
class UTextBlock;
class UPA_ShopItem;
/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UItemToolTip : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetItem(const UPA_ShopItem* Item);
    void SetPrice(float NewPrice);

private:
    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemTitle;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemDescription;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemPrice;
};
