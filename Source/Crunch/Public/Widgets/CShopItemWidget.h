// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/CItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CShopItemWidget.generated.h"

class UCShopWidget;
class UCShopItemWidget;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UCShopItemWidget*);

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCShopItemWidget : public UCItemWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    FOnItemPurchaseIssued OnItemPurchaseIssued;
    FOnShopItemSelected OnShopItemClicked;

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

private:
    UPROPERTY()
    UPA_ShopItem* ShopItem;

    virtual void RightButtonClicked() override;
    virtual void LeftButtonClicked() override;
};
