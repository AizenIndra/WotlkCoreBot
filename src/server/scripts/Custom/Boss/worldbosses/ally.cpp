#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuras.h"
#include "Player.h"

enum eNums
{
	SPELL_FROSTSHOCK = 23115,
	SPELL_CHILL = 21098,
	SPELL_ICEBLOCK = 41590,
};

class alliance_boss : public CreatureScript
{
public:
	alliance_boss() : CreatureScript("allianceboss"){}

	CreatureAI * GetAI(Creature * pCreature) const
	{
		return new alliance_bossAI(pCreature);
	}

	struct alliance_bossAI : public ScriptedAI
	{
		alliance_bossAI(Creature *c) : ScriptedAI(c){}

		uint32 Frostshock_Timer;
		uint32 Chill_Timer;

		int phase;
		int percentPassed;

		void Reset()
		{
			me->SetHealth(me->GetMaxHealth());
			phase = 1;
			percentPassed = 0;
			Frostshock_Timer = 8000;
			Chill_Timer = 3000;
		}
		void JustEngagedWith(Unit * /*who*/)
		{
			percentPassed = 1;
		}
		void JustDied(Unit* )
		{
		 me->Say("МЕСТЬ ЗА ОРДУ БУДЕТ ЖЕСТОКОЙ!", LANG_UNIVERSAL, 0);

		}
		void UpdateAI(const uint32 uiDiff)
		{
			ScriptedAI::UpdateAI(uiDiff);

			if (!UpdateVictim())
				return;

			if (Frostshock_Timer <= uiDiff)
			{
				DoCast(me, SPELL_FROSTSHOCK);
				Frostshock_Timer = 15000;
			}
			else
				Frostshock_Timer -= uiDiff;
			DoMeleeAttackIfReady();

			if (Chill_Timer <= uiDiff)
			{
				DoCast(me, SPELL_CHILL);
				Chill_Timer = 3000;
			}
			else
				Chill_Timer -= uiDiff;
			DoMeleeAttackIfReady();

			if (me->GetHealthPct() <= 10 && percentPassed == 1)
			{
				DoCast(me, SPELL_ICEBLOCK, true);
				percentPassed = 2;
			}

			switch (phase)
			{
			case 1:
				if (percentPassed == 2)
				{
					phase = 2;
				}
				break;
			}
		}
	};
};


/*
 * Copyright (C) 2023     Stormscale.Ru 
 * script compl = inc <Martynov.M> (c) 2023 stormscale project
 * boss_gen  worldboss hujal zone 
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
	NPC_SCHMORRSCHUPPEN = 90147
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

class boss_gen : public CreatureScript
{
public:
	boss_gen() : CreatureScript("boss_gen") { }

	struct boss_genAI : public ScriptedAI
	{
		boss_genAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

		void Reset() override
		{
			_events.Reset();
			Summons.DespawnAll();
		}

		void JustEngagedWith(Unit* /*who*/) override
		{
			me->Say("Я, темный Владыка Ген - само разрушение мира, само исчадие ада, и Вы посмели бросить мне вызов?", LANG_UNIVERSAL, 0);
			_events.SetPhase(PHASE_ONE);
			_events.ScheduleEvent(EVENT_COLD, Milliseconds(1000));
			_events.ScheduleEvent(EVENT_BEBENDE_ERDE, Milliseconds(10000));
			_events.ScheduleEvent(EVENT_WASTE, Milliseconds(8000));
						
		}

		void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
			if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
			{
				me->Say("Достаточно лишь взглянуть на ваши жалкие глаза, и мне становится только смешнее..На что Вы надеятесь?", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_TWO);
				_events.ScheduleEvent(EVENT_SLIME_BOLT, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_WASTE, Milliseconds(20000));
			
			}

			if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
			{
				me->Say("Темная сила, надели меня и моих слуг забвением темного племени, узрите всю мощь Темного Владыки!", LANG_UNIVERSAL, 0);
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
			
			me->Say("Это только начало...", LANG_UNIVERSAL, 0);
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
					me->Say("Темный Залп, узрите мощь!", LANG_UNIVERSAL, 0);
					DoCastVictim(SPELL_BLISTERING_COLD);
					_events.ScheduleEvent(EVENT_COLD, Milliseconds(30000));
					break;
				case EVENT_SUMMONS:
					me->Say("Слуши, придите на защиту своего владыки!", LANG_UNIVERSAL, 0);
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
		return new boss_genAI(creature);
	}



};

void AddSC_alliance_boss()
{
	new alliance_boss();
new boss_gen();
}