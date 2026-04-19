/*
 * Copyright (C) 2023     Stormscale.Ru 
 * script compl = inc <Martynov.M> (c) 2023 stormscale project
 * boss_tolreos worldboss hujal zone 
 * need test and how to kill :D
 */


#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "ObjectMgr.h"
#include "Player.h"

enum Spells2
{
    SPELL_SLIME_BOLT = 43621,
    SPELL_BEBENDE_ERDE = 43661,
    SPELL_BLISTERING_COLD = 44008,
    SPELL_BERSERK = 43648,
    SPELL_ENRAGE = 44779,
    SPELL_TOXIC_WASTE = 43303
};

enum Events2
{
    EVENT_COLD = 1,
    EVENT_BEBENDE_ERDE = 2,
    EVENT_SLIME_BOLT = 3,
    EVENT_SUMMONS2 = 4,
    EVENT_BERSERK = 5,
    EVENT_ENRAGE = 6,
    EVENT_WASTE = 7

};

enum Phases2
{
    PHASE_ONE = 1,
    PHASE_TWO = 2,
    PHASE_THREE = 3
};

enum Summons2
{
    NPC_SCHMORRSCHUPPEN = 60766
};

enum Texts2
{
    SAY_AGGRO = 0,
    SAY_RANDOM = 1,
    SAY_HELP = 2,
    SAY_BERSERK = 3,
    SAY_ENRAGE = 4,
    SAY_DEAD = 5
};

class boss_two : public CreatureScript
{
public:
    boss_two() : CreatureScript("boss_two") { }

    struct boss_twoAI : public ScriptedAI
    {
        boss_twoAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        void Reset() override
        {
            _events.Reset();
            Summons.DespawnAll();
        }

        void JustEngagedWith(Unit*) override
        {
           // Talk(SAY_AGGRO);
            _events.SetPhase(PHASE_ONE);
            _events.ScheduleEvent(EVENT_COLD, Milliseconds(1000));
            _events.ScheduleEvent(EVENT_BEBENDE_ERDE, Milliseconds(10000));
            _events.ScheduleEvent(EVENT_WASTE, Milliseconds(8000));

        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
            {
                _events.SetPhase(PHASE_TWO);
                _events.ScheduleEvent(EVENT_SLIME_BOLT, Milliseconds(10000));
                _events.ScheduleEvent(EVENT_WASTE, Milliseconds(20000));

            }

            if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
            {
                _events.SetPhase(PHASE_THREE);
                _events.ScheduleEvent(EVENT_SUMMONS2, Milliseconds(5000));
                _events.ScheduleEvent(EVENT_BERSERK, Milliseconds(1000));
                _events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(12000)); //2Minuten sonst alle tot!
            }
        }

