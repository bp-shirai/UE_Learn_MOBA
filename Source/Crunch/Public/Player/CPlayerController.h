// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"


class ACPlayerCharacter;
class UGameplayWidget;


/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Only called on the server
	virtual void OnPossess(APawn* NewPawn) override;

	// Only called on the client, also on the listening server.
	virtual void AcknowledgePossession(APawn* NewPawn) override;

private:

	void SpawnGameplayWidget();


	UPROPERTY()
	ACPlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Crunch|UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

};
