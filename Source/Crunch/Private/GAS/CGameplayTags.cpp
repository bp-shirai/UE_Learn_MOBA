#include "GAS/CGameplayTags.h"

namespace Tags
{
    namespace Ability
    {
        UE_DEFINE_GAMEPLAY_TAG(BasicAttack, "Ability.BasicAttack");

        namespace Combo
        {
            UE_DEFINE_GAMEPLAY_TAG(_Change, "Ability.Combo.Change");

            namespace Change
            {
                UE_DEFINE_GAMEPLAY_TAG(Combo01, "Ability.Combo.Change.Combo01");
                UE_DEFINE_GAMEPLAY_TAG(Combo02, "Ability.Combo.Change.Combo02");
                UE_DEFINE_GAMEPLAY_TAG(Combo03, "Ability.Combo.Change.Combo03");
                UE_DEFINE_GAMEPLAY_TAG(Combo04, "Ability.Combo.Change.Combo04");
                UE_DEFINE_GAMEPLAY_TAG(End, "Ability.Combo.Change.End");
            }
        }

    }
} // namespace Tags