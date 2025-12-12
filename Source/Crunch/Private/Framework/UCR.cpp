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
            // UCR::Delay(CR->GetWorld(), 0.f, Next);
        }
    };

    Next();
    return InvalidCRHandle; // Currently does not return the handle (can be extended if necessary)
}

// ====================================================================
// 汎用アニメーション本体（これが TimelineComponent を完全に殺す）
// ====================================================================
FCRAnimHandle UCR::Anim(UObject* Owner, float Duration, TFunction<void(float, float)> Update, UCurveFloat* Curve)
{
    if (Duration <= 0.f || !Update) return {};

    // 共有タスクを作成（これが天才的なポイント）
    TSharedPtr<FCRAnimTask> Task = MakeShared<FCRAnimTask>();
    Task->Duration               = Duration;
    Task->Curve                  = Curve;
    Task->Update                 = MoveTemp(Update);

    TWeakObjectPtr<UObject> WeakOwner = Owner;
    TWeakPtr<FCRAnimTask> WeakTask    = Task;

    FCRHandle RawHandle = Run(Owner, [WeakOwner, WeakTask](float DeltaTime) mutable -> bool
                              {
        if (!WeakOwner.IsValid() || !WeakTask.IsValid()) return false;

        TSharedPtr<FCRAnimTask> PinnedTask = WeakTask.Pin();
        if (!PinnedTask) return false;

        PinnedTask->Elapsed += DeltaTime;
        float Alpha = FMath::Clamp(PinnedTask->Elapsed / PinnedTask->Duration, 0.f, 1.f);
        float Eased = PinnedTask->Curve 
            ? PinnedTask->Curve->GetFloatValue(PinnedTask->bReversing ? (1.f - Alpha) : Alpha)
            : Alpha;

        PinnedTask->Update(Alpha, Eased);

        if (Alpha >= 1.f)
        {
            if (PinnedTask->OnComplete) PinnedTask->OnComplete();

            if (PinnedTask->bLoop)
            {
                if (PinnedTask->OnLoop) PinnedTask->OnLoop();
                PinnedTask->Reset();
                if (PinnedTask->bAutoReverse) PinnedTask->bReversing = !PinnedTask->bReversing;
                return true;
            }
            return false;
        }
        return true; });

    // ハンドルにタスクを紐づける
    return FCRAnimHandle(Task);
}

// ====================================================================
// 便利高級関数（これで TimelineComponent は完全に不要）
// ====================================================================

// Float 補間
FCRAnimHandle UCR::LerpFloat(UObject* Owner, float& TargetValue, float From, float To, float Duration, UCurveFloat* Curve)
{
    TargetValue = From;
    return Anim(Owner, Duration, [&TargetValue, From, To, Curve](float Alpha, float Eased)
                { TargetValue = FMath::Lerp(From, To, Curve ? Eased : Alpha); }, Curve);
}

FCRAnimHandle UCR::MoveTo(UObject* Owner, USceneComponent* Component, FVector TargetLocation, float Duration, UCurveFloat* Curve)
{
    if (!Component) return {};
    FVector Start = Component->GetRelativeLocation();
    return Anim(Owner, Duration, [Component, Start, TargetLocation](float Alpha, float Eased)
                { Component->SetRelativeLocation(FMath::Lerp(Start, TargetLocation, Eased)); }, Curve);
}

FCRAnimHandle UCR::RotateTo(UObject* Owner, USceneComponent* Component, FRotator TargetRotation, float Duration, UCurveFloat* Curve)
{
    if (!Component) return {};
    FRotator Start = Component->GetRelativeRotation();
    return Anim(Owner, Duration, [Component, Start, TargetRotation](float Alpha, float Eased)
                { Component->SetRelativeRotation(FMath::Lerp(Start, TargetRotation, Eased)); }, Curve);
}

FCRAnimHandle& FCRAnimHandle::OnComplete(TFunction<void()> Callback)
{
    if (auto Task = WeakTask.Pin())
    {
        Task->OnComplete = MoveTemp(Callback);
    }
    return *this;
}

FCRAnimHandle& FCRAnimHandle::OnLoop(TFunction<void()> Callback)
{
    if (auto Task = WeakTask.Pin())
    {
        Task->OnLoop = MoveTemp(Callback);
    }
    return *this;
}

FCRAnimHandle& FCRAnimHandle::SetLoop(bool bEnable)
{
    if (auto Task = WeakTask.Pin())
    {
        Task->bLoop = bEnable;
    }
    return *this;
}

FCRAnimHandle& FCRAnimHandle::SetAutoReverse(bool bEnable)
{
    if (auto Task = WeakTask.Pin())
    {
        Task->bAutoReverse = bEnable;
    }
    return *this;
}

void FCRAnimHandle::Cancel()
{
    if (auto Task = WeakTask.Pin())
    {
        if (Task->OnCancelled) Task->OnCancelled();
        // Runしたタスクは自動で消えるので、ここでは何もしない（安全）
    }
}

bool FCRAnimHandle::IsRunning() const
{
    return WeakTask.IsValid() && WeakTask.Pin().IsValid();
}