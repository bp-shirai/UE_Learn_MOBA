// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CrosshairWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SlateWrapperTypes.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"
#include "Math/MathFwd.h"

void UCrosshairWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CrosshairImage->SetVisibility(ESlateVisibility::Hidden);

    UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (OwnerASC)
    {
        OwnerASC->RegisterGameplayTagEvent(Tags::Stats::Crosshair).AddUObject(this, &ThisClass::CrosshairTagUpdated);
        OwnerASC->GenericGameplayEventCallbacks.Add(Tags::Ability::Target_Updated).AddUObject(this, &ThisClass::TargetUpdated);
    }

    CachedPlayerController = GetOwningPlayer();

    CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
    if (!CrosshairCanvasPanelSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("Crosshair widget need to be parented under a canvas panel to place itself properly"));
    }
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible)
    {
        UpdateCrosshairPosition();
    }
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
    if (!CachedPlayerController || !CrosshairCanvasPanelSlot) return;

    float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
    int32 SizeX, SizeY;
    CachedPlayerController->GetViewportSize(SizeX, SizeY);

    if (!AimTarget)
    {
        FVector2D ViewportSize = FVector2D(SizeX, SizeY);
        CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
        return;
    }

    FVector2D TargetScreenPosition;
    CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
    if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX && TargetScreenPosition.Y > 0 && TargetScreenPosition.Y < SizeY)
    {
        CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
    }
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
    CrosshairImage->SetVisibility(NewCount ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::TargetUpdated(const struct FGameplayEventData* EventData)
{
    AimTarget = EventData->Target;
    CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}
