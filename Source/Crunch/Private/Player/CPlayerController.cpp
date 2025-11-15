// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerController.h"
#include "Player/CPlayerCharacter.h"

#include "Widgets/GameplayWidget.h"

#include "Net/UnrealNetwork.h"

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamID);
}

void ACPlayerController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
    if (PlayerCharacter)
    {
        PlayerCharacter->ServerSideInit();
        PlayerCharacter->SetGenericTeamId(TeamID);
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
            GameplayWidget->ConfigureAbilities(PlayerCharacter->GetAbilities());
        }
    }
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
    return TeamID;
}
