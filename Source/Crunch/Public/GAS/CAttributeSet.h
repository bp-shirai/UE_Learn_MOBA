// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class CRUNCH_API UCAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    void RescaleHealth();
    void RescaleMana();

    ATTRIBUTE_ACCESSORS(UCAttributeSet, Health)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Attack)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Armor)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveSpeed)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveAcceleration)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedHealthPercent)
    ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedManaPercent)

private:
    UPROPERTY(ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;

    UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;

    UPROPERTY(ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;

    UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;

    UPROPERTY(ReplicatedUsing = OnRep_Attack)
    FGameplayAttributeData Attack;

    UPROPERTY(ReplicatedUsing = OnRep_Armor)
    FGameplayAttributeData Armor;

    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;

    UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
    FGameplayAttributeData MoveAcceleration;

    UPROPERTY()
    FGameplayAttributeData CachedHealthPercent;

    UPROPERTY()
    FGameplayAttributeData CachedManaPercent;

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Attack(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Armor(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue);
};
