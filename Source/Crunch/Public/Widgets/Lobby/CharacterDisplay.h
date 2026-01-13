// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UPA_CharacterDefinition;

UCLASS(Abstract)
class CRUNCH_API ACharacterDisplay : public AActor
{
    GENERATED_BODY()

public:
    ACharacterDisplay();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Character Display")
    class USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditDefaultsOnly, Category = "Character Display")
    class UCameraComponent* ViewCamera;
};
