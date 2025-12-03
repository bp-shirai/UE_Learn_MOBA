// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/TreeNodeInterface.h"
#include "ItemTreeWidget.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UItemTreeWidget : public UUserWidget
{
    GENERATED_BODY()

private:
    void ClearTree();

    UUserWidget* CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot);
    void CreateConnection(const UUserWidget* From, UUserWidget* To);
    void CreateTree();
    void DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface, UUserWidget* StartingNodeWidget, UCanvasPanelSlot* StartingNodeSlot, int StartingNodeDepth, float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots);

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* RootPanel;

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D NodeSize{60.f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D NodeGap{16.f, 30.f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FLinearColor ConnectionColor{.8f, .8f, .8f, 1.f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    float ConnectionThickness{3.f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D SourcePortLocalPos{0.5f, 0.9f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D DestinationPortLocalPos{0.5f, 0.1f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D SourcePortDirection{0.f, 90.f};

    UPROPERTY(EditDefaultsOnly, Category = "Tree")
    FVector2D DestinationPortDirection{0.f, 90.f};
};