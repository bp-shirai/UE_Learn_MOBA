// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryItemWidget.generated.h"

class UTextBlock;
class UInventoryItem;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UInventoryItemWidget : public UItemWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    bool IsEmpty() const;
    void UpdateInventoryItem(const UInventoryItem* Item);
    FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
    FORCEINLINE void SetSlotNumber(int NewSlotNumber) { SlotNumber = NewSlotNumber; }
    void EmptySlot();

private:
    void UpdateStackCount();

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    UTexture2D* EmptyTexture;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StackCount;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CooldownCount;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CooldownDuration;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ManaCost;

    UPROPERTY()
    const UInventoryItem* InventoryItem;

    int SlotNumber;
};
