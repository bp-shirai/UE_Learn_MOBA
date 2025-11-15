// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UGameplayAbility;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ACPlayerCharacter : public ACCharacter
{
    GENERATED_BODY()

public:
    ACPlayerCharacter();

    virtual void BeginPlay() override;

    virtual void PawnClientRestart() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "View")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleDefaultsOnly, Category = "View")
    UCameraComponent* ViewCam;

#pragma region----- Input --------------------------------------------------
private:
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* GameplayInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* Jump_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* Look_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* Move_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;

    void HandleLookInput(const FInputActionValue& Value);
    void HandleMoveInput(const FInputActionValue& Value);
    void HandleAbilityInput(const FInputActionValue& Value, ECAbilityInputID InputID);

    void SetInputEnableFromPlayerController(bool bEnable);

    FVector GetLookRightDir() const;
    FVector GetLookFwdDir() const;
    FVector GetMoveFwdDir() const;

#pragma endregion
#pragma region----- Death and Respawn ------------------------------------------
private:
    virtual void OnDead() override;
    virtual void OnRespawn() override;

    FRotator DefaultCameraRotation;
    FRotator DefaultPawnRotation;

#pragma endregion
#pragma region----- Stun ------------------------------------------
private:
    virtual void OnStun() override;
    virtual void OnRecoverFromStun() override;

#pragma endregion
};
