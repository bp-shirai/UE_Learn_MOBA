// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Lobby/LobbyWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TileView.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "Player/CPlayerState.h"
#include "Player/LobbyPlayerController.h"
#include "Player/PlayerInfoTypes.h"
#include "Character/PA_CharacterDefinition.h"
#include "Widgets/Lobby/TeamSelectionWidget.h"
#include "Widgets/Lobby/CharacterEntryWidget.h"
#include "Widgets/Lobby/CharacterDisplay.h"
#include "Widgets/AbilityListView.h"
#include "Network/CNetStatics.h"
#include "Framework/CGameState.h"
#include "Framework/CAssetManager.h"

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

    if (CharacterSelectionTileView)
    {
        CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);
    }

    SpawnCharacterDisplay();
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

    for (UUserWidget* CharacterEntryAsWidget : CharacterSelectionTileView->GetDisplayedEntryWidgets())
    {
        if (UCharacterEntryWidget* CharacterEntry = Cast<UCharacterEntryWidget>(CharacterEntryAsWidget))
        {
            CharacterEntry->SetSelected(false);
        }
    }

    for (const FPlayerSelection& PlayerSelection : PlayerSelections)
    {
        if (!PlayerSelection.IsValid()) continue;

        UTeamSelectionWidget* SelectionSlot = TeamSelectionSlots[PlayerSelection.GetPlayerSlot()];
        if (SelectionSlot)
        {
            SelectionSlot->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
        }

        UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition());
        if (SelectedEntry)
        {
            SelectedEntry->SetSelected(true);
        }

        if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
        {
            UpdateCharacterDisplay(PlayerSelection);
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

void ULobbyWidget::CharacterSelected(UObject* SelectedObject)
{
    if (!CPlayerState)
    {
        CPlayerState = GetOwningPlayerState<ACPlayerState>();
    }

    if (!CPlayerState) return;

    if (const UPA_CharacterDefinition* CharacterDefinition = Cast<UPA_CharacterDefinition>(SelectedObject))
    {
        CPlayerState->Server_SetSelectedCharacterDefinition(CharacterDefinition);
    }
}

void ULobbyWidget::SpawnCharacterDisplay()
{
    if (!CharacterDisplayClass) return;
    if (CharacterDisplay) return;

    FTransform SpawnTransform = FTransform::Identity;
    AActor* PlayerStart       = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
    if (PlayerStart)
    {
        SpawnTransform = PlayerStart->GetActorTransform();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, SpawnTransform, SpawnParams);
    if (CharacterDisplay)
    {
        GetOwningPlayer()->SetViewTarget(CharacterDisplay);
    }
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
    if (!PlayerSelection.GetCharacterDefinition()) return;
    if (!CharacterDisplay) return;

    CharacterDisplay->ConfigureWithCharacterDefinition(PlayerSelection.GetCharacterDefinition());

    AbilityListView->ClearListItems();

    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* Abilities = PlayerSelection.GetCharacterDefinition()->GetAbilities();
    if (Abilities)
    {
        AbilityListView->ConfigureAbilities(*Abilities);
    }
}
