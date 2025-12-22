// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TargetActor/CTargetActor_Blackhole.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "GAS/CGameplayTags.h"
#include "UObject/CoreNetTypes.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "PhysicsEngine/RadialForceComponent.h"

ACTargetActor_Blackhole::ACTargetActor_Blackhole()
{
    RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
    SetRootComponent(RootComp);

    DetectionSphere = CreateDefaultSubobject<USphereComponent>("DetectionSphere");
    DetectionSphere->SetupAttachment(GetRootComponent());
    DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ActorInBlackholeRange);
    DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ActorLeftBlackholeRange);

    BlackholeVFX = CreateDefaultSubobject<UParticleSystemComponent>("BlackholeVFX");
    BlackholeVFX->SetupAttachment(GetRootComponent());

    bReplicates                     = true;
    ShouldProduceTargetDataOnServer = true;
    PrimaryActorTick.bCanEverTick   = true;

    // RadialForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");
    // RadialForce->SetupAttachment(GetRootComponent());
    // RadialForce->Radius             = 1000.f;
    // RadialForce->bImpulseVelChange  = false;
    // RadialForce->ForceStrength      = -2000000.f;
    // RadialForce->bIgnoreOwningActor = true;
}

void ACTargetActor_Blackhole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamId);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BlackholeRange, COND_None, REPNOTIFY_Always);
}

void ACTargetActor_Blackhole::ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TryAddTarget(OtherActor);
}

void ACTargetActor_Blackhole::ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    RemoveTarget(OtherActor);
}

void ACTargetActor_Blackhole::ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId)
{
    BlackholeRange = InBlackholeRange;
    DetectionSphere->SetSphereRadius(InBlackholeRange);
    PullSpeed         = InPullSpeed;
    BlackholeDuration = InBlackholeDuration;
    SetGenericTeamId(InTeamId);

    //RadialForce->ForceStrength = -PullSpeed * 100.f;
}

void ACTargetActor_Blackhole::OnRep_BlackholeRange()
{
    DetectionSphere->SetSphereRadius(BlackholeRange);
}

void ACTargetActor_Blackhole::TryAddTarget(AActor* OtherTarget)
{
    if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget)) return;

    if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile) return;

    UNiagaraComponent* VFXComp = nullptr;
    if (NS_BlackholeLink)
    {
        VFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(NS_BlackholeLink, OtherTarget->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        if (VFXComp)
        {
            VFXComp->SetVariablePosition(Name_BlackholeVFXOrigin, BlackholeVFX->GetComponentLocation());
        }
    }

    ActorsInRangeMap.Add(OtherTarget, VFXComp);
}

void ACTargetActor_Blackhole::RemoveTarget(AActor* OtherTarget)
{
    if (!OtherTarget) return;

    if (ActorsInRangeMap.Contains(OtherTarget))
    {
        UNiagaraComponent* VFXComp;
        ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp);
        if (IsValid(VFXComp))
        {
            VFXComp->DestroyComponent();
        }
    }
}

void ACTargetActor_Blackhole::StartTargeting(class UGameplayAbility* Ability)
{
    Super::StartTargeting(Ability);

    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(BlackholeDurationTimer, this, &ThisClass::StopBlackhole, BlackholeDuration);
    }

    //RadialForce->SetAutoActivate(true);
    //RadialForce->AddTickPrerequisiteComponent(RadialForce);
}

void ACTargetActor_Blackhole::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        for (auto& [Target, VFXComp] : ActorsInRangeMap)
        {
            FVector PullDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
            Target->AddActorWorldOffset(PullDir * PullSpeed * DeltaTime);

            if (IsValid(VFXComp))
            {
                VFXComp->SetVariablePosition(Name_BlackholeVFXOrigin, VFXComp->GetComponentLocation());
            }
        }
    }
}

void ACTargetActor_Blackhole::StopBlackhole()
{
    TArray<TWeakObjectPtr<AActor>> FinalTargets;
    for (auto& [Target, VFXComp] : ActorsInRangeMap)
    {
        FinalTargets.Add(Target);
        if (IsValid(VFXComp))
        {
            VFXComp->DestroyComponent();
        }
    }

    FGameplayAbilityTargetDataHandle TargetData;

    FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
    ActorArray->SetActors(FinalTargets);
    TargetData.Add(ActorArray);

    FGameplayAbilityTargetData_SingleTargetHit* BlowupLocation = new FGameplayAbilityTargetData_SingleTargetHit();
    BlowupLocation->HitResult.ImpactPoint                      = GetActorLocation();
    TargetData.Add(BlowupLocation);

    TargetDataReadyDelegate.Broadcast(TargetData);
}

void ACTargetActor_Blackhole::ConfirmTargetingAndContinue()
{
    StopBlackhole();
}

void ACTargetActor_Blackhole::CancelTargeting()
{
    StopBlackhole();
    Super::CancelTargeting();
}
