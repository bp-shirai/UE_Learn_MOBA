// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "GenericTeamAgentInterface.h"
#include "CGameMode.generated.h"

class AStormCore;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ACGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
    virtual void StartPlay() override;
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
    virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

private:
    FGenericTeamId GetTeamIDForPlayer(const AController* InController) const;

    AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;

    UPROPERTY(EditDefaultsOnly, Category = "Team")
    TMap<FGenericTeamId, FName> TeamStartSpotTagMap;

    UPROPERTY(EditDefaultsOnly, Category = "Team")
    TSubclassOf<APawn> BackupPawnClass;

    AStormCore* GetStormCore();

    void MatchFinished(AActor* ViewTarget, int WiningTeam);
};
