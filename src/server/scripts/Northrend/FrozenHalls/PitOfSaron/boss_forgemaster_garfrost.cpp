/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "pit_of_saron.h"

enum Texts
{
    SAY_AGGRO            = -1658001,
    SAY_SLAY_1           = -1658002,
    SAY_SLAY_2           = -1658003,
    SAY_DEATH            = -1658004,
    SAY_PHASE_2          = -1658005,
    SAY_PHASE_3          = -1658006,
	SAY_TYRANNUS_DEATH   = -1658007,
    EMOTE_THROW_SARONITE = -1658022,
    EMOTE_DEEP_FREEZE    = -1658023,
};

enum Spells
{
    SPELL_PERMAFROST            = 70326,
    SPELL_THROW_SARONITE        = 68788,
    SPELL_THUNDERING_STOMP      = 68771,
    SPELL_CHILLING_WAVE         = 68778,
    SPELL_DEEP_FREEZE           = 70381,
    SPELL_FORGE_MACE            = 68785,
    SPELL_FORGE_BLADE           = 68774,
	SPELL_SARONITE_ROCK_1       = 68792,
	SPELL_SARONITE_ROCK_2       = 68794,
	SPELL_SARONITE_ROCK_3       = 68795,
};

#define SPELL_PERMAFROST_HELPER RAID_MODE<uint32>(68786, 70336)
#define SPELL_FORGE_BLADE_HELPER RAID_MODE<uint32>(68774, 70334)

enum Phases
{
    PHASE_ONE                   = 1,
    PHASE_TWO                   = 2,
    PHASE_THREE                 = 3
};

enum MiscData
{
    EQUIP_ID_SWORD              = 49345,
    EQUIP_ID_MACE               = 49344,
    ACHIEV_DOESNT_GO_TO_ELEVEN  = 0,
    POINT_FORGE                 = 0
};

enum Events
{
    EVENT_THROW_SARONITE        = 1,
    EVENT_CHILLING_WAVE         = 2,
    EVENT_DEEP_FREEZE           = 3,
    EVENT_FORGE_JUMP            = 4,
    EVENT_RESUME_ATTACK         = 5
};

Position const northForgePos = { 722.5643f, -234.1615f, 527.182f, 2.16421f };
Position const southForgePos = { 639.257f, -210.1198f, 529.015f, 0.523599f };

const Position HordeSlave[26] =
{
	{726.79f, -167.496f, 526.693f, 4.75562f},
	{722.698f, -171.581f, 526.692f, 0.000807056f},
	{726.478f, -174.893f, 526.694f, 1.61941f},
	{718.572f, -204.756f, 527.136f, 5.37361f},
	{717.469f, -212.485f, 527.197f, 0.659648f},
	{725.492f, -211.398f, 527.342f, 2.43698f},
	{738.739f, -226.906f, 528.932f, 5.11362f},
	{744.045f, -230.674f, 528.932f, 3.42109f},
	{736.349f, -233.476f, 528.932f, 0.520089f},
	{721.94f, -244.143f, 528.932f, 4.66908f},
	{716.828f, -249.289f, 528.932f, 6.2311f},
	{695.013f, -239.476f, 526.718f, 2.31196f},
	{695.225f, -231.886f, 526.717f, 3.98093f},
	{687.86f,  -232.653f, 526.717f, 5.54344f},
	{678.071f, -234.056f, 528.932f, 2.39556f},
	{671.136f, -226.994f, 528.932f, 3.33019f},
	{664.349f, -219.779f, 528.932f, 3.92885f},
	{656.092f, -214.798f, 528.932f, 0.138507f},
	{645.967f, -222.02f, 528.932f, 3.40193f},
	{641.042f, -217.71f, 528.932f, 4.67506f},
	{635.453f, -203.079f, 528.932f, 2.98253f},
	{630.799f, -195.95f, 528.932f, 4.71826f},
	{633.7f, -188.839f, 531.282f, 2.2796f},
	{648.05f,-179.978f, 526.697f, 0.826612f},
	{657.106f, -180.588f, 526.693f, 2.42097f},
	{655.27f, -173.085f, 526.689f, 3.99491f},
};

