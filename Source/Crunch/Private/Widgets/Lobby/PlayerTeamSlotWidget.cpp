// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Lobby/PlayerTeamSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Character/PA_CharacterDefinition.h"

void UPlayerTeamSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    UpdateSlot(TEXT(""), nullptr);
}

void UPlayerTeamSlotWidget::UpdateNameText()
{
    if (IsHovered())
    {
        NameText->SetText(FText::FromString(CachedCharacterNameStr));
    }
    else
    {
        NameText->SetText(FText::FromString(CachedPlayerNameStr));
    }
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseEnter(MyGeometry, MouseEvent);
    UpdateNameText();
    PlayAnimationForward(HoveredAnim);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseLeave(MouseEvent);
    UpdateNameText();
    PlayAnimationReverse(HoveredAnim);
}

void UPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UPA_CharacterDefinition* CharacterDefinition)
{
    CachedPlayerNameStr = PlayerName;

    if (CharacterDefinition)
    {
        PlayerCharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(CharacterIconParamName, CharacterDefinition->LoadIcon());
        PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyParamName, 0);
        CachedCharacterNameStr = CharacterDefinition->GetCharacterName();
    }
    else
    {
        PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyParamName, 1);
        CachedCharacterNameStr = TEXT("");
    }

    UpdateNameText();
}
