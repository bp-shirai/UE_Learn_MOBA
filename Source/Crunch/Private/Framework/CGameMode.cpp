// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Controller.h"
#include "EngineUtils.h"

#include "Framework/StormCore.h"
#include "GenericTeamAgentInterface.h"
#include "Player/CPlayerController.h"
#include "Player/CPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CGameMode)

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    APlayerController* NewPlayerController             = Super::SpawnPlayerController(InRemoteRole, Options);
    IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
    FGenericTeamId TeamId                              = GetTeamIDForPlayer(NewPlayerController);
    if (NewPlayerTeamInterface)
    {
        NewPlayerTeamInterface->SetGenericTeamId(TeamId);
    }

    NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);
    return NewPlayerController;
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
    const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);

    if (!StartSpotTag)
    {
        UE_LOG(LogTemp, Error, TEXT("FindNextStartSpotForTeam: %d not found in TeamStartSpotTagMap"), TeamID.GetId());
        return nullptr;
    }

    UWorld* World = GetWorld();

    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        if (It->PlayerStartTag == *StartSpotTag)
        {
            It->PlayerStartTag = FName("Taken");
            return *It;
        }
    }

    return nullptr;
}

AStormCore* ACGameMode::GetStormCore()
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    for (TActorIterator<AStormCore> It(World); It; ++It)
    {
        return *It;
    }

    return nullptr;
}

void ACGameMode::StartPlay()
{
    Super::StartPlay();

    if (AStormCore* StormCore = GetStormCore())
    {
        StormCore->OnGoalReached.AddUObject(this, &ThisClass::MatchFinished);
    }
}

void ACGameMode::MatchFinished(AActor* ViewTarget, int WiningTeam)
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ACPlayerController> It(World); It; ++It)
    {
        It->MatchFinished(ViewTarget, WiningTeam);
    }
}

UClass* ACGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    ACPlayerState* PlayerState = InController->GetPlayerState<ACPlayerState>();
    if (PlayerState && PlayerState->GetSelectedCharacterClass())
    {
        return PlayerState->GetSelectedCharacterClass();
    }

    return BackupPawnClass;
}

APawn* ACGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    IGenericTeamAgentInterface* NewPlayerTeam = Cast<IGenericTeamAgentInterface>(NewPlayer);
    FGenericTeamId TeamId                     = GetTeamIDForPlayer(NewPlayer);

    if (NewPlayerTeam)
    {
        NewPlayerTeam->SetGenericTeamId(TeamId);
    }

    StartSpot            = FindNextStartSpotForTeam(TeamId);
    NewPlayer->StartSpot = StartSpot;

    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ACGameMode::GetTeamIDForPlayer(const AController* InController) const
{
    ACPlayerState* PlayerState = InController->GetPlayerState<ACPlayerState>();
    if (PlayerState)
    {
        return PlayerState->GetTeamIdBaseOnSlot();
    }

    static int32 PlayerCount = 0;
    ++PlayerCount;

    return FGenericTeamId((PlayerCount % 2) + 1); // 1 or 2
}
