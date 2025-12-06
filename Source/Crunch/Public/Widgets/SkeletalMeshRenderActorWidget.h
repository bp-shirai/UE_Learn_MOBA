// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RenderActorWidget.h"
#include "SkeletalMeshRenderActorWidget.generated.h"

class ASkeletalMeshRenderActor;

/**
 *
 */
UCLASS()
class CRUNCH_API USkeletalMeshRenderActorWidget : public URenderActorWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

private:
	virtual void SpawnRenderActor() override;
	virtual ARenderActor* GetRenderActor() const override;


    UPROPERTY(EditDefaultsOnly, Category = "SkeletalMesh RenderActor")
    TSubclassOf<ASkeletalMeshRenderActor> SkeletalMeshRenderActorClass;

    UPROPERTY()
    ASkeletalMeshRenderActor* SkeletalMeshRenderActor;
};
