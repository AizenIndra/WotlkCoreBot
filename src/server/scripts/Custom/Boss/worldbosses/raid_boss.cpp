/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CreatureScript.h"
#include "ScriptedCreature.h"

enum Texts
{
    SAY_AGGRO               = 0,
    SAY_EARTHQUAKE          = 1,
    SAY_OVERRUN             = 2,
    SAY_SLAY                = 3,
    SAY_DEATH               = 4
};

enum Spells
{
    SPELL_EARTHQUAKE        = 32686,
    SPELL_SUNDER_ARMOR      = 33661,
    SPELL_CHAIN_LIGHTNING   = 33665,
    SPELL_OVERRUN           = 32636,
    SPELL_ENRAGE            = 33653,
    SPELL_MARK_DEATH        = 37128,
    SPELL_AURA_DEATH        = 37131
};

struct storm_boss : public ScriptedAI
{
    storm_boss(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _inEnrage = false;
        scheduler.CancelAll();
    }

    void KilledUnit(Unit* victim) override
    {
        victim->CastSpell(victim, SPELL_MARK_DEATH, 0);

        if (roll_chance_i(25) && victim->IsPlayer())
        {
            Talk(SAY_SLAY);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);
        scheduler.Schedule(1ms, [this](TaskContext context)
        {
            if (!HealthAbovePct(20))
            {
                DoCastSelf(SPELL_ENRAGE);
                context.Repeat(6s);
                _inEnrage = true;
            }
        }).Schedule(5s, 13s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SUNDER_ARMOR);
            context.Repeat(10s, 25s);
        }).Schedule(10s, 30s, [this](TaskContext context)
        {
            DoCastRandomTarget(SPELL_CHAIN_LIGHTNING, 1);
            context.Repeat(7s, 27s);
        }).Schedule(25s, 35s, [this](TaskContext context)
        {
            if (urand(0, 1))
            {
                return;
            }
            Talk(SAY_EARTHQUAKE);
            if (_inEnrage) // avoid enrage + earthquake
            {
                me->RemoveAurasDueToSpell(SPELL_ENRAGE);
            }
            DoCastAOE(SPELL_EARTHQUAKE);
            context.Repeat(30s, 55s);
        }).Schedule(30s, 45s, [this](TaskContext context)
        {
            Talk(SAY_OVERRUN);
            DoCastVictim(SPELL_OVERRUN);
            context.Repeat(25s, 40s);
        });
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who && who->GetTypeId() == TYPEID_PLAYER && me->IsValidAttackTarget(who))
        {
            if (who->HasAura(SPELL_MARK_DEATH) && !who->HasAura(27827)) // Spirit of Redemption
            {
                who->CastSpell(who, SPELL_AURA_DEATH, 1);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoMeleeAttackIfReady();
    }

private:
    bool _inEnrage;
};


enum Spells2
{
    SPELL_PHASE1_1 = 47502, // Замените на фактические ID заклинаний
    SPELL_PHASE1_2 = 43661,
    SPELL_PHASE1_3 = 44008,

    SPELL_PHASE2_1 = 43648,
    SPELL_PHASE2_2 = 44779,
    SPELL_PHASE2_3 = 43303,

    SPELL_PHASE3_1 = 43622,
    SPELL_PHASE3_2 = 123,
    SPELL_PHASE3_3 = 11,
};

enum Events
{
    EVENT_CAST_SPELL_1,
    EVENT_CAST_SPELL_2,
    EVENT_CAST_SPELL_3,
    EVENT_CAST_SPELL_4,

    EVENT_CHANGE_PHASE_2,
    EVENT_CHANGE_PHASE_3
};

enum Phases
{
    PHASE_ONE = 1,
    PHASE_TWO = 2,
    PHASE_THREE = 3
};

class boss_custom_boss : public CreatureScript
{
public:
    boss_custom_boss() : CreatureScript("boss_custom_boss") { }

    struct boss_custom_bossAI : public ScriptedAI
    {
        boss_custom_bossAI(Creature* creature) : ScriptedAI(creature) { }

        EventMap events;
        uint8 phase;

        void Reset() override
        {
            events.Reset();
            phase = PHASE_ONE;
            events.ScheduleEvent(EVENT_CAST_SPELL_1, Milliseconds(5000));
            events.ScheduleEvent(EVENT_CAST_SPELL_2, Milliseconds(10000));
            events.ScheduleEvent(EVENT_CAST_SPELL_3, Milliseconds(15000));
            events.ScheduleEvent(EVENT_CHANGE_PHASE_2, Milliseconds(60000)); // Переход на фазу 2 через 60 секунд
        }

         void JustEngagedWith(Unit* /*who*/) override

        {
            Reset();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_CAST_SPELL_1:
                    if (phase == PHASE_ONE)
                        DoCastVictim(SPELL_PHASE1_1);
                    else if (phase == PHASE_TWO)
                        DoCastVictim(SPELL_PHASE2_1);
                    else
                        DoCastVictim(SPELL_PHASE3_1);
                    events.ScheduleEvent(EVENT_CAST_SPELL_1, Milliseconds(5000));
                    break;

                case EVENT_CAST_SPELL_2:
                    if (phase == PHASE_ONE)
                        DoCastVictim(SPELL_PHASE1_2);
                    else if (phase == PHASE_TWO)
                        DoCastVictim(SPELL_PHASE2_2);
                    else
                        DoCastVictim(SPELL_PHASE3_2);
                    events.ScheduleEvent(EVENT_CAST_SPELL_2, Milliseconds(10000));
                    break;

                case EVENT_CAST_SPELL_3:
                    if (phase == PHASE_ONE)
                        DoCastVictim(SPELL_PHASE1_3);
                    else if (phase == PHASE_TWO)
                        DoCastVictim(SPELL_PHASE2_3);
                    else
                        DoCastVictim(SPELL_PHASE3_3);
                    events.ScheduleEvent(EVENT_CAST_SPELL_3, Milliseconds(15000));
                    break;

                case EVENT_CHANGE_PHASE_2:
                    phase = PHASE_TWO;
                    events.ScheduleEvent(EVENT_CAST_SPELL_1, Milliseconds(5000));  // Переназначение событий для новой фазы
                    events.ScheduleEvent(EVENT_CAST_SPELL_2, Milliseconds(10000));
                    events.ScheduleEvent(EVENT_CAST_SPELL_3, Milliseconds(15000));
                    events.ScheduleEvent(EVENT_CHANGE_PHASE_3, Milliseconds(60000)); // Переход на фазу 3 через 60 секунд
                    break;

                case EVENT_CHANGE_PHASE_3:
                    phase = PHASE_THREE;
                    events.ScheduleEvent(EVENT_CAST_SPELL_1, Milliseconds(5000));  // Переназначение событий для новой фазы
                    events.ScheduleEvent(EVENT_CAST_SPELL_2, Milliseconds(10000));
                    events.ScheduleEvent(EVENT_CAST_SPELL_3, Milliseconds(15000));
                    break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_custom_bossAI(creature);
    }
};


void AddSC_storm_boss()
{
    RegisterCreatureAI(storm_boss);
new boss_custom_boss();

}
