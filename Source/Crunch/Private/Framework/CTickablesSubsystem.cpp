// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CTickablesSubsystem.h"
#include "UObject/Object.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTickablesSubsystem)

void UCTickablesSubsystem::Tick(float DeltaTime)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(CTickablesSubsystem::Tick);

    for (int i = TickableFuncs.Num() - 1; i >= 0; --i)
    {
        TickableFuncs[i].Callback(DeltaTime);

        if (TickableFuncs[i].IsFinished())
        {
            TickableFuncs.RemoveAtSwap(i);
        }
    }

    for (int i = ActiveEasingFuncs.Num() - 1; i >= 0; --i)
    {
        ActiveEasingFuncs[i].Tick(DeltaTime);

        if (ActiveEasingFuncs[i].IsFinished())
        {         
            ActiveEasingFuncs.RemoveAtSwap(i);
        }
    }
}

TStatId UCTickablesSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(CTickablesSubsystem, STATGROUP_Tickables);
}

void UCTickablesSubsystem::AddTickable(UObject* Owner, TFunction<bool(float)> Callback)
{
    if (!Owner || !Callback) return;
    TickableFuncs.Add(FCTickableFunction(Owner, Callback));
}

void FCTickableFunction::Tick(float DeltaTime)
{
    if (!Owner.IsValid())
    {
        bIsActive = false;
        return;
    }

    bIsActive = Callback(DeltaTime);
}

bool FCTickableFunction::IsFinished() const
{
    return bIsActive == false;
}

void UCTickablesSubsystem::PlayEasingFunc(ECEasingType EasingType, float EasingExp, float InPlayRate, const TFunction<void(float)>& Func)
{
    ActiveEasingFuncs.Add(FCActiveEasingFunc(EasingType, EasingExp, InPlayRate, Func));
}
