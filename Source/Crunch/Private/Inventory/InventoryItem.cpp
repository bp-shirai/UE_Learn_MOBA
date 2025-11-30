// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryItem.h"

uint32 FInventoryItemHandle::InvalidId                   = 0;
FInventoryItemHandle FInventoryItemHandle::InvalidHandle = FInventoryItemHandle();

FInventoryItemHandle::FInventoryItemHandle() : HandleId(InvalidId)
{
}

FInventoryItemHandle::FInventoryItemHandle(uint32 Id) : HandleId(Id)
{
}

uint32 FInventoryItemHandle::GenerateNextId()
{
    static uint32 StaticId = 1;
    return StaticId++;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
    return FInventoryItemHandle(GenerateNextId());
}

bool FInventoryItemHandle::IsValid() const
{
    return HandleId != InvalidId;
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
    return Key.GetHandleId();
}

bool operator==(const FInventoryItemHandle& A, const FInventoryItemHandle& B)
{
    return A.GetHandleId() == B.GetHandleId();
}