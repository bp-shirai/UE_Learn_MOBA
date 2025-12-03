// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemTreeWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/SplineWidget.h"

void UItemTreeWidget::ClearTree()
{
    RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot)
{
    if (!Node) return nullptr;

    UUserWidget* NodeWidget = Node->GetWidget();
    OutCanvasSlot           = RootPanel->AddChildToCanvas(NodeWidget);
    if (OutCanvasSlot)
    {
        OutCanvasSlot->SetSize(NodeSize);
        OutCanvasSlot->SetAnchors(FAnchors(0.5f));
        OutCanvasSlot->SetAlignment(FVector2D(0.5f));
        OutCanvasSlot->SetZOrder(1);
    }

    return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, UUserWidget* To)
{
    if (!From || !To) return;

    USplineWidget* Connection             = CreateWidget<USplineWidget>(GetOwningPlayer());
    UCanvasPanelSlot* ConnectionPanelSlot = RootPanel->AddChildToCanvas(Connection);
    if (ConnectionPanelSlot)
    {
        ConnectionPanelSlot->SetAnchors(FAnchors(0.f));
        ConnectionPanelSlot->SetAlignment(FVector2D(0.f));
        ConnectionPanelSlot->SetPosition(FVector2D::Zero());
        ConnectionPanelSlot->SetZOrder(0);
    }

    Connection->SetupSpline(From, To, SourcePortLocalPos, DestinationPortLocalPos, SourcePortDirection, DestinationPortDirection);
    Connection->SetSplineStyle(ConnectionColor, ConnectionThickness);
}
void UItemTreeWidget::CreateTree()
{
}

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface, UUserWidget* StartingNodeWidget, UCanvasPanelSlot* StartingNodeSlot, int StartingNodeDepth, float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
    TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
    float StartingNodeYPos                                   = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1 : 1);

    if (NextTreeNodeInterfaces.Num() == 0)
    {
        StartingNodeSlot->SetPosition(FVector2D(NextLeafXPosition, StartingNodeYPos));
        NextLeafXPosition += NodeSize.X + NodeGap.X;
        return;
    }

    float NextNodeXPosSum = 0;
    for (const ITreeNodeInterface* NextNodeInterface : NextTreeNodeInterfaces)
    {
        UCanvasPanelSlot* NextNodeSlot;
        UUserWidget* NextNodeWidget = CreateWidgetForNode(NextNodeInterface, NextNodeSlot);
        OutStreamSlots.Add(NextNodeSlot);

        if (bUpperStream)
        {
            CreateConnection(NextNodeWidget, StartingNodeWidget);
        }
        else
        {
            CreateConnection(StartingNodeWidget, NextNodeWidget);
        }

        DrawStream(bUpperStream, NextNodeInterface, NextNodeWidget, NextNodeSlot, StartingNodeDepth + 1, NextLeafXPosition, OutStreamSlots);   
        NextNodeXPosSum += NextNodeSlot->GetPosition().X;    
    }

    float StartingNodeXPos = NextNodeXPosSum / NextTreeNodeInterfaces.Num();
    StartingNodeSlot->SetPosition(FVector2D(StartingNodeXPos, StartingNodeYPos));
}
