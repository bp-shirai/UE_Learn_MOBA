// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FItemCollection
{
    GENERATED_BODY()
public:
    FItemCollection() : Items() {}
    FItemCollection(const TArray<const UPA_ShopItem*>& InItems) : Items(InItems) {}
    void AddItem(const UPA_ShopItem* NewItem, bool bAddUnique = false);
    bool Contains(const UPA_ShopItem* Item) const;
    const TArray<const UPA_ShopItem*>& GetItems() const { return Items; }

private:
    TArray<const UPA_ShopItem*> Items;
};

/**
 *
 */
UCLASS()
class CRUNCH_API UPA_ShopItem : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
    static FPrimaryAssetType GetShopItemAssetType();

    UTexture2D* GetIcon() const;
    FText GetItemName() const { return ItemName; }
    FText GetItemDescription() const { return ItemDescription; }
    float GetPrice() const { return Price; }
    float GetSellPrice() const { return Price * 0.5f; }
    TSubclassOf<UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
    TSubclassOf<UGameplayEffect> GetConsumeEffect() const { return ConsumeEffect; }
    TSubclassOf<UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }
    UGameplayAbility* GetGrantedAbilityCDO() const;

    bool IsConsumable() const { return bIsConsumable; }
    bool IsStackable() const { return bIsStackable; }
    int32 GetMaxStackCount() const { return MaxStackCount; }
    const TArray<TSoftObjectPtr<UPA_ShopItem>>& GetIngredients() const { return IngredientItems; }

private:
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    float Price{0.f};

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    bool bIsConsumable{false};

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayEffect> EquippedEffect;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayEffect> ConsumeEffect;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayAbility> GrantedAbility;

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    bool bIsStackable{false};

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    int32 MaxStackCount{5};

    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems;
};
