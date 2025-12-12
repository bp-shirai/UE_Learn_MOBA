// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RenderActor.h"

#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "UObject/Object.h"

ARenderActor::ARenderActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MyRootComponent = CreateDefaultSubobject<USceneComponent>("MyRootComponent");
    SetRootComponent(MyRootComponent);

    CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("CaptureComponent");
    CaptureComponent->SetupAttachment(MyRootComponent);

    CaptureComponent->bCaptureEveryFrame = false;
    CaptureComponent->FOVAngle           = 30.f;
}

void ARenderActor::BeginPlay()
{
    Super::BeginPlay();

	CaptureComponent->ShowOnlyActorComponents(this);
	
	SetActorLocation({0.f, 0.f, 100000.f});
}



void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
    CaptureComponent->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRender()
{
    if (CaptureComponent)
    {
		CaptureComponent->CaptureScene();
    }
}
