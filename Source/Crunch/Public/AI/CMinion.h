// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "CMinion.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API ACMinion : public ACCharacter
{
    GENERATED_BODY()

public:
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    bool IsActive() const;
    void Activate();

private:
    void PickSkinBasedOnTeamID();

    virtual void OnRep_TeamID() override;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};