        void JustSummoned(Creature* summon) override
        {
            Summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_SCHMORRSCHUPPEN:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 300.0f, true))
                    summon->AI()->AttackStart(target); // I think it means the Tank !
                break;
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
                        Summons.DespawnAll();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SLIME_BOLT:
                    DoCast(me, EVENT_SLIME_BOLT);
                    break;
                case EVENT_BEBENDE_ERDE:
                    DoCastVictim(SPELL_BEBENDE_ERDE);
                    _events.ScheduleEvent(EVENT_BEBENDE_ERDE, Milliseconds(8000));
                    break;
                case EVENT_COLD:
                    //Talk(SAY_RANDOM);
                    DoCastVictim(SPELL_BLISTERING_COLD);
                    _events.ScheduleEvent(EVENT_COLD, Milliseconds(30000));
                    break;
                case EVENT_SUMMONS2:
                   // Talk(SAY_HELP);
                    me->SummonCreature(NPC_SCHMORRSCHUPPEN, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() + 5, 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
                    break;
                case EVENT_BERSERK:
                   // Talk(SAY_BERSERK);
                    DoCast(me, SPELL_BERSERK);
                    _events.ScheduleEvent(EVENT_BERSERK, Milliseconds(120000));
                    break;
                case EVENT_ENRAGE:
                   // Talk(SAY_ENRAGE);
                    DoCast(me, SPELL_ENRAGE);
                    _events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(10000));
                    break;
                case EVENT_WASTE:
                    DoCast(SPELL_TOXIC_WASTE);
                    _events.ScheduleEvent(EVENT_WASTE, Milliseconds(15000));
                    break;

                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        SummonList Summons;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_twoAI(creature);
    }
};

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_PRE_PHASE_1 = 74403,
    SPELL_PRE_PHASE_2 = 1002,
    SPELL_PHASE_1_1 = 73576,
    SPELL_PHASE_1_2 = 76561,
    SPELL_PHASE_1_3 = 78862,
    SPELL_PHASE_2_1 = 76560,
    SPELL_PHASE_2_2 = 74806,
    SPELL_PHASE_2_3 = 78723,
    SPELL_PHASE_2_4 = 74430,
    SPELL_PHASE_3_1 = 4001,
    SPELL_PHASE_3_2 = 4002,
    SPELL_PHASE_3_3 = 4003,
    SPELL_PHASE_3_4 = 4004,
    SPELL_PHASE_3_5 = 4005,
    SPELL_PHASE_3_6 = 4006,
    SPELL_PHASE_3_7 = 4007,
    SPELL_TRANSITION_1 = 73529,
    SPELL_TRANSITION_2 = 5002,
    SPELL_TRANSITION_3 = 5003
};

enum Phases
{
    PHASE_PRE_PHASE = 1,
    PHASE_1 = 2,
    PHASE_2 = 3,
    PHASE_3 = 4
};

class boss_three_phase : public CreatureScript
{
public:
    boss_three_phase() : CreatureScript("boss_three_phase") { }

    struct boss_three_phaseAI : public ScriptedAI
    {
        boss_three_phaseAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            phase = PHASE_PRE_PHASE;
            events.Reset();
            events.ScheduleEvent(SPELL_PRE_PHASE_1, Milliseconds(5000));
            events.ScheduleEvent(SPELL_PRE_PHASE_2, Milliseconds(10000));
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            Talk(0); // Optional: Add a yell or talk when entering combat
        }

