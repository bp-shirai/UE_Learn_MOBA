// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CAIController.h"

#include "GenericTeamAgentInterface.h"
#include "Logging/LogVerbosity.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Character/CCharacter.h"

ACAIController::ACAIController()
{
    UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SetPerceptionComponent(*PerceptionComp);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals   = false;

    SightConfig->SightRadius                  = 1000.f;
    SightConfig->LoseSightRadius              = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 180.f;
    SightConfig->SetMaxAge(5.f);

    PerceptionComponent->ConfigureSense(*SightConfig);
    // AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
    // AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetForgotten);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    SetGenericTeamId(FGenericTeamId(1));

    IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
    if (PawnTeamInterface)
    {
        PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
    }

    if (PerceptionComponent)
    {
        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
        PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetForgotten);
    }
}

void ACAIController::OnUnPossess()
{
    Super::OnUnPossess();
}

void ACAIController::BeginPlay()
{
    Super::BeginPlay();

    if (!RunBehaviorTree(BehaviorTree))
    {
        UE_LOG(LogTemp, Warning, TEXT("AIController: %s, couldn't run behavior tree!"), *GetName());
    }
}

void ACAIController::OnTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
    {
        // Handle updates to the hearing sense
        // Example: calculate the distance between the stimulus receiver and the source
        // const float Distance = FVector::Dist(Stimulus.ReceiverLocation, Stimulus.StimulusLocation);
        //UE_LOG(LogTemp, Warning, TEXT("OnTargetPerceptionUpdated: AI: %s, TargetActor: %s, Distance: %f"), *GetName(), *GetNameSafe(TargetActor), Distance);

        if (GetCurrentTarget() == nullptr)
        {
            SetCurrentTarget(TargetActor);
        }
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("OnTargetPerceptionUpdated: AI: %s, TargetActor: %s, Lost sight of the target !"), *GetName(), *GetNameSafe(TargetActor));

        if (TargetActor == GetCurrentTarget())
        {
            SetCurrentTarget(nullptr);
        }
    }
}

void ACAIController::OnTargetForgotten(AActor* ForgottenActor)
{
    if (!ForgottenActor) return;

    if (ForgottenActor == GetCurrentTarget())
    {
        SetCurrentTarget(GetNextPerceivedTarget());
    }
}

const UObject* ACAIController::GetCurrentTarget() const
{
    const UBlackboardComponent* BBComp = GetBlackboardComponent();
    return BBComp ? BBComp->GetValueAsObject(Target_KeyName) : nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp) return;

    if (IsValid(NewTarget))
    {
        BBComp->SetValueAsObject(Target_KeyName, NewTarget);
    }
    else
    {
        BBComp->ClearValue(Target_KeyName);
    }
}

AActor* ACAIController::GetNextPerceivedTarget() const
{
    if (PerceptionComponent)
    {
        TArray<AActor*> Actors;
        PerceptionComponent->GetPerceivedHostileActors(Actors);

        if (Actors.Num() != 0)
        {
            return Actors[0];
        }
    }

    return nullptr;
}
