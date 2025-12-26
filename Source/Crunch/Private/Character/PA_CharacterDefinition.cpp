// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PA_CharacterDefinition.h"

#include "Animation/AnimInstance.h"
#include "Character/CCharacter.h"
#include "Components/SkeletalMeshComponent.h"

UTexture2D* UPA_CharacterDefinition::LoadIcon() const
{
    CharacterIcon.LoadSynchronous();
    if (CharacterIcon.IsValid())
    {
        return CharacterIcon.Get();
    }
    else
    {
        return nullptr;
    }
}

TSubclassOf<ACCharacter> UPA_CharacterDefinition::LoadCharacterClass() const
{
    CharacterClass.LoadSynchronous();
    if (CharacterClass.IsValid())
    {
        return CharacterClass.Get();
    }
    else
    {
        return TSubclassOf<ACCharacter>();
    }
}

TSubclassOf<UAnimInstance> UPA_CharacterDefinition::LoadDisplayAnimBP() const
{
    DisplayAnimBP.LoadSynchronous();
    if (DisplayAnimBP.IsValid())
    {
        return DisplayAnimBP.Get();
    }
    else
    {
        return TSubclassOf<UAnimInstance>();
    }
}

USkeletalMesh* UPA_CharacterDefinition::LoadDisplayMesh() const
{
    TSubclassOf<ACCharacter> LoadedCharacterClass = LoadCharacterClass();
    if (LoadedCharacterClass)
    {
        ACharacter* Character = Cast<ACharacter>(LoadedCharacterClass.GetDefaultObject());
        if (Character)
        {
            return Character->GetMesh()->GetSkeletalMeshAsset();
        }
    }

    return nullptr;
}

FPrimaryAssetId UPA_CharacterDefinition::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
    return FPrimaryAssetType(TEXT("CharacterDefinition"));
}