        void JustDied(Unit* /*killer*/) override
        {
            events.Reset();
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            // Optional: Add a yell or talk when killing a unit
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (HealthBelowPct(85) && phase == PHASE_PRE_PHASE)
            {
                phase = PHASE_1;
                events.Reset();
                Talk(1); // Optional: Add a yell or talk when transitioning to phase 1
                DoCast(SPELL_TRANSITION_1);
                events.ScheduleEvent(SPELL_PHASE_1_1, Milliseconds(5000));
                events.ScheduleEvent(SPELL_PHASE_1_2, Milliseconds(10000));
                events.ScheduleEvent(SPELL_PHASE_1_3, Milliseconds(15000));
            }
            else if (HealthBelowPct(60) && phase == PHASE_1)
            {
                phase = PHASE_2;
                events.Reset();
                Talk(2); // Optional: Add a yell or talk when transitioning to phase 2
                DoCast(SPELL_TRANSITION_2);
                events.ScheduleEvent(SPELL_PHASE_2_1, Milliseconds(5000));
                events.ScheduleEvent(SPELL_PHASE_2_2, Milliseconds(10000));
                events.ScheduleEvent(SPELL_PHASE_2_3, Milliseconds(15000));
                events.ScheduleEvent(SPELL_PHASE_2_4, Milliseconds(20000));
            }
            else if (HealthBelowPct(25) && phase == PHASE_2)
            {
                phase = PHASE_3;
                events.Reset();
                Talk(3); // Optional: Add a yell or talk when transitioning to phase 3
                DoCast(SPELL_TRANSITION_3);
                events.ScheduleEvent(SPELL_PHASE_3_1, Milliseconds(5000));
                events.ScheduleEvent(SPELL_PHASE_3_2, Milliseconds(10000));
                events.ScheduleEvent(SPELL_PHASE_3_3, Milliseconds(15000));
                events.ScheduleEvent(SPELL_PHASE_3_4, Milliseconds(20000));
                events.ScheduleEvent(SPELL_PHASE_3_5, Milliseconds(25000));
                events.ScheduleEvent(SPELL_PHASE_3_6, Milliseconds(30000));
                events.ScheduleEvent(SPELL_PHASE_3_7, Milliseconds(35000));
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case SPELL_PRE_PHASE_1:
                        DoCastVictim(SPELL_PRE_PHASE_1);
                        events.ScheduleEvent(SPELL_PRE_PHASE_1, Milliseconds(5000));
                        break;
                    case SPELL_PRE_PHASE_2:
                        DoCastVictim(SPELL_PRE_PHASE_2);
                        events.ScheduleEvent(SPELL_PRE_PHASE_2, Milliseconds(10000));
                        break;
                    case SPELL_PHASE_1_1:
                        DoCastVictim(SPELL_PHASE_1_1);
                        events.ScheduleEvent(SPELL_PHASE_1_1, Milliseconds(5000));
                        break;
                    case SPELL_PHASE_1_2:
                        DoCastVictim(SPELL_PHASE_1_2);
                        events.ScheduleEvent(SPELL_PHASE_1_2, Milliseconds(10000));
                        break;
                    case SPELL_PHASE_1_3:
                        DoCastVictim(SPELL_PHASE_1_3);
                        events.ScheduleEvent(SPELL_PHASE_1_3, Milliseconds(15000));
                        break;
                    case SPELL_PHASE_2_1:
                        DoCastVictim(SPELL_PHASE_2_1);
                        events.ScheduleEvent(SPELL_PHASE_2_1, Milliseconds(5000));
                        break;
                    case SPELL_PHASE_2_2:
                        DoCastVictim(SPELL_PHASE_2_2);
                        events.ScheduleEvent(SPELL_PHASE_2_2, Milliseconds(10000));
                        break;
                    case SPELL_PHASE_2_3:
                        DoCastVictim(SPELL_PHASE_2_3);
                        events.ScheduleEvent(SPELL_PHASE_2_3, Milliseconds(15000));
                        break;
                    case SPELL_PHASE_2_4:
                        DoCastVictim(SPELL_PHASE_2_4);
                        events.ScheduleEvent(SPELL_PHASE_2_4, Milliseconds(20000));
                        break;
                    case SPELL_PHASE_3_1:
                        DoCastVictim(SPELL_PHASE_3_1);
                        events.ScheduleEvent(SPELL_PHASE_3_1, Milliseconds(5000));
                        break;
                    case SPELL_PHASE_3_2:
                        DoCastVictim(SPELL_PHASE_3_2);
                        events.ScheduleEvent(SPELL_PHASE_3_2, Milliseconds(10000));
                        break;
                    case SPELL_PHASE_3_3:
                        DoCastVictim(SPELL_PHASE_3_3);
                        events.ScheduleEvent(SPELL_PHASE_3_3, Milliseconds(15000));
                        break;
                    case SPELL_PHASE_3_4:
                        DoCastVictim(SPELL_PHASE_3_4);
                        events.ScheduleEvent(SPELL_PHASE_3_4, Milliseconds(20000));
                        break;
                    case SPELL_PHASE_3_5:
                        DoCastVictim(SPELL_PHASE_3_5);
                        events.ScheduleEvent(SPELL_PHASE_3_5, Milliseconds(25000));
                        break;
                    case SPELL_PHASE_3_6:
                        DoCastVictim(SPELL_PHASE_3_6);
                        events.ScheduleEvent(SPELL_PHASE_3_6, Milliseconds(30000));
                        break;
                    case SPELL_PHASE_3_7:
                        DoCastVictim(SPELL_PHASE_3_7);
                        events.ScheduleEvent(SPELL_PHASE_3_7, Milliseconds(35000));
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        uint8 phase;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_three_phaseAI(creature);
    }
};

void AddSC_two()
{
    new boss_two();
new boss_three_phase();
	
}
