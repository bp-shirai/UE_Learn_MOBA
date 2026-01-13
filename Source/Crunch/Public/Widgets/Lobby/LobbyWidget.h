// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UUniformGridPanel;
class UTeamSelectionWidget;
class UTileView;
class ALobbyPlayerController;
class ACGameState;
class ACPlayerState;
class UAbilityListView;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ULobbyWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* MainSwitcher;

    UPROPERTY(meta = (BindWidget))
    UWidget* TeamsSelectionRoot;

    UPROPERTY(meta = (BindWidget))
    UButton* StartHeroSelectionButton;

    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* TeamSelectionSlotGridPanel;

    UPROPERTY(meta = (BindWidget))
    UWidget* HeroSelectionRoot;

    UPROPERTY(meta = (BindWidget))
    UTileView* CharacterSelectionTileView;

    UPROPERTY(meta=(BindWidget))
    UAbilityListView* AbilityListView;


    UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
    TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

    UPROPERTY()
    TArray<UTeamSelectionWidget*> TeamSelectionSlots;

    UPROPERTY()
    ACPlayerState* CPlayerState;

    void ClearAndPopulateTeamSelectionSlots();
    void SlotSelected(uint8 NewSlotID);

    ALobbyPlayerController* LobbyPlayerController;

    void ConfigureGameState();

    UPROPERTY()
    ACGameState* CGameState;

    FTimerHandle ConfigureGameStateTimer;

    void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

    UFUNCTION()
    void StartHeroSelectionButtonClicked();

    
    void SwitchToHeroSelection();

    void CharacterDefinitionsLoaded();


    void CharacterSelected(UObject* SelectedObject);

    UPROPERTY(EditDefaultsOnly, Category = "Character Display")
    TSubclassOf<class ACharacterDisplay> CharacterDisplayClass;

    UPROPERTY()
    class ACharacterDisplay* CharacterDisplay;

    void SpawnCharacterDisplay();
    void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection);
};

