// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryTypes.generated.h"

USTRUCT()
struct FInventoryItemHandle
{
    GENERATED_BODY()
public:
    FInventoryItemHandle();
    static FInventoryItemHandle CreateHandle();
    static FInventoryItemHandle InvalidHandle;

    bool IsValid() const;
    uint32 GetHandleId() const { return HandleId; }

private:
    explicit FInventoryItemHandle(uint32 Id);

    UPROPERTY()
    uint32 HandleId;

    static uint32 GenerateNextId();
    static uint32 InvalidId;
};

bool operator==(const FInventoryItemHandle& A, const FInventoryItemHandle& B);

uint32 GetTypeHash(const FInventoryItemHandle& Key);
