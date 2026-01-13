// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CGameState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerInfoTypes.h"

#include "Character/PA_CharacterDefinition.h"

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PlayerSelections, COND_None, REPNOTIFY_Always);
}

void ACGameState::OnRep_PlayerSelections()
{
    OnPlayerSelectionUpdated.Broadcast(PlayerSelections);
}

void ACGameState::RequestPlayerSelectionChange(const APlayerState* RequestPlayer, uint8 DesiredSlot)
{
    if (!HasAuthority() || IsSlotOccupied(DesiredSlot))
    {
        return;
    }

    FPlayerSelection* PlayerSelectionPtr = PlayerSelections.FindByPredicate(
        [&](const FPlayerSelection& PlayerSelection)
        {
            return PlayerSelection.IsForPlayer(RequestPlayer);
        });

    if (PlayerSelectionPtr)
    {
        PlayerSelectionPtr->SetSlot(DesiredSlot);
    }
    else
    {
        PlayerSelections.Add(FPlayerSelection(DesiredSlot, RequestPlayer));
    }

    OnPlayerSelectionUpdated.Broadcast(PlayerSelections);
}

bool ACGameState::IsSlotOccupied(uint8 SlotId) const
{
    for (const FPlayerSelection& PlayerSelection : PlayerSelections)
    {
        if (PlayerSelection.GetPlayerSlot() == SlotId)
        {
            return true;
        }
    }

    return false;
}

bool ACGameState::CanStartHeroSelection() const
{
    return PlayerSelections.Num() == PlayerArray.Num();
}

void ACGameState::SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefinition* SelectedDefinition)
{
    if (IsDefinitionSelected(SelectedDefinition))
    {
        return;
    }

    FPlayerSelection* FoundPlayerSelection = PlayerSelections.FindByPredicate(
        [&](const FPlayerSelection& PlayerSelection)
        {
            return PlayerSelection.IsForPlayer(SelectingPlayer);
        });

    if (FoundPlayerSelection)
    {
        FoundPlayerSelection->SetCharacterDefinition(SelectedDefinition);
        OnPlayerSelectionUpdated.Broadcast(PlayerSelections);
    }
}

bool ACGameState::IsDefinitionSelected(const UPA_CharacterDefinition* Definition) const
{
    const FPlayerSelection* FoundPlayerSelection = PlayerSelections.FindByPredicate(
        [&](const FPlayerSelection& PlayerSelection)
        {
            return PlayerSelection.GetCharacterDefinition() == Definition;
        });

    return FoundPlayerSelection != nullptr;
}

void ACGameState::SetCharacterDeselected(const UPA_CharacterDefinition* Definition)
{
    if (!Definition) return;

    FPlayerSelection* FoundPlayerSelection = PlayerSelections.FindByPredicate(
        [&](const FPlayerSelection& PlayerSelection)
        {
            return PlayerSelection.GetCharacterDefinition() == Definition;
        });
    
    if (FoundPlayerSelection)
    {
        FoundPlayerSelection->SetCharacterDefinition(nullptr);
        OnPlayerSelectionUpdated.Broadcast(PlayerSelections);   
    }
}
