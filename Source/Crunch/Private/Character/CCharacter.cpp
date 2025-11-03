// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "Components/SkeletalMeshComponent.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}


void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