const Position AllianceSlave[26] =
{
	{726.79f, -167.496f, 526.693f, 4.75562f},
	{722.698f, -171.581f, 526.692f, 0.000807056f},
	{726.478f, -174.893f, 526.694f, 1.61941f},
	{718.572f, -204.756f, 527.136f, 5.37361f},
	{717.469f, -212.485f, 527.197f, 0.659648f},
	{725.492f, -211.398f, 527.342f, 2.43698f},
	{738.739f, -226.906f, 528.932f, 5.11362f},
	{744.045f, -230.674f, 528.932f, 3.42109f},
	{736.349f, -233.476f, 528.932f, 0.520089f},
	{721.94f, -244.143f, 528.932f, 4.66908f},
	{716.828f, -249.289f, 528.932f, 6.2311f},
	{695.013f, -239.476f, 526.718f, 2.31196f},
	{695.225f, -231.886f, 526.717f, 3.98093f},
	{687.86f,  -232.653f, 526.717f, 5.54344f},
	{678.071f, -234.056f, 528.932f, 2.39556f},
	{671.136f, -226.994f, 528.932f, 3.33019f},
	{664.349f, -219.779f, 528.932f, 3.92885f},
	{656.092f, -214.798f, 528.932f, 0.138507f},
	{645.967f, -222.02f, 528.932f, 3.40193f},
	{641.042f, -217.71f, 528.932f, 4.67506f},
	{635.453f, -203.079f, 528.932f, 2.98253f},
	{630.799f, -195.95f, 528.932f, 4.71826f},
	{633.7f,  -188.839f, 531.282f, 2.2796f},
	{648.05f, -179.978f, 526.697f, 0.826612f},
	{657.106f, -180.588f, 526.693f, 2.42097f},
	{655.27f, -173.085f, 526.689f, 3.99491f},
};

class boss_garfrost : public CreatureScript
{
    public:
        boss_garfrost() : CreatureScript("boss_garfrost") { }

        struct boss_garfrostAI : public BossAI
        {
            boss_garfrostAI(Creature* creature) : BossAI(creature, DATA_GARFROST){}

			uint32 PermafrostStack;

			void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(PoSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->IsDead())
                    Reset();
            }

            void Reset()
            {
                _Reset();

				events.Reset();
                events.SetPhase(PHASE_ONE);

                SetEquipmentSlots(true);

				WrathboneSiegeSmithRespawn();

                PermafrostStack = 0;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				DoCast(me, SPELL_SARONITE_ROCK_1, true);
				DoCast(me, SPELL_SARONITE_ROCK_2, true);
				DoCast(me, SPELL_SARONITE_ROCK_3, true);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				if (GameObject* SaroniteRock = instance->instance->GetGameObject(instance->GetData64(GO_SARONITE_ROCK)))
					SaroniteRock->RemoveFromWorld();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                DoCast(me, SPELL_PERMAFROST);

                me->CallForHelp(75.0f);

                events.ScheduleEvent(EVENT_THROW_SARONITE, 7000);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void EnterEvadeMode()
			{
				if (!_EnterEvadeMode())
					return;

				me->GetMotionMaster()->MoveTargetedHome();
				Reset();
			}

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();

                DoSendQuantumText(SAY_DEATH, me);

				WrathboneSiegeSmithDespawn();

				TyrannusDeath();
				SummonSlaves();

				if (GameObject* SaroniteRock = instance->instance->GetGameObject(instance->GetData64(GO_SARONITE_ROCK)))
					SaroniteRock->RemoveFromWorld();
            }

