// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "PA_CharacterDefinition.generated.h"

class ACCharacter;
class UAnimInstance;
class USkeletalMesh;
class UGameplayAbility;

/**
 *
 */
UCLASS()
class CRUNCH_API UPA_CharacterDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
    static FPrimaryAssetType GetCharacterDefinitionAssetType();

    FString GetCharacterName() const { return CharacterName; }
    UTexture2D* LoadIcon() const;
    TSubclassOf<ACCharacter> LoadCharacterClass() const;
    TSubclassOf<UAnimInstance> LoadDisplayAnimBP() const;
    USkeletalMesh* LoadDisplayMesh() const;

    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* GetAbilities() const;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    FString CharacterName;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftObjectPtr<UTexture2D> CharacterIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftClassPtr<ACCharacter> CharacterClass;

    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftClassPtr<UAnimInstance> DisplayAnimBP;
};
