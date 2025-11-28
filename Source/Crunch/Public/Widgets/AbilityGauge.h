// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "AbilityGauge.generated.h"

class UImage;
class UTextBlock;
class UGameplayAbility;
class UTexture2D;
class UAbilitySystemComponent;
struct FGameplayAbilitySpec;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayAbility> AbilityClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
    float CooldownUpdateInterval{0.1f};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName IconParamName{"Icon"};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName CooldownPercentParamName{"Percent"};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName AbilityLevelParamName{"Level"};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName CanCanstParamName{"CanCast"};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName UpgradePointAvailableParamName{"UpgradeAvailable"};

    UPROPERTY(meta = (BindWidget))
    UImage* Icon;

    UPROPERTY(meta = (BindWidget))
    UImage* LevelGauge;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CooldownCounterText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CooldownDurationText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CostText;

    UPROPERTY()
    UGameplayAbility* AbilityCDO;

    void AbilityCommitted(UGameplayAbility* Ability);

    void StartCooldown(float TimeRemaining, float CooldownDuration);

    float CachedCooldownDuration;
    float CachedTimeRemaining;

    FTimerHandle CooldownTimerHandle;
    FTimerHandle CooldownTimerUpdateHandle;

    FNumberFormattingOptions WholeNumberFormattingOptions;
    FNumberFormattingOptions TwoDigitNumberFormattingOptions;

    void CooldownFinished();
    void CooldownUpdate();

    const UAbilitySystemComponent* OwnerAbilitySystemComponent;
    const FGameplayAbilitySpec* CachedAbilitySpec;

    const FGameplayAbilitySpec* GetAbilitySpec();

    bool bIsAbilityLearned{false};

    void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
    void UpdateCanCast();

    void UpgradePointUpdated(const FOnAttributeChangeData& Data);
    void ManaUpdated(const FOnAttributeChangeData& Data);

};
