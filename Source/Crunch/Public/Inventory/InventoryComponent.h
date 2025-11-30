// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    void TryPurchase(const UPA_ShopItem* ItemToPurchase);
    float GetGold() const;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UAbilitySystemComponent* OwnerASC;

#pragma region--------------- Server ---------------------------------------------

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

#pragma endregion
};
