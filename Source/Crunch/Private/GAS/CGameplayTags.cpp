#include "GAS/CGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack, "Ability.BasicAttack");
        UE_DEFINE_GAMEPLAY_TAG(AbilityOne, "Ability.AbilityOne");

        namespace Combo
        {
            UE_DEFINE_GAMEPLAY_TAG(_Change, "Ability.Combo.Change");
            UE_DEFINE_GAMEPLAY_TAG(Damage, "Ability.Combo.Damage");

            namespace Change
            {
                UE_DEFINE_GAMEPLAY_TAG(Combo01, "Ability.Combo.Change.Combo01");
                UE_DEFINE_GAMEPLAY_TAG(Combo02, "Ability.Combo.Change.Combo02");
                UE_DEFINE_GAMEPLAY_TAG(Combo03, "Ability.Combo.Change.Combo03");
                UE_DEFINE_GAMEPLAY_TAG(Combo04, "Ability.Combo.Change.Combo04");
                UE_DEFINE_GAMEPLAY_TAG(End, "Ability.Combo.Change.End");
            } // namespace Change
        } // namespace Combo

        namespace UpperCut
        {
            UE_DEFINE_GAMEPLAY_TAG(Launch, "Ability.UpperCut.Launch");
        }

        namespace Passive
        {
            namespace Launch
            {
                UE_DEFINE_GAMEPLAY_TAG(Activate, "Ability.Passive.Launch.Activate");
            }
        } // namespace Passive

    } // namespace Ability

    namespace Stats
    {
        UE_DEFINE_GAMEPLAY_TAG(Dead, "Stats.Dead");

    }
} // namespace Tags