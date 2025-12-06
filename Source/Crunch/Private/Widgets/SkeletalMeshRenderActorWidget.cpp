// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/SkeletalMeshRenderActorWidget.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"

#include "Widgets/SkeletalMeshRenderActor.h"

void USkeletalMeshRenderActorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
    if (PlayerCharacter && SkeletalMeshRenderActor)
    {
        SkeletalMeshRenderActor->ConfigureSkeletalMesh(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), PlayerCharacter->GetMesh()->GetAnimClass());
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
