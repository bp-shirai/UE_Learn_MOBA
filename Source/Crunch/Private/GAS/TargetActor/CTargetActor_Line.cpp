#include "GAS/TargetActor/CTargetActor_Line.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Crunch.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

#include "GenericTeamAgentInterface.h"

#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"

ACTargetActor_Line::ACTargetActor_Line()
{
    RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
    SetRootComponent(RootComp);

    TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("TargetEndDetectionSphere");
    TargetEndDetectionSphere->SetupAttachment(GetRootComponent());
    TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);

    LaserVFX = CreateDefaultSubobject<UNiagaraComponent>("LaserVFX");
    LaserVFX->SetupAttachment(GetRootComponent());

    PrimaryActorTick.bCanEverTick   = true;
    bReplicates                     = true;
    ShouldProduceTargetDataOnServer = true;

    AvatarActor = nullptr;
}

void ACTargetActor_Line::ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius, float NewTargetingInterval, FGenericTeamId OwnerTeamId, bool bShouldDrawDebug)
{
    TargetRange             = NewTargetRange;
    DetectionCylinderRadius = NewDetectionCylinderRadius;
    TargetingInterval       = NewTargetingInterval;
    SetGenericTeamId(OwnerTeamId);
    bDrawDebug = bShouldDrawDebug;
}

void ACTargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ThisClass, TeamId);
    DOREPLIFETIME(ThisClass, TargetRange);
    DOREPLIFETIME(ThisClass, DetectionCylinderRadius);
    DOREPLIFETIME(ThisClass, AvatarActor);
}

void ACTargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
    Super::StartTargeting(Ability);
    if (!OwningAbility) return;

    AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();

    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(PeriodicalTargetingTimer, this, &ThisClass::DoTargetCheckAndReport, TargetingInterval, true, TargetingInterval / 2.f);
    }
}

void ACTargetActor_Line::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateTargetTrace();
}

void ACTargetActor_Line::BeginDestroy()
{
    if (GetWorld() && PeriodicalTargetingTimer.IsValid())
    {
        GetWorldTimerManager().ClearTimer(PeriodicalTargetingTimer);
    }

    Super::BeginDestroy();
}

void ACTargetActor_Line::DoTargetCheckAndReport()
{
    if (!HasAuthority()) return;

    TSet<AActor*> OverlappingActorSet;
    TargetEndDetectionSphere->GetOverlappingActors(OverlappingActorSet);

    TArray<TWeakObjectPtr<AActor>> OverlappingActors;
    for (AActor* Actor : OverlappingActorSet)
    {
        if (ShouldReportActorAsTarget(Actor))
        {
            OverlappingActors.Add(Actor);
        }
    }

    FGameplayAbilityTargetDataHandle TargetData;
    FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
    ActorArray->SetActors(OverlappingActors);
    TargetData.Add(ActorArray);

    TargetDataReadyDelegate.Broadcast(TargetData);
}

void ACTargetActor_Line::UpdateTargetTrace()
{
    const FVector ActorLocation = GetActorLocation();
    FVector ViewLocation        = ActorLocation;
    FRotator ViewRotation       = GetActorRotation();
    if (AvatarActor)
    {
        AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
    }

    const FVector LookEndPoint  = ViewLocation + (ViewRotation.Vector() * 100000.f);
    const FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, LookEndPoint);
    SetActorRotation(LookRotation);

    const FVector SweepEndLocation = ActorLocation + (LookRotation.Vector() * TargetRange);

    TArray<FHitResult> HitResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(AvatarActor);

    FCollisionResponseParams ResponseParam(ECR_Overlap);
    GetWorld()->SweepMultiByChannel(HitResults, ActorLocation, SweepEndLocation, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(DetectionCylinderRadius), QueryParams, ResponseParam);

    FVector LineEndLocation = SweepEndLocation;
    float LineLength        = TargetRange;
    for (FHitResult HitResult : HitResults)
    {
        if (AActor* HitActor = HitResult.GetActor())
        {
            if (GetTeamAttitudeTowards(*HitActor) != ETeamAttitude::Friendly)
            {
                LineEndLocation = HitResult.ImpactPoint;
                LineLength      = FVector::Distance(GetActorLocation(), LineEndLocation);
                break;
            }
        }
    }

    TargetEndDetectionSphere->SetWorldLocation(LineEndLocation);
    if (LaserVFX)
    {
        LaserVFX->SetVariableFloat(ParamName_LaserFXLength, LineLength / 100.f);
    }
}

bool ACTargetActor_Line::ShouldReportActorAsTarget(const AActor* ActorToCheck) const
{
    if (!ActorToCheck) return false;
    if (ActorToCheck == AvatarActor) return false;
    if (ActorToCheck == this) return false;
    if (ActorToCheck->IsPendingKillPending()) return false;
    if (GetTeamAttitudeTowards(*ActorToCheck) != ETeamAttitude::Hostile) return false;

    return true;
}

void ACTargetActor_Line::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    TeamId = NewTeamID;
}
