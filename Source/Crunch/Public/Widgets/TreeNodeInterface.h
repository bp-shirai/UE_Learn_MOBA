// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TreeNodeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTreeNodeInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class CRUNCH_API ITreeNodeInterface
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual UUserWidget* GetWidget() const                       = 0;
    virtual TArray<const ITreeNodeInterface*> GetInputs() const  = 0;
    virtual TArray<const ITreeNodeInterface*> GetOutputs() const = 0;
    virtual const UObject* GetItemObject() const                 = 0;
};
