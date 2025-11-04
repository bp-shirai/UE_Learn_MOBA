// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerController.h"
#include "Player/CPlayerCharacter.h"

#include "Widgets/GameplayWidget.h"

void ACPlayerController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
    if (PlayerCharacter)
    {
        PlayerCharacter->ServerSideInit();
    }
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
    Super::AcknowledgePossession(NewPawn);

    PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
    if (PlayerCharacter)
    {
        PlayerCharacter->ClientSideInit();
        
        if (IsLocalPlayerController())
        {
            SpawnGameplayWidget();
        }
    }
}

void ACPlayerController::SpawnGameplayWidget()
{
    if (GameplayWidget)
    {
        GameplayWidget->RemoveFromParent();
        GameplayWidget = nullptr;
    }

    if (GameplayWidgetClass)
    {
        GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
        if (GameplayWidget)
        {
            GameplayWidget->AddToViewport();
        }
    }
}
