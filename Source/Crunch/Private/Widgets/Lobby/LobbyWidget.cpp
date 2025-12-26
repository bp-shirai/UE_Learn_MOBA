// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Lobby/LobbyWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TileView.h"

#include "Engine/World.h"
#include "Player/LobbyPlayerController.h"
#include "Player/PlayerInfoTypes.h"
#include "Widgets/Lobby/TeamSelectionWidget.h"
#include "Network/CNetStatics.h"
#include "Framework/CGameState.h"
#include "Framework/CAssetManager.h"
#include "Character/PA_CharacterDefinition.h"

void ULobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ClearAndPopulateTeamSelectionSlots();

    LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
    if (LobbyPlayerController)
    {
        LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ThisClass::SwitchToHeroSelection);
    }

    ConfigureGameState();

    StartHeroSelectionButton->SetIsEnabled(false);
    StartHeroSelectionButton->OnClicked.AddDynamic(this, &ThisClass::StartHeroSelectionButtonClicked);

    UCAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &ThisClass::CharacterDefinitionsLoaded));
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
    TeamSelectionSlotGridPanel->ClearChildren();

    const int PlayerCountPerTeam = UCNetStatics::GetPlayerCountPerTeam();
    for (int i = 0; i < PlayerCountPerTeam * 2; i++)
    {
        UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
        if (NewSelectionSlot)
        {
            NewSelectionSlot->SetSlotID(i);

            const int Row = i % PlayerCountPerTeam;
            const int Col = i / PlayerCountPerTeam;

            UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot, Row, Col);

            NewSelectionSlot->OnSlotClicked.AddUObject(this, &ThisClass::SlotSelected);
            TeamSelectionSlots.Add(NewSelectionSlot);
        }
    }
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
    if (LobbyPlayerController)
    {
        LobbyPlayerController->Server_RequestPlayerSelectionChange(NewSlotID);
    }
}

void ULobbyWidget::ConfigureGameState()
{
    UWorld* World = GetWorld();
    if (!World) return;

    CGameState = World->GetGameState<ACGameState>();
    if (!CGameState)
    {
        World->GetTimerManager().SetTimer(ConfigureGameStateTimer, this, &ThisClass::ConfigureGameState, 1.f);
    }
    else
    {
        CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::UpdatePlayerSelectionDisplay);
        UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelections());
    }
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
    for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
    {
        SelectionSlot->UpdateSlotInfo(TEXT("Empty"));
    }

    for (const FPlayerSelection& PlayerSelection : PlayerSelections)
    {
        if (!PlayerSelection.IsValid()) continue;

        UTeamSelectionWidget* SelectionSlot = TeamSelectionSlots[PlayerSelection.GetPlayerSlot()];
        if (SelectionSlot)
        {
            SelectionSlot->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
        }
    }

    if (CGameState)
    {
        StartHeroSelectionButton->SetIsEnabled(CGameState->CanStartHeroSelection());
    }
}

void ULobbyWidget::StartHeroSelectionButtonClicked()
{
    if (LobbyPlayerController)
    {
        LobbyPlayerController->Server_StartHeroSelection();
    }
}

void ULobbyWidget::SwitchToHeroSelection()
{
    MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionsLoaded()
{
    TArray<UPA_CharacterDefinition*> CharacterDefinitions;
    if (UCAssetManager::Get().GetLoadedCharacterDefinitions(CharacterDefinitions))
    {
        CharacterSelectionTileView->SetListItems(CharacterDefinitions);
    }
}
