// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Lobby/CharacterDisplay.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Character/PA_CharacterDefinition.h"

ACharacterDisplay::ACharacterDisplay()
{
    PrimaryActorTick.bCanEverTick = true;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComp"));

    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
    MeshComp->SetupAttachment(GetRootComponent());

    ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
    ViewCamera->SetupAttachment(GetRootComponent());
}

void ACharacterDisplay::BeginPlay()
{
    Super::BeginPlay();
}

void ACharacterDisplay::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition)
{
	if (!CharacterDefinition) return;

	MeshComp->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComp->SetAnimInstanceClass(CharacterDefinition->LoadDisplayAnimBP());

}
