// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityToolTip.generated.h"

/**
 * 
 */
UCLASS()
class UAbilityToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityTexture, const FText& AbilityDescription, float AbilityCooldown, float AbilityCost);

private:	
	UPROPERTY(meta=(BindWidget))	
	class UTextBlock* AbilityNameText;

	UPROPERTY(meta=(BindWidget))	
	class UImage* AbilityIcon;

	UPROPERTY(meta=(BindWidget))	
	class UTextBlock* AbilityDescriptionText;

	UPROPERTY(meta=(BindWidget))	
	class UTextBlock* AbilityCooldownText;

	UPROPERTY(meta=(BindWidget))	
	class UTextBlock* AbilityCostText;
};
