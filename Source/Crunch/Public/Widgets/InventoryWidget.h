// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryWidget.generated.h"

class UWrapBox;
class UInventoryItemWidget;
class UInventoryComponent;
class UInventoryItem;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	
private:
    UPROPERTY(meta = (BindWidget))
    UWrapBox* ItemList;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    TArray<UInventoryItemWidget*> ItemWidgets;

    TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets;

    void ItemAdded(const UInventoryItem* InventoryItem);
    void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

    UInventoryItemWidget* GetNextAvailableSlot() const;

    void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget);
};
