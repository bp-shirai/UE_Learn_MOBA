// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"

class ACPlayerCharacter;
class UGameplayWidget;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Only called on the server
    virtual void OnPossess(APawn* NewPawn) override;

    // Only called on the client, also on the listening server.
    virtual void AcknowledgePossession(APawn* NewPawn) override;

    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    virtual FGenericTeamId GetGenericTeamId() const override;

    virtual void SetupInputComponent() override;

    void MatchFinished(AActor* ViewTarget, int WiningTeam);

private:
    UFUNCTION(Client, Reliable)
    void Client_MatchFinished(AActor* ViewTarget, int WiningTeam);

    void SpawnGameplayWidget();

    void ShowWinLoseState();

    UPROPERTY(EditDefaultsOnly, Category = "View")
    float MatchFinishViewBlendTime{2.f};

    UPROPERTY()
    ACPlayerCharacter* PlayerCharacter;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGameplayWidget> GameplayWidgetClass;

    UPROPERTY()
    UGameplayWidget* GameplayWidget;

    UPROPERTY(Replicated)
    FGenericTeamId TeamID;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* UIInputMapping;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_ToggleShop;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_ToggleGameplayMenu;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Test;

public:
    UFUNCTION()
    void ToggleShop();

    UFUNCTION()
    void ToggleGameplayMenu();

    UFUNCTION()
    void Test(const FInputActionValue& Value);
};
