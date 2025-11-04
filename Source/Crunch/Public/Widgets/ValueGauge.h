// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"


class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UValueGauge : public UUserWidget
{
	GENERATED_BODY()
	

public:

	virtual void NativePreConstruct() override;
	
	void SetValue(float NewValue, float NewMaxValue);


private:

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* ValueText;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor BarColor;
};
