/*
 * Copyright (C) 2023     Stormscale.Ru 
 * script compl = inc <Martynov.M> (c) 2023 stormscale project
 * boss_galadriel  worldboss hujal zone 
 * need test and how to kill :D
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "GridNotifiers.h"



enum Spells
{
	SPELL_SLIME_BOLT = 32309,
	SPELL_BEBENDE_ERDE = 6524,
	SPELL_BLISTERING_COLD = 70123,
	SPELL_BERSERK = 26662,
	SPELL_ENRAGE = 68335,
	SPELL_TOXIC_WASTE = 69024
};

enum Events
{
	EVENT_COLD = 1,
	EVENT_BEBENDE_ERDE = 2,
	EVENT_SLIME_BOLT = 3,
	EVENT_SUMMONS = 4,
	EVENT_BERSERK = 5,
	EVENT_ENRAGE = 6,
	EVENT_WASTE = 7

};

enum Phases
{
	PHASE_ONE = 1,
	PHASE_TWO = 2,
	PHASE_THREE = 3
};

enum Summons
{
	NPC_SCHMORRSCHUPPEN = 91262
};

enum Texts
{
	SAY_AGGRO = 0,
	SAY_RANDOM = 1,
	SAY_HELP = 2,
	SAY_BERSERK = 3,
	SAY_ENRAGE = 4,
	SAY_DEAD = 5
};

class boss_galadriel : public CreatureScript
{
public:
	boss_galadriel() : CreatureScript("boss_galadriel") { }

	struct boss_twoAI : public ScriptedAI
	{
		boss_twoAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

		void Reset() override
		{
			_events.Reset();
			Summons.DespawnAll();
		}

		void JustEngagedWith(Unit* /*who*/) override
		{
                          me->PlayDirectSound(5855);
			me->Say("Кто посмел нарушить покой Аркедаса? у Кого хватило дерзозти прогневать создателей?!", LANG_UNIVERSAL, 0);
			_events.SetPhase(PHASE_ONE);
			_events.ScheduleEvent(EVENT_COLD, Milliseconds(1000));
			_events.ScheduleEvent(EVENT_BEBENDE_ERDE, Milliseconds(10000));
			_events.ScheduleEvent(EVENT_WASTE, Milliseconds(8000));
						
		}

		void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
			if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
			{
				me->PlayDirectSound(5859);
                                me->Say("Узрите мощь падшего брата!", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_TWO);
				_events.ScheduleEvent(EVENT_SLIME_BOLT, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_WASTE, Milliseconds(20000));
			
			}

			if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
			{
me->PlayDirectSound(5860);

				me->Say("Я спляшу на ваших костях, скоро вы станите моими личными душами", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_THREE);
				_events.ScheduleEvent(EVENT_SUMMONS, Milliseconds(5000));
				_events.ScheduleEvent(EVENT_BERSERK, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(120000)); //2min and gg
			}
		}

		void JustSummoned(Creature* summon) override
		{
			Summons.Summon(summon);

			switch (summon->GetEntry())
			{
			case NPC_SCHMORRSCHUPPEN:
if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 300.0f, true))
				
					summon->AI()->AttackStart(target); // iwd just target first target(tank) 
				break;
			}
		}

		void JustDied(Unit* ) override
		{
			
			me->Say("Это было весьма забавно..", LANG_UNIVERSAL, 0);
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
                                         me->PlayDirectSound(5856);
					me->Say("ЛЕДЯНОЙ ЗАЛП!!!", LANG_UNIVERSAL, 0);
					DoCastVictim(SPELL_BLISTERING_COLD);
					_events.ScheduleEvent(EVENT_COLD, Milliseconds(30000));
					break;
				case EVENT_SUMMONS:
                                        me->PlayDirectSound(5857);
					me->Say("Верные слуги, встаньте на защиту своего хозяина", LANG_UNIVERSAL, 0);
					me->SummonCreature(NPC_SCHMORRSCHUPPEN, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() + 5, 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
					me->SummonCreature(NPC_SCHMORRSCHUPPEN, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() + 5, 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
					me->SummonCreature(NPC_SCHMORRSCHUPPEN, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() + 5, 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
					break;
				case EVENT_BERSERK:
					Talk(SAY_BERSERK);
					DoCast(me, SPELL_BERSERK);
					_events.ScheduleEvent(EVENT_BERSERK, Milliseconds(120000));
					break;
				case EVENT_ENRAGE:
					Talk(SAY_ENRAGE);
					DoCast(me, SPELL_ENRAGE);
					_events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(10000));
					break;
				case EVENT_WASTE:
					DoCastToAllHostilePlayers(SPELL_TOXIC_WASTE);
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


class boss_custom : public CreatureScript
{
public:
    boss_custom() : CreatureScript("boss_custom") { }

    struct boss_customAI : public BossAI
    {
        boss_customAI(Creature* creature) : BossAI(creature, 0) { }
        uint32 phase = 1;
        uint32 spellTimer = 30000; // 30 seconds for phase 1
        uint32 spellTimer2 = 15000; // 15 seconds for phase 2 (first spell)
        uint32 spellTimer3 = 20000; // 20 seconds for phase 2 (second spell)
        uint32 targetChangeTimer = 40000; // 40 seconds to change target

        void Reset() override
        {
            _Reset();
            phase = 1;
            spellTimer = 30000;
            spellTimer2 = 15000;
            spellTimer3 = 20000;
            targetChangeTimer = 40000;
            
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
           
                         }

   void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
}

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (phase == 1)
            {
                if (spellTimer <= diff)
                {
                    DoCast(me->GetVictim(), 55543); // Cast the spell
                    spellTimer = 30000; // Reset timer
                    DoCast(me->GetVictim(), 44993); // Cast the spell2
                    spellTimer = 15000; // Reset timer

                }
                else
                    spellTimer -= diff;

                if (me->GetHealthPct() <= 100.0f)
                {
                    phase = 2; // Switch to phase 2
me->PlayDirectSound(11213);
			me->Say("Внимание, вы подлежите уничтожению, все попытки будут провалены!Первый кто попастся на моем пути будет моментально уничтожен. Повтораяю, все попытки будут провалены!", LANG_UNIVERSAL, 0);

                    me->PlayDirectSound(11218);

			me->Say("Прнимаю альтернативные меры для уничтожения этих глупых созданий.", LANG_UNIVERSAL, 0);

                }
            }
            else if (phase == 2)
            {
                if (spellTimer2 <= diff)
                {
                    DoCast(me->GetVictim(), 81209); // Cast first spell
                    spellTimer2 = 15000; // Reset timer for first spell
                }
                else
                    spellTimer2 -= diff;

                if (spellTimer3 <= diff)
                {
                    DoCast(me->GetVictim(), 48094); // Cast second spell
                    spellTimer3 = 20000; // Reset timer for second spell
                }
                else
                    spellTimer3 -= diff;

                if (targetChangeTimer <= diff)
                {
                    ChangeTarget(); // Change target randomly
                    targetChangeTimer = 25000; // Reset target change timer
                }
                else
                    targetChangeTimer -= diff;
 if (spellTimer <= diff)
                {
                    DoCast(me->GetVictim(), 81208); // Cast the spell
                    spellTimer = 30000; // Reset timer
                    DoCast(me->GetVictim(), 57581); // Cast the spell2
                    spellTimer = 23000; // Reset timer

                }
                else
                    spellTimer -= diff;

DoMeleeAttackIfReady();
            }
        }

        void ChangeTarget()
        {
            std::list<HostileReference*> threatList = me->GetThreatMgr().GetThreatList();
            if (threatList.empty())
                return;

            // Randomly select a new target from the threat list
            auto it = threatList.begin();
            std::advance(it, urand(0, threatList.size() - 1));
            Unit* newTarget = ObjectAccessor::GetUnit(*me, (*it)->getUnitGuid());

            if (newTarget && newTarget != me->GetVictim())
            {
                me->Attack(newTarget, true);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_customAI(creature);
    }
};

void AddSC_boss_galadriel()
{
	new boss_galadriel();
new boss_custom();
}