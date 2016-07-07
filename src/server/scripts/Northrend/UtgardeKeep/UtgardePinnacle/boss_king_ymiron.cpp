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
#include "utgarde_pinnacle.h"
#include "SpellInfo.h"

enum Texts
{
    SAY_AGGRO                = -1575028,
    SAY_SLAY_1               = -1575029,
    SAY_SLAY_2               = -1575030,
    SAY_SLAY_3               = -1575031,
    SAY_SLAY_4               = -1575032,
    SAY_DEATH                = -1575033,
    SAY_SUMMON_BJORN         = -1575034,
    SAY_SUMMON_HALDOR        = -1575035,
    SAY_SUMMON_RANULF        = -1575036,
    SAY_SUMMON_TORGYN        = -1575037,
};

enum Spells
{
	SPELL_BANE_HIT                            = 59203,
    SPELL_BANE_5N                             = 48294,
    SPELL_BANE_5H                             = 59301,
    SPELL_DARK_SLASH                          = 48292,
    SPELL_FETID_ROT_5N                        = 48291,
    SPELL_FETID_ROT_5H                        = 59300,
    SPELL_SCREAMS_OF_THE_DEAD                 = 51750,
    SPELL_SPIRIT_BURST_5N                     = 48529,
    SPELL_SPIRIT_BURST_5H                     = 59305,
    SPELL_SPIRIT_STRIKE_5N                    = 48423,
    SPELL_SPIRIT_STRIKE_5H                    = 59304,
    SPELL_ANCESTORS_VENGEANCE                 = 16939,
    SPELL_SUMMON_AVENGING_SPIRIT              = 48592,
    SPELL_SUMMON_SPIRIT_FOUNT                 = 48386,
    SPELL_CHANNEL_SPIRIT_TO_YMIRON            = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT            = 48307,
    SPELL_SPIRIT_FOUNT_5N                     = 48380,
    SPELL_SPIRIT_FOUNT_5H                     = 59320,
};

struct ActiveBoatStruct
{
    uint32 npc;
    int32 say;
    float MoveX, MoveY, MoveZ, SpawnX, SpawnY, SpawnZ, SpawnO;
};

static ActiveBoatStruct ActiveBoat[4] =
{
    {NPC_BJORN_VISUAL,  SAY_SUMMON_BJORN,  404.379f, -335.335f, 104.756f, 413.594f, -335.408f, 107.995f, 3.157f},
    {NPC_HALDOR_VISUAL, SAY_SUMMON_HALDOR, 380.813f, -335.069f, 104.756f, 369.994f, -334.771f, 107.995f, 6.232f},
    {NPC_RANULF_VISUAL, SAY_SUMMON_RANULF, 381.546f, -314.362f, 104.756f, 370.841f, -314.426f, 107.995f, 6.232f},
    {NPC_TORGYN_VISUAL, SAY_SUMMON_TORGYN, 404.310f, -314.761f, 104.756f, 413.992f, -314.703f, 107.995f, 3.157f},
};

#define DATA_KINGS_BANE 2157

class boss_king_ymiron : public CreatureScript
{
public:
    boss_king_ymiron() : CreatureScript("boss_king_ymiron") { }

    struct boss_king_ymironAI : public QuantumBasicAI
    {
        boss_king_ymironAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

            for (int i = 0; i < 4; ++i)
                ActiveOrder[i] = i;
            for (int i = 0; i < 3; ++i)
            {
                int r = i + (rand() % (4 - i));
                int creature = ActiveOrder[i];
                ActiveOrder[i] = ActiveOrder[r];
                ActiveOrder[r] = creature;
            }
			ActivedCreatureGUID = 0;
			OrbGUID = 0;
        }

        bool IsWalking;
        bool IsPause;
        bool IsActiveWithBjorn;
        bool IsActiveWithHaldor;
        bool IsActiveWithRanulf;
        bool IsActiveWithTorgyn;
        bool KingsBane; // Achievement King's Bane

        uint8 ActiveOrder[4];
        uint8 ActivedNumber;

        uint32 FetidRotTimer;
        uint32 BaneTimer;
        uint32 DarkSlashTimer;
        uint32 AncestorsVengeanceTimer;

        uint32 AbilityBjornTimer;
        uint32 AbilityHaldorTimer;
        uint32 AbilityRanulfTimer;
        uint32 AbilityTorgynTimer;

