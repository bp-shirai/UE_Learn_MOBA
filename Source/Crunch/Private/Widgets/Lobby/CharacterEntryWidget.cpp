// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/CharacterEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Character/PA_CharacterDefinition.h"



void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    CharacterDefinition = Cast<UPA_CharacterDefinition>(ListItemObject);
    if (CharacterDefinition)
    {
        CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureParamName, CharacterDefinition->LoadIcon());
        CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterName()));
    }
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
    CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationParamName, bIsSelected ? 0.f : 1.f);
}
