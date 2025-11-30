#include "Animations/AnimNotify/CAnimNotifyState_ApplyLooseGameplayTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Logging/LogVerbosity.h"

UCAnimNotifyState_ApplyLooseGameplayTag::UCAnimNotifyState_ApplyLooseGameplayTag()
{
    bIsNativeBranchingPoint = true;
}

void UCAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    Super::BranchingPointNotifyBegin(BranchingPointPayload);
    USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
    AActor* Owner                    = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (Owner && Owner->Implements<UAbilitySystemInterface>())
    {
        if (GameplayTagsToApply.IsValid())
        {
            UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(Owner, GameplayTagsToApply, false);
        }

        if (BeginNotifyTag.IsValid())
        {
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, BeginNotifyTag, FGameplayEventData());
        }
    }
}

void UCAnimNotifyState_ApplyLooseGameplayTag::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
    Super::BranchingPointNotifyEnd(BranchingPointPayload);
    USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
    AActor* Owner                    = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (Owner && Owner->Implements<UAbilitySystemInterface>())
    {
        if (GameplayTagsToApply.IsValid())
        {
            UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(Owner, GameplayTagsToApply, false);
        }

        if (EndNotifyTag.IsValid())
        {
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EndNotifyTag, FGameplayEventData());
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
    if (GameplayTagsToApply.IsValid())
    {
        return GameplayTagsToApply.ToStringSimple();
    }

    return FString::Format(TEXT("{0} ~ {1}"), {BeginNotifyTag.GetTagLeafName().ToString(), EndNotifyTag.GetTagLeafName().ToString()});
}