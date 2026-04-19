/*
 * Copyright (C) 2023     Stormscale.Ru 
 * script compl = inc <Martynov.M> (c) 2023 stormscale project
 * boss_orrig  worldboss hujal zone 
 * need test and how to kill :D
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "GridNotifiers.h"



enum Spells
{
	SPELL_SHADOW_SPIKE = 46589,
	SPELL_FEAR = 68950,
	SPELL_CHAIN_LIGHTNING = 33665,
	SPELL_ENRAGE = 68335,
	SPELL_BLIZZARD = 26607,
	SPELL_ARCANE_BOMB = 56431,
	SPELL_ACID_BLAST = 75637,
	SPELL_POISON_SHOCK = 28741,

};

enum Events
{
	EVENT_SHADOW_SPIKE = 1,
	EVENT_ENRAGE = 2,
	EVENT_FEAR = 3,
	EVENT_CHAIN_LIGHTNING = 4,
	EVENT_BLIZZARD = 5,
	EVENT_ARCANE_BOMB = 6,
	EVENT_ACID_BLAST = 7,
	EVENT_POISON_SHOCK = 8



};

enum Phases
{
	PHASE_ONE = 1,
	PHASE_TWO = 2,
	PHASE_THREE = 3
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

class orrig : public CreatureScript
{
public:
	orrig() : CreatureScript("orrig") { }

	struct orrigAI : public ScriptedAI
	{
		orrigAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

		uint32 kills = 0;
		void Reset() override
		{
			_events.Reset();
			Summons.DespawnAll();
			
		}

		void JustEngagedWith(Unit* /*who*/) override
		{
			me->Say("Смертные, вы действительно думаете что сможете меня одалеть?", LANG_UNIVERSAL, 0);
			_events.SetPhase(PHASE_ONE);
			_events.ScheduleEvent(EVENT_SHADOW_SPIKE, Milliseconds(8000));
			_events.ScheduleEvent(EVENT_FEAR, Milliseconds(10000));
			_events.ScheduleEvent(EVENT_BLIZZARD, Milliseconds(30000));
		
		}

		void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
		{
			if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
			{
				me->Say("Все еще думаете что сможете? сейчас посмотрим как вы справитесь!", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_TWO);
				_events.ScheduleEvent(EVENT_POISON_SHOCK, Milliseconds(10000));
				_events.ScheduleEvent(EVENT_ACID_BLAST, Milliseconds(15000));
				_events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, Milliseconds(30000));
				_events.ScheduleEvent(EVENT_FEAR, Milliseconds(45000));

			}

			if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
			{
				me->Say("Это на все что вы способны? узрите всю мою силу!!", LANG_UNIVERSAL, 0);
				_events.SetPhase(PHASE_THREE);
				_events.ScheduleEvent(EVENT_ARCANE_BOMB, Milliseconds(5000));
				_events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(100000));
				_events.ScheduleEvent(EVENT_ACID_BLAST, Milliseconds(16000));
				_events.ScheduleEvent(EVENT_ENRAGE, Milliseconds(200000));
				_events.ScheduleEvent(EVENT_BLIZZARD, Milliseconds(45000));
			}
		}

		


		void JustDied(Unit* /*pPlayer*/) override
		{
			me->Say("Неееееет, это не возможно.....как..вы....аааа", LANG_UNIVERSAL, 0);
            return;
		}


		void KilledUnit(Unit* victim) override
		{
			
			if (victim->GetTypeId() != TYPEID_PLAYER)
				return;
			char msg[250];
			
			++kills;
			me->Say("Жалкая букашка, Я заберу твою душу!", LANG_UNIVERSAL, 0);
            return;
			
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
				case EVENT_SHADOW_SPIKE:
					DoCastAOE(SPELL_SHADOW_SPIKE);
					break;
				case EVENT_FEAR:
					DoCastToAllHostilePlayers(SPELL_FEAR);
					_events.ScheduleEvent(EVENT_FEAR, Milliseconds(8000));
					break;
				case EVENT_ENRAGE:
					Talk(SAY_RANDOM);
					DoCast(SPELL_ENRAGE);
					break;
				case EVENT_BLIZZARD:
					if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true)){
						DoCast(target, SPELL_BLIZZARD);
					}
					_events.ScheduleEvent(EVENT_BLIZZARD, Milliseconds(15000));
					break;
				case EVENT_POISON_SHOCK:
					Talk(SAY_BERSERK);
					DoCastVictim(SPELL_POISON_SHOCK);
					_events.ScheduleEvent(EVENT_POISON_SHOCK, Milliseconds(12000));
					break;
				case EVENT_ARCANE_BOMB:
					Talk(SAY_ENRAGE);
					DoCast(SPELL_ARCANE_BOMB);
					_events.ScheduleEvent(EVENT_ARCANE_BOMB, Milliseconds(10000));
					break;
				case EVENT_ACID_BLAST:
					DoCastVictim( SPELL_ACID_BLAST);
					_events.ScheduleEvent(EVENT_ACID_BLAST, Milliseconds(15000));
					break;
				case EVENT_CHAIN_LIGHTNING:
					DoCastToAllHostilePlayers(SPELL_CHAIN_LIGHTNING);
					_events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, Milliseconds(10000));
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
		return new orrigAI(creature);
	}



};




// ID Зоны и ID баффов на определенные зоны, можно добавить хоть 1000%,  дается бафф только в определнной зоне , которую выставишь!
#include <map>
#include <vector>

class buff_zone : public PlayerScript
{
public:
    buff_zone() : PlayerScript("buff_zone") {}

    void OnPlayerUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
    {
       LoadZoneBuffs(); // Загружаем данные при создании скрипта
        // Получаем баффы для текущей зоны
        auto it = zoneBuffs.find(newZone);
        if (it != zoneBuffs.end())
        {
            uint32 buffId = it->second;
            player->AddAura(buffId, player);
        }
        else
        {
            // Удаляем все баффы, если зона не найдена
            for (const auto& pair : zoneBuffs)
            {
                player->RemoveAurasDueToSpell(pair.second);
            }
        }
    }

    void LoadZoneBuffs()
    {
        // Загрузка данных из базы данных
        QueryResult result = WorldDatabase.Query("SELECT zone_id, buff_id FROM zone_buffs");
        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 zoneId = fields[0].Get<uint32>();
                uint32 buffId = fields[1].Get<uint32>();
                zoneBuffs[zoneId] = buffId;
            } while (result->NextRow());
        }
    }

private:
    std::map<uint32, uint32> zoneBuffs; // Хранит соответствия между зонами и баффами
};

// Не забудьте вызвать LoadZoneBuffs() в нужном месте, например, в инициализации вашего скрипта.    


void AddSC_orrig()
{
	new orrig();
new buff_zone();
}
