// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RenderActorWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Engine/World.h"
#include "Widgets/RenderActor.h"

void URenderActorWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (RenderSizeBox)
    {
        RenderSizeBox->SetWidthOverride(RenderSize.X);
        RenderSizeBox->SetHeightOverride(RenderSize.Y);
    }
}

void URenderActorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    FrameRate = FMath::Max(FrameRate, 1);

    SpawnRenderActor();
    ConfigureRenderActor();
    BeginRenderCapture();
}

void URenderActorWidget::ConfigureRenderActor()
{
    if (!GetRenderActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] : No Render Actor, Nothing will be rendered"), *GetName());
        return;
    }

    RenderTarget = NewObject<UTextureRenderTarget2D>();
    RenderTarget->InitAutoFormat(static_cast<int>(RenderSize.X), static_cast<int>(RenderSize.Y));
    RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
    GetRenderActor()->SetRenderTarget(RenderTarget);

    UMaterialInstanceDynamic* DMI_DisplayImage = DisplayImage->GetDynamicMaterial();
    if (DMI_DisplayImage)
    {
        DMI_DisplayImage->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
    }
}

void URenderActorWidget::BeginRenderCapture()
{
    RenderTickInterval = 1.f / static_cast<float>(FrameRate);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(RenderTimerHandle, this, &ThisClass::UpdateRender, RenderTickInterval, true);
    }
}

void URenderActorWidget::UpdateRender()
{
    if (GetRenderActor())
    {
        GetRenderActor()->UpdateRender();
    }
}

void URenderActorWidget::StopRenderCapture()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(RenderTimerHandle);
    }
}

void URenderActorWidget::BeginDestroy()
{
    StopRenderCapture();
    Super::BeginDestroy();
}
