#include "GAS/CGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DEFINE_GAMEPLAY_TAG(Generic_Damage, "Ability.Generic.Damage");
        UE_DEFINE_GAMEPLAY_TAG(Generic_Target, "Ability.Generic.Target");
        UE_DEFINE_GAMEPLAY_TAG(Target_Updated, "Ability.Target.Updated");

        UE_DEFINE_GAMEPLAY_TAG(BasicAttack, "Ability.BasicAttack");
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack_Pressed, "Ability.BasicAttack.Pressed");
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack_Released, "Ability.BasicAttack.Released");
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack_SendServer, "Ability.BasicAttack.SendServer");

        UE_DEFINE_GAMEPLAY_TAG(AbilityOne, "Ability.AbilityOne");
        UE_DEFINE_GAMEPLAY_TAG(AbilityTwo, "Ability.AbilityTwo");
        UE_DEFINE_GAMEPLAY_TAG(AbilityThree, "Ability.AbilityThree");
        UE_DEFINE_GAMEPLAY_TAG(AbilityFour, "Ability.AbilityFour");

        UE_DEFINE_GAMEPLAY_TAG(Combo, "Ability.Combo");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change, "Ability.Combo.Change");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Damage, "Ability.Combo.Damage");

        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo01, "Ability.Combo.Change.Combo01");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo02, "Ability.Combo.Change.Combo02");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo03, "Ability.Combo.Change.Combo03");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Combo04, "Ability.Combo.Change.Combo04");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_Begin, "Ability.Combo.Change.Begin");
        UE_DEFINE_GAMEPLAY_TAG(Combo_Change_End, "Ability.Combo.Change.End");

        UE_DEFINE_GAMEPLAY_TAG(Combo_InputAllowed, "Ability.Combo.InputAllowed");

        UE_DEFINE_GAMEPLAY_TAG(UpperCut_Launch, "Ability.UpperCut.Launch");
        UE_DEFINE_GAMEPLAY_TAG(UpperCut_Cooldown, "Ability.UpperCut.Cooldown");

        UE_DEFINE_GAMEPLAY_TAG(Shoot, "Ability.Shoot");
        UE_DEFINE_GAMEPLAY_TAG(Shoot_Hand_L, "Ability.Shoot.Hand_L");
        UE_DEFINE_GAMEPLAY_TAG(Shoot_Hand_R, "Ability.Shoot.Hand_R");

        UE_DEFINE_GAMEPLAY_TAG(Laser_Shoot, "Ability.Laser.Shoot");
        UE_DEFINE_GAMEPLAY_TAG(Laser_Cooldown, "Ability.Laser.Cooldown");

        UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Ability.Data.Damage");
        UE_DEFINE_GAMEPLAY_TAG(Data_Duration, "Ability.Data.Duration");

        UE_DEFINE_GAMEPLAY_TAG(Type_Attack_Melee, "Ability.Type.Attack.Melee");
        UE_DEFINE_GAMEPLAY_TAG(Type_Attack_Ranged, "Ability.Type.Attack.Ranged");
        UE_DEFINE_GAMEPLAY_TAG(Type_StateChange, "Ability.Type.StateChange");
        UE_DEFINE_GAMEPLAY_TAG(Type_Passive, "Ability.Type.Passive");

        namespace Passive
        {
            UE_DEFINE_GAMEPLAY_TAG(Launch_Activate, "Ability.Passive.Launch.Activate");
        }
    } // namespace Ability
    namespace Stats
    {
        UE_DEFINE_GAMEPLAY_TAG(Dead, "Stats.Dead");
        UE_DEFINE_GAMEPLAY_TAG(Stun, "Stats.Stun");
        UE_DEFINE_GAMEPLAY_TAG(Aim, "Stats.Aim");
        UE_DEFINE_GAMEPLAY_TAG(CanBeCanceled, "Stats.CanBeCanceled");
        UE_DEFINE_GAMEPLAY_TAG(Attacking, "Stats.Attacking");
        UE_DEFINE_GAMEPLAY_TAG(Combat_ComboWindow, "Stats.Combat.ComboWindow");
        UE_DEFINE_GAMEPLAY_TAG(Health_Full, "Stats.Health.Full");
        UE_DEFINE_GAMEPLAY_TAG(Health_Empty, "Stats.Health.Empty");
        UE_DEFINE_GAMEPLAY_TAG(Mana_Full, "Stats.Mana.Full");
        UE_DEFINE_GAMEPLAY_TAG(Mana_Empty, "Stats.Mana.Empty");

        UE_DEFINE_GAMEPLAY_TAG(Crosshair, "Stats.Crosshair");
        UE_DEFINE_GAMEPLAY_TAG(Focus, "Stats.Focus");

    } // namespace Stats

    namespace GameplayCue
    {
        UE_DEFINE_GAMEPLAY_TAG(CameraShake, "GameplayCue.CameraShake");
    }

    namespace Role
    {
        UE_DEFINE_GAMEPLAY_TAG(Hero, "Role.Hero");
    }

    namespace Attribute
    {
        UE_DEFINE_GAMEPLAY_TAG(Health, "Attribute.Health");
        UE_DEFINE_GAMEPLAY_TAG(Mana, "Attribute.Mana");
        UE_DEFINE_GAMEPLAY_TAG(Experience, "Attribute.Experience");
        UE_DEFINE_GAMEPLAY_TAG(Gold, "Attribute.Gold");
        UE_DEFINE_GAMEPLAY_TAG(Level, "Attribute.Level");
    } // namespace Attribute
} // namespace Tags