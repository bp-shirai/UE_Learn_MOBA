// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UPA_CharacterDefinition;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


	FORCEINLINE const UPA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }

	void SetSelected(bool bIsSelected);
	
private:
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureParamName{"Icon"};

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationParamName{"Saturation"};

	UPROPERTY()
	const UPA_CharacterDefinition* CharacterDefinition;	

};
