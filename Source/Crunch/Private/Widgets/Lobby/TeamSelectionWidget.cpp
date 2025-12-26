// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/TeamSelectionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void UTeamSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SelectButton->OnClicked.AddDynamic(this, &ThisClass::SelectButtonClicked);
}

void UTeamSelectionWidget::SetSlotID(uint8 NewID)
{
    SlotID = NewID;
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
    InfoText->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::SelectButtonClicked()
{
    OnSlotClicked.Broadcast(SlotID);
}
