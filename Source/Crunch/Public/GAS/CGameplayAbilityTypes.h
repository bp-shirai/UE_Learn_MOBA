#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "CGameplayAbilityTypes.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECAbilityInputID : uint8
{
    None UMETA(DisplayName = "None"),
    BasicAttack UMETA(DisplayName = "Basic Attack"),
    AbilityOne UMETA(DisplayName = "Ability One"),
    AbilityTwo UMETA(DisplayName = "Ability Two"),
    AbilityThree UMETA(DisplayName = "Ability Three"),
    AbilityFour UMETA(DisplayName = "Ability Four"),
    AbilityFive UMETA(DisplayName = "Ability Five"),
    AbilitySix UMETA(DisplayName = "Ability Six"),
    Confirm UMETA(DisplayName = "Confirm"),
    Cancel UMETA(DisplayName = "Cancel"),
};

USTRUCT(BlueprintType)
struct FCGenericDamageEffectDef
{
    GENERATED_BODY()

    FCGenericDamageEffectDef() : DamageEffect(nullptr), PushVelocity(FVector::ZeroVector) {}

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageEffect")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageEffect")
    FVector PushVelocity;
};

USTRUCT(BlueprintType)
struct FCHeroBaseStats : public FTableRowBase
{
    GENERATED_BODY()

    FCHeroBaseStats() : Class{nullptr}, Strength(0.f), Intelligence(0.f), StrengthGrowRate(0.f), IntelligenceGrowRate(0.f), BaseMaxHealth(0.f), BaseMaxMana(0.f), BaseAttack(0.f), BaseArmor(0.f), BaseMoveSpeed(0.f) {}

    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> Class;

    UPROPERTY(EditAnywhere)
    float Strength;

    UPROPERTY(EditAnywhere)
    float Intelligence;

    UPROPERTY(EditAnywhere)
    float StrengthGrowRate;

    UPROPERTY(EditAnywhere)
    float IntelligenceGrowRate;

    UPROPERTY(EditAnywhere)
    float BaseMaxHealth;

    UPROPERTY(EditAnywhere)
    float BaseMaxMana;

    UPROPERTY(EditAnywhere)
    float BaseAttack;

    UPROPERTY(EditAnywhere)
    float BaseArmor;

    UPROPERTY(EditAnywhere)
    float BaseMoveSpeed;
};

USTRUCT(BlueprintType)
struct FCAttributeTagMetadata
{
    GENERATED_BODY()

    // Specify which attribute set the attribute is from
    UPROPERTY(EditAnywhere, Category = "Attribute")
    FGameplayAttribute Attribute;

    // Name for UI display
    UPROPERTY(EditAnywhere, Category = "UI")
    FText DisplayName;

    // Description text for UI display
    UPROPERTY(EditAnywhere, Category = "UI", meta = (MultiLine = true))
    FText Description;

    // Icon texture for UI display
    UPROPERTY(EditAnywhere, Category = "UI")
    TSoftObjectPtr<UTexture2D> Icon;
};

UENUM()
enum class ECConfirmType : uint8
{
    Yes,
    No,
};

UENUM()
enum class ECValidType : uint8
{
    Valid,
    Invalid,
};

UENUM()
enum class ECSuccessType : uint8
{
    Successful,
    Failed,
};

// UENUM()
// enum class ECCountdownActionInput : uint8
// {
// 	Start,
// 	Cancel,
// };

// UENUM()
// enum class ECCountdownActionOutput : uint8
// {
// 	Updated,
// 	Completed,
// 	Canceled,
// };

UENUM(BlueprintType)
enum class ECGameDifficulty : uint8
{
    Easy,
    Normal,
    Hard,
    VeryHard,
};

UENUM(BlueprintType)
enum class ECInputMode : uint8
{
    GameOnly,
    UIOnly,
};