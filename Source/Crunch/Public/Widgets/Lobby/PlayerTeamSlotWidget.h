// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UWidgetAnimation;
class UPA_CharacterDefinition;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UPlayerTeamSlotWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;

    void UpdateSlot(const FString& PlayerName, const UPA_CharacterDefinition* CharacterDefinition);

private:
    UPROPERTY(meta = (BindWidget))
    UImage* PlayerCharacterIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HoveredAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName CharacterIconParamName{"Icon"};

    UPROPERTY(EditDefaultsOnly, Category = "Visual")
    FName CharacterEmptyParamName{"Empty"};

    FString CachedPlayerNameStr;
    FString CachedCharacterNameStr;

    void UpdateNameText();
};
