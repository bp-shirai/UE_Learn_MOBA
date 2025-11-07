// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    APlayerController* NewPC                       = Super::SpawnPlayerController(InRemoteRole, Options);
    FGenericTeamId TeamID                          = GetTeamIDForPlayer(NewPC);
    IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPC);
    if (TeamAgentInterface)
    {
        TeamAgentInterface->SetGenericTeamId(TeamID);
    }

    NewPC->StartSpot = FindNextStartSpotFromTeam(TeamID);

    return NewPC;
}

FGenericTeamId ACGameMode::GetTeamIDForPlayer(const APlayerController* PlayerController) const
{
    static int32 PlayerCount = 0;
    ++PlayerCount;

    return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotFromTeam(const FGenericTeamId& TeamID) const
{
    const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
    if (!StartSpotTag)
    {
        return nullptr;
    }

    UWorld* World = GetWorld();

    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        if (It->PlayerStartTag == *StartSpotTag)
        {
            It->PlayerStartTag = FName(TEXT("Taken"));
            return *It;
        }
    }

    return nullptr;
}
