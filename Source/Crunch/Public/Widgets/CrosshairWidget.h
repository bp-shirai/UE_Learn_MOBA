// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CrosshairWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class UCrosshairWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "View")
    FLinearColor HasTargetColor = FLinearColor::Red;

    UPROPERTY(EditDefaultsOnly, Category = "View")
    FLinearColor NoTargetColor = FLinearColor::White;

    UPROPERTY(meta = (BindWidget))
    class UImage* CrosshairImage;

    void CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount);

    UPROPERTY()
    class UCanvasPanelSlot* CrosshairCanvasPanelSlot;

    UPROPERTY()
    class APlayerController* CachedPlayerController;

    void UpdateCrosshairPosition();

    UPROPERTY()
    const AActor* AimTarget;

    void TargetUpdated(const struct FGameplayEventData* EventData);
};
