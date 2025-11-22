// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "CAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetSpeed() const { return Speed; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE bool IsMoving() const { return Speed != 0.f; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE bool IsNotMoving() const { return Speed == 0.f; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetFwdSpeed() const { return FwdSpeed; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetRightSpeed() const { return RightSpeed; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE bool IsJumping() const { return bIsJumping; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE bool IsOnGround() const { return !bIsJumping; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE bool IsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetLookRotYawOffset() const { return LookRotOffset.Yaw; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    FORCEINLINE float GetLookRotPitchOffset() const { return LookRotOffset.Pitch; }

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    bool ShouldDoFullBody() const;


private:
    UPROPERTY(Transient)
    ACharacter* OwnerCharacter;

    UPROPERTY(Transient)
    UCharacterMovementComponent* OwnerMovementComp;

    float Speed;
    float YawSpeed;
    float SmoothedYawSpeed;
    float FwdSpeed;
    float RightSpeed;
    FRotator PrevBodyRot;
    bool bIsJumping;
    bool bIsAiming;

    FRotator LookRotOffset;

    UPROPERTY(EditAnywhere, Category = "Animation")
    float YawSpeedSmoothLerpSpeed{1.f};

    UPROPERTY(EditAnywhere, Category = "Animation")
    bool bUseYawSpeed{true};
    UPROPERTY(EditAnywhere, Category = "Animation")
    bool bUseLookRotOffset{true};

    UPROPERTY(EditAnywhere, Category = "Animation")
    bool bUseFwdAndRightSpeed{true};

    void OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount);
};
