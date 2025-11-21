// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/AbilityGauge.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GAS/CAbilitySystemStatics.h"
#include "Internationalization/Text.h"
#include "Logging/LogVerbosity.h"

void UAbilityGauge::NativeConstruct()
{
    Super::NativeConstruct();

    CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

    UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (OwnerASC)
    {
        OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityGauge: %s, ASC is null : %s"), *GetName(), *GetOwningPlayer()->GetName());
    }

    WholeNumberFormattingOptions.MaximumFractionalDigits    = 0;
    TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

    float CooldownDuration = UCAbilitySystemStatics::GetStaticColldownDurationForAbility(AbilityCDO);
    float Cost             = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

    CostText->SetText(FText::AsNumber(Cost));
    CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
    if (WidgetData)
    {
        Icon->GetDynamicMaterial()->SetTextureParameterValue(IconParamName, WidgetData->Icon.LoadSynchronous());
    }
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
    if (Ability && Ability->GetClass()->GetDefaultObject() == AbilityCDO)
    {
        float TimeRemaining    = 0.f;
        float CooldownDuration = 0.f;

        Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), TimeRemaining, CooldownDuration);

        StartCooldown(TimeRemaining, CooldownDuration);
    }
}

void UAbilityGauge::StartCooldown(float TimeRemaining, float CooldownDuration)
{
    CachedCooldownDuration = CooldownDuration;
    CachedTimeRemaining    = TimeRemaining;

    CooldownCounterText->SetVisibility(ESlateVisibility::Visible);

    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ThisClass::CooldownFinished, TimeRemaining);
    GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle, this, &ThisClass::CooldownUpdate, CooldownUpdateInterval, true, 0.f);
}

void UAbilityGauge::CooldownFinished()
{
    CachedCooldownDuration = CachedTimeRemaining = 0.f;
    CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
    GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
    Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f);
}

void UAbilityGauge::CooldownUpdate()
{
    CachedTimeRemaining -= CooldownUpdateInterval;

    const FNumberFormattingOptions* FormattingOptions = CachedTimeRemaining > 1 ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;

    CooldownCounterText->SetText(FText::AsNumber(CachedTimeRemaining, FormattingOptions));

    Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f - CachedTimeRemaining / CachedCooldownDuration);
}
