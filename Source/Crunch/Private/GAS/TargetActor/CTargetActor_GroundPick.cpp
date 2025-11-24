// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TargetActor/CTargetActor_GroundPick.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "Engine/OverlapResult.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/DecalComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Math/MathFwd.h"

ACTargetActor_GroundPick::ACTargetActor_GroundPick()
{
    PrimaryActorTick.bCanEverTick = true;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));

    TargetAreaDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetAreaDecal"));
    TargetAreaDecal->SetupAttachment(GetRootComponent());
    TargetAreaDecal->DecalSize = FVector(TargetAreaRadius);
    TargetAreaDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}

void ACTargetActor_GroundPick::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
    {
        SetActorLocation(GetTargetPoint());
    }
}

FVector ACTargetActor_GroundPick::GetTargetPoint() const
{
    if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();

    FHitResult TraceResult;

    FVector ViewLoc;
    FRotator ViewRot;
    PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

    const FVector TraceStart = ViewLoc;
    const FVector TraceEnd   = TraceStart + ViewRot.Vector() * TargetTraceRange;

    GetWorld()->LineTraceSingleByChannel(TraceResult, TraceStart, TraceEnd, TraceChannel);

    if (!TraceResult.bBlockingHit)
    {
        GetWorld()->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), TraceChannel);
    }

    if (!TraceResult.bBlockingHit)
    {
        return GetActorLocation();
    }

    if (bDebug)
    {
        DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 12, FColor::Red);
    }

    return TraceResult.ImpactPoint;
}

void ACTargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
    TargetAreaRadius           = NewRadius;
    TargetAreaDecal->DecalSize = FVector(NewRadius);
}

void ACTargetActor_GroundPick::SetTargetTraceRange(float NewRange)
{
    TargetTraceRange = NewRange;
}

void ACTargetActor_GroundPick::ConfirmTargetingAndContinue()
{
    const AActor* OwningActor = OwningAbility ? OwningAbility->GetAvatarActorFromActorInfo() : nullptr;
    if (!OwningActor)
    {
        TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
        UE_LOG(LogTemp, Error, TEXT("ACTargetActor_GroundPick::ConfirmTargetingAndContinue - No Owning Actor"));
        return;
    }

    TArray<FOverlapResult> OverlapResults;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(OwningActor);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(TargetAreaRadius);

    const FVector Start = GetActorLocation();

    GetWorld()->OverlapMultiByObjectType(OverlapResults, Start, FQuat::Identity, ObjectQueryParams, CollisionShape, Params);

    if (bDebug)
    {
        DrawDebugSphere(GetWorld(), Start, TargetAreaRadius, 12, FColor::Green, false, 2.0f);
    }

    const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwningActor);

    TSet<AActor*> TargetActors;
    for (const FOverlapResult& OverlapResult : OverlapResults)
    {
        const ETeamAttitude::Type TeamAttitude = OwnerTeamInterface ? OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) : ETeamAttitude::Neutral;
        if (TeamAttitude == ETeamAttitude::Hostile && !bShouldTargetEnemy) continue;
        if (TeamAttitude == ETeamAttitude::Friendly && !bShouldTargetFriendly) continue;

        TargetActors.Add(OverlapResult.GetActor());

        if (bDebug)
        {
            FVector Origin, BoxExtent;
            OverlapResult.GetActor()->GetActorBounds(true, Origin, BoxExtent);
            DrawDebugBox(GetWorld(), Origin, BoxExtent, FColor::Red, false, 2.0f);
        }
    }

    FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false);

    FHitResult HitLoc;
    HitLoc.ImpactPoint = GetActorLocation();
    TargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitLoc));

    TargetDataReadyDelegate.Broadcast(TargetData);
}

void ACTargetActor_GroundPick::SetTargetOptions(bool bTargetEnemy, bool bTargetFriendly)
{
    bShouldTargetEnemy    = bTargetEnemy;
    bShouldTargetFriendly = bTargetFriendly;
}
