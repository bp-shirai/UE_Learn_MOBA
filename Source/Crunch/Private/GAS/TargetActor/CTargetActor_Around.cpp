// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TargetActor/CTargetActor_Around.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Components/SphereComponent.h"
#include "Engine/HitResult.h"
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "GAS/CAbilitySystemStatics.h"

ACTargetActor_Around::ACTargetActor_Around()
{
    RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
    SetRootComponent(RootComp);

    DetectionSphere = CreateDefaultSubobject<USphereComponent>("DetectionSphere");
    DetectionSphere->SetupAttachment(GetRootComponent());
    DetectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ActorInDetectionRange);

    bReplicates                     = true;
    ShouldProduceTargetDataOnServer = true;
}

void ACTargetActor_Around::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamId);
    DOREPLIFETIME(ThisClass, TargetDetectionRadius);
    DOREPLIFETIME(ThisClass, LocalGameplayCueTag);
}

void ACTargetActor_Around::ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag& InLocalGameplayCueTag)
{
    SetGenericTeamId(InTeamId);
    DetectionSphere->SetSphereRadius(DetectionRadius);
    TargetDetectionRadius = DetectionRadius;
    LocalGameplayCueTag   = InLocalGameplayCueTag;
}

void ACTargetActor_Around::ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* AvatarActor = OwningAbility ? OwningAbility->GetAvatarActorFromActorInfo() : nullptr;
    if (!OtherActor) return;
    if (OtherActor == AvatarActor) return;
    if (OtherActor == this) return;
    if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;

    if (HasAuthority())
    {
        FGameplayAbilityTargetDataHandle TargetData;
        FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
        ActorArray->SetActors({OtherActor});
        TargetData.Add(ActorArray);
        TargetDataReadyDelegate.Broadcast(TargetData);
    }

    FHitResult HitResult;
    HitResult.ImpactPoint  = OtherActor->GetActorLocation();
    HitResult.ImpactNormal = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    UCAbilitySystemStatics::SendLocalGameplayCue(OtherActor, HitResult, LocalGameplayCueTag);
}

void ACTargetActor_Around::OnRep_TargetDetectionRadius()
{
    DetectionSphere->SetSphereRadius(TargetDetectionRadius);
}
