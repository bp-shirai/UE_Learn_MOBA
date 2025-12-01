// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"


class UTileView;
class UShopItemWidget;
class UPA_ShopItem;
class UInventoryComponent;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UTileView* ShopItemList;

	void LoadShopItems();
	void ShopItemLoadFinished();
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);

	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ItemMap;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
};
