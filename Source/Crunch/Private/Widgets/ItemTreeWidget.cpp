// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemTreeWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Logging/LogVerbosity.h"
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

    if (Connection)
    {
        Connection->SetupSpline(From, To, SourcePortLocalPos, DestinationPortLocalPos, SourcePortDirection, DestinationPortDirection);
        Connection->SetSplineStyle(ConnectionColor, ConnectionThickness);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemTreeWidget::CreateConnection: USprintWidget Connection is nullptr"));
    }
}
void UItemTreeWidget::CreateTree()
{
    RootPanel->ClearChildren();
}

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface, UUserWidget* StartingNodeWidget, UCanvasPanelSlot* StartingNodeSlot, int StartingNodeDepth, float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
    TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
    float StartingNodeYPos                                   = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1 : 1);

    if (NextTreeNodeInterfaces.Num() == 0)
    {
        StartingNodeSlot->SetPosition(FVector2D{NextLeafXPosition, StartingNodeYPos});
        NextLeafXPosition += NodeSize.X + NodeGap.X;
        return;
    }

    float NextNodeXPosSum = 0;
    for (const ITreeNodeInterface* NextTreeNodeInteface : NextTreeNodeInterfaces)
    {
        UCanvasPanelSlot* NextWidgetSlot;
        UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInteface, NextWidgetSlot);
        OutStreamSlots.Add(NextWidgetSlot);
        if (bUpperStream)
        {
            CreateConnection(NextWidget, StartingNodeWidget);
        }
        else
        {
            CreateConnection(StartingNodeWidget, NextWidget);
        }

        DrawStream(bUpperStream, NextTreeNodeInteface, NextWidget, NextWidgetSlot, StartingNodeDepth + 1, NextLeafXPosition, OutStreamSlots);
        NextNodeXPosSum += NextWidgetSlot->GetPosition().X;
    }

    float StartingNodeXPos = NextNodeXPosSum / NextTreeNodeInterfaces.Num();
    StartingNodeSlot->SetPosition(FVector2D{StartingNodeXPos, StartingNodeYPos});
}

void UItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
    if (!NodeInterface)
        return;

    if (CurrentCenterItem == NodeInterface->GetItemObject())
        return;

    ClearTree();
    CurrentCenterItem = NodeInterface->GetItemObject();

    float NextLeafXPos                      = 0.f;
    UCanvasPanelSlot* CenterWidgetPanelSlot = nullptr;
    UUserWidget* CenterWidget               = CreateWidgetForNode(NodeInterface, CenterWidgetPanelSlot);
    TArray<UCanvasPanelSlot*> LowerStreamSlots, UpperStreamSlots;

    DrawStream(false, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafXPos, LowerStreamSlots);
    float LowerStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;

    float LowerMoveAmt = 0.f - LowerStreamXMax / 2.0f;
    for (UCanvasPanelSlot* StreamSlot : LowerStreamSlots)
    {
        StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{LowerMoveAmt, 0.f});
    }

    NextLeafXPos = 0.f;
    DrawStream(true, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafXPos, UpperStreamSlots);
    float UpperStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;

    float UpperMoveAmt = 0.f - UpperStreamXMax / 2.0f;
    for (UCanvasPanelSlot* StreamSlot : UpperStreamSlots)
    {
        StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{UpperMoveAmt, 0.f});
    }

    CenterWidgetPanelSlot->SetPosition(FVector2D::Zero());
}