#include "GAS/CProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SceneComponent.h"
#include "Engine/HitResult.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Logging/LogVerbosity.h"
#include "Net/UnrealNetwork.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/UObjectBaseUtility.h"

ACProjectileActor::ACProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates                   = true;

    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
    SetRootComponent(RootComp);
}

void ACProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle)
{
    Target          = InTarget;
    ProjectileSpeed = InSpeed;

    FRotator OwnerViewRot = GetActorRotation();
    SetGenericTeamId(InTeamId);

    if (GetOwner())
    {
        FVector OwnerViewLoc;
        GetOwner()->GetActorEyesViewPoint(OwnerViewLoc, OwnerViewRot);
    }

    MoveDir             = OwnerViewRot.Vector();
    HitEffectSpecHandle = InHitEffectHandle;

    float TravelMaxTime = InMaxDistance / InSpeed;
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ThisClass::TravelMaxDIstanceReached, TravelMaxTime);
}

void ACProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamId);
    DOREPLIFETIME(ThisClass, MoveDir);
    DOREPLIFETIME(ThisClass, ProjectileSpeed);
}

void ACProjectileActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    TeamId = NewTeamID;
}

void ACProjectileActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    // Super::NotifyActorBeginOverlap(OtherActor);

    if (!OtherActor || OtherActor == GetOwner()) return;
    if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;
 
    UE_LOG(LogTemp, Warning, TEXT("[%s] : NotifyActorBeginOverlap : %s"), *GetName(), *GetNameSafe(OtherActor));
 
    UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
    if (IsValid(OtherASC))
    {
        if (HasAuthority() && HitEffectSpecHandle.IsValid())
        {
            OtherASC->BP_ApplyGameplayEffectSpecToSelf(HitEffectSpecHandle);
            GetWorldTimerManager().ClearTimer(ShootTimerHandle);
        }

        FHitResult HitResult;
        HitResult.ImpactPoint  = GetActorLocation();
        HitResult.ImpactNormal = GetActorForwardVector();
        SendLocalGameplayCue(OtherActor, HitResult);

        Destroy();
    }
}

void ACProjectileActor::BeginPlay()
{
    Super::BeginPlay();
}

void ACProjectileActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        if (Target)
        {
            MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        }
    }

    SetActorLocation(GetActorLocation() + MoveDir * ProjectileSpeed * DeltaTime);
}

void ACProjectileActor::TravelMaxDIstanceReached()
{
    Destroy();
}

void ACProjectileActor::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult)
{
    FGameplayCueParameters Params;
    Params.Location = HitResult.ImpactPoint;
    Params.Normal   = HitResult.ImpactNormal;

    UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitGameplayCueTag, EGameplayCueEvent::Executed, Params);
}
