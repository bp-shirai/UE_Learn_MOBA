#include "GAS/CGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack, "Ability.BasicAttack");
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack_Pressed, "Ability.BasicAttack.Pressed");
        UE_DEFINE_GAMEPLAY_TAG(AbilityOne, "Ability.AbilityOne");
        UE_DEFINE_GAMEPLAY_TAG(AbilityTwo, "Ability.AbilityTwo");
        UE_DEFINE_GAMEPLAY_TAG(AbilityThree, "Ability.AbilityThree");
        UE_DEFINE_GAMEPLAY_TAG(AbilityFour, "Ability.AbilityFour");

        UE_DEFINE_GAMEPLAY_TAG(Combo_Change, "Ability.Combo.Change");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Damage, "Ability.Combo.Damage");

        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo01, "Ability.Combo.Change.Combo01");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo02, "Ability.Combo.Change.Combo02");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo03, "Ability.Combo.Change.Combo03");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo04, "Ability.Combo.Change.Combo04");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_End, "Ability.Combo.Change.End");
        UE_DEFINE_GAMEPLAY_TAG(Combo_InputAllowed, "Ability.Combo.InputAllowed");

        UE_DEFINE_GAMEPLAY_TAG(UpperCut_Launch, "Ability.UpperCut.Launch");
        UE_DEFINE_GAMEPLAY_TAG(UpperCut_Cooldown, "Ability.UpperCut.Cooldown");

        UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Ability.Data.Damage");
        UE_DEFINE_GAMEPLAY_TAG(Data_Duration, "Ability.Data.Duration");

        namespace Passive
        {
            UE_DEFINE_GAMEPLAY_TAG(Launch_Activate, "Ability.Passive.Launch.Activate");
        } 
    } 
    namespace Stats
    {
        UE_DEFINE_GAMEPLAY_TAG(Dead, "Stats.Dead");
        UE_DEFINE_GAMEPLAY_TAG(Stun, "Stats.Stun");
    }
} 