// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CBTTask_SendInputToAbilitySystem.h"

#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "BlackboardKeyType_GameplayTag.h"

UCBTTask_SendInputToAbilitySystem::UCBTTask_SendInputToAbilitySystem()
{
    NodeName            = TEXT("Send Input to AbilitySystem");
    bNotifyTick         = false;
    bNotifyTaskFinished = false;
    bCreateNodeInstance = false;
    INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UCBTTask_SendInputToAbilitySystem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* OwnerAIC = OwnerComp.GetAIOwner();
    if (OwnerAIC)
    {
        UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerAIC->GetPawn());
        if (OwnerASC)
        {
            OwnerASC->PressInputID(static_cast<int32>(InputID));
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}

FString UCBTTask_SendInputToAbilitySystem::GetStaticDescription() const
{
    return FString::Format(TEXT("InputID: {0}"), {UEnum::GetValueAsString(InputID)});
}
