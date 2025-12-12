// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameplayMenu.generated.h"

class UTextBlock;
class UButton;
class UImage;

/**
 *
 */
UCLASS()
class CRUNCH_API UGameplayMenu : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    FOnButtonClickedEvent& GetResumeButtonDelegate() { return ResumeButton->OnClicked; }
  
	void SetTitleText(const FString& NewTitle);


private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MenuTitle;

    UPROPERTY(meta = (BindWidget))
    UButton* ResumeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* MainMenuButton;

    UPROPERTY(meta = (BindWidget))
    UButton* QuitGameButton;


    UFUNCTION()
    void BackToMainMenu();

    UFUNCTION()
    void QuitGame();
};
