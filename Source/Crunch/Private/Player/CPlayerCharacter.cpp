// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"

ACPlayerCharacter::ACPlayerCharacter()
{
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength         = 800.f;
    CameraBoom->bUsePawnControlRotation = true;

    ViewCam = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCam"));
    ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    ViewCam->bUsePawnControlRotation = false;

    // bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    // bUseControllerRotationRoll  = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate              = FRotator(0.f, 720.f, 0.f);
}

void ACPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
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

    for(const auto& Pair : GameplayAbilityInputActions)
    {
        EnhancedInputComp->BindAction(Pair.Value, ETriggerEvent::Triggered, this, &ThisClass::HandleAbilityInput, Pair.Key);
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
}

void ACPlayerCharacter::OnDead()
{
    APlayerController* PC = GetController<APlayerController>();
    if (PC)
    {
        PC->DisableInput(PC);
    }
}

void ACPlayerCharacter::OnRespawn()
{
    APlayerController* PC = GetController<APlayerController>();
    if (PC)
    {
        PC->EnableInput(PC);
    }
}
