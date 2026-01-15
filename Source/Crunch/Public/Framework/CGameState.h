// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "CGameState.generated.h"

class UPA_CharacterDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/);
/**
 *
 */
UCLASS()
class CRUNCH_API ACGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void RequestPlayerSelectionChange(const APlayerState* RequestPlayer, uint8 DesiredSlot);
    void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefinition* SelectedDefinition);
    bool IsSlotOccupied(uint8 SlotId) const;
    bool IsDefinitionSelected(const UPA_CharacterDefinition* Definition) const;
    void SetCharacterDeselected(const UPA_CharacterDefinition* Definition);

    FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;

    const TArray<FPlayerSelection>& GetPlayerSelections() const { return PlayerSelections; }

    bool CanStartHeroSelection() const;
    bool CanStartMatch() const;

private:
    UPROPERTY(ReplicatedUsing = OnRep_PlayerSelections)
    TArray<FPlayerSelection> PlayerSelections;

    UFUNCTION()
    void OnRep_PlayerSelections();
};
