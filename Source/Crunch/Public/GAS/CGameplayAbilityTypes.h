#pragma once

#include "CoreMinimal.h"

#include "CGameplayAbilityTypes.generated.h"

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