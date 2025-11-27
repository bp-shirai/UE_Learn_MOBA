// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelGauge.generated.h"

class UImage;
class UTextBlock;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API ULevelGauge : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName PercentMaterialParamName{"Progress"};

    UPROPERTY(meta = (BindWidget))
    UImage* LevelProgressImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LevelText;

    FNumberFormattingOptions NumberFormattingOptions;

    UAbilitySystemComponent* OwnerASC;

    void UpdateGauge(const FOnAttributeChangeData& ChangedData);
};
