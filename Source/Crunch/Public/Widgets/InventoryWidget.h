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
class UInventoryContextMenu;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

private:
    UPROPERTY(meta = (BindWidget))
    UWrapBox* ItemList;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventoryContextMenu> ContextMenuWidgetClass;

    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    UPROPERTY()
    UInventoryContextMenu* ContextMenuWidget;

    TArray<UInventoryItemWidget*> ItemWidgets;

    TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets;

    void ItemAdded(const UInventoryItem* InventoryItem);
    void ItemRemoved(const FInventoryItemHandle& Handle);
    void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

    UInventoryItemWidget* GetNextAvailableSlot() const;

    void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget);

    void SpawnContextMenu();
    UFUNCTION()
    void SellFocusedItem();
    UFUNCTION()
    void UseFocusedItem();
    void SetContextMenuVisible(bool bVisible);
    void ToggleContextMenu(const FInventoryItemHandle& ItemHandle);
    void ClearContextMenu();

    FInventoryItemHandle CurrentFocusedItemHandle;
};
