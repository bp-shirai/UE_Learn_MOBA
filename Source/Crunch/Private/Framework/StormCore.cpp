// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/StormCore.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/TimerHandle.h"
#include "GenericTeamAgentInterface.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/MathFwd.h"
#include "Net/UnrealNetwork.h"

AStormCore::AStormCore()
{
    PrimaryActorTick.bCanEverTick = true;

    InfluenceRange = CreateDefaultSubobject<USphereComponent>("InfluenceRange");
    InfluenceRange->SetupAttachment(GetRootComponent());
    InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::NewInfluenceInRange);
    InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::InfluenceLeftRange);

    ViewCam = CreateDefaultSubobject<UCameraComponent>("ViewCam");
    ViewCam->SetupAttachment(GetRootComponent());

    GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>("GroundDecalComponent");
    GroundDecalComponent->SetupAttachment(GetRootComponent());
    GroundDecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}

void AStormCore::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CoreToCapture, COND_None, REPNOTIFY_Always);
}

void AStormCore::BeginPlay()
{
    Super::BeginPlay();

    if (TeamOneGoal && TeamTwoGoal)
    {
        FVector TeamOneGoalLoc = TeamOneGoal->GetActorLocation();
        FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();

        FVector GoalOffset = TeamOneGoalLoc - TeamTwoGoalLoc;
        GoalOffset.Z       = 0;

        TravelLength = GoalOffset.Length();
    }

    CoreCaptureSpeed = 0.f;
    TravelLength     = FMath::Max(TravelLength, 1.f);
}

void AStormCore::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CoreToCapture)
    {
        FVector CoreMoveDir = (GetMesh()->GetComponentLocation() - CoreToCapture->GetActorLocation()).GetSafeNormal();
        CoreToCapture->AddActorWorldOffset(CoreMoveDir * CoreCaptureSpeed * DeltaTime);
    }
}

void AStormCore::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    OwnerAIController = Cast<AAIController>(NewController);
    if (!OwnerAIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("AStormCore::PossessedBy() OwnerAIController is null"));
    }
}

void AStormCore::NewInfluenceInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor == TeamOneGoal)
    {
        GoalReached(1);
    }

    if (OtherActor == TeamTwoGoal)
    {
        GoalReached(2);
    }

    IGenericTeamAgentInterface* OtherTeam = Cast<IGenericTeamAgentInterface>(OtherActor);
    if (OtherTeam)
    {
        if (OtherTeam->GetGenericTeamId() == 1)
        {
            TeamOneInfluencerCount++;
        }
        else if (OtherTeam->GetGenericTeamId() == 2)
        {
            TeamTwoInfluencerCount++;
        }

        UpdateTeamWeight();
    }
}

void AStormCore::InfluenceLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    IGenericTeamAgentInterface* OtherTeam = Cast<IGenericTeamAgentInterface>(OtherActor);
    if (OtherTeam)
    {
        if (OtherTeam->GetGenericTeamId().GetId() == 1)
        {
            TeamOneInfluencerCount = FMath::Max(TeamOneInfluencerCount - 1, 0);
        }
        else if (OtherTeam->GetGenericTeamId().GetId() == 2)
        {
            TeamTwoInfluencerCount = FMath::Max(TeamTwoInfluencerCount - 1, 0);
        }

        UpdateTeamWeight();
    }
}

void AStormCore::UpdateTeamWeight()
{
    if (TeamOneInfluencerCount == TeamTwoInfluencerCount)
    {
        TeamWeight = 0.f;
    }
    else
    {
        float TeamOffset = TeamOneInfluencerCount - TeamTwoInfluencerCount;
        float TeamTotal  = TeamOneInfluencerCount + TeamTwoInfluencerCount;
        TeamWeight       = TeamOffset / TeamTotal;
    }

    OnTeamInfluencerCountUpdate.Broadcast(TeamOneInfluencerCount, TeamTwoInfluencerCount);

    UpdateGoal();
}

void AStormCore::UpdateGoal()
{
    if (!HasAuthority()) return;
    if (!OwnerAIController) return;
    if (!GetCharacterMovement()) return;

    if (TeamWeight > 0)
    {
        OwnerAIController->MoveToActor(TeamOneGoal);
    }
    else
    {
        OwnerAIController->MoveToActor(TeamTwoGoal);
    }

    float Speed = MaxMoveSpeed * FMath::Abs(TeamWeight);

    GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AStormCore::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, InfluenceRadius))
    {
        InfluenceRange->SetSphereRadius(InfluenceRadius);
        FVector DecalSize               = GroundDecalComponent->DecalSize;
        GroundDecalComponent->DecalSize = FVector(DecalSize.X, InfluenceRadius, InfluenceRadius);
    }
}

void AStormCore::OnRep_CoreToCapture()
{
    if (CoreToCapture)
    {
        CaptureCore();
    }
}

void AStormCore::GoalReached(int WiningTeam)
{
    UE_LOG(LogTemp, Warning, TEXT("AStormCore::GoalReached() Team: %d"), WiningTeam);

    OnGoalReached.Broadcast(this, WiningTeam);

    if (!HasAuthority()) return;

    MaxMoveSpeed = 0.f;

    CoreToCapture = WiningTeam == 1 ? TeamTwoCore : TeamOneCore; // Replicate CoreToCapture

    CaptureCore();
}

void AStormCore::CaptureCore()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance || !AM_Expand || !CoreToCapture)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] AStormCore::CaptureCore() Error"), *GetName());
        return;
    }

    const float ExpandDuration = AnimInstance->Montage_Play(AM_Expand);

    CoreCaptureSpeed = FVector::Distance(GetMesh()->GetComponentLocation(), CoreToCapture->GetActorLocation()) / ExpandDuration;

    CoreToCapture->SetActorEnableCollision(false);
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::ExpandFinished, ExpandDuration);
}

void AStormCore::ExpandFinished()
{
    if (CoreToCapture)
    {
        CoreToCapture->SetActorLocation(GetMesh()->GetComponentLocation());
        CoreToCapture->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "root");
    }

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(AM_Capture);
    }
}

float AStormCore::GetProgress() const
{
    if (!TeamTwoGoal) return 0.f;

    FVector TeamTwoGoalLoc    = TeamTwoGoal->GetActorLocation();
    FVector VectorFromTeamOne = GetActorLocation() - TeamTwoGoalLoc;
    VectorFromTeamOne.Z       = 0;

    return VectorFromTeamOne.Length() / TravelLength;
}
