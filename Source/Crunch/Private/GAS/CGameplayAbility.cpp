// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpec.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GAS/CGameplayTags.h"
#include "UObject/UnrealNames.h"

UCGameplayAbility::UCGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    ActivationBlockedTags.AddTag(Tags::Stats::Stun);
}

bool UCGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
    if (AbilitySpec && AbilitySpec->Level <= 0)
    {
        return false;
    }

    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
    USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
    if (OwnerSkeletalMeshComp)
    {
        return OwnerSkeletalMeshComp->GetAnimInstance();
    }

    // GetCurrentActorInfo()->GetAnimInstance();

    return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam, bool bIgnoreSelf, bool bDrawDebug)
{
    TArray<FHitResult> OutResults;
    TSet<AActor*> HitActors; // Prevent adding the same Actor

    AActor* AvatarActor                                  = GetAvatarActorFromActorInfo();
    const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<const IGenericTeamAgentInterface>(AvatarActor);

    for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
    {
        const FVector StartLoc = TargetData->GetOrigin().GetTranslation();
        const FVector EndLoc   = TargetData->GetEndPoint();

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

        TArray<AActor*> ActorsToIgnore;
        if (bIgnoreSelf)
        {
            ActorsToIgnore.Add(AvatarActor);
        }

        EDrawDebugTrace::Type DrawDebugTraceType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        // UE_LOG(LogTemp, Warning, TEXT("GetHitResultsFromSweepLocationTargetData: DEBUG %s"), bDrawDebug ? TEXT("TRUE") : TEXT("FALSE"));
        //  UE_LOG(LogTemp, Warning, TEXT("DrawDebugTraceType: %d"), DrawDebugTraceType);

        TArray<FHitResult> Results;
        UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTraceType, Results, false);

        for (const FHitResult& Result : Results)
        {
            if (HitActors.Contains(Result.GetActor())) continue;

            // Confirm hostile relationship.
            if (OwnerTeamInterface)
            {
                ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor());
                if (OtherActorTeamAttitude != TargetTeam)
                {
                    continue;
                }
            }

            HitActors.Add(Result.GetActor());
            OutResults.Add(Result);
        }
    }

    return OutResults;
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
    if (!AvatarCharacter.IsValid())
    {
        AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    }

    return AvatarCharacter.Get();
}

void UCGameplayAbility::PushSelf(const FVector& PushVelocity)
{
    ACharacter* OwnerCharacter = GetOwningAvatarCharacter();
    if (OwnerCharacter)
    {
        OwnerCharacter->LaunchCharacter(PushVelocity, true, true);
    }
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVelocity)
{
    if (!IsValid(Target)) return;

    FGameplayEventData EventData;

    FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
    FHitResult HitResult;
    HitResult.ImpactNormal = PushVelocity;
    HitData->HitResult     = HitResult;
    EventData.TargetData.Add(HitData);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, Tags::Ability::Passive::Launch_Activate, EventData);
}

void UCGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level)
{
    const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffectClass, Level);

    FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
    EffectContext.AddHitResult(HitResult);

    EffectSpecHandle.Data->SetContext(EffectContext);

    const FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
    K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, TargetData);
}

void UCGameplayAbility::PushTargets(const TArray<AActor*> Targets, const FVector& PushVelocity)
{
    for (AActor* Target : Targets)
    {
        PushTarget(Target, PushVelocity);
    }
}

void UCGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVelocity)
{
    const TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
    PushTargets(TargetActors, PushVelocity);
}

void UCGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
    {
        OwnerASC->HandleGameplayEvent(EventTag, &EventData);
    }
}

AActor* UCGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
    AActor* OwnerActor = GetAvatarActorFromActorInfo();
    if (!OwnerActor) return nullptr;

    FVector Location;
    FRotator Rotation;
    OwnerActor->GetActorEyesViewPoint(Location, Rotation);

    const FVector AimEnd = Location + Rotation.Vector() * AimDistance;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(OwnerActor);
    FCollisionObjectQueryParams ObjectQueryParams(ECC_Pawn);

    if (ShouldDrawDebug())
    {
        DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 2.f);
    }

    TArray<FHitResult> HitResults;
    if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, ObjectQueryParams, CollisionQueryParams))
    {
        for (const FHitResult& HitResult : HitResults)
        {
            if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude))
            {
                return HitResult.GetActor();
            }
        }
    }

    return nullptr;
}

FGenericTeamId UCGameplayAbility::GetOwnerTeamId() const
{
    if (const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo()))
    {
        return OwnerTeam->GetGenericTeamId();
    }

    return FGenericTeamId::NoTeam;
}

bool UCGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
    if (!OtherActor) return false;

    if (const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo()))
    {
        return OwnerTeam->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
    }

    return false;
}

void UCGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
    if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
    {
        if (!OwnerAnimInstance->Montage_IsPlaying(MontageToPlay))
        {
            OwnerAnimInstance->Montage_Play(MontageToPlay);
        }
    }
}

void UCGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
    if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
    {
        const FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(MontageToStop);
        OwnerAnimInstance->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
    }
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
    AActor* OwnerActor = GetAvatarActorFromActorInfo();
    if (!OwnerActor) return;

    FVector OwnerLocation = OwnerActor->GetActorLocation();
    PushTargetsFromLocation(Targets, OwnerLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetData, float PushSpeed)
{
    TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
    PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& FromLocation, float PushSpeed)
{
    TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
    PushTargetsFromLocation(TargetActors, FromLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed)
{
    for (AActor* Target : Targets)
    {
        FVector PushDirection = Target->GetActorLocation() - FromLocation;
        PushDirection.Z = 0;
        PushDirection.Normalize();

        PushTarget(Target, PushDirection * PushSpeed);
    }
}
