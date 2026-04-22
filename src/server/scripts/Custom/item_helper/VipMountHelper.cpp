#include "VipMountHelper.h"
#include "SpellMgr.h"

namespace VipMountHelper
{
    namespace
    {
        // ИД заклинаний маунтов, которые выдаются/отнимаются у VIP
        static constexpr uint32 PremiumMountSpells[] = { 31700, 18991, 18992 };
    }

    void VipMountLearn(Player* player)
    {
        if (!player)
            return;

        // Если у игрока есть премиум – убеждаемся, что все премиум‑маунты выучены.
        if (player->IsPremium())
        {
            for (uint32 spellId : PremiumMountSpells)
            {
                if (!spellId)
                    continue;

                if (!player->HasSpell(spellId))
                    player->learnSpell(spellId, false, false);
            }
        }
        else
        {
            // Если премиума нет – удаляем премиум‑маунтов.
            for (uint32 spellId : PremiumMountSpells)
            {
                if (!spellId)
                    continue;

                if (player->HasSpell(spellId))
                    player->removeSpell(spellId, SPEC_MASK_ALL, false);
            }
        }
    }
}

