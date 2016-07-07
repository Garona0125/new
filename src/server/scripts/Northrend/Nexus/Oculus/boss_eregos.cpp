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
#include "oculus.h"

enum Texts
{
    SAY_AGGRO                       = -1578030,
	SAY_SLAY_1                      = -1578031,
    SAY_SLAY_2                      = -1578032,
    SAY_SLAY_3                      = -1578033,
	SAY_DEATH                       = -1578034,
    SAY_ARCANE_SHIELD               = -1578035,
    SAY_FIRE_SHIELD                 = -1578036,
    SAY_NATURE_SHIELD               = -1578037,
    SAY_FRENZY                      = -1578038,
    SAY_SPAWN                       = -1578039,
	EMOTE_ASTRAL_PLANE              = -1578040,
};

enum Spells
{
    SPELL_ARCANE_BARRAGE            = 50804,
    SPELL_ARCANE_BARRAGE_H          = 59381,
    SPELL_ARCANE_VOLLEY             = 51153,
    SPELL_ARCANE_VOLLEY_H           = 59382,
    SPELL_ENRAGED_ASSAULT           = 51170,
    SPELL_PLANAR_ANOMALIES          = 57959,
    SPELL_PLANAR_SHIFT              = 51162,
    SPELL_SUMMON_LEY_WHELP          = 51175,
    SPELL_PLANAR_ANOMALY_AGGRO      = 57971,
    SPELL_PLANAR_BLAST              = 57976,
    SPELL_PLANAR_DISTORTION         = 59379,
    SPELL_RUBY_DRAKE_SADDLE         = 49464,
    SPELL_EMERALD_DRAKE_SADDLE      = 49346,
    SPELL_AMBER_DRAKE_SADDLE        = 49460,
};

enum Events
{
    EVENT_ARCANE_BARRAGE   = 1,
    EVENT_ARCANE_VOLLEY    = 2,
    EVENT_ENRAGED_ASSAULT  = 3,
    EVENT_SUMMON_LEY_WHELP = 4,
};

enum Phases
{
    PHASE_NORMAL          = 1,
    PHASE_FIRST_PLANAR    = 2,
    PHASE_SECOND_PLANAR   = 3,
};

enum AchievementData
{
    DATA_RUBY_VOID    = 1,
    DATA_EMERALD_VOID = 2,
    DATA_AMBER_VOID   = 3,
};

enum Actions
{
    ACTION_SET_NORMAL_EVENTS = 1,
};

class boss_eregos : public CreatureScript
{
    public:
        boss_eregos() : CreatureScript("boss_eregos") { }

        struct boss_eregosAI : public BossAI
        {
            boss_eregosAI(Creature* creature) : BossAI(creature, DATA_EREGOS_EVENT) { }

			uint8 _phase;
            bool RubyVoid;
            bool EmeraldVoid;
            bool AmberVoid;

            uint32 SpawnTextTimer;

            void Reset()
            {
                _Reset();

                RubyVoid = true;
                EmeraldVoid = true;
                AmberVoid = true;

                _phase = PHASE_NORMAL;
                DoAction(ACTION_SET_NORMAL_EVENTS);

                SpawnTextTimer = urand (1, 60) *IN_MILLISECONDS;
            }

			void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

				CheckVoids();

                DoSendQuantumText(SAY_AGGRO, me);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

            void CheckVoids()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    Map::PlayerList const& Players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
                            if (player->IsGameMaster())
                                continue;

