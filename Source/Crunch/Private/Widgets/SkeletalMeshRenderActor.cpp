// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SkeletalMeshRenderActor.h"

#include "Components/SkeletalMeshComponent.h"

ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    MeshComponent->SetupAttachment(GetRootComponent());
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComponent->SetLightingChannels(false, true, false); // Set to the lighting channel dedicated to mesh.
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint)
{
    MeshComponent->SetSkeletalMeshAsset(MeshAsset);
    MeshComponent->SetAnimInstanceClass(AnimBlueprint);
}

void ASkeletalMeshRenderActor::BeginPlay()
{
    Super::BeginPlay();

    MeshComponent->SetVisibleInSceneCaptureOnly(true);
}
