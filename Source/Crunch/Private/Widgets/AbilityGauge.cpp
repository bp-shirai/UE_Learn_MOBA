// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/AbilityGauge.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"

#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"
#include "GAS/CGameplayTags.h"
#include "Widgets/AbilityToolTip.h"

void UAbilityGauge::NativeConstruct()
{
    Super::NativeConstruct();

    CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

    UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (OwnerASC)
    {
        OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
        OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &ThisClass::AbilitySpecUpdated); // from UCAbilitySystemComponent::Clinet_AbilitySpecLevelUpdated
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &ThisClass::UpgradePointUpdated);
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);

        float UpgradePoint = OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetUpgradePointAttribute());
        FOnAttributeChangeData Data;
        Data.NewValue = UpgradePoint;
        UpgradePointUpdated(Data);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityGauge: %s, OwnerAbilitySystemComponent is null : %s"), *GetName(), *GetOwningPlayer()->GetName());
    }

    OwnerAbilitySystemComponent = OwnerASC;

    WholeNumberFormattingOptions.MaximumFractionalDigits    = 0;
    TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

    float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
    float Cost             = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

    CostText->SetText(FText::AsNumber(Cost));
    CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
    LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
    if (WidgetData)
    {
        Icon->GetDynamicMaterial()->SetTextureParameterValue(IconParamName, WidgetData->Icon.LoadSynchronous());
        CreateToolTipWidget(WidgetData);
    }
}

void UAbilityGauge::CreateToolTipWidget(const FAbilityWidgetData* AbilityWidgetData)
{
    if (!AbilityWidgetData || !AbilityToolTipClass)
        return;

    UAbilityToolTip* InstantiatedToolTip = CreateWidget<UAbilityToolTip>(GetOwningPlayer(), AbilityToolTipClass);
    if (InstantiatedToolTip)
    {
        float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
        float Cost             = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);
        InstantiatedToolTip->SetAbilityInfo(AbilityWidgetData->AbilityName, AbilityWidgetData->Icon.LoadSynchronous(), AbilityWidgetData->Description, CooldownDuration, Cost);

        SetToolTip(InstantiatedToolTip);
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

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
    if (!OwnerAbilitySystemComponent) return nullptr;
    if (!AbilityCDO) return nullptr;

    if (!CachedAbilitySpecHandle.IsValid())
    {
        FGameplayAbilitySpec* FoundAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
        CachedAbilitySpecHandle                = FoundAbilitySpec ? FoundAbilitySpec->Handle : FGameplayAbilitySpecHandle();
        return FoundAbilitySpec;
    }

    return OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(CachedAbilitySpecHandle);
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
    if (AbilitySpec.Ability != AbilityCDO) return;

    bIsAbilityLearned = AbilitySpec.Level > 0;
    LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level);
    UpdateCanCast();

    const float NewCooldownDuration = UCAbilitySystemStatics::GetCooldownDurationFor(AbilitySpec.Ability, OwnerAbilitySystemComponent, AbilitySpec.Level);
    const float NewCost             = UCAbilitySystemStatics::GetManaCostFor(AbilitySpec.Ability, OwnerAbilitySystemComponent, AbilitySpec.Level);

    CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
    CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
    bool bCanCast = bIsAbilityLearned;

    const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    if (AbilitySpec)
    {
        if (!UCAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, OwnerAbilitySystemComponent))
        {
            bCanCast = false;
        }
    }

    Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCanstParamName, bCanCast ? 1 : 0);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
    bool bHasUpgradePoint                   = Data.NewValue > 0;
    const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    if (AbilitySpec)
    {
        if (UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
        {
            Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, 0);
            return;
        }
    }

    Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, bHasUpgradePoint ? 1 : 0);
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
    UpdateCanCast();
}
