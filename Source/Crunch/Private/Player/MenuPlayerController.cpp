// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/MenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void AMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    SetShowMouseCursor(true);

    if (HasAuthority() && IsLocalPlayerController())
    {
        SpawnWidget();
    }
}

void AMenuPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (IsLocalPlayerController())
    {
        SpawnWidget();
    }
}

void AMenuPlayerController::SpawnWidget()
{
    if (!MenuWidgetClass || MenuWidget) return;

    MenuWidget = CreateWidget<UUserWidget>(this, MenuWidgetClass);
    if (MenuWidget)
    {
        MenuWidget->AddToViewport();
    }
}
