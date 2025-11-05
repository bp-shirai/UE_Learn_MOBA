// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/CAnimInstance.h"

#include "Character/CCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/MathFwd.h"

void UCAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwnerCharacter)
    {
        OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
    }
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (OwnerCharacter)
    {
        Speed                 = OwnerMovementComp->Velocity.Length();
        FRotator BodyRot      = OwnerCharacter->GetActorRotation();
        FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, PrevBodyRot);
        PrevBodyRot           = BodyRot;

        YawSpeed         = BodyRotDelta.Yaw / DeltaSeconds;
        SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpSpeed);

        // const FRotator OwnerAimRot = OwnerCharacter->IsPawnControlled()
        //                               ? OwnerCharacter->GetControlRotation()
        //                               : OwnerCharacter->GetBaseAimRotation();

        FRotator ControlRot = OwnerCharacter->GetBaseAimRotation();
        LookRotOffset       = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);
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
