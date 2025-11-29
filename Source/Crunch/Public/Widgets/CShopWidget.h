// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CShopWidget.generated.h"


class UTileView;
class UCShopItemWidget;
class UPA_ShopItem;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UCShopWidget : public UUserWidget
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
	TMap<const UPA_ShopItem*, const UCShopItemWidget*> ItemMap;
};
