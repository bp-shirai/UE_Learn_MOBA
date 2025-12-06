// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryItemWidget.generated.h"

class UTextBlock;
class UInventoryItem;
class UInventoryItemDragDropOp;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDropped, UInventoryItemWidget* /*DestinationWidget*/, UInventoryItemWidget* /*SourceWidget*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClick, const FInventoryItemHandle& /*ItemHandle*/);
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
    FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
    FInventoryItemHandle GetItemHandle() const;

    void EmptySlot();

    void UpdateStackCount();

    UTexture2D* GetIconTexture() const;

    FOnInventoryItemDropped OnInventoryItemDropped;
    FOnButtonClick OnLeftButtonClicked;
    FOnButtonClick OnRightButtonClicked;

private:
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

    virtual void RightButtonClicked() override;
    virtual void LeftButtonClicked() override;

#pragma region---------------- Drag Drop ---------------------------------------------------

private:
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
    TSubclassOf<UInventoryItemDragDropOp> DragDropClass;

#pragma endregion

#pragma region---------------- GAS ------------------------------------------------------
public:
    void StartCooldown(float InCooldownDuration, float InTimeRemaining);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    float CooldownUpdateInterval{0.1f};

    void CooldownFinished();
    void UpdateCooldown();
    void ClearCooldown();

    FTimerHandle Handle_CooldownDuration;
    FTimerHandle Handle_CooldownUpdate;

    float CooldownTimeRemaining{0.f};
    float CooldownTimeDuration{0.f};

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    FName MaterialParam_CooldownAmt{"Percent"};

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    FName MaterialParam_IconTexture{"Icon"};

    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    FName MaterialParam_CanCast{"CanCast"};

    virtual void SetIcon(UTexture2D* IconTexture) override;
    FNumberFormattingOptions CooldownFormatOpt;

    void UpdateCanCastDisplay(bool bCanCast);
    void BindCanCastAbilityDelegate();
    void UnBindCanCastAbilityDelegate();

#pragma endregion
};