        uint32 PauseTimer;
        uint32 HealthAmountModifier;
        uint32 HealthAmountMultipler;

        uint64 ActivedCreatureGUID;
        uint64 OrbGUID;

        InstanceScript* instance;

        void Reset()
        {
			IsWalking = false;
            IsPause = false;
            IsActiveWithBjorn = false;
            IsActiveWithHaldor = false;
            IsActiveWithRanulf = false;
            IsActiveWithTorgyn = false;
            KingsBane = true;

            FetidRotTimer = urand(8000, 13000);
            BaneTimer = urand(18000, 23000);
            DarkSlashTimer = urand(28000, 33000);
            AncestorsVengeanceTimer = DUNGEON_MODE(60000, 45000);
            PauseTimer = 0;

            AbilityBjornTimer = 0;
            AbilityHaldorTimer = 0;
            AbilityRanulfTimer = 0;
            AbilityTorgynTimer = 0;

            ActivedNumber = 0;
            HealthAmountModifier = 1;
            HealthAmountMultipler = DUNGEON_MODE(33, 20);

            DespawnBoatGhosts(ActivedCreatureGUID);
            DespawnBoatGhosts(OrbGUID);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SIT_CHAIR_HIGH);

			instance->SetData(DATA_KING_YMIRON_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_KING_YMIRON_EVENT, IN_PROGRESS);
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_BANE_HIT)
                KingsBane = false;
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_KINGS_BANE)
                return KingsBane ? 1 : 0;

            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
			if (instance->GetData(DATA_KING_YMIRON_EVENT) ==  NOT_STARTED)
				return;

            if (IsWalking)
            {
                if (PauseTimer <= diff)
                {
                    DoSendQuantumText(ActiveBoat[ActiveOrder[ActivedNumber]].say, me);
                    if (Creature* creature = me->SummonCreature(ActiveBoat[ActiveOrder[ActivedNumber]].npc, ActiveBoat[ActiveOrder[ActivedNumber]].SpawnX, ActiveBoat[ActiveOrder[ActivedNumber]].SpawnY, ActiveBoat[ActiveOrder[ActivedNumber]].SpawnZ, ActiveBoat[ActiveOrder[ActivedNumber]].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
						DoCast(creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT); // should be on spirit
                        ActivedCreatureGUID = creature->GetGUID();
                        creature->CastSpell(me, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        creature->SetDisableGravity(true);
                        switch (ActiveOrder[ActivedNumber])
                        {
                            case 0:
								IsActiveWithBjorn = true;
								break;
                            case 1:
								IsActiveWithHaldor = true;
								break;
                            case 2:
								IsActiveWithRanulf = true;
								break;
                            case 3:
								IsActiveWithTorgyn = true;
								break;
                        }
                    }

                    IsPause = true;
                    IsWalking = false;
                    PauseTimer = 3000;
                }
				else PauseTimer -= diff;
                return;
            }
            else if (IsPause)
            {
                if (PauseTimer <= diff)
                {
                    AbilityBjornTimer = 5000;
                    AbilityHaldorTimer = 5000;
                    AbilityRanulfTimer = 5000;
                    AbilityTorgynTimer = 5000;

                    IsPause = false;
                    PauseTimer = 0;
                }
				else PauseTimer -= diff;
                return;
            }
			
			if (!UpdateVictim())
				return;

            if (!IsPause)
            {
                if (BaneTimer <= diff)
                {
                    DoCast(me, DUNGEON_MODE(SPELL_BANE_5N, SPELL_BANE_5H));
                    BaneTimer = urand(20000, 25000);
                }
				else BaneTimer -= diff;

                if (FetidRotTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_FETID_ROT_5N, SPELL_FETID_ROT_5H));
                    FetidRotTimer = urand(10000, 15000);
                }
				else FetidRotTimer -= diff;

                if (DarkSlashTimer <= diff)
                {
                    DoCastVictim(SPELL_DARK_SLASH);
                    DarkSlashTimer = urand(30000, 35000);
                }
				else DarkSlashTimer -= diff;

                if (AncestorsVengeanceTimer <= diff)
                {
                    DoCast(me, SPELL_ANCESTORS_VENGEANCE);
                    AncestorsVengeanceTimer = DUNGEON_MODE(urand(60000, 65000), urand(45000, 50000));
                }
				else AncestorsVengeanceTimer -= diff;

                if (IsActiveWithBjorn && AbilityBjornTimer <= diff)
                {
                    if (Creature* creature = me->SummonCreature(NPC_SPIRIT_FOUNT, 385.0f + rand() % 10, -330.0f + rand() % 10, 104.756f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
                    {
                        creature->SetSpeed(MOVE_RUN, 0.4f);
						creature->CastSpell(creature, DUNGEON_MODE(SPELL_SPIRIT_FOUNT_5N, SPELL_SPIRIT_FOUNT_5H), true);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        creature->SetDisplayId(MODEL_ID_INVISIBLE);
                        OrbGUID = creature->GetGUID();
                    }
                    IsActiveWithBjorn = false;
                }
				else AbilityBjornTimer -= diff;

                if (IsActiveWithHaldor && AbilityHaldorTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_SPIRIT_STRIKE_5N, SPELL_SPIRIT_STRIKE_5H));
                    AbilityHaldorTimer = 5000;
                }
				else AbilityHaldorTimer -= diff;

                if (IsActiveWithRanulf && AbilityRanulfTimer <= diff)
                {
                    DoCast(me, DUNGEON_MODE(SPELL_SPIRIT_BURST_5N, SPELL_SPIRIT_BURST_5H));
                    AbilityRanulfTimer = 10000;
                }
				else AbilityRanulfTimer -= diff;

                if (IsActiveWithTorgyn && AbilityTorgynTimer <= diff)
                {
                    float x, y, z;
                    x = me->GetPositionX()-5;
                    y = me->GetPositionY()-5;
                    z = me->GetPositionZ();
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        //DoCast(me, SPELL_SUMMON_AVENGING_SPIRIT); // works fine, but using summon has better control
                        if (Creature* creature = me->SummonCreature(NPC_AVENGING_SPIRIT, x + rand() % 10, y + rand() % 10, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                creature->AddThreat(target, 0.0f);
                                creature->AI()->AttackStart(target);
                            }
                        }
                    }
                    AbilityTorgynTimer = 15000; // overtime
                }
				else AbilityTorgynTimer -= diff;

                // Health check -----------------------------------------------------------------------------
                if ((HealthAmountModifier - 1) < (uint32)DUNGEON_MODE(2, 4) && me->HealthBelowPct(100 - HealthAmountMultipler * HealthAmountModifier))
                {
                    uint8 Order = HealthAmountModifier - 1;
                    ++HealthAmountModifier;

                    me->InterruptNonMeleeSpells(true);
                    DoCast(me, SPELL_SCREAMS_OF_THE_DEAD);
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->AttackStop();
                    me->GetMotionMaster()->MovePoint(0, ActiveBoat[ActiveOrder[Order]].MoveX, ActiveBoat[ActiveOrder[Order]].MoveY, ActiveBoat[ActiveOrder[Order]].MoveZ);

                    DespawnBoatGhosts(ActivedCreatureGUID);
                    DespawnBoatGhosts(OrbGUID);

                    IsActiveWithBjorn  = false;
                    IsActiveWithHaldor = false;
                    IsActiveWithRanulf = false;
                    IsActiveWithTorgyn = false;

                    BaneTimer += 8000;
                    FetidRotTimer += 8000;
                    DarkSlashTimer += 8000;
                    AncestorsVengeanceTimer += 8000;

                    ActivedNumber = Order;
                    IsWalking = true;
                    PauseTimer = 2000;
                    return;
                }
                DoMeleeAttackIfReady();
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            DespawnBoatGhosts(ActivedCreatureGUID);
            DespawnBoatGhosts(OrbGUID);

			instance->SetData(DATA_KING_YMIRON_EVENT, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3, SAY_SLAY_4), me);
        }

        void DespawnBoatGhosts(uint64 CreatureGUID)
        {
            if (CreatureGUID)
			{
                if (Creature* creature = Unit::GetCreature(*me, CreatureGUID))
                    creature->DisappearAndDie();
			}

            CreatureGUID = 0;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_king_ymironAI(creature);
    }
};

class achievement_kings_bane : public AchievementCriteriaScript
{
public:
	achievement_kings_bane() : AchievementCriteriaScript("achievement_kings_bane") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* ymiron = target->ToCreature())
			if (ymiron->AI()->GetData(DATA_KINGS_BANE))
				return true;

		return false;
	}
};

void AddSC_boss_king_ymiron()
{
    new boss_king_ymiron();
    new achievement_kings_bane();
}