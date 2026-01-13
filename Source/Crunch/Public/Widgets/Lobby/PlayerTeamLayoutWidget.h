// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UPlayerTeamSlotWidget;
class UHorizontalBox;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UPlayerTeamLayoutWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TSubclassOf<UPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    float PlayerTeamWidgetSlotMargin{5.f};


    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* TeamOneLayoutBox;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* TeamTwoLayoutBox;

    UPROPERTY()
    TArray<UPlayerTeamSlotWidget*> TeamSlotWidgets;
};
