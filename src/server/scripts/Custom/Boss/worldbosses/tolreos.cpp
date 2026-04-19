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

enum Spells
{
	SPELL_CORRUPTION = 65810,
	SPELL_ENRAGE = 68335,
	SPELL_CRIPPLE = 31477,
	SPELL_ARCANE_BARRAGE = 65799,
	SPELL_DOMINATE_MIND = 63713,
	SPELL_EARTH = 30129,
	SPELL_HEX = 66054,
	SPELL_PSYCHOSIS = 63795,
	SPELL_ARCANE_DEVASTION = 34799,
	SPELL_ARMY_OF_DEAD = 67761,
	SPELL_SCHATTENFALLE = 73529
	
};

enum Events
{
	EVENT_CURRUPTION = 1,
	EVENT_ENRAGE = 2,
	EVENT_CRIPPLE = 3,
	EVENT_ARCANE_BARRAGE = 4,
	EVENT_DOMINATE_MIND = 5,
	EVENT_EARTH = 6,
	EVENT_HEX = 7,
	EVENT_PSYCHOSIS = 8,
	EVENT_ARCANE_DEVASTION = 9,
	EVENT_ARMY_OF_DEAD = 10,
	EVENT_SUMMONS = 11,
	EVENT_KILL = 12,
	EVENT_SCHATTENFALLE = 13

};

enum Phases
{
	PHASE_ONE = 1,
	PHASE_TWO = 2,
	PHASE_THREE = 3
};

enum Summons
{
	NPC_TOLREOSADD = 32772
};

enum Texts
{
	SAY_AGGRO = 0,
	SAY_RANDOM = 1,
	SAY_HELP = 2,
	SAY_BERSERK = 3,
	SAY_ENRAGE = 4,
	SAY_DEATH = 5,
	SAY_SLAY = 6
};




class tolreos : public CreatureScript
{
public:
	tolreos() : CreatureScript("tolreos") { }

	struct tolreosAI : public ScriptedAI
	{
		tolreosAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }
        uint32 kills;
		void Reset() override
		{
            kills = 0;
			me->SetObjectScale(1);
			me->SetReactState(REACT_DEFENSIVE);
			_events.Reset();
			Summons.DespawnAll();
			//me->SetCurrentEquipmentId(2193);
		}

		void JustEngagedWith(Unit*) override
		{
			            me->Say("Что за жалкие существа тут мешаются под ногами?Сейчас я Вам покажу!", LANG_UNIVERSAL, 0);
        

			_events.SetPhase(PHASE_ONE);
			_events.ScheduleEvent(EVENT_CURRUPTION, Milliseconds(8000));
			_events.ScheduleEvent(EVENT_CRIPPLE, Milliseconds(10000));
			_events.ScheduleEvent(EVENT_ARCANE_BARRAGE, Milliseconds(8000));
			_events.ScheduleEvent(EVENT_SUMMONS, Milliseconds(30000));


		}

		void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
			if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
			{
				
				//me->SetCurrentEquipmentId(2214);
				me->Say("Узри мощь великого Торлеоса!", LANG_UNIVERSAL, 0);
				me->SetObjectScale(2);
				_events.SetPhase(PHASE_TWO);
				_events.ScheduleEvent(EVENT_EARTH, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_HEX, Milliseconds(8000));
				_events.ScheduleEvent(EVENT_ARCANE_DEVASTION, Milliseconds(12000));
				_events.ScheduleEvent(EVENT_PSYCHOSIS, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_SUMMONS, Milliseconds(30000));
				
				

			}

