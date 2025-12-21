// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TargetActor/CTargetActor_Blackhole.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/EngineTypes.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "GAS/CGameplayTags.h"
#include "UObject/CoreNetTypes.h"

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
}

void ACTargetActor_Blackhole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamId);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BlackholeRange, COND_None, REPNOTIFY_Always);
}

void ACTargetActor_Blackhole::ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ACTargetActor_Blackhole::ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ACTargetActor_Blackhole::ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId)
{
    BlackholeRange    = InBlackholeRange;
    DetectionSphere->SetSphereRadius(InBlackholeRange);
    PullSpeed         = InPullSpeed;
    BlackholeDuration = InBlackholeDuration;
    SetGenericTeamId(InTeamId);
}

void ACTargetActor_Blackhole::OnRep_BlackholeRange()
{
    DetectionSphere->SetSphereRadius(BlackholeRange);
}
