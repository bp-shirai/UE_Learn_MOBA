// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Delegates/Delegate.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Character/CCharacter.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"

ACAIController::ACAIController()
{
    UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SetPerceptionComponent(*PerceptionComp);
    ensureMsgf(PerceptionComponent, TEXT("AI: %s, AIPerceptionComponent is nullptr"), *GetName());

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->DetectionByAffiliation.bDetectEnemies    = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SightConfig->DetectionByAffiliation.bDetectNeutrals   = false;

    SightConfig->SightRadius                  = 1000.f;
    SightConfig->LoseSightRadius              = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 180.f;
    SightConfig->SetMaxAge(5.f);

    PerceptionComponent->ConfigureSense(*SightConfig);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    SetGenericTeamId(FGenericTeamId(1)); // Forces a team to be set to 1.

    IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
    if (PawnTeamInterface)
    {
        PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
    }

    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
    PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetForgotten);

    UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewPawn);
    if (PawnASC)
    {
        PawnASC->RegisterGameplayTagEvent(Tags::Stats::Dead).AddUObject(this, &ThisClass::PawnDeadTagUpdated);
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
        // if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
        // Handle updates to the hearing sense
        // Example: calculate the distance between the stimulus receiver and the source
        // const float Distance = FVector::Dist(Stimulus.ReceiverLocation, Stimulus.StimulusLocation);

        if (GetCurrentTarget() == nullptr)
        {
            SetCurrentTarget(TargetActor);
        }
    }
    else
    {
        ForgetActorIfDead(TargetActor);
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
    TArray<AActor*> Actors;
    PerceptionComponent->GetPerceivedHostileActors(Actors);

    if (Actors.Num() != 0)
    {
        return Actors[0];
    }

    return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
    if (UCAbilitySystemStatics::DoesActorHaveTag(ActorToForget, Tags::Stats::Dead))
    {
        // PerceptionComponent->ForgetActor(ActorToForget);

        for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator It = PerceptionComponent->GetPerceptualDataIterator(); It; ++It)
        {
            if (It->Key != ActorToForget)
            {
                continue;
            }

            for (FAIStimulus& Stimulus : It->Value.LastSensedStimuli)
            {
                Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
            }
        }
    }
}

void ACAIController::ClearAndDisableAllSenses()
{
    // PerceptionComponent->ForgetAll();
    PerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

    for (auto SenseConfig = PerceptionComponent->GetSensesConfigIterator(); SenseConfig; ++SenseConfig)
    {
        PerceptionComponent->SetSenseEnabled((*SenseConfig)->GetSenseImplementation(), false);
    }

    if (GetBlackboardComponent())
    {
        GetBlackboardComponent()->ClearValue(Target_KeyName);
    }
}

void ACAIController::EnableAllSenses()
{
    for (auto SenseConfig = PerceptionComponent->GetSensesConfigIterator(); SenseConfig; ++SenseConfig)
    {
        PerceptionComponent->SetSenseEnabled((*SenseConfig)->GetSenseImplementation(), true);
    }
}

void ACAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
    if (Count)
    {
        if (GetBrainComponent())
        {
            GetBrainComponent()->StopLogic(TEXT("Dead"));
        }
        ClearAndDisableAllSenses();
    }
    else
    {
        if (GetBrainComponent())
        {
            GetBrainComponent()->StartLogic();
        }
        EnableAllSenses();
    }
}
