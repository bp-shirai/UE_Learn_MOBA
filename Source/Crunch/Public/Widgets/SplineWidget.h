// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplineWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API USplineWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget, const FVector2D& InStartPortLocalCoord, const FVector2D& InEndPortLocalCoord, const FVector2D& InStartPortDirection, const FVector2D& InEndPortDirection);

    void SetSplineStyle(const FLinearColor& InColor, float InThickness);

private:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;


    UPROPERTY(EditAnywhere, Category = "Spline")
    FVector2D TestStartPosition;

    UPROPERTY(EditAnywhere, Category = "Spline")
    FVector2D TestEndPosition{100.f, 100.f};

    UPROPERTY(EditAnywhere, Category = "Spline")
    FLinearColor Color{FLinearColor::White};

    UPROPERTY(EditAnywhere, Category = "Spline")
    float Thickness{3.f};

    UPROPERTY()
    const UUserWidget* StartWidget;

    UPROPERTY()
    const UUserWidget* EndWidget;

    FVector2D StartPortLocalCoord;

    FVector2D EndPortLocalCoord;

    UPROPERTY(EditAnywhere, Category = "Spline")
    FVector2D StartPortDirection;

    UPROPERTY(EditAnywhere, Category = "Spline")
    FVector2D EndPortDirection;
};
