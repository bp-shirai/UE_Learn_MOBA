// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /*SlotID*/);

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UTeamSelectionWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    void SetSlotID(uint8 NewID);
    void UpdateSlotInfo(const FString& PlayerNickName);

    FOnSlotClicked OnSlotClicked;

private:
    UPROPERTY(meta = (BindWidget))
    UButton* SelectButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* InfoText;

    UFUNCTION()
    void SelectButtonClicked();

    uint8 SlotID;
};
