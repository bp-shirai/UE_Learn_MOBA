// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CMinion.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"



#include "AI/CAIController.h"

void ACMinion::PickSkinBasedOnTeamID()
{
    USkeletalMesh* Skin = SkinMap.FindRef(GetGenericTeamId());
    if (Skin)
    {
        GetMesh()->SetSkeletalMesh(Skin);
    }
}

void ACMinion::OnRep_TeamID()
{
    PickSkinBasedOnTeamID();
}

void ACMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    Super::SetGenericTeamId(NewTeamID);
    PickSkinBasedOnTeamID();
}

bool ACMinion::IsActive() const
{
    return !IsDead();
}

void ACMinion::Activate()
{
    Respawn_Immediately();
}

void ACMinion::SetGoal(AActor* Goal)
{
    if (AAIController* AIC = GetController<AAIController>())
    {
        if (UBlackboardComponent* BBComp = AIC->GetBlackboardComponent())
        {
            BBComp->SetValueAsObject(Goal_KeyName, Goal);
        }
    }
}
