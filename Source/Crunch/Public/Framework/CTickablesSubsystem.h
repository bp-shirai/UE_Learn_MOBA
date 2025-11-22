// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "CTickablesSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECEasingType : uint8
{
    /** Simple linear interpolation. */
    Linear,

    /** Simple step interpolation. */
    Step,

    /** Sinusoidal in interpolation. */
    SinusoidalIn,

    /** Sinusoidal out interpolation. */
    SinusoidalOut,

    /** Sinusoidal in/out interpolation. */
    SinusoidalInOut,

    /** Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by BlendExp. */
    EaseIn,

    /** Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by BlendExp. */
    EaseOut,

    /** Smoothly accelerates and decelerates.  Ease amount controlled by BlendExp. */
    EaseInOut,

    /** Easing in using an exponential */
    ExpoIn,

    /** Easing out using an exponential */
    ExpoOut,

    /** Easing in/out using an exponential method */
    ExpoInOut,

    /** Easing is based on a half circle. */
    CircularIn,

    /** Easing is based on an inverted half circle. */
    CircularOut,

    /** Easing is based on two half circles. */
    CircularInOut,

};

USTRUCT()
struct FCTickableFunction
{
    GENERATED_BODY()

    FCTickableFunction() {}
    FCTickableFunction(UObject* InOwner, TFunction<bool(float)> InCallback) : Owner(InOwner), Callback(InCallback) {}

    TWeakObjectPtr<UObject> Owner;
    TFunction<bool(float)> Callback;
    bool bIsActive;

    void Tick(float DeltaTime);
    bool IsFinished() const;
};

USTRUCT()
struct FCActiveEasingFunc
{
    GENERATED_BODY()

    FCActiveEasingFunc() {}

    FCActiveEasingFunc(ECEasingType InEasingType, float InEasingExp, float InPlayRate, TFunction<void(float)> InCallback)
        : EasingType(InEasingType), EasingExp(InEasingExp), PlayRate(InPlayRate), Callback(InCallback) {}

    ECEasingType EasingType = ECEasingType::Linear;

    float EasingExp = 2.0f;

    float PlayRate = 1.0f;

    float Time = 0.0f;

    TFunction<void(float)> Callback;

    void Tick(float DeltaTime)
    {
        Time += (DeltaTime * PlayRate);

        // Clamp
        Time = FMath::Min(Time, 1.0f);

        float CurrentValue = 0.f;
		
        switch (EasingType)
        {
            case ECEasingType::Linear:
                CurrentValue = FMath::Lerp(0.0f, 1.0f, Time);
                break;
            case ECEasingType::EaseIn:
                CurrentValue = FMath::InterpEaseIn(0.0f, 1.0f, Time, EasingExp);
                break;
			case ECEasingType::EaseOut:
				CurrentValue = FMath::InterpEaseOut(0.0f, 1.0f, Time, EasingExp);
				break;
			case ECEasingType::EaseInOut:
				CurrentValue = FMath::InterpEaseInOut(0.0f, 1.0f, Time, EasingExp);
				break;
        }

        Callback(CurrentValue);
    }

    bool IsFinished()
    {
        return Time >= 1.0f;
    }
};

/**
 *
 */
UCLASS()
class CRUNCH_API UCTickablesSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

    TArray<FCTickableFunction> TickableFuncs;

    TArray<FCActiveEasingFunc> ActiveEasingFuncs;

public:
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    void AddTickable(UObject* Owner, TFunction<bool(float)> Callback);

    /* Play easing function with output between 0.0 - 1.0 */
    void PlayEasingFunc(ECEasingType EasingType, float EasingExp, float InPlayRate, const TFunction<void(float)>& Func);
};
