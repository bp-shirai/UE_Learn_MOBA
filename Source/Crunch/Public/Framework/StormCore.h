// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

class USphereComponent;
class AAIController;
class UDecalComponent;
class UCameraComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReached, AActor* /*ViewTarget*/, int /*WiningTeam*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamInfluencerCountUpdate, int /*TeamOneInfluencerCount*/, int /*TeamTwoInfluencerCount*/);

UCLASS()
class CRUNCH_API AStormCore : public ACharacter
{
    GENERATED_BODY()

public:
    AStormCore();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void PossessedBy(AController* NewController) override;

    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    float GetProgress() const;

    FOnGoalReached OnGoalReached;
    FOnTeamInfluencerCountUpdate OnTeamInfluencerCountUpdate;

private:
    UPROPERTY(EditDefaultsOnly, Category = "StormCore|Move")
    UAnimMontage* AM_Expand;

    UPROPERTY(EditDefaultsOnly, Category = "StormCore|Move")
    UAnimMontage* AM_Capture;

    UPROPERTY(EditDefaultsOnly, Category = "StormCore|Move")
    float InfluenceRadius{1000.f};

    UPROPERTY(EditDefaultsOnly, Category = "StormCore|Move")
    float MaxMoveSpeed{500.f};

    UPROPERTY(VisibleDefaultsOnly, Category = "StormCore|Detection")
    USphereComponent* InfluenceRange;

    UPROPERTY(VisibleDefaultsOnly, Category = "StormCore|Detection")
    UDecalComponent* GroundDecalComponent;

    UPROPERTY(VisibleDefaultsOnly, Category = "StormCore|Detection")
    UCameraComponent* ViewCam;

    UFUNCTION()
    void NewInfluenceInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void NewInfluenceLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void UpdateTeamWeight();
    void UpdateGoal();
    void GoalReached(int WiningTeam);
    void CaptureCore();
    void ExpandFinished();

    UPROPERTY(EditAnywhere, Category = "StormCore|Team")
    AActor* TeamOneGoal;

    UPROPERTY(EditAnywhere, Category = "StormCore|Team")
    AActor* TeamTwoGoal;

    UPROPERTY(EditAnywhere, Category = "StormCore|Team")
    AActor* TeamOneCore;

    UPROPERTY(EditAnywhere, Category = "StormCore|Team")
    AActor* TeamTwoCore;

    UPROPERTY(ReplicatedUsing = OnRep_CoreToCapture)
    AActor* CoreToCapture;

    float CoreCaptureSpeed{0.f};
    float TravelLength{0.f};


    UFUNCTION()
    void OnRep_CoreToCapture();

    UPROPERTY()
    AAIController* OwnerAIController;

    int TeamOneInfluencerCount{0};
    int TeamTwoInfluencerCount{0};
    float TeamWeight{0.f};
};
