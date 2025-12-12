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
class UCHeroAttributeSet;
class UInventoryComponent;

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
    virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "View")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleDefaultsOnly, Category = "View")
    UCameraComponent* ViewCam;

#pragma region------- Gameplay Ability ---------------------------------------

private:
    virtual void OnAimStateChanged(bool bIsAiming) override;

    UPROPERTY()
    UCHeroAttributeSet* HeroAttributeSet;

#pragma endregion

#pragma region------- Input --------------------------------------------------
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
    UInputAction* LearnAbilityLeader_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* UseInventoryItem_InputAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;

    void HandleLookInput(const FInputActionValue& Value);
    void HandleMoveInput(const FInputActionValue& Value);
    void HandleAbilityInput(const FInputActionValue& Value, ECAbilityInputID InputID);
    void HandleLearnAbilityLeaderDown(const FInputActionValue& Value);
    void HandleLearnAbilityLeaderUp(const FInputActionValue& Value);
    void UseInventoryItem(const FInputActionValue& Value);

    bool bIsLearnAbilityLeaderDown{false};

    void SetInputEnableFromPlayerController(bool bEnable);

    FVector GetLookRightDir() const;
    FVector GetLookFwdDir() const;
    FVector GetMoveFwdDir() const;

#pragma endregion
#pragma region------- Death and Respawn -----------------------------------
private:
    virtual void OnDead() override;
    virtual void OnRespawn() override;

    FRotator DefaultCameraRotation;
    FRotator DefaultPawnRotation;

#pragma endregion
#pragma region------- Stun ------------------------------------------------
private:
    virtual void OnStun() override;
    virtual void OnRecoverFromStun() override;

#pragma endregion
#pragma region------- Camera View ------------------------------------------
private:
    UPROPERTY(EditDefaultsOnly, Category = "View")
    FVector CameraAimLocalOffset;

    UPROPERTY(EditDefaultsOnly, Category = "View")
    float CameraLerpSpeed{20.f};

    FTimerHandle CameraLerpTimerHandle;

    void LerpCameraToLocalOffsetLocation(const FVector& Goal);
    void TickCameraLocalOffsetLerp(FVector Goal);
#pragma endregion

#pragma region------- Inventory ------------------------------------------
private:
    UPROPERTY()
    UInventoryComponent* InventoryComponent;

#pragma endregion
};
