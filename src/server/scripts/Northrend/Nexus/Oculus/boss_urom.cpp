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
#include "oculus.h"

enum Yells
{
    SAY_AGGRO         = -1578012,
    SAY_SLAY_1        = -1578013,
    SAY_SLAY_2        = -1578014,
    SAY_SLAY_3        = -1578015,
    SAY_DEATH         = -1578016,
    SAY_EXPLOSION_1   = -1578017,
    SAY_EXPLOSION_2   = -1578018,
    SAY_SUMMON_1      = -1578019,
    SAY_SUMMON_2      = -1578020,
    SAY_SUMMON_3      = -1578021,
};

enum Spells
{
    SPELL_EMPOWERED_ARCANE_EXPLOSION_5N           = 51110,
    SPELL_EMPOWERED_ARCANE_EXPLOSION_5H           = 59377,
    SPELL_FROSTBOMB                               = 51103, //Urom throws a bomb, hitting its target with the highest aggro which inflict directly 650 frost damage and drops a frost zone on the ground. This zone deals 650 frost damage per second and reduce the movement speed by 35%. Lasts 1 minute.
    SPELL_FROST_BUFFET                            = 58025,
    SPELL_SUMMON_MENAGERIE                        = 50476, //Summons an assortment of creatures and teleports the caster to safety.
    SPELL_SUMMON_MENAGERIE_2                      = 50495,
    SPELL_SUMMON_MENAGERIE_3                      = 50496,
    SPELL_TELEPORT                                = 51112, //Teleports to the center of Oculus
    SPELL_TIME_BOMB_5N                            = 51121, //Deals arcane damage to a random player, and after 6 seconds, deals zone damage to nearby equal to the health missing of the target afflicted by the debuff.
    SPELL_TIME_BOMB_5H                            = 59376
};

struct Summons
{
    uint32 entry[4];
};

static Summons Group[] =
{
    {{NPC_PHANTASMAL_CLOUDSCRAPER, NPC_PHANTASMAL_CLOUDSCRAPER, NPC_PHANTASMAL_MAMMOTH, NPC_PHANTASMAL_WOLF}},
    {{NPC_PHANTASMAL_AIR, NPC_PHANTASMAL_AIR, NPC_PHANTASMAL_WATER, NPC_PHANTASMAL_FIRE}},
    {{NPC_PHANTASMAL_OGRE, NPC_PHANTASMAL_OGRE, NPC_PHANTASMAL_NAGAL, NPC_PHANTASMAL_MURLOC}}
};

static uint32 TeleportSpells[] =
{
    SPELL_SUMMON_MENAGERIE, SPELL_SUMMON_MENAGERIE_2, SPELL_SUMMON_MENAGERIE_3
};

static int32 SayAggro[] =
{
    SAY_SUMMON_1, SAY_SUMMON_2, SAY_SUMMON_3, SAY_AGGRO
};

class boss_urom : public CreatureScript
{
    public:
        boss_urom() : CreatureScript("boss_urom") { }

        struct boss_uromAI : public BossAI
        {
            boss_uromAI(Creature* creature) : BossAI(creature, DATA_UROM_EVENT)
			{
				if (instance->GetBossState(DATA_VAROS_EVENT) != DONE)
					DoCast(me, SPELL_ARCANE_SHIELD, true);
			}

			float x, y;

			bool canCast;
			bool canGoBack;

			uint8 group[3];

			uint32 EvocationTimer;
			uint32 TeleportTimer;
			uint32 ArcaneExplosionTimer;
			uint32 CastArcaneExplosionTimer;
			uint32 FrostBombTimer;
			uint32 TimeBombTimer;

            void Reset()
            {
                _Reset();

                if (instance->GetData(DATA_UROM_PLATFORM) == 0)
                {
                    for (uint8 i = 0; i < 3; ++i)
						group[i] = 0;
                }

                x = 0.0f;
                y = 0.0f;
                canCast = false;
                canGoBack = false;

                me->GetMotionMaster()->MoveIdle();

				EvocationTimer = 2000;
                TeleportTimer = urand(30000, 35000);
                ArcaneExplosionTimer = 9000;
                CastArcaneExplosionTimer = 2000;
                FrostBombTimer = urand(5000, 8000);
                TimeBombTimer = urand(20000, 25000);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                SetGroups();
                SummonGroups();
                CastTeleport();

                if (instance->GetData(DATA_UROM_PLATFORM) != 3)
                    instance->SetData(DATA_UROM_PLATFORM, instance->GetData(DATA_UROM_PLATFORM) + 1);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
                _JustDied();
                DoCast(me, SPELL_DEATH_SPELL, true);
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                if (me->GetPositionZ() > 518.63f)
                    DoStartNoMovement(who);

                if (me->GetPositionZ() < 518.63f)
                {
                    if (me->Attack(who, true))
                    {
                        DoSendQuantumText(SayAggro[3], me);

                        me->SetInCombatWith(who);
                        who->SetInCombatWith(me);

                        me->GetMotionMaster()->MoveChase(who, 0, 0);
                    }
                }
            }

            void SetGroups()
            {
                if (!instance || instance->GetData(DATA_UROM_PLATFORM) != 0)
                    return;

                while (group[0] == group[1] || group[0] == group[2] || group[1] == group[2])
                {
                    for (uint8 i = 0; i < 3; ++i)
                        group[i] = urand(0, 2);
                }
            }

