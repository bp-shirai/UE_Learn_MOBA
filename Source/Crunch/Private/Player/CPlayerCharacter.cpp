// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerCharacter.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GAS/CGameplayTags.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CHeroAttributeSet.h"
#include "GAS/CAbilitySystemStatics.h"

#include "Framework/CTickablesSubsystem.h"
#include "Crunch.h"

ACPlayerCharacter::ACPlayerCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength         = 800.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->ProbeChannel            = ECC_SpringArm;

    ViewCam = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCam"));
    ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    ViewCam->bUsePawnControlRotation = false;

    // bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    // bUseControllerRotationRoll  = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate              = FRotator(0.f, 720.f, 0.f);

    HeroAttributeSet = CreateDefaultSubobject<UCHeroAttributeSet>(TEXT("HeroAttributeSet"));
}

void ACPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultCameraRotation = ViewCam->GetComponentRotation();
    DefaultPawnRotation   = GetActorRotation();
}

void ACPlayerCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();

    APlayerController* OwningPlayerController = GetController<APlayerController>();
    if (OwningPlayerController)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayerController->GetLocalPlayer());
        if (Subsystem)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(GameplayInputMappingContext, 0);
        }
    }
}

void ACPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

    EnhancedInputComp->BindAction(Jump_InputAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
    EnhancedInputComp->BindAction(Look_InputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
    EnhancedInputComp->BindAction(Move_InputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

    for (const auto& [InputID, InputAction] : GameplayAbilityInputActions)
    {
        EnhancedInputComp->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::HandleAbilityInput, InputID);
    }
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();

    AddControllerPitchInput(LookAxis.Y);
    AddControllerYawInput(LookAxis.X);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& Value)
{
    const FVector2D MoveAxis = Value.Get<FVector2D>().GetSafeNormal();

    AddMovementInput(GetMoveFwdDir() * MoveAxis.Y + GetLookRightDir() * MoveAxis.X);
}

FVector ACPlayerCharacter::GetLookRightDir() const
{
    return ViewCam->GetRightVector();
}

FVector ACPlayerCharacter::GetLookFwdDir() const
{
    return ViewCam->GetForwardVector();
}

FVector ACPlayerCharacter::GetMoveFwdDir() const
{
    return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& Value, ECAbilityInputID InputID)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC) return;

    const int32 inputID = static_cast<int32>(InputID);
    const bool bPressed = Value.Get<bool>();
    if (bPressed)
    {
        ASC->AbilityLocalInputPressed(inputID);
    }
    else
    {
        ASC->AbilityLocalInputReleased(inputID);
    }

    if (InputID == ECAbilityInputID::BasicAttack)
    {
        // Server_SendGameplayEventToSelf(Tags::Ability::BasicAttack_Pressed, FGameplayEventData());
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Tags::Ability::BasicAttack_Pressed, FGameplayEventData());
    }
}

void ACPlayerCharacter::OnDead()
{
    SetInputEnableFromPlayerController(false);
}

void ACPlayerCharacter::OnRespawn()
{
    SetInputEnableFromPlayerController(true);

    if (GetController()) GetController()->SetControlRotation(DefaultCameraRotation);
    SetActorRotation(DefaultPawnRotation);
}

void ACPlayerCharacter::OnStun()
{
    SetInputEnableFromPlayerController(false);
}

void ACPlayerCharacter::OnRecoverFromStun()
{
    if (IsDead()) return;
    SetInputEnableFromPlayerController(true);
}

void ACPlayerCharacter::SetInputEnableFromPlayerController(bool bEnable)
{
    if (APlayerController* PC = GetController<APlayerController>())
    {
        if (bEnable)
        {
            EnableInput(PC);
        }
        else
        {
            DisableInput(PC);
        }
    }
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& ACPlayerCharacter::GetAbilities() const
{
    return AbilitySystemComponent->GetAbilities();
}

void ACPlayerCharacter::OnAimStateChanged(bool bIsAiming)
{
    if (GetController() && GetController()->IsLocalPlayerController())
    {
        LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector::ZeroVector);
    }
}

void ACPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{

    GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
    CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp, Goal));
}

void ACPlayerCharacter::TickCameraLocalOffsetLerp(FVector Goal)
{
    FVector CurrentLocalOffset = ViewCam->GetRelativeLocation();
    if (FVector::Dist(CurrentLocalOffset, Goal) < 1.f)
    {
        ViewCam->SetRelativeLocation(Goal);
        return;
    }

    float LerpAlpha        = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f);
    FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, LerpAlpha);
    ViewCam->SetRelativeLocation(NewLocalOffset);

    CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp, Goal));
}