			void WrathboneSiegeSmithRespawn()
			{
				std::list<Creature*> WrathboneSiegesmithList;
				me->GetCreatureListWithEntryInGrid(WrathboneSiegesmithList, NPC_WRATHBONE_SIEGESMITH, 500.0f);

				if (!WrathboneSiegesmithList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = WrathboneSiegesmithList.begin(); itr != WrathboneSiegesmithList.end(); ++itr)
					{
						if (Creature* creature = *itr)
						{
							creature->Respawn();
							creature->GetMotionMaster()->MoveTargetedHome();
						}
					}
				}
			}

			void WrathboneSiegeSmithDespawn()
			{
				std::list<Creature*> WrathboneSiegesmithList;
				me->GetCreatureListWithEntryInGrid(WrathboneSiegesmithList, NPC_WRATHBONE_SIEGESMITH, 500.0f);

				if (!WrathboneSiegesmithList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = WrathboneSiegesmithList.begin(); itr != WrathboneSiegesmithList.end(); ++itr)
					{
						if (Creature* creature = *itr)
							creature->Kill(creature);
					}
				}
			}

			void TyrannusDeath()
			{
				if (Creature* tyrannus = me->SummonCreature(NPC_SCOURGELORD_TYRANNUS_INTRO, 686.427f, -223.217f, 552.722f, 1.32728f, TEMPSUMMON_TIMED_DESPAWN, 20000))
					DoSendQuantumText(SAY_TYRANNUS_DEATH, tyrannus);
			}

			void SummonSlaves()
			{
				if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
				{
					for (uint8 i = 0; i < 26; i++)
						me->SummonCreature(NPC_HORDE_SLAVE_2, HordeSlave[i], TEMPSUMMON_MANUAL_DESPAWN);
				}
				else
				{
					for (uint8 i = 0; i < 26; i++)
						me->SummonCreature(NPC_ALLIANCE_SLAVE_2, AllianceSlave[i], TEMPSUMMON_MANUAL_DESPAWN);
				}
			}

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (events.IsInPhase(PHASE_ONE) && !HealthAbovePct(HEALTH_PERCENT_65))
                {
                    events.SetPhase(PHASE_TWO);
                    DoSendQuantumText(SAY_PHASE_2, me);
                    events.DelayEvents(8000);
                    DoCast(me, SPELL_THUNDERING_STOMP);
                    events.ScheduleEvent(EVENT_FORGE_JUMP, 1500);
                    return;
                }

                if (events.IsInPhase(PHASE_TWO) && !HealthAbovePct(HEALTH_PERCENT_35))
                {
                    events.SetPhase(PHASE_THREE);
                    DoSendQuantumText(SAY_PHASE_3, me);
                    events.DelayEvents(8000);
                    DoCast(me, SPELL_THUNDERING_STOMP);
                    events.ScheduleEvent(EVENT_FORGE_JUMP, 1500);
                    return;
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != EFFECT_MOTION_TYPE || id != POINT_FORGE)
                    return;

                if (events.IsInPhase(PHASE_TWO))
                {
                    DoCast(me, SPELL_FORGE_BLADE);
                    SetEquipmentSlots(false, EQUIP_ID_SWORD);
                }
                if (events.IsInPhase(PHASE_THREE))
                {
                    me->RemoveAurasDueToSpell(SPELL_FORGE_BLADE_HELPER);
                    DoCast(me, SPELL_FORGE_MACE);
                    SetEquipmentSlots(false, EQUIP_ID_MACE);
                }
                events.ScheduleEvent(EVENT_RESUME_ATTACK, 5000);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_PERMAFROST_HELPER)
                {
                    if (Aura* aura = target->GetAura(SPELL_PERMAFROST_HELPER))
                        PermafrostStack = std::max<uint32>(PermafrostStack, aura->GetStackAmount());
                }
            }

            uint32 GetData(uint32 /*type*/) const
            {
                return PermafrostStack;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THROW_SARONITE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                            {
								DoSendQuantumText(EMOTE_THROW_SARONITE, me, target);
                                DoCast(target, SPELL_THROW_SARONITE);
                            }
                            events.ScheduleEvent(EVENT_THROW_SARONITE, urand(12500, 20000));
                            break;
                        case EVENT_CHILLING_WAVE:
                            DoCast(me, SPELL_CHILLING_WAVE);
                            events.ScheduleEvent(EVENT_CHILLING_WAVE, 40000, 0, PHASE_TWO);
                            break;
                        case EVENT_DEEP_FREEZE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                            {
                                DoSendQuantumText(EMOTE_DEEP_FREEZE, me, target);
                                DoCast(target, SPELL_DEEP_FREEZE);
                            }
                            events.ScheduleEvent(EVENT_DEEP_FREEZE, 35000, 0, PHASE_THREE);
                            break;
                        case EVENT_FORGE_JUMP:
                            me->AttackStop();
                            if (events.IsInPhase(PHASE_TWO))
                                me->GetMotionMaster()->MoveJump(northForgePos, 25.0f, 15.0f, POINT_FORGE);
                            else if (events.IsInPhase(PHASE_THREE))
                                me->GetMotionMaster()->MoveJump(southForgePos, 25.0f, 15.0f, POINT_FORGE);
                            break;
                        case EVENT_RESUME_ATTACK:
                            if (events.IsInPhase(PHASE_TWO))
                                events.ScheduleEvent(EVENT_CHILLING_WAVE, 5000, 0, PHASE_TWO);
                            else if (events.IsInPhase(PHASE_THREE))
                                events.ScheduleEvent(EVENT_DEEP_FREEZE, 10000, 0, PHASE_THREE);
                            AttackStart(me->GetVictim());
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();

				if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 85)
				{
					EnterEvadeMode();
					return;
				}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_garfrostAI(creature);
        }
};

