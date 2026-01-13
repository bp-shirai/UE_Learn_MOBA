// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Framework/CGameState.h"

ACPlayerState::ACPlayerState()
{
    bReplicates        = true;
    NetUpdateFrequency = 100.f;
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, PlayerSelection);
}

void ACPlayerState::BeginPlay()
{
    Super::BeginPlay();

    CGameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));
    if (CGameState)
    {
        CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::PlayerSelectionUpdated);
    }
}

void ACPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
    for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections)
    {
        if (NewPlayerSelection.IsForPlayer(this))
        {
            PlayerSelection = NewPlayerSelection;
        }
    }
}

void ACPlayerState::Server_SetSelectedCharacterDefinition_Implementation(const UPA_CharacterDefinition* NewDefinition)
{
    if (!CGameState) return;
    if (!NewDefinition) return;

    if (CGameState->IsDefinitionSelected(NewDefinition)) return;

    if (PlayerSelection.GetCharacterDefinition())
    {
        CGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
    }

    PlayerSelection.SetCharacterDefinition(NewDefinition);
    CGameState->SetCharacterSelected(this, NewDefinition);
}

bool ACPlayerState::Server_SetSelectedCharacterDefinition_Validate(const UPA_CharacterDefinition* NewDefinition)
{
    return true;
}
