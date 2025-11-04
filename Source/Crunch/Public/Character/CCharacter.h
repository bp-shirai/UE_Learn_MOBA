// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "CCharacter.generated.h"

class UAbilitySystemComponent;
class UCAbilitySystemComponent;
class UAttributeSet;
class UCAttributeSet;

/**
 *
 */

UCLASS(Abstract)
class CRUNCH_API ACCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ACCharacter();
    void ServerSideInit();
    void ClientSideInit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Gameplay Ability System

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
    UPROPERTY(VisibleDefaultsOnly, Category = "Crunch|Gameplay Ability")
    UCAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UCAttributeSet* AttributeSet;

#pragma endregion
};
