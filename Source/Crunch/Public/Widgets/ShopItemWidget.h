// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/TreeNodeInterface.h"
#include "ShopItemWidget.generated.h"

class UShopWidget;
class UShopItemWidget;
class UPA_ShopItem;
class UListView;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
    GENERATED_BODY()

public:
    FOnItemPurchaseIssued OnItemPurchaseIssued;
    FOnShopItemSelected OnShopItemClicked;

    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

    virtual UUserWidget* GetWidget() const override;
    virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
    virtual TArray<const ITreeNodeInterface*> GetOutputs() const override;
    virtual const UObject* GetItemObject() const override;

private:
    UPROPERTY()
    const UPA_ShopItem* ShopItem;

    virtual void RightButtonClicked() override;
    virtual void LeftButtonClicked() override;

    void CopyFromOther(const UShopItemWidget* OtherWidget);
    void InitWithShopItem(const UPA_ShopItem* NewShopItem);
    TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const;

    const UListView* ParentListView;
};
