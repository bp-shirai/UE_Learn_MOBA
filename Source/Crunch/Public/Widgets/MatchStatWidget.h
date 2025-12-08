// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class UImage;
class UTextBlock;
class AStormCore;
/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UMatchStatWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
    float ProgressUpdateInterval{0.5f};

    UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
    FName ParamName_Progress{"Progress"};

    UPROPERTY(meta = (BindWidget))
    UImage* ProgressImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamOneCounter;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamTwoCounter;

    UPROPERTY()
    AStormCore* StormCore;

    void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount);
    void MatchFinished(AActor* ViewTarget, int WiningTeam);

    FTimerHandle UpdateProgressTimerHandle;
    void UpdateProgress();
};
