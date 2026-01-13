// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/LobbyPlayerController.h"

#include "Engine/World.h"
#include "Framework/CGameState.h"
#include "GameFramework/PlayerState.h"
#include "Logging/LogVerbosity.h"


ALobbyPlayerController::ALobbyPlayerController()
{
    bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::Server_RequestPlayerSelectionChange_Implementation(uint8 NewSlotID)
{
    if (!GetWorld()) return;

    ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
    if (!CGameState) return;

    CGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotID);
}

bool ALobbyPlayerController::Server_RequestPlayerSelectionChange_Validate(uint8 NewSlotID)
{
    return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
    if (!HasAuthority() || !GetWorld()) return;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(*It);
        if (LobbyPC)
        {
            LobbyPC->Client_StartHeroSelection();
        }
    }
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
    return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
    OnSwitchToHeroSelection.ExecuteIfBound();
}