            void SetPosition(uint8 i)
            {
                switch (i)
                {
                    case 0:
                        x = me->GetPositionX() + 4;
                        y = me->GetPositionY() - 4;
                        break;
                    case 1:
                        x = me->GetPositionX() + 4;
                        y = me->GetPositionY() + 4;
                        break;
                    case 2:
                        x = me->GetPositionX() - 4;
                        y = me->GetPositionY() + 4;
                        break;
                    case 3:
                        x = me->GetPositionX() - 4;
                        y = me->GetPositionY() - 4;
                        break;
                    default:
                        break;
                }
            }

            void SummonGroups()
            {
                if (!instance || instance->GetData(DATA_UROM_PLATFORM) > 2)
                    return;

                for (uint8 i = 0; i < 4; ++i)
                {
                    SetPosition(i);
                    me->SummonCreature(Group[group[instance->GetData(DATA_UROM_PLATFORM)]].entry[i], x, y, me->GetPositionZ(), me->GetOrientation());
                }
            }

            void CastTeleport()
            {
                if (!instance || instance->GetData(DATA_UROM_PLATFORM) > 2)
                    return;

                DoSendQuantumText(SayAggro[instance->GetData(DATA_UROM_PLATFORM)], me);
                DoCast(TeleportSpells[instance->GetData(DATA_UROM_PLATFORM)]);
            }

			void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType)
            {
                if (!IsHeroic())
                    return;

                if (damageType == DOT)
                    DoCast(victim, SPELL_FROST_BUFFET, true);
            }

			void LeaveCombat()
            {
                me->RemoveAllAuras();
                me->CombatStop(false);
                me->DeleteThreatList();
            }

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_SUMMON_MENAGERIE:
						me->SetSpeed(MOVE_WALK, 50.0f, true);
						me->SetSpeed(MOVE_RUN, 50.0f, true);
						me->GetMotionMaster()->MovePoint(0, 968.66f, 1042.53f, 527.32f);
                        LeaveCombat();
                        break;
                    case SPELL_SUMMON_MENAGERIE_2:
						me->SetSpeed(MOVE_WALK, 50.0f, true);
						me->SetSpeed(MOVE_RUN, 50.0f, true);
						me->GetMotionMaster()->MovePoint(0, 1164.02f, 1170.85f, 527.321f);
                        LeaveCombat();
                        break;
                    case SPELL_SUMMON_MENAGERIE_3:
						me->SetSpeed(MOVE_WALK, 50.0f, true);
						me->SetSpeed(MOVE_RUN, 50.0f, true);
						me->GetMotionMaster()->MovePoint(0, 1118.31f, 1080.377f, 508.361f);
                        LeaveCombat();
                        break;
                    case SPELL_TELEPORT:
						me->SetCanFly(true);
						me->SetDisableGravity(true);
                        canCast = true;
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
				// Out of Combat Timer
				if (EvocationTimer <= diff && !me->IsInCombatActive() && instance->GetBossState(DATA_VAROS_EVENT) == DONE)
				{
					DoCast(me, SPELL_EVOCATION, true);
					EvocationTimer = 120000; // 2 Minutes
				}
				else EvocationTimer -= diff;

                if (!me->IsNonMeleeSpellCasted(false) && !UpdateVictim())
                    return;

                if (!instance || instance->GetData(DATA_UROM_PLATFORM) < 2)
                    return;

                if (TeleportTimer <= diff)
                {
                    me->InterruptNonMeleeSpells(false);
                    if (FrostBombTimer <= 8000)
                        FrostBombTimer += 8000;
                    if (TimeBombTimer <= 2500)
                        TimeBombTimer += 2500;
                    DoSendQuantumText(RAND(SAY_EXPLOSION_1, SAY_EXPLOSION_2), me);
                    me->GetMotionMaster()->MoveIdle();
                    DoCast(me, SPELL_TELEPORT, true);
                    TeleportTimer = urand(30000, 35000);

                }
                else TeleportTimer -= diff;

                if (canCast && !me->FindCurrentSpellBySpellId(DUNGEON_MODE(SPELL_EMPOWERED_ARCANE_EXPLOSION_5N, SPELL_EMPOWERED_ARCANE_EXPLOSION_5H)))
                {
                    if (CastArcaneExplosionTimer <= diff)
                    {
                        canCast = false;
                        canGoBack = true;
                        DoCastAOE(DUNGEON_MODE(SPELL_EMPOWERED_ARCANE_EXPLOSION_5N, SPELL_EMPOWERED_ARCANE_EXPLOSION_5H));
                        CastArcaneExplosionTimer = 2000;
                    }
                    else CastArcaneExplosionTimer -= diff;
                }

                if (canGoBack)
                {
                    if (ArcaneExplosionTimer <= diff)
                    {
                        Position pos;
                        me->GetVictim()->GetPosition(&pos);

                        me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                        me->GetMotionMaster()->MoveChase(me->GetVictim(), 0, 0);

                        canCast = false;
                        canGoBack = false;
                        ArcaneExplosionTimer = 9000;
                    }
                    else ArcaneExplosionTimer -= diff;
                }

                if (!me->IsNonMeleeSpellCasted(false, true, true))
                {
                    if (FrostBombTimer <= diff)
                    {
                        DoCastVictim(SPELL_FROSTBOMB);
                        FrostBombTimer = urand(7000, 15000);
                    }
                    else FrostBombTimer -= diff;

                    if (TimeBombTimer <= diff)
                    {
						me->SetCanFly(false);
						me->SetDisableGravity(false);
						me->SetSpeed(MOVE_WALK, 1.5f, true);
						me->SetSpeed(MOVE_RUN, 1.5f, true);

                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_TIME_BOMB_5N, SPELL_TIME_BOMB_5H));
							TimeBombTimer = urand(20000, 25000);
						}
                    }
                    else TimeBombTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_uromAI(creature);
        }
};

void AddSC_boss_urom()
{
    new boss_urom();
}