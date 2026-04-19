
/*
 * Copyright (C) 2024     Stormscale.Ru 
 * script compl = inc <Martynov.M> (c) 2024 stormscale project
 *  
 * 
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "GridNotifiers.h"



enum Spells{
	SPELL_HEX = 66054,							// FLUCH
	SPELL_SHEEP = 59634,						// PENGUIN!
	SPELL_FLUCH_DER_PEIN = 65814,				// FLUCH alle 2 Sekunden 2k Schaden 
	SPELL_SPALTEN = 40504,						// bis zu 3 Ziele
	SPELL_SCHATTENWORT_SCHMERZ = 65541,			// MAGIE
	SPELL_FROSTFIEBER = 67767,					// KRANKHEIT
	SPELL_RUESTUNG_ZERREISSEN = 74367,			// stackbar bis 5x
	SPELL_VERDERBNIS = 65810,					// MAGIE
	SPELL_FLEISCH_EINAESCHERN = 66237,			// 30k muessen weggeheilt werden sonst explodiert der Spell und fuegt AOE Feuerschaden zu
	SPELL_ZAUBERSCHILD = 33054,					// 75% weniger Zauberschaden BUFF
	SPELL_WIRBELWIND = 40219,					// 6 Sekunden Wirbelwind
	SPELL_VERDERBENDE_SEUCHE = 60588,			// STERBT EINFACH
	SPELL_WUNDGIFT = 65962,						// GIFT
	SPELL_HAMMER_DER_GERECHTIGKEIT = 66613,		// STUN
	SPELL_GOETTLICHER_STURM = 66006				// Melee AOE auf 4 Targets, Waffenschaden+Heiligschaden


};

enum Events{
	EVENT_HEX = 1,
	EVENT_SHEEP = 2,
	EVENT_FLUCH_DER_PEIN = 3,
	EVENT_SPALTEN = 4,
	EVENT_SCHATTENWORT_SCHMERZ = 5,
	EVENT_FROSTFIEBER = 6,
	EVENT_RUESTUNG_ZERREISSEN = 7,
	EVENT_VERDERBNIS = 8,
	EVENT_FLEISCH_EINAESCHERN = 9,
	EVENT_ZAUBERSCHILD = 10,
	EVENT_WIRBELWIND = 11,
	EVENT_VERDERBENDE_SEUCHE = 12,
	EVENT_WUNDGIFT = 13,
	EVENT_HAMMER_DER_GERECHTIGKEIT = 14,
	EVENT_GOETTLICHER_STURM = 15
};

enum Phases{
	PHASE_ONE = 1,
	PHASE_TWO = 2,
	PHASE_THREE = 3,
	PHASE_FOUR = 4,
	PHASE_FIVE = 5
};

class anna : public CreatureScript{
public:
	anna() : CreatureScript("anna") { }

	struct annaAI : public ScriptedAI {

		annaAI(Creature* creature) : ScriptedAI(creature) {}

		void Reset() override
		{
			_events.Reset();
		}

		void JustEngagedWith(Unit* /*who*/) override
		{
			me->Say("Кто вторгся в эти земли?? Глупцы на что вы расчитываете?", LANG_UNIVERSAL, 0);
			_events.SetPhase(PHASE_ONE);
			_events.ScheduleEvent(EVENT_FLUCH_DER_PEIN, Milliseconds(1000));
			_events.ScheduleEvent(EVENT_SPALTEN, Milliseconds(10000));
			_events.ScheduleEvent(EVENT_FROSTFIEBER, Milliseconds(1000));
		}

		void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
			if (me->HealthBelowPctDamaged(80, damage) && _events.IsInPhase(PHASE_ONE))
			{
				me->Say("Как это ни странно, но я чувствую что мои силы растут, испытайте прилив моих сил!", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_TWO);
				_events.ScheduleEvent(EVENT_HEX, Milliseconds(0));
				_events.ScheduleEvent(EVENT_FLUCH_DER_PEIN, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_SPALTEN, Milliseconds(8000));
				_events.ScheduleEvent(EVENT_SCHATTENWORT_SCHMERZ, Milliseconds(10000));
			}

			if (me->HealthBelowPctDamaged(60, damage) && _events.IsInPhase(PHASE_TWO))
			{
				_events.SetPhase(PHASE_THREE);
				_events.ScheduleEvent(EVENT_VERDERBNIS, Milliseconds(500));
				_events.ScheduleEvent(EVENT_HEX, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_SHEEP, Milliseconds(1500));
				_events.ScheduleEvent(EVENT_FROSTFIEBER, Milliseconds(5000));
				_events.ScheduleEvent(EVENT_HAMMER_DER_GERECHTIGKEIT, Milliseconds(20000));
				_events.ScheduleEvent(EVENT_WIRBELWIND, Milliseconds(21000));
			}

			if (me->HealthBelowPctDamaged(40, damage) && _events.IsInPhase(PHASE_THREE))
			{
				_events.SetPhase(PHASE_FOUR);
				_events.ScheduleEvent(EVENT_HAMMER_DER_GERECHTIGKEIT, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_HEX, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_SHEEP, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_RUESTUNG_ZERREISSEN, Milliseconds(2000));
				_events.ScheduleEvent(EVENT_FLEISCH_EINAESCHERN, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_SCHATTENWORT_SCHMERZ, Milliseconds(12000));
				_events.ScheduleEvent(EVENT_WIRBELWIND, Milliseconds(15000));
			}

			if (me->HealthBelowPctDamaged(20, damage) && _events.IsInPhase(PHASE_FOUR))
			{
				_events.SetPhase(PHASE_FIVE);
				_events.ScheduleEvent(EVENT_ZAUBERSCHILD, Milliseconds(0));
				_events.ScheduleEvent(EVENT_RUESTUNG_ZERREISSEN, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_FLEISCH_EINAESCHERN, Milliseconds(2000));
				_events.ScheduleEvent(EVENT_VERDERBENDE_SEUCHE, Milliseconds(800));
				_events.ScheduleEvent(EVENT_WUNDGIFT, Milliseconds(1000));
				_events.ScheduleEvent(EVENT_GOETTLICHER_STURM, Milliseconds(15000));

			}
		}

		void JustDied(Unit* /*killer*/) override
		{
		me->Say("Я....еще...вернусь", LANG_UNIVERSAL, 0);
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

				case EVENT_HEX:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_HEX);
					}
					_events.ScheduleEvent(EVENT_HEX, Milliseconds(16000));
					break;

				case EVENT_SHEEP:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_SHEEP);
					}
					_events.ScheduleEvent(EVENT_SHEEP, Milliseconds(20000));
					break;

				case EVENT_FLUCH_DER_PEIN:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){	
						DoCast(target, SPELL_FLUCH_DER_PEIN);
					}
					_events.ScheduleEvent(EVENT_FLUCH_DER_PEIN, Milliseconds(12000));
					break;

				case EVENT_SPALTEN:
					DoCastVictim(SPELL_SPALTEN);
					_events.ScheduleEvent(EVENT_SPALTEN, Milliseconds(25000));
					break;

				case EVENT_SCHATTENWORT_SCHMERZ:
					DoCastToAllHostilePlayers(SPELL_SCHATTENWORT_SCHMERZ);
					_events.ScheduleEvent(EVENT_SCHATTENWORT_SCHMERZ, Milliseconds(20000));
					break;

				case EVENT_RUESTUNG_ZERREISSEN:
					DoCastVictim(SPELL_RUESTUNG_ZERREISSEN);
					_events.ScheduleEvent(EVENT_RUESTUNG_ZERREISSEN, Milliseconds(20000));
					break;

				case EVENT_VERDERBNIS:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_VERDERBNIS);
					}
					_events.ScheduleEvent(EVENT_VERDERBNIS, Milliseconds(15000));
					break;

				case EVENT_FLEISCH_EINAESCHERN:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_FLEISCH_EINAESCHERN);
					}
					_events.ScheduleEvent(EVENT_FLEISCH_EINAESCHERN, Milliseconds(15000));
					break;

				case EVENT_WIRBELWIND:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_WIRBELWIND);
					}
					_events.ScheduleEvent(EVENT_WIRBELWIND, Milliseconds(30000));
					break;

				case EVENT_VERDERBENDE_SEUCHE:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_VERDERBENDE_SEUCHE);
					}
					_events.ScheduleEvent(EVENT_VERDERBENDE_SEUCHE, Milliseconds(30000));
						break;

				case EVENT_WUNDGIFT:
					DoCastVictim(SPELL_WUNDGIFT);
					_events.ScheduleEvent(EVENT_WUNDGIFT, Milliseconds(10000));
					break;

				case EVENT_HAMMER_DER_GERECHTIGKEIT:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_HAMMER_DER_GERECHTIGKEIT);
					}
					_events.ScheduleEvent(EVENT_HAMMER_DER_GERECHTIGKEIT, Milliseconds(12000));
					break;

				case EVENT_GOETTLICHER_STURM:
					DoCastVictim(SPELL_GOETTLICHER_STURM);
					_events.ScheduleEvent(EVENT_GOETTLICHER_STURM, Milliseconds(20000));
					break;

				case EVENT_ZAUBERSCHILD:
					DoCast(me, SPELL_ZAUBERSCHILD);
					_events.ScheduleEvent(EVENT_ZAUBERSCHILD, Milliseconds(30000));
					break;

				default:
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap _events;
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new annaAI(creature);
	}

};

void AddSC_anna()
{
	new anna();
}