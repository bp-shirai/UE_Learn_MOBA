// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
/**
 *
 */
UCLASS()
class CRUNCH_API ACAIController : public AAIController
{
    GENERATED_BODY()

public:
    ACAIController();
    virtual void OnPossess(APawn* NewPawn) override;
    virtual void OnUnPossess() override;
    virtual void BeginPlay() override;

private:
    // UPROPERTY(VisibleAnywhere, Category = "AI Perception")
    // UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
    UBehaviorTree* BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
    FName Target_KeyName{TEXT("Target")};

    UPROPERTY(EditDefaultsOnly, Category = "AI Perception")
    UAISenseConfig_Sight* SightConfig;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

    UFUNCTION()
    void OnTargetForgotten(AActor* ForgottenActor);

    const UObject* GetCurrentTarget() const;
    void SetCurrentTarget(AActor* NewTarget);

    AActor* GetNextPerceivedTarget() const;
};
