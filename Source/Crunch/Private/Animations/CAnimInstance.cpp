// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/CAnimInstance.h"

#include "Character/CCharacter.h"
#include "GAS/CGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "KismetAnimationLibrary.h"

#include "Math/MathFwd.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UCAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwnerCharacter)
    {
        OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
    }

    UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
    if (OwnerASC)
    {
        OwnerASC->RegisterGameplayTagEvent(Tags::Stats::Aim).AddUObject(this, &ThisClass::OwnerAimTagChanged);
    }
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (OwnerCharacter)
    {
        const FVector Velocity = OwnerCharacter->GetVelocity();
        Speed                  = Velocity.Length();

        if (bUseYawSpeed)
        {
            const FRotator BodyRot      = OwnerCharacter->GetActorRotation();
            const FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, PrevBodyRot);
            PrevBodyRot                 = BodyRot;

            YawSpeed         = BodyRotDelta.Yaw / DeltaSeconds;
            SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpSpeed);

            if (bUseLookRotOffset)
            {
                const FRotator ControlRot = OwnerCharacter->GetBaseAimRotation();
                LookRotOffset             = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);

                if (bUseFwdAndRightSpeed)
                {
                    const FVector FwdDir = ControlRot.Vector();
                    FwdSpeed             = Velocity.Dot(FwdDir);
                    RightSpeed           = -Velocity.Dot(FwdDir.Cross(FVector::UpVector));

                    // const FVector ForwardDir = GetActorForwardVector();
                    // const FVector RightDir   = GetActorRightVector();
                    // FwdSpeed   = Velocity.Dot(ForwardDir);
                    // RightSpeed = Velocity.Dot(RightDir);
                }
            }
        }

        // const FRotator OwnerAimRot = OwnerCharacter->IsPawnControlled()
        //                               ? OwnerCharacter->GetControlRotation()
        //                               : OwnerCharacter->GetBaseAimRotation();
    }

    if (OwnerMovementComp)
    {
        bIsJumping = OwnerMovementComp->IsFalling();
        // bShouldMove =  OwnerMovementComp->GetCurrentAcceleration().IsNearlyZero() && Speed > 5.f;
    }
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UCAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    bIsAiming = NewCount != 0;
}

bool UCAnimInstance::ShouldDoFullBody() const
{
    return (GetSpeed() <= 0) && !IsAiming();
}