			if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
			{
				me->Say("МОЩЬ И СИЛА , ВАМ НЕ ОДАЛЕТЬ МЕНЯ!", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_THREE);
				_events.ScheduleEvent(EVENT_ARMY_OF_DEAD, Milliseconds(5000));
				_events.ScheduleEvent(EVENT_CURRUPTION, Milliseconds(6000));
				_events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(25000));
				_events.ScheduleEvent(EVENT_HEX, Milliseconds(12000));
				_events.ScheduleEvent(EVENT_SUMMONS, Milliseconds(30000));
			}
		}


		void JustDied(Unit* /*pPlayer*/) override
		{
			me->Say("Это было так легко..", LANG_UNIVERSAL, 0);

			Summons.DespawnAll();
		}
		 
		void KilledUnit(Unit* victim) override
		{
			me->Say("Это.....не....возможно.....", LANG_UNIVERSAL, 0);
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
				case EVENT_CURRUPTION:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_CORRUPTION);
					}
					_events.ScheduleEvent(EVENT_CURRUPTION, Milliseconds(10000));
					break;
				case EVENT_ENRAGE:
					DoCastToAllHostilePlayers(SPELL_ENRAGE);
					break;
				case EVENT_CRIPPLE:
					Talk(SAY_RANDOM);
					DoCastToAllHostilePlayers(SPELL_CRIPPLE);
					_events.ScheduleEvent(EVENT_CRIPPLE, Milliseconds(25000));
					break;
				case EVENT_ARCANE_BARRAGE:
					DoCastToAllHostilePlayers(SPELL_ARCANE_BARRAGE);
					_events.ScheduleEvent(EVENT_ARCANE_BARRAGE, Milliseconds(5000));
					break;
				case EVENT_SUMMONS:
					Talk(SAY_HELP);
					me->SummonCreature(NPC_TOLREOSADD, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() + 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
					_events.ScheduleEvent(EVENT_SUMMONS, Milliseconds(45000));
					break;
				case EVENT_EARTH:
					Talk(SAY_ENRAGE);
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_EARTH);
					}
					_events.ScheduleEvent(EVENT_EARTH, Milliseconds(10000));
					break;
				case EVENT_PSYCHOSIS:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_PSYCHOSIS);
					}	
					_events.ScheduleEvent(EVENT_PSYCHOSIS, Milliseconds(18000));
					break;
				case EVENT_HEX:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target,SPELL_HEX);
					}
					_events.ScheduleEvent(EVENT_HEX, Milliseconds(10000));
					break;
				case EVENT_ARCANE_DEVASTION:
					DoCastVictim(SPELL_ARCANE_DEVASTION);
					_events.ScheduleEvent(EVENT_ARCANE_DEVASTION, Milliseconds(12000));
					break;
				case EVENT_ARMY_OF_DEAD:
					
					DoCastToAllHostilePlayers(SPELL_ARMY_OF_DEAD);
					_events.ScheduleEvent(EVENT_ARMY_OF_DEAD, Milliseconds(20000));
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
		return new tolreosAI(creature);
	}



};


#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "Item.h"
#include "SpellInfo.h"
#include "ObjectMgr.h"
#include "Chat.h"  // Для уведомлений игроку
 
// Скрипт для заражения игрока при луте предметов
class ItemInfectionSystem : public PlayerScript
{
public:
    ItemInfectionSystem() : PlayerScript("ItemInfectionSystem") { }
 
    // Хук на событие, когда игрок лутает предмет
    void OnPlayerLootItem(Player* player, Item* item, uint32 /*count*/, ObjectGuid /*lootGuid*/) override
    {
        // Список ID заклинаний, которые могут "заразить" игрока
        std::vector<uint32> infectionSpells = { 60842, 30843, 16867, 31977, 3269, 69391, 51442 }; // Замените на реальные ID заклинаний
 
        // 25% шанс заразить предмет
        if (urand(1, 100) <= 1)
        {
            // Выбираем случайное заклинание из списка
            uint32 selectedSpell = infectionSpells[urand(0, infectionSpells.size() - 1)];
 
            
                player->AddAura(selectedSpell, player);
                // Сообщаем игроку о том, что предмет заражен
                ChatHandler(player->GetSession()).SendSysMessage("При сборе добычи, Вы были заражены неизвестной чумой, в следуюзий раз буд-те предельно внимальны!");
            
        }
    }
};
 
#include "ScriptMgr.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Player.h"

enum Spellss
{
    SPELL_TARGET_ATTACK = 47467,     // Замените на фактический ID заклинания
    SPELL_DIRECTED_ATTACK = 70305,   // Замените на фактический ID заклинания
    SPELL_AOE_ATTACK = 41524,        // Замените на фактический ID заклинания
    SPELL_RANDOM_ATTACK = 71377,     // Замените на фактический ID заклинания

    // Заклинания второй фазы
    SPELL_PHASE_TWO_ATTACK1 = 62576, // Замените на фактический ID заклинания
    SPELL_PHASE_TWO_ATTACK2 = 26546, // Замените на фактический ID заклинания
    SPELL_PHASE_TWO_ATTACK3 = 66882, // Замените на фактический ID заклинания

    // ID призываемого НПС и заклинание для убийства игроков в радиусе 50 метров
    NPC_SUMMON_ID = 34606,          // Замените на ID НПС
    KILL_PLAYERS_SPELL = 265       // Замените на заклинание, убивающее игроков
};

enum Eventss
{
    EVENT_TARGET_ATTACK = 1,
    EVENT_DIRECTED_ATTACK,
    EVENT_AOE_ATTACK,
    EVENT_RANDOM_ATTACK,
    EVENT_SUMMON_NPCS,
    EVENT_CHECK_NPCS,
    EVENT_PHASE_TWO_ATTACK1,
    EVENT_PHASE_TWO_ATTACK2,
    EVENT_PHASE_TWO_ATTACK3
};

