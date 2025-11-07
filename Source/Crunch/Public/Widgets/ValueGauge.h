// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
struct FGameplayAttribute;
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UValueGauge : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;

    void SetAndBoundToGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);
    void SetValue(float NewValue, float NewMaxValue);

private:
    void ValueChanged(const FOnAttributeChangeData& ChangedData);
    void MaxValueChanged(const FOnAttributeChangeData& ChangedData);

    float CachedValue;
    float CachedMaxValue;

    UPROPERTY(VisibleAnywhere, meta = (BindWidget))
    UProgressBar* ProgressBar;

    UPROPERTY(VisibleAnywhere, meta = (BindWidget))
    UTextBlock* ValueText;

    UPROPERTY(EditAnywhere, Category = "Visual")
    FLinearColor BarColor;
    UPROPERTY(EditAnywhere, Category = "Visual")
    float FontSize{22.f};
};
