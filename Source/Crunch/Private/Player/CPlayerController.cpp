// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, TeamID);
}

void ACPlayerController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
    if (PlayerCharacter)
    {
        PlayerCharacter->ServerSideInit();
        PlayerCharacter->SetGenericTeamId(TeamID);
    }
}

void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
    Super::AcknowledgePossession(NewPawn);

    PlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
    if (PlayerCharacter)
    {
        PlayerCharacter->ClientSideInit();

        // GameplayWidget is created only for the local player.
        if (IsLocalPlayerController())
        {
            SpawnGameplayWidget();
        }
    }
}

void ACPlayerController::SpawnGameplayWidget()
{
    if (GameplayWidget)
    {
        GameplayWidget->RemoveFromParent();
        GameplayWidget = nullptr;
    }

    if (GameplayWidgetClass)
    {
        GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
        if (GameplayWidget)
        {
            GameplayWidget->AddToViewport();
            GameplayWidget->ConfigureAbilities(PlayerCharacter->GetAbilities());
        }
    }
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
    TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
    return TeamID;
}

void ACPlayerController::ToggleShop()
{
    if (HasLocalNetOwner())
    {
        
        if (GameplayWidget)
        {
            GameplayWidget->ToggleShop();
        }
    }
}

void ACPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (Subsystem)
    {
        Subsystem->RemoveMappingContext(UIInputMapping);
        Subsystem->AddMappingContext(UIInputMapping, 1);
    }

    if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComp->BindAction(ShopToggle_InputAction, ETriggerEvent::Triggered, this, &ThisClass::ToggleShop);
        EnhancedInputComp->BindAction(IA_Test, ETriggerEvent::Triggered, this, &ThisClass::Test);
    }
}

void ACPlayerController::Test(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Test"));
}
