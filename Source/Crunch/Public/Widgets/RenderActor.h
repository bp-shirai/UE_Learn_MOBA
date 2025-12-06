// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

class USceneComponent;
class USceneCaptureComponent2D;

UCLASS()
class CRUNCH_API ARenderActor : public AActor
{
    GENERATED_BODY()

public:
    ARenderActor();
    void SetRenderTarget(UTextureRenderTarget2D* RenderTarget);
    void UpdateRender();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
    USceneComponent* MyRootComponent;

    UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
    USceneCaptureComponent2D* CaptureComponent;
};
