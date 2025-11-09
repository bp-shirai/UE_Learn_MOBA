// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CBTTask_SendInputToAbilitySystem.h"

#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "BlackboardKeyType_GameplayTag.h"

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
