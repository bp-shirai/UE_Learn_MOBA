// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/SkeletalMeshRenderActorWidget.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Widgets/SkeletalMeshRenderActor.h"
#include "Widgets/RenderActorTargetInterface.h"

void USkeletalMeshRenderActorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
    if (PlayerCharacter && SkeletalMeshRenderActor)
    {
        SkeletalMeshRenderActor->ConfigureSkeletalMesh(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), PlayerCharacter->GetMesh()->GetAnimClass());

        if (USceneCaptureComponent2D* SceneCapture = SkeletalMeshRenderActor->GetCaptureComponent())
        {
            IRenderActorTargetInterface* PlayerCharacterRenderTargetInterface = Cast<IRenderActorTargetInterface>(PlayerCharacter);
            if (PlayerCharacterRenderTargetInterface)
            {
                SceneCapture->SetRelativeLocation(PlayerCharacterRenderTargetInterface->GetCaptureLocalPosition());
                SceneCapture->SetRelativeRotation(PlayerCharacterRenderTargetInterface->GetCaptureLocalRotation());
            }
        }
    }
}

void USkeletalMeshRenderActorWidget::SpawnRenderActor()
{
    if (!SkeletalMeshRenderActorClass) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SkeletalMeshRenderActor                    = World->SpawnActor<ASkeletalMeshRenderActor>(SkeletalMeshRenderActorClass, SpawnParams);
}

ARenderActor* USkeletalMeshRenderActorWidget::GetRenderActor() const
{
    return SkeletalMeshRenderActor;
}
