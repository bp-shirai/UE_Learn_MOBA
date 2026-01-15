// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"
#include "Engine/EngineBaseTypes.h"
#include "Misc/PackageName.h"

void UCGameInstance::StartMatch()
{
    const ENetMode NetMode = GetWorld()->GetNetMode();
    if (NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer)
    {
        LoadLevelAndListen(GameLevel);
    }
 
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
    const FName LevelURL = FName(FPackageName::ObjectPathToPackageName(Level.ToString()));

    if (LevelURL != "")
    {
        GetWorld()->ServerTravel(LevelURL.ToString() + TEXT("?listen"));
    }
}

