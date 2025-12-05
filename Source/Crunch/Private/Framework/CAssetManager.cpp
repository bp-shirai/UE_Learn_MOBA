// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CAssetManager.h"
#include "Engine/StreamableManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CAssetManager)

UCAssetManager& UCAssetManager::Get()
{
    UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
    if (Singleton)
    {
        return *Singleton;
    }

    UE_LOG(LogTemp, Fatal, TEXT("Asset Manager Needs to be of the type UCAssetManager."));
    return (*NewObject<UCAssetManager>());
}

void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
    LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished, LoadFinishedCallback));
}

bool UCAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
    TArray<UObject*> LoadedObjects;
    bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);
    if (bLoaded)
    {
        for (UObject* LoadedObject : LoadedObjects)
        {
            OutItems.Add(Cast<UPA_ShopItem>(LoadedObject));
        }
    }

    return bLoaded;
}

void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
    Callback.ExecuteIfBound();
    BuildItemMaps();
}

void UCAssetManager::BuildItemMaps()
{
    TArray<const UPA_ShopItem*> LoadedItems;
    if (GetLoadedShopItems(LoadedItems))
    {
        for (const UPA_ShopItem* Item : LoadedItems)
        {
            if (Item->GetIngredients().Num() == 0) continue;

            TArray<const UPA_ShopItem*> IngredientItems;
            for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
            {
                UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
                IngredientItems.Add(IngredientItem);
                AddToCombinationMap(IngredientItem, Item);
            }

            IngredientMap.Add(Item, FItemCollection(IngredientItems));
        }
    }
}

void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
    FItemCollection* Combinations = CombinationMap.Find(Ingredient);
    if (Combinations)
    {
        if (!Combinations->Contains(CombinationItem))
        {
            // CombinationMap.Add(CombinationItem);
            Combinations->AddItem(CombinationItem);
        }
    }
    else
    {
        CombinationMap.Add(Ingredient, FItemCollection({CombinationItem}));
    }
}

const FItemCollection* UCAssetManager::GetCombinationForItem(const UPA_ShopItem* Item) const
{
    return CombinationMap.Find(Item);
}

const FItemCollection* UCAssetManager::GetIngredientsForItem(const UPA_ShopItem* Item) const
{
    return IngredientMap.Find(Item);
}
