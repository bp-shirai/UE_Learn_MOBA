// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Lobby/PlayerTeamLayoutWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "Layout/Margin.h"
#include "Widgets/Lobby/PlayerTeamSlotWidget.h"
#include "Network/CNetStatics.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TeamOneLayoutBox->ClearChildren();
    TeamTwoLayoutBox->ClearChildren();

    if (!PlayerTeamSlotWidgetClass) return;

    const int PlayerCountPerTeam = UCNetStatics::GetPlayerCountPerTeam();
    const int MaxPlayerCount     = PlayerCountPerTeam * 2;

    for (int i = 0; i < MaxPlayerCount; i++)
    {
        UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass);
        if (NewSlotWidget)
        {
            TeamSlotWidgets.Add(NewSlotWidget);

            UHorizontalBoxSlot* NewSlot;
            if (i < PlayerCountPerTeam)
            {
                NewSlot = TeamOneLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
            }
            else
            {
                NewSlot = TeamTwoLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
            }

            NewSlot->SetPadding(FMargin{PlayerTeamWidgetSlotMargin});
        }
    }
}

void UPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections)
{

    for (UPlayerTeamSlotWidget* SlotWidget : TeamSlotWidgets)
    {
        SlotWidget->UpdateSlot(TEXT(""), nullptr);
    }

    for (const FPlayerSelection& PlayerSelection : PlayerSelections)
    {
        if (!PlayerSelection.IsValid()) continue;

        const uint8 PlayerSlot = PlayerSelection.GetPlayerSlot();

        UPlayerTeamSlotWidget* SlotWidget = TeamSlotWidgets.IsValidIndex(PlayerSlot) ? TeamSlotWidgets[PlayerSlot] : nullptr;
        if (SlotWidget)
        {
            SlotWidget->UpdateSlot(PlayerSelection.GetPlayerNickName(), PlayerSelection.GetCharacterDefinition());
        }
    }
}
