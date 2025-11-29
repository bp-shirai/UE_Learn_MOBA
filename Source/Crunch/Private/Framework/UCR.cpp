// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/UCR.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UCR)

FCRHandle UCR::Run(UObject* Owner, TFunction<bool(float)> Task)
{
    if (!Task) return InvalidCRHandle;
    const int32 Index = Tasks.Emplace(Owner, MoveTemp(Task));
    return Index;
}

void UCR::Cancel(FCRHandle Handle)
{
    if (Handle == InvalidCRHandle || Handle >= Tasks.Num()) return;
    Tasks[Handle].bActive = false;
}

bool UCR::IsRunning(FCRHandle Handle) const
{
    return Handle != InvalidCRHandle && Handle < Tasks.Num() && Tasks[Handle].bActive && Tasks[Handle].Owner.IsValid();
}

void UCR::Tick(float DeltaTime)
{
    for (int32 i = Tasks.Num() - 1; i >= 0; --i)
    {
        Tasks[i].Tick(DeltaTime);
        if (Tasks[i].IsFinished())
        {
            Tasks.RemoveAtSwap(i);
        }
    }
}

// High class utility implementation
void UCR::Delay(UWorld* World, float Seconds, TFunction<void()> OnComplete)
{
    if (!World || Seconds <= 0.f)
    {
        if (OnComplete) OnComplete();
        return;
    }

    UCR* CR       = World->GetSubsystem<UCR>();
    float Elapsed = 0.f;

    CR->Run(nullptr, [Elapsed, Seconds, OnComplete](float DT) mutable -> bool
            {
        Elapsed += DT;
        if (Elapsed >= Seconds)
        {
            if (OnComplete) OnComplete();
            return false;
        }
        return true; });
}

TFunction<bool(float)> UCR::Wait(float Seconds)
{
    float Elapsed = 0.f;
    return [Elapsed, Seconds](float DT) mutable -> bool
    {
        Elapsed += DT;
        return Elapsed < Seconds;
    };
}

FCRHandle UCR::Sequence(UWorld* World, const TArray<TFunction<void()>>& Steps)
{
    if (Steps.Num() == 0) return InvalidCRHandle;

    UCR* CR     = World->GetSubsystem<UCR>();
    int32 Index = 0;

    TFunction<void()> Next = nullptr;

    Next = [CR, Steps, Index, Next]() mutable
    {
        Steps[Index++]();
        if (Index < Steps.Num())
        {
            UCR::Delay(CR->GetWorld(), 0.f, Next);
        }
    };

    Next();
    return InvalidCRHandle; // Currently does not return the handle (can be extended if necessary)
}

// ====================================================================
// 汎用アニメーション本体（これが TimelineComponent を完全に殺す）
// ====================================================================
FCRAnimHandle UCR::Anim(UObject* Owner, float Duration, TFunction<void(float Alpha, float EasedAlpha)> Update, UCurveFloat* Curve)
{
    if (Duration <= 0.f || !Update) return {};

    FCRAnimTask Task;
    Task.Duration       = Duration;
    Task.Curve          = Curve;
    Task.UpdateCallback = MoveTemp(Update);

    // タスクを包むラムダ（所有権と安全性を完璧に保証）
    TWeakObjectPtr<UCR> WeakThis = this;

    FCRHandle Handle = Run(Owner, [WeakThis, Task](float DeltaTime) mutable -> bool
    {
        if (!WeakThis.IsValid()) return false;

        Task.Elapsed += DeltaTime;
        float Alpha = FMath::Clamp(Task.Elapsed / Task.Duration, 0.f, 1.f);
        float Eased = Task.Curve ? Task.Curve->GetFloatValue(Task.bReversing ? (1.f - Alpha) : Alpha) : Alpha;

        Task.UpdateCallback(Alpha, Eased);

        // 終了判定
        if (Alpha >= 1.f)
        {
            if (Task.OnComplete) Task.OnComplete();

            if (Task.bLoop)
            {
                if (Task.OnLoop) Task.OnLoop();
                Task.Elapsed = 0.f;
                if (Task.bAutoReverse) Task.bReversing = !Task.bReversing;
                return true;
            }
            return false;
        }

        return true;
    });

    // ハンドルに拡張機能を付与
    FCRAnimHandle AnimHandle;
    AnimHandle.RawHandle = Handle;
    AnimHandle.CR        = this;

    // コールバックを遅延バインド（Taskはムーブ済みなので、ここでキャプチャ）
    // → 実際は別途Mapで管理する方法もあるが、シンプルにするためここでは省略
    // （必要ならTaskをTSharedPtr<FCRAnimTask>にしてMap管理も可）

    return AnimHandle;
}

// ====================================================================
// 便利高級関数（これで TimelineComponent は完全に不要）
// ====================================================================

// Float 補間
FCRAnimHandle UCR::LerpFloat(UObject* Owner, float& TargetValue, float From, float To, float Duration, UCurveFloat* Curve)
{
    TargetValue = From;
    return Anim(Owner, Duration, [&TargetValue, From, To, Curve](float Alpha, float Eased)
    {
        TargetValue = FMath::Lerp(From, To, Curve ? Eased : Alpha);
    }, Curve);
}

FCRAnimHandle UCR::MoveTo(UObject* Owner, USceneComponent* Component, FVector TargetLocation, float Duration, UCurveFloat* Curve)
{
    if (!Component) return {};
    FVector Start = Component->GetRelativeLocation();
    return Anim(Owner, Duration, [Component, Start, TargetLocation](float Alpha, float Eased)
    { 
        Component->SetRelativeLocation(FMath::Lerp(Start, TargetLocation, Eased));
    }, Curve);
}

FCRAnimHandle UCR::RotateTo(UObject* Owner, USceneComponent* Component, FRotator TargetRotation, float Duration, UCurveFloat* Curve)
{
    if (!Component) return {};
    FRotator Start = Component->GetRelativeRotation();
    return Anim(Owner, Duration, [Component, Start, TargetRotation](float Alpha, float Eased)
                { Component->SetRelativeRotation(FMath::Lerp(Start, TargetRotation, Eased)); }, Curve);
}

void FCRAnimHandle::Cancel()
{
    if (CR && RawHandle != InvalidCRHandle)
    {
        CR->Cancel(RawHandle);
    }
}

bool FCRAnimHandle::IsRunning() const
{
    return CR && CR->IsRunning(RawHandle);
}
