#pragma once

#include "NativeGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(BasicAttack);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(BasicAttack_Pressed);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityOne);

        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change_Combo01);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change_Combo02);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change_Combo03);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change_Combo04);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Change_End);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combo_Damage);

        UE_DECLARE_GAMEPLAY_TAG_EXTERN(UpperCut_Launch);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(UpperCut_Cooldown);

        namespace Passive
        {
            UE_DECLARE_GAMEPLAY_TAG_EXTERN(Launch_Activate);
        }
    } // namespace Ability

    namespace Stats
    {
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead);
        UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stun);
    } // namespace Stats
} // namespace Tags
