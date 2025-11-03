// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "CPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 *
 */
UCLASS()
class CRUNCH_API ACPlayerCharacter : public ACCharacter
{
    GENERATED_BODY()

public:
    ACPlayerCharacter();

    virtual void BeginPlay() override;

    virtual void PawnClientRestart() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "Crunch|View")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleDefaultsOnly, Category = "Crunch|View")
    UCameraComponent* ViewCam;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Input")
    UInputMappingContext* GameplayInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Input")
    UInputAction* Jump_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Input")
    UInputAction* Look_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Input")
    UInputAction* Move_InputAction;

    void HandleLookInput(const FInputActionValue& Value);
    void HandleMoveInput(const FInputActionValue& Value);

    FVector GetLookRightDir() const;
    FVector GetLookFwdDir() const;
    FVector GetMoveFwdDir() const;
};