class boss_last : public CreatureScript
{
public:
    boss_last() : CreatureScript("boss_last") { }

    struct boss_lastAI : public ScriptedAI
    {
        boss_lastAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

        EventMap events;
        SummonList summons;
        bool phaseTwo;

        void Reset() override
        {
            events.Reset();
            summons.DespawnAll();
            phaseTwo = false;
        }

        void JustEngagedWith(Unit*) override

        {
		 me->Say("Что за жалкие существа тут мешаются под ногами?Сейчас я Вам покажу!", LANG_UNIVERSAL, 0);

            // События первой фазы запускаются при входе в бой
            events.ScheduleEvent(EVENT_SUMMON_NPCS, Milliseconds(10000));       // Призыв НПС через 10 секунд
            events.ScheduleEvent(EVENT_TARGET_ATTACK, Milliseconds(5000));      // Атака цели через 5 секунд
            events.ScheduleEvent(EVENT_DIRECTED_ATTACK, Milliseconds(8000));    // Направленное заклинание через 8 секунд
            events.ScheduleEvent(EVENT_AOE_ATTACK, Milliseconds(12000));        // АоЕ заклинание через 12 секунд
            events.ScheduleEvent(EVENT_RANDOM_ATTACK, Milliseconds(15000));     // Рандомное заклинание через 15 секунд
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            summons.Despawn(summon);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            // Переход на вторую фазу при достижении 60% здоровья
            if (!phaseTwo && HealthBelowPct(60))
            {
                phaseTwo = true;
                events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK1, Milliseconds(5000));  // Новое заклинание 1 через 5 секунд
                events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK2, Milliseconds(10000)); // Новое заклинание 2 через 10 секунд
                events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK3, Milliseconds(15000)); // Новое заклинание 3 через 15 секунд
                me->Say("Вы не понимаете истинной силы!", LANG_UNIVERSAL, 0);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            // Выполнение событий
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SUMMON_NPCS:
                        for (int i = 0; i < 3; ++i)
                        {
                            me->SummonCreature(NPC_SUMMON_ID, me->GetPositionX() + frand(-5.0f, 5.0f), me->GetPositionY() + frand(-5.0f, 5.0f), me->GetPositionZ(), TEMPSUMMON_CORPSE_DESPAWN);
                        }
                        events.ScheduleEvent(EVENT_CHECK_NPCS, Milliseconds(15000)); // Проверка НПС через 15 секунд
                        break;
					case EVENT_CHECK_NPCS:
                        if (!summons.empty())
                        {
                            DoCast(me, KILL_PLAYERS_SPELL); // Убийство игроков, если НПС не были убиты
                        }
                        break;

                    case EVENT_TARGET_ATTACK:
                        DoCastVictim(SPELL_TARGET_ATTACK);
                        events.ScheduleEvent(EVENT_TARGET_ATTACK, Milliseconds(5000)); // Повторение атаки на цель каждые 5 секунд
                        break;

                    case EVENT_DIRECTED_ATTACK:
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                            DoCast(target, SPELL_DIRECTED_ATTACK);
                        events.ScheduleEvent(EVENT_DIRECTED_ATTACK, Milliseconds(8000));
                        break;

                    case EVENT_AOE_ATTACK:
                        DoCastAOE(SPELL_AOE_ATTACK);
                        events.ScheduleEvent(EVENT_AOE_ATTACK, Milliseconds(12000));
                        break;

                    case EVENT_RANDOM_ATTACK:
                        if (Unit* randomTarget = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                            DoCast(randomTarget, SPELL_RANDOM_ATTACK);
                        events.ScheduleEvent(EVENT_RANDOM_ATTACK, Milliseconds(15000));
                        break;

                    case EVENT_PHASE_TWO_ATTACK1:
                        DoCastVictim(SPELL_PHASE_TWO_ATTACK1);
                        events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK1, Milliseconds(7000)); // Перезарядка 7 секунд
                        break;

                    case EVENT_PHASE_TWO_ATTACK2:
                        DoCastAOE(SPELL_PHASE_TWO_ATTACK2);
                        events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK2, Milliseconds(14000)); // Перезарядка 14 секунд
                        break;

                    case EVENT_PHASE_TWO_ATTACK3:
                        DoCast(me, SPELL_PHASE_TWO_ATTACK3);
                        events.ScheduleEvent(EVENT_PHASE_TWO_ATTACK3, Milliseconds(20000)); // Перезарядка 20 секунд
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/) override
        {
            summons.DespawnAll();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_lastAI(creature);
    }
};


void AddSC_tolreos()
{
	new tolreos();
	new ItemInfectionSystem();
	new boss_last();
}