class spell_garfrost_permafrost : public SpellScriptLoader
{
    public:
        spell_garfrost_permafrost() : SpellScriptLoader("spell_garfrost_permafrost") { }

        class spell_garfrost_permafrost_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_garfrost_permafrost_SpellScript);

        public:
            spell_garfrost_permafrost_SpellScript()
            {
                prevented = false;
            }

        private:
            void PreventHitByLoS()
            {
                if (Unit* target = GetHitUnit())
                {
                    Unit* caster = GetCaster();
                    //Temporary Line of Sight Check
                    std::list<GameObject*> blockList;
                    caster->GetGameObjectListWithEntryInGrid(blockList, GO_SARONITE_ROCK, 100.0f);
                    if (!blockList.empty())
                    {
                        for (std::list<GameObject*>::const_iterator itr = blockList.begin(); itr != blockList.end(); ++itr)
                        {
                            if (!(*itr)->IsInvisibleDueToDespawn())
                            {
                                if ((*itr)->IsInBetween(caster, target, 4.0f))
                                {
                                    prevented = true;
                                    target->ApplySpellImmune(GetSpellInfo()->Id, IMMUNITY_ID, GetSpellInfo()->Id, true);
                                    PreventHitDefaultEffect(EFFECT_0);
                                    PreventHitDefaultEffect(EFFECT_1);
                                    PreventHitDefaultEffect(EFFECT_2);
                                    PreventHitDamage();
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            void RestoreImmunity()
            {
                if (Unit* target = GetHitUnit())
                {
                    target->ApplySpellImmune(GetSpellInfo()->Id, IMMUNITY_ID, GetSpellInfo()->Id, false);
                    if (prevented)
                        PreventHitAura();
                }
            }

            void Register()
            {
                BeforeHit += SpellHitFn(spell_garfrost_permafrost_SpellScript::PreventHitByLoS);
                AfterHit += SpellHitFn(spell_garfrost_permafrost_SpellScript::RestoreImmunity);
            }

            bool prevented;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_garfrost_permafrost_SpellScript();
        }
};

class achievement_doesnt_go_to_eleven : public AchievementCriteriaScript
{
    public:
        achievement_doesnt_go_to_eleven() : AchievementCriteriaScript("achievement_doesnt_go_to_eleven") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (target)
                if (Creature* garfrost = target->ToCreature())
                    if (garfrost->AI()->GetData(ACHIEV_DOESNT_GO_TO_ELEVEN) <= 10)
                        return true;

            return false;
        }
};

void AddSC_boss_garfrost()
{
    new boss_garfrost();
    new spell_garfrost_permafrost();
    new achievement_doesnt_go_to_eleven();
}