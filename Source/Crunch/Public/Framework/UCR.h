// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Curves/CurveFloat.h"
#include "UCR.generated.h"

using FCRHandle                     = int32;
constexpr FCRHandle InvalidCRHandle = INDEX_NONE;

USTRUCT()
struct FCRTask
{
    GENERATED_BODY()

    FCRTask() = default;
    FCRTask(UObject* InOwner, TFunction<bool(float)> InCallback)
        : Owner(InOwner), Callback(MoveTemp(InCallback)) {}

    TWeakObjectPtr<UObject> Owner;
    TFunction<bool(float)> Callback;
    bool bActive = true;

    void Tick(float DeltaTime)
    {
        if (!Owner.IsValid())
        {
            bActive = false;
            return;
        }
        bActive = Callback(DeltaTime);
    }

    bool IsFinished() const { return !bActive || !Owner.IsValid(); }
};

// アニメーション制御用拡張構造体
struct FCRAnimTask
{
    float Duration     = 0.f;
    float Elapsed      = 0.f;
    UCurveFloat* Curve = nullptr;
    bool bLoop         = false;
    bool bAutoReverse  = false;
    bool bReversing    = false;

    TFunction<void(float Alpha, float EasedAlpha)> Update; // 毎フレーム呼ばれる
    TFunction<void()> OnComplete;
    TFunction<void()> OnCancelled;
    TFunction<void()> OnLoop;

    // 内部用
    void Reset()
    {
        Elapsed    = 0.f;
        bReversing = false;
    }
};

// ハンドルにアニメーション機能を持たせるためのラッパー（ユーザーは意識しない）
struct FCRAnimHandle
{
  friend class UCR;

private:
    TWeakPtr<FCRAnimTask> WeakTask;   // これが鍵！

    explicit FCRAnimHandle(TSharedPtr<FCRAnimTask> InTask)
        : WeakTask(InTask) {}

public:
    FCRAnimHandle() = default;

    // チェーン可能な最強API
    FCRAnimHandle& OnComplete(TFunction<void()> Callback);
    FCRAnimHandle& OnCancelled(TFunction<void()> Callback);
    FCRAnimHandle& OnLoop(TFunction<void()> Callback);
    FCRAnimHandle& SetLoop(bool bEnable = true);
    FCRAnimHandle& SetAutoReverse(bool bEnable = true);

    void Cancel();
    bool IsRunning() const;
    operator bool() const { return WeakTask.IsValid(); }
};

/**
 *
 */
UCLASS()
class CRUNCH_API UCR : public UTickableWorldSubsystem
{
    GENERATED_BODY()

    TArray<FCRTask> Tasks;

public:
    static UCR* Get(UObject* WorldContextObject) { return WorldContextObject->GetWorld()->GetSubsystem<UCR>(); }

    // Basic Registration
    FCRHandle Run(UObject* Owner, TFunction<bool(float)> Task);

    // Cancel
    void Cancel(FCRHandle Handle);

    bool IsRunning(FCRHandle Handle) const;

    // Luxury utility
    static void Delay(UWorld* World, float Seconds, TFunction<void()> OnComplete = nullptr);
    static FCRHandle Sequence(UWorld* World, const TArray<TFunction<void()>>& Steps);
    static TFunction<bool(float)> Wait(float Seconds);

    // Subsystem interface
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UCR, STATGROUP_Tickables); }
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    // 1行でイージング付きアニメーション（Curve不要でもOK）
    FCRAnimHandle Anim(UObject* Owner, float Duration, TFunction<void(float Alpha, float EasedAlpha)> Update, UCurveFloat* Curve = nullptr);

    // 便利オーバーロード
    FCRAnimHandle LerpFloat(UObject* Owner, float& TargetValue, float From, float To, float Duration, UCurveFloat* Curve = nullptr);
    FCRAnimHandle MoveTo(UObject* Owner, USceneComponent* Component, FVector TargetLocation, float Duration, UCurveFloat* Curve = nullptr);
    FCRAnimHandle RotateTo(UObject* Owner, USceneComponent* Component, FRotator TargetRotation, float Duration, UCurveFloat* Curve = nullptr);
    // FCRHandle ScaleTo(UObject* Owner, USceneComponent* Component, FVector TargetScale, float Duration, UCurveFloat* Curve = nullptr);
};
