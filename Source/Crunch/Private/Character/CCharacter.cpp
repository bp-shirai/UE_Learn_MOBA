// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
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
}

void ACCharacter::BeginPlay()
{
    Super::BeginPlay();

    ConfigureOverHeadStatsWidget();
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

void ACCharacter::ConfigureOverHeadStatsWidget()
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
        GetWorldTimerManager().ClearTimer(OverHeadVisibilityUpdateTimerHandle);
        GetWorldTimerManager().SetTimer(OverHeadVisibilityUpdateTimerHandle, this, &ThisClass::UpdateOverHeadVisibility, OverHeadVisibilityCheckUpdateGap, true);
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

void ACCharacter::UpdateOverHeadVisibility()
{
    APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (LocalPlayerPawn)
    {
        const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
        const bool bNewHidden = DistSquared > OverHeadVisibilityRangeSquared;
        OverHeadWidgetComponent->SetHiddenInGame(bNewHidden);
    }
}
