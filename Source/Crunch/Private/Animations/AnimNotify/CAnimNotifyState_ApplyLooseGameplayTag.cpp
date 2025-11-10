#include "Animations/AnimNotify/CAnimNotifyState_ApplyLooseGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SkeletalMeshComponent.h"

UCAnimNotifyState_ApplyLooseGameplayTag::UCAnimNotifyState_ApplyLooseGameplayTag()
{
    bIsNativeBranchingPoint = true;
}

void UCAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    Super::BranchingPointNotifyBegin(BranchingPointPayload);
    USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
    AActor* Actor                    = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (Actor)
    {
        UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(Actor, GameplayTagsToApply, false);

        if (BeginNotifyTag.IsValid())
        {
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, BeginNotifyTag, FGameplayEventData());
        }
    }
}

void UCAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    Super::BranchingPointNotifyEnd(BranchingPointPayload);
    USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
    AActor* Actor                    = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (Actor)
    {
        UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(Actor, GameplayTagsToApply, false);

        if (EndNotifyTag.IsValid())
        {
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EndNotifyTag, FGameplayEventData());
        }
    }
}

#if WITH_EDITOR
bool UCAnimNotifyState_ApplyLooseGameplayTag::CanBePlaced(UAnimSequenceBase* Animation) const
{
    return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}
#endif

FString UCAnimNotifyState_ApplyLooseGameplayTag::GetNotifyName_Implementation() const
{
    return GameplayTagsToApply.ToStringSimple();
}