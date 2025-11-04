// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"


class UValueGauge;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


private:

	UPROPERTY(meta=(BindWidget))
	UValueGauge* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UValueGauge* ManaBar;
};
