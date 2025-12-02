// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryContextMenu.generated.h"


class UButton;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UInventoryContextMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;


private:
	UPROPERTY(meta=(BindWidget))
	UButton* SellButton;

	UPROPERTY(meta=(BindWidget))
	UButton* UseButton;


};
