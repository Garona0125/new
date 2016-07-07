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
#include "the_botanica.h"

enum Says
{
    SAY_AGGRO          = -1441026,
    SAY_SLAY_1         = -1441027,
	SAY_SLAY_2         = -1441028,
    SAY_SUMMON_1       = -1441029,
	SAY_SUMMON_2       = -1441030,
    SAY_DEATH          = -1441031,
};

enum Spells
{
    SPELL_WAR_STOMP        = 34716,
    SPELL_SUMMON_TREANTS   = 34727, // DBC: 34727, 34731, 34733, 34734, 34736, 34739, 34741 (with Ancestral Life spell 34742)   // won't work (guardian summon)
    SPELL_ARCANE_VOLLEY_5N = 36705,
    SPELL_ARCANE_VOLLEY_5H = 39133,
    SPELL_HEAL_FATHER      = 6262,
};

enum Others
{
    TREANT_SPAWN_DIST  = 50,
};

float treant_pos[6][3] =
{
    {24.301233f, 427.221100f, -27.060635f},
    {16.795492f, 359.678802f, -27.355425f},
    {53.493484f, 345.381470f, -26.196192f},
    {61.867096f, 439.362732f, -25.921030f},
    {109.861877f, 423.201630f, -27.356019f},
    {106.780159f, 355.582581f, -27.593357f},
};

class npc_sapling : public CreatureScript
{
    public:
		npc_sapling(): CreatureScript("npc_sapling"){}

		struct npc_saplingAI  : public QuantumBasicAI
        {
            npc_saplingAI (Creature* creature) : QuantumBasicAI(creature)
            {
                WarpGuid = 0;
            }

            uint64 WarpGuid;
            uint32 CheckTimer;

            void Reset()
            {
                CheckTimer = 0;
            }

            void EnterToBattle(Unit* /*who*/) {}

            void MoveInLineOfSight(Unit* /*who*/) {}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                {
                    if (WarpGuid && CheckTimer <= diff)
                    {
                        if (Unit* warp = Unit::GetUnit(*me, WarpGuid))
                        {
                            if (me->IsWithinMeleeRange(warp, 2.5f))
                            {
                                int32 CurrentHP_Treant = (int32)me->GetHealth();
                                warp->CastCustomSpell(warp, SPELL_HEAL_FATHER, &CurrentHP_Treant, 0, 0, true, 0, 0, me->GetGUID());
                                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                                return;
                            }
                            me->GetMotionMaster()->MoveFollow(warp, 0, 0);
                        }
                        CheckTimer = 1000;
                    }
                    else CheckTimer -= diff;
                    return;
                }

                if (me->GetVictim()->GetGUID() !=  WarpGuid)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_saplingAI(creature);
        }
};

class boss_warp_splinter : public CreatureScript
{
    public:
        boss_warp_splinter() : CreatureScript("boss_warp_splinter"){}

        struct boss_warp_splinterAI : public BossAI
        {
            boss_warp_splinterAI(Creature* creature) : BossAI(creature, DATA_WARP_SPLINTER), Summons(me)
            {
                TreantSpawnPosX = creature->GetPositionX();
                TreantSpawnPosY = creature->GetPositionY();
            }

			uint32 ArcaneVolleyTimer;
            uint32 WarStompTimer;
            uint32 SummonTreantsTimer;

			SummonList Summons;

            float TreantSpawnPosX;
            float TreantSpawnPosY;

            void Reset()
            {
				ArcaneVolleyTimer = 500;
                WarStompTimer = 4000;
                SummonTreantsTimer = 8000;

				Summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
                me->SetSpeed(MOVE_RUN, 0.83f, true);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				Summons.DespawnAll();

                DoSendQuantumText(SAY_DEATH, me);
            }

			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == NPC_SAPLING)
					Summons.Summon(summon);
			}

            void SummonTreants()
            {
				DoSendQuantumText(RAND(SAY_SUMMON_1, SAY_SUMMON_2), me);

                for (uint8 i = 0; i < 6; ++i)
                {
                    float angle = (M_PI / 3)* i;

                    float X = TreantSpawnPosX + TREANT_SPAWN_DIST* std::cos(angle);
                    float Y = TreantSpawnPosY + TREANT_SPAWN_DIST* std::sin(angle);
                    float O = - me->GetAngle(X, Y);

                    if (Creature* sapling = me->SummonCreature(NPC_SAPLING, treant_pos[i][0], treant_pos[i][1], treant_pos[i][2], O, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 25000))
                        CAST_AI(npc_sapling::npc_saplingAI, sapling->AI())->WarpGuid = me->GetGUID();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (ArcaneVolleyTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_ARCANE_VOLLEY_5N, SPELL_ARCANE_VOLLEY_5H));
						ArcaneVolleyTimer = urand(3000, 4000);
					}
                }
                else ArcaneVolleyTimer -= diff;

                if (WarStompTimer <= diff)
                {
                    DoCastAOE(SPELL_WAR_STOMP);
                    WarStompTimer = urand(6000, 8000);
                }
                else WarStompTimer -= diff;

                if (SummonTreantsTimer <= diff)
                {
                    SummonTreants();
                    SummonTreantsTimer = urand(10000, 12000);
                }
                else SummonTreantsTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_warp_splinterAI(creature);
        }
};

void AddSC_boss_warp_splinter()
{
    new boss_warp_splinter();
    new npc_sapling();
}