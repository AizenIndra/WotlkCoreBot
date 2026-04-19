// Guldan World PvE Boss by Stormscale.Ru //
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "ObjectMgr.h"
#include "Player.h"


enum Texts
{
    SAY_AGGRO                                     = 0,
    SAY_SLAY                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_IMPENDING_DESPAIR                         = 3,
    SAY_DEFILING_HORROR                           = 4,
    SAY_SHADOW_NOVA                               = 5,
    SAY_DECIMATE                                  = 6,
    SAY_ZOMBIES                                   = 7,
    SAY_PLAGUED_ZOMBIES                           = 8
};

enum Spells
{
    SPELL_QUIVERING_STRIKE                        = 72422, //attack
    SPELL_IMPENDING_DESPAIR                       = 72426, //stun
    SPELL_DEFILING_HORROR                         = 72435, //bun fear
    SPELL_HOPELESSNESS_1                          = 72395, //bun curse
    SPELL_HOPELESSNESS_2                          = 72396, //bun curse
    SPELL_HOPELESSNESS_3                          = 72397, //bun curse
    SPELL_SHADOW_NOVA                             = 36127, //boom nasol trebuie rar
    SPELL_DECIMATE                                = 71123, //15% hp down
    SPELL_AWAKEN_PLAGUED_ZOMBIES                  = 71159 //spawn Zombies
};

enum Events
{
    EVENT_NONE,
    EVENT_QUIVERING_STRIKE,
    EVENT_IMPENDING_DESPAIR,
    EVENT_DEFILING_HORROR,
    EVENT_VOID_ZONE,
    EVENT_SHADOW_NOVA,
    EVENT_SUMMON_ZOMBIES
};

uint32 const HopelessnessHelper[3] = { SPELL_HOPELESSNESS_1, SPELL_HOPELESSNESS_2, SPELL_HOPELESSNESS_3 };

#define FALRIC_BOSS_ID 90109

//Zombalau - will rename later :)
class CustomBossFalric : public CreatureScript
{
    public:
        CustomBossFalric() : CreatureScript("CustomBossFalric") { }

        struct boss_falricAI : public BossAI
        {
            boss_falricAI(Creature* creature) : BossAI(creature, FALRIC_BOSS_ID)
            {
                Initialize();
            }

            void Initialize()
            {
                _hopelessnessCount = 0;
            }

            void Reset() override
            {
                BossAI::Reset();
                Initialize();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->PlayDirectSound(8621);
                events.ScheduleEvent(EVENT_QUIVERING_STRIKE, Milliseconds(9000));
                events.ScheduleEvent(EVENT_IMPENDING_DESPAIR, Milliseconds(15000));
                events.ScheduleEvent(EVENT_DEFILING_HORROR, Milliseconds(90000));
                events.ScheduleEvent(EVENT_SHADOW_NOVA, Milliseconds(120000));
                events.ScheduleEvent(EVENT_SUMMON_ZOMBIES, Milliseconds(urand(20000, 22000)));
            }

            void DoAction(int32 action) override
            {
            }

            void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
            {
                if ((_hopelessnessCount < 1 && me->HealthBelowPctDamaged(66, damage))
                    || (_hopelessnessCount < 2 && me->HealthBelowPctDamaged(33, damage))
                    || (_hopelessnessCount < 3 && me->HealthBelowPctDamaged(10, damage)))
                {
                    if (_hopelessnessCount)
                        me->RemoveOwnedAura(sSpellMgr->GetSpellIdForDifficulty(HopelessnessHelper[_hopelessnessCount - 1], me));
                    DoCast(me, HopelessnessHelper[_hopelessnessCount]);
                    ++_hopelessnessCount;
                }
            }

            void JustDied(Unit* )
		{
		 me->PlayDirectSound(8619);
		 me->Say("Моя смерть никак не повлияет на ситуацию в этом проклятом мире..", LANG_UNIVERSAL, 0);

		}

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_QUIVERING_STRIKE:
                        DoCastVictim(SPELL_QUIVERING_STRIKE);
                        events.ScheduleEvent(EVENT_QUIVERING_STRIKE, Milliseconds(9000));
                        break;
                    case EVENT_IMPENDING_DESPAIR:
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                        {
                            Talk(SAY_IMPENDING_DESPAIR);
                            DoCast(target, SPELL_IMPENDING_DESPAIR);
                        }
                        events.ScheduleEvent(EVENT_IMPENDING_DESPAIR, Milliseconds(15000));
                        break;
                    case EVENT_DEFILING_HORROR:
                            DoCastAOE(SPELL_DEFILING_HORROR);
                            events.ScheduleEvent(EVENT_DEFILING_HORROR, Milliseconds(90000));
                        break;
                    case EVENT_SHADOW_NOVA:
                            DoCastVictim(SPELL_SHADOW_NOVA, true);
                            Talk(SAY_SHADOW_NOVA);
                            events.ScheduleEvent(EVENT_SHADOW_NOVA, Milliseconds(120000));
                        break;
                    case EVENT_SUMMON_ZOMBIES:
                        Talk(SAY_PLAGUED_ZOMBIES);
                        for (uint32 i = 0; i < 2; ++i)
                            DoCast(me, SPELL_AWAKEN_PLAGUED_ZOMBIES, false);
                        events.ScheduleEvent(EVENT_SUMMON_ZOMBIES, Milliseconds(urand(60000, 120000)));
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            uint8 _hopelessnessCount;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_falricAI(creature);
        }
};

#include "ScriptMgr.h"
#include "Creature.h"
#include "CreatureAIImpl.h"
#include "SpellAuraEffects.h"

enum Spellss
{
    INITIAL_AURA = 70768,        // ID начальной ауры для босса
    ATTACK_SPELL_1 = 72504,      // ID первого атакующего заклинания
    ATTACK_SPELL_2 = 71480,      // ID второго атакующего заклинания
    FINAL_PHASE_SPELL = 72008    // ID заклинания, которое босс кастует на финальной фазе
};

enum Npcs
{
    MINION_1 = 91059,            // ID первого прислужника
    MINION_2 = 91262,            // ID второго прислужника
    MINION_3 = 91259             // ID третьего прислужника
};

class stormfortress_boss_two : public CreatureScript
{
public:
    stormfortress_boss_two() : CreatureScript("stormfortress_boss_two") { }

    struct stormfortress_boss_twoAI : public ScriptedAI
    {
        stormfortress_boss_twoAI(Creature* creature) : ScriptedAI(creature), minionCounter(0), inFinalPhase(false) { }

        void Reset() override
        {
            minionCounter = 0;
            inFinalPhase = false;
            ApplyInitialAura();
            summonedMinionGUID.Clear();
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            me->Say("кххммхаха, не званные гости.. убить их!! Сожрать их!!!", LANG_UNIVERSAL, 0);
            ApplyInitialAura();
            SummonNextMinion();

            // Босс взлетает в начале боя
            me->SetDisableGravity(true);
            me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10.0f);  // Поднимаем босса на 10 метров
        }

        void JustDied(Unit* /*killer*/) override
        {
            me->RemoveAurasDueToSpell(INITIAL_AURA);
        }

        void JustSummoned(Creature* summoned) override
        {
            summonedMinionGUID = summoned->GetGUID();
        }

        void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/) override
        {
            if (summoned->GetGUID() == summonedMinionGUID)
            {
                SummonNextMinion();
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            // Если босс в финальной фазе, используем заклинания атаки
            if (inFinalPhase)
            {
                // Дополнительное заклинание на финальной фазе
                DoCastVictim(FINAL_PHASE_SPELL);

                if (events.ExecuteEvent() == 1)
                {
                    DoCastVictim(ATTACK_SPELL_1);
                    events.ScheduleEvent(1, Milliseconds(5000));  // Кастует каждое 5 секунд
                }
                else if (events.ExecuteEvent() == 2)
                {
                    DoCastVictim(ATTACK_SPELL_2);
                    events.ScheduleEvent(2, Milliseconds(7000));  // Кастует каждое 7 секунд
                }
            }
            else
            {
                DoMeleeAttackIfReady();
            }
        }

    private:
        uint32 minionCounter;
        ObjectGuid summonedMinionGUID;
        bool inFinalPhase;
        EventMap events;

        void ApplyInitialAura()
        {
            me->CastSpell(me, INITIAL_AURA, true);
        }

        void SummonNextMinion()
        {
            switch (++minionCounter)
            {
                case 1:
                    me->SummonCreature(MINION_1, me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    break;
                case 2:
                    me->SummonCreature(MINION_2, me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    break;
                case 3:
                    me->SummonCreature(MINION_3, me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                    break;
                default:
                    StartFinalPhase();
                    break;
            }
        }
void StartFinalPhase()
        {
            inFinalPhase = true;
            me->RemoveAurasDueToSpell(INITIAL_AURA);
            me->SetHealth(me->CountPctFromMaxHealth(30));  // Устанавливаем здоровье босса на 30% при переходе на финальную фазу
            me->SetDisableGravity(false);                  // Босс приземляется на финальной фазе
            me->GetMotionMaster()->MovePoint(2, me->GetPositionX(), me->GetPositionY(), me->GetMap()->GetHeight(me->GetPhaseMask(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ())); // Приземляем босса

            // Запускаем два атакующих заклинания с задержками
            events.ScheduleEvent(1, Milliseconds(5000));  // Первое заклинание через 5 секунд
            events.ScheduleEvent(2, Milliseconds(7000));  // Второе заклинание через 7 секунд
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new stormfortress_boss_twoAI(creature);
    }
};



void AddSC_CustomBossFalricScripts()
{
    new CustomBossFalric();
new stormfortress_boss_two();

}