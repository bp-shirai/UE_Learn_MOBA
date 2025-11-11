#pragma once

#include "NativeGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(BasicAttack);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityOne);

        namespace Combo
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(_Change);
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);

            namespace Change
            {
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo01);
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo02);
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo03);
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo04);
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(End);
            } // namespace Change
        } // namespace Combo
        namespace UpperCut
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Launch);
        }

        namespace Passive
        {
            namespace Launch
            {
                UE_DECLARE_GAMEPLAY_TAG_EXTERN(Activate);
            }
        } // namespace Passive
    } // namespace Ability

    namespace Stats
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
    }
} // namespace Tags
