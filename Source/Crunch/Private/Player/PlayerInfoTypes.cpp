// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerInfoTypes.h"
#include "Character/PA_CharacterDefinition.h"
#include "GameFramework/PlayerState.h"
#include "Network/CNetStatics.h"

FPlayerSelection::FPlayerSelection()
    : Slot{GetInvalidSlot()}, PlayerUniqueId{FUniqueNetIdRepl::Invalid()}, PlayerNickName{TEXT("")}, SelectedDefinition{nullptr}
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState)
    : Slot{InSlot}, SelectedDefinition{nullptr}
{
    if (InPlayerState)
    {
        PlayerUniqueId = InPlayerState->GetUniqueId();
        PlayerNickName = InPlayerState->GetPlayerName();
    }
}

uint8 FPlayerSelection::GetInvalidSlot()
{
    return uint8(255);
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
    if (!PlayerState) return false;

#if WITH_EDITOR
    return PlayerState->GetPlayerName() == PlayerNickName;
#else
    return PlayerState->GetUniqueId() == PlayerUniqueId;
#endif
}

bool FPlayerSelection::IsValid() const
{
    const int MaxLocalPlayers = UCNetStatics::GetPlayerCountPerTeam() * 2;

#if WITH_EDITOR
    return true;
#else
    return PlayerUniqueId.IsValid() && Slot != GetInvalidSlot() && Slot < MaxLocalPlayers;
#endif
}
