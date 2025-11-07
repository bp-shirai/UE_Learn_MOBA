// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"


class ACPlayerCharacter;
class UGameplayWidget;


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





private:

	void SpawnGameplayWidget();


	UPROPERTY()
	ACPlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

};