                            if (player->HasAura(SPELL_RUBY_DRAKE_SADDLE))
                                RubyVoid = false;
                            if (player->HasAura(SPELL_EMERALD_DRAKE_SADDLE))
                                EmeraldVoid = false;
                            if (player->HasAura(SPELL_AMBER_DRAKE_SADDLE))
                                AmberVoid = false;
                        }
                    }
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_RUBY_VOID:
                        return RubyVoid ? 1 : 0;
                    case DATA_EMERALD_VOID:
                        return EmeraldVoid ? 1 : 0;
                    case DATA_AMBER_VOID:
                        return AmberVoid ? 1 : 0;
                }

                return 0;
            }

            void DoAction(int32 const action)
            {
                if (action != ACTION_SET_NORMAL_EVENTS)
                    return;

                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3, 10) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_ARCANE_VOLLEY, urand(10, 25) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_ENRAGED_ASSAULT, urand(30, 45) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_SUMMON_LEY_WHELP, urand(15, 20) * IN_MILLISECONDS, 0, PHASE_NORMAL);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (!IsHeroic())
                    return;

                if ((me->GetHealthPct() < 60.0f && me->GetHealthPct() > 20.0f && _phase < PHASE_FIRST_PLANAR) || (me->GetHealthPct() < 20.0f && _phase < PHASE_SECOND_PLANAR))
                {
                    events.Reset();
                    _phase = (me->GetHealthPct() < 60.0f && me->GetHealthPct() > 20.0f) ? PHASE_FIRST_PLANAR : PHASE_SECOND_PLANAR;


                    // not sure about the amount, and if we should despawn previous spawns (dragon trashs)
                    summons.DespawnAll();

                    DoCast(me, SPELL_PLANAR_ANOMALIES, true);
                    DoCast(me, SPELL_PLANAR_SHIFT, true);
					DoSendQuantumText(RAND(SAY_ARCANE_SHIELD, SAY_FIRE_SHIELD, SAY_NATURE_SHIELD), me);
                    DoSendQuantumText(EMOTE_ASTRAL_PLANE, me);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!me->IsInCombatActive() && me->IsAlive())
                {
                    if (SpawnTextTimer <= diff)
                    {
                        DoSendQuantumText(SAY_SPAWN, me);
                        SpawnTextTimer = 60*IN_MILLISECONDS;
                    }
                    else SpawnTextTimer -= diff;
                }                
            
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(DUNGEON_MODE(SPELL_ARCANE_BARRAGE, SPELL_ARCANE_BARRAGE_H));
                            events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3, 10) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                            break;
                        case EVENT_ARCANE_VOLLEY:
                            DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_VOLLEY, SPELL_ARCANE_VOLLEY_H));
                            events.ScheduleEvent(EVENT_ARCANE_VOLLEY, urand(10, 25) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                            break;
                        case EVENT_ENRAGED_ASSAULT:
                            DoSendQuantumText(SAY_FRENZY, me);
                            DoCast(SPELL_ENRAGED_ASSAULT);
                            events.ScheduleEvent(EVENT_ENRAGED_ASSAULT, urand(35, 50) * IN_MILLISECONDS, 0, PHASE_NORMAL);
                            break;
                        case EVENT_SUMMON_LEY_WHELP:
                            for (uint8 i = 0; i < 3; ++i)
							{
                                DoCast(SPELL_SUMMON_LEY_WHELP);
							}
                            events.ScheduleEvent(EVENT_SUMMON_LEY_WHELP, 15*IN_MILLISECONDS, 0, PHASE_NORMAL);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
                _JustDied();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_eregosAI(creature);
        }
};

class npc_planar_anomaly : public CreatureScript
{
    public:
        npc_planar_anomaly() : CreatureScript("npc_planar_anomaly") { }

        struct npc_planar_anomalyAI : public QuantumBasicAI
        {
            npc_planar_anomalyAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->SetDisplayId(MODEL_ID_INVISIBLE);
                me->SetCanFly(true);

                DoCast(me, SPELL_PLANAR_ANOMALY_AGGRO, true);
                DoCast(me, SPELL_PLANAR_DISTORTION, true);
                ChaseTimer = 500;
                BlastTimer = 15000;
            }

			uint32 ChaseTimer;
            uint32 BlastTimer;

            void Reset()
            {
                DoZoneInCombat(me, 200.0f);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (ChaseTimer <= diff)
                {
                     if (me->ToTempSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            me->GetMotionMaster()->MoveFollow(summoner, 0.0f, 0.0f);

                    ChaseTimer = 20000;
                }
                else ChaseTimer -= diff;

                if (BlastTimer <= diff)
                {
                    DoCast(SPELL_PLANAR_BLAST);
                    me->RemoveAllAuras();
                    BlastTimer = 10000;
                }
                else BlastTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_planar_anomalyAI(creature);
        }
};

class spell_eregos_planar_shift : public SpellScriptLoader
{
    public:
        spell_eregos_planar_shift() : SpellScriptLoader("spell_eregos_planar_shift") { }

        class spell_eregos_planar_shift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eregos_planar_shift_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Creature* creatureCaster = caster->ToCreature())
                        creatureCaster->AI()->DoAction(ACTION_SET_NORMAL_EVENTS);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_eregos_planar_shift_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_eregos_planar_shift_AuraScript();
        }
};

class achievement_ruby_void : public AchievementCriteriaScript
{
public:
	achievement_ruby_void() : AchievementCriteriaScript("achievement_ruby_void") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_RUBY_VOID);

		return false;
	}
};

class achievement_emerald_void : public AchievementCriteriaScript
{
public:
	achievement_emerald_void() : AchievementCriteriaScript("achievement_emerald_void") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_EMERALD_VOID);

		return false;
	}
};

class achievement_amber_void : public AchievementCriteriaScript
{
public:
	achievement_amber_void() : AchievementCriteriaScript("achievement_amber_void") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_AMBER_VOID);

		return false;
	}
};

void AddSC_boss_eregos()
{
    new boss_eregos();
    new npc_planar_anomaly();
    new spell_eregos_planar_shift();
    new achievement_ruby_void();
    new achievement_emerald_void();
    new achievement_amber_void();
}