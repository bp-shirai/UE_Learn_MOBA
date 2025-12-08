// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MatchStatWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "Framework/StormCore.h"

void UMatchStatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StormCore = Cast<AStormCore>(UGameplayStatics::GetActorOfClass(this, AStormCore::StaticClass()));
    if (StormCore)
    {
        StormCore->OnTeamInfluencerCountUpdate.AddUObject(this, &ThisClass::UpdateTeamInfluence);
        StormCore->OnGoalReached.AddUObject(this, &ThisClass::MatchFinished);
        GetWorld()->GetTimerManager().SetTimer(UpdateProgressTimerHandle, this, &ThisClass::UpdateProgress, ProgressUpdateInterval, true);
    }
}

void UMatchStatWidget::UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount)
{
    TeamOneCounter->SetText(FText::AsNumber(TeamOneCount));
    TeamTwoCounter->SetText(FText::AsNumber(TeamTwoCount));
}

void UMatchStatWidget::MatchFinished(AActor* ViewTarget, int WiningTeam)
{
    float Progress = WiningTeam == 1 ? 1.f : 0.f;

    GetWorld()->GetTimerManager().ClearTimer(UpdateProgressTimerHandle);
    ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ParamName_Progress, Progress);
}

void UMatchStatWidget::UpdateProgress()
{
    if (StormCore)
    {
        float Progress = StormCore->GetProgress();
        ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ParamName_Progress, Progress);
    }
}
