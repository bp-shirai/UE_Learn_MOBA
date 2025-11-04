// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "Components/SkeletalMeshComponent.h"

#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"


ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));


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


UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACCharacter::ServerSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->ApplyInitialEffects();
}

void ACCharacter::ClientSideInit()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
