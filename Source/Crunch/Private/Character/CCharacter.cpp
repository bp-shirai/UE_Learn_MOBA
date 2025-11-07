// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GAS/CGameplayTags.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Widgets/OverHeadStatsGauge.h"

ACCharacter::ACCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSet           = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));

    OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComponent"));
    OverHeadWidgetComponent->SetupAttachment(RootComponent);
    OverHeadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    OverHeadWidgetComponent->SetDrawAtDesiredSize(true);

    BindGASChangeDelegates();
}

void ACCharacter::BeginPlay()
{
    Super::BeginPlay();

    ConfigureOverHeadWidget();
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
    AbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACCharacter::ConfigureOverHeadWidget()
{
    if (IsLocallyControlledByPlayer())
    {
        OverHeadWidgetComponent->SetHiddenInGame(true);
        return;
    }

    if (UOverHeadStatsGauge* OverHeadStatsWidget = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject()))
    {
        OverHeadStatsWidget->ConfigureWithASC(GetAbilitySystemComponent());
        OverHeadWidgetComponent->SetHiddenInGame(false);

        // Widget Visiblity Update Timer
        GetWorldTimerManager().ClearTimer(StatsGaugeVisibilityUpdateTimerHandle);
        GetWorldTimerManager().SetTimer(StatsGaugeVisibilityUpdateTimerHandle, this, &ThisClass::UpdateStatsGaugeVisibility, StatsGaugeVisibilityCheckUpdateGap, true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CCharacter: %s, OverHeadStatsWidget is null"), *GetName());
    }
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
    return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Initialize the AI Controller
    if (NewController && !NewController->IsPlayerController())
    {
        ServerSideInit();
    }
}

void ACCharacter::UpdateStatsGaugeVisibility()
{
    APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (LocalPlayerPawn)
    {
        const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
        const bool bIsEnable    = DistSquared > StatsGaugeVisibilityRangeSquared;
        OverHeadWidgetComponent->SetHiddenInGame(bIsEnable);
    }
}

void ACCharacter::SetStatsGaugeEnable(bool bIsEnable)
{
    GetWorldTimerManager().ClearTimer(StatsGaugeVisibilityUpdateTimerHandle);
    if (bIsEnable)
    {
        ConfigureOverHeadWidget();
    }
    else
    {
        OverHeadWidgetComponent->SetHiddenInGame(bIsEnable);
    }
}

void ACCharacter::BindGASChangeDelegates()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RegisterGameplayTagEvent(Tags::Stats::Dead).AddUObject(this, &ThisClass::DeathTagUpdated);
    }
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount)
    {
        StartDeathSequence();
    }
    else
    {
        Respawn();
    }
}

void ACCharacter::StartDeathSequence()
{
    OnDead();

    SetStatsGaugeEnable(false);
    PlayDeathAnimation();

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACCharacter::Respawn()
{
    OnRespawn();

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
    SetStatsGaugeEnable(true);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->ApplyFullStatEffect();
    }
}

void ACCharacter::PlayDeathAnimation()
{
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
}

void ACCharacter::OnDead(){}

void ACCharacter::OnRespawn(){}
