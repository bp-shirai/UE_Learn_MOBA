// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/TickablesSubsystem.h"
#include "UObject/Object.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TickablesSubsystem)

void UTickablesSubsystem::Tick(float DeltaTime)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TickablesSubsystem::Tick);

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

TStatId UTickablesSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(TickablesSubsystem, STATGROUP_Tickables);
}

void UTickablesSubsystem::AddTickable(UObject* Owner, TFunction<bool(float)> Callback)
{
    if (!Owner || !Callback) return;
    TickableFuncs.Add(FTickableFunction(Owner, Callback));
}

void FTickableFunction::Tick(float DeltaTime)
{
    if (!Owner.IsValid())
    {
        bIsActive = false;
        return;
    }

    bIsActive = Callback(DeltaTime);
}

bool FTickableFunction::IsFinished() const
{
    return bIsActive == false;
}

void UTickablesSubsystem::PlayEasingFunc(ECEasingType EasingType, float EasingExp, float InPlayRate, const TFunction<void(float)>& Func)
{
    ActiveEasingFuncs.Add(FActiveEasingFunc(EasingType, EasingExp, InPlayRate, Func));
}
