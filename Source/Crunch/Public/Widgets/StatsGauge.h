// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "StatsGauge.generated.h"

class UImage;
class UTextBlock;
//class FGameplayAttribute;
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UStatsGauge : public UUserWidget
{
    GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;	

private:
    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttributeText;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;	

	void SetValue(float NewValue);

	FNumberFormattingOptions NumberFormatOptions;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};
