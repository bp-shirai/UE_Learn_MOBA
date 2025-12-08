// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameplayMenu.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameplayMenu::NativeConstruct()
{
    Super::NativeConstruct();

    // ResumeButton->OnClicked.AddDynamic(this, &ThisClass::ResumeButtonClicked);
    MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::BackToMainMenu);
    QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::QuitGame);
}

void UGameplayMenu::ResumeButtonClicked()
{
}

void UGameplayMenu::BackToMainMenu()
{
}

void UGameplayMenu::QuitGame()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);

}
void UGameplayMenu::SetTitleText(const FString& NewTitle)
{
    MenuTitle->SetText(FText::FromString(NewTitle));
}
