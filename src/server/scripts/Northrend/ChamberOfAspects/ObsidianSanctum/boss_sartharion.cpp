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
#include "obsidian_sanctum.h"

enum Texts
{
	// Shadron
	SAY_SHADRON_AGGRO                       = -1615000,
    SAY_SHADRON_SLAY_1                      = -1615001,
    SAY_SHADRON_SLAY_2                      = -1615002,
    SAY_SHADRON_DEATH                       = -1615003,
    SAY_SHADRON_BREATH                      = -1615004,
    SAY_SHADRON_RESPOND                     = -1615005,
    SAY_SHADRON_SPECIAL_1                   = -1615006,
    SAY_SHADRON_SPECIAL_2                   = -1615007,
	WHISPER_SHADRON_DICIPLE                 = -1615008,
	//Tenebron
    SAY_TENEBRON_AGGRO                      = -1615009,
    SAY_TENEBRON_SLAY_1                     = -1615010,
    SAY_TENEBRON_SLAY_2                     = -1615011,
    SAY_TENEBRON_DEATH                      = -1615012,
    SAY_TENEBRON_BREATH                     = -1615013,
    SAY_TENEBRON_RESPOND                    = -1615014,
    SAY_TENEBRON_SPECIAL_1                  = -1615015,
    SAY_TENEBRON_SPECIAL_2                  = -1615016,
	WHISPER_HATCH_EGGS                      = -1615017,
	// Sartarion
	SAY_SARTHARION_AGGRO                    = -1615018,
    SAY_SARTHARION_BERSERK                  = -1615019,
    SAY_SARTHARION_BREATH                   = -1615020,
    SAY_SARTHARION_CALL_SHADRON             = -1615021,
    SAY_SARTHARION_CALL_TENEBRON            = -1615022,
    SAY_SARTHARION_CALL_VESPERON            = -1615023,
    SAY_SARTHARION_DEATH                    = -1615024,
    SAY_SARTHARION_SPECIAL_1                = -1615025,
    SAY_SARTHARION_SPECIAL_2                = -1615026,
    SAY_SARTHARION_SPECIAL_3                = -1615027,
    SAY_SARTHARION_SPECIAL_4                = -1615028,
    SAY_SARTHARION_SLAY_1                   = -1615029,
    SAY_SARTHARION_SLAY_2                   = -1615030,
    SAY_SARTHARION_SLAY_3                   = -1615031,
    WHISPER_LAVA_CHURN                      = -1615032,
	// Vesperon
    SAY_VESPERON_AGGRO                      = -1615033,
    SAY_VESPERON_SLAY_1                     = -1615034,
    SAY_VESPERON_SLAY_2                     = -1615035,
    SAY_VESPERON_DEATH                      = -1615036,
    SAY_VESPERON_BREATH                     = -1615037,
    SAY_VESPERON_RESPOND                    = -1615038,
    SAY_VESPERON_SPECIAL_1                  = -1615039,
    SAY_VESPERON_SPECIAL_2                  = -1615040,
	WHISPER_VESPERON_DICIPLE                = -1615041,
	WHISPER_OPEN_PORTAL                     = -1615042,
};

enum Spells
{
    SPELL_BERSERK                               = 61632,
    SPELL_CLEAVE                                = 56909,
    SPELL_FLAME_BREATH_10                       = 56908,
    SPELL_FLAME_BREATH_25                       = 58956,
    SPELL_TAIL_LASH_10                          = 56910,
    SPELL_TAIL_LASH_25                          = 58957,
    SPELL_WILL_OF_SARTHARION                    = 61254,
    SPELL_LAVA_STRIKE                           = 57571,
    SPELL_LAVA_STRIKE_EFFECT1                   = 57591,
    SPELL_TWILIGHT_REVENGE                      = 60639,
    SPELL_PYROBUFFET                            = 57557,
    SPELL_TWILIGHT_SHIFT_ENTER                  = 57620,
    SPELL_TWILIGHT_SHIFT                        = 57874,
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,
    SPELL_TWILIGHT_RESIDUE                      = 61885,
	SPELL_SHADOW_BREATH_10                      = 57570,
    SPELL_SHADOW_BREATH_25                      = 59126,
	SPELL_SHADOW_FISSURE_10                     = 57579,
	SPELL_SHADOW_FISSURE_25                     = 59127,
    SPELL_POWER_OF_VESPERON                     = 61251,
    SPELL_TWILIGHT_TORMENT_VESP                 = 57948,
    SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER       = 57935,
    SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER   = 58835,
    SPELL_POWER_OF_SHADRON                      = 58105,
    SPELL_GIFT_OF_TWILIGTH_SHA                  = 57835,
    SPELL_GIFT_OF_TWILIGTH_SAR                  = 58766,
    SPELL_VOID_BLAST_10                         = 57581,
    SPELL_VOID_BLAST_25                         = 59128,
    SPELL_POWER_OF_TENEBRON                     = 61248,
    SPELL_FADE_ARMOR                            = 60708,
    SPELL_FLAME_TSUNAMI                         = 57494,
    SPELL_FLAME_TSUNAMI_DMG_AURA                = 57491,
    SPELL_FLAME_TSUNAMI_BUFF                    = 60430,
};

enum Points
{
    POINT_ID_INIT                               = 100,
    POINT_ID_LAND                               = 200,
};

const Position Tene[] =
{
    {3212.854f, 575.597f, 109.856f, 0.0f},                           //init
    {3246.425f, 565.367f, 61.249f, 0.0f},                            //end
};

const Position Shad[] =
{
    {3293.238f, 472.223f, 106.968f, 0.0f},
    {3271.669f, 526.907f, 61.931f, 0.0f},
};

const Position Vesp[] =
{
    {3193.310f, 472.861f, 102.697f, 0.0f},
    {3227.268f, 533.238f, 59.995f, 0.0f},
};

const Position DragonCommon[MAX_WAYPOINT] =
{
    {3214.012f, 468.932f, 98.652f, 0.0f},
    {3244.950f, 468.427f, 98.652f, 0.0f},
    {3283.520f, 496.869f, 98.652f, 0.0f},
    {3287.316f, 555.875f, 98.652f, 0.0f},
    {3250.479f, 585.827f, 98.652f, 0.0f},
    {3209.969f, 566.523f, 98.652f, 0.0f},
};

const Position FlameRight1Spawn = { 3200.00f, 573.211f, 57.1551f, 0.0f };
const Position FlameRight1Direction = { 3289.28f, 573.211f, 57.1551f, 0.0f };
const Position FlameRight2Spawn = { 3200.00f, 532.211f, 57.1551f, 0.0f };
const Position FlameRight2Direction = { 3289.28f, 532.211f, 57.1551f, 0.0f };
const Position FlameRight3Spawn = { 3200.00f, 491.211f, 57.1551f, 0.0f };
const Position FlameRight3Direction = { 3289.28f, 491.211f, 57.1551f, 0.0f };
const Position FlameLeft1Spawn = { 3289.28f, 511.711f, 57.1551f, 0.0f };
const Position FlameLeft1Direction = { 3200.00f, 511.711f, 57.1551f, 0.0f };
const Position FlameLeft2Spawn = { 3289.28f, 552.711f, 57.1551f, 0.0f };
const Position FlameLeft2Direction = { 3200.00f, 552.711f, 57.1551f, 0.0f };

void HandleAuraOnRaidSartharion(Unit* caller, uint32 spellId, bool remove, bool add, uint32 addAmount, bool onlyInTwilight, bool onlyInRealWorld)
{
    if (spellId <= 0 || (add && addAmount <= 0) || !caller)
        return;

    Map* map = caller->GetMap();
    if (map && map->IsDungeon())
    {
        Map::PlayerList const &PlayerList = map->GetPlayers();

        if (PlayerList.isEmpty())
            return;

        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (i->getSource())
            {
                if (i->getSource()->IsAlive())
                {
                    if (onlyInTwilight)
                    {
                        if (!i->getSource()->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                            continue;
                    }

                    if (onlyInRealWorld)
                    {
                        if (i->getSource()->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                            continue;
                    }

                    if (remove)
                        if (i->getSource()->HasAura(spellId))
                            i->getSource()->RemoveAurasDueToSpell(spellId);

                    if (add)
                    {
                        if (addAmount > 0)
                        {
                            if (Aura* aur = caller->AddAura(spellId, i->getSource()))
                            {
                                aur->SetStackAmount(addAmount);
                                aur->SetCharges(addAmount);
                            }
                        }
                    }
                }
            }
		}
    }
}

class boss_sartharion : public CreatureScript
{
public:
    boss_sartharion() : CreatureScript("boss_sartharion") { }

    struct boss_sartharionAI : public QuantumBasicAI
    {
        boss_sartharionAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		SummonList Summons;

        bool IsBerserk;
        bool IsSoftEnraged;

        uint32 EnrageTimer;

        uint32 TenebronTimer;
        uint32 ShadronTimer;
        uint32 VesperonTimer;

        uint32 FlameTsunamiTimer;
        uint32 FlameBreathTimer;
        uint32 TailSweepTimer;
        uint32 CleaveTimer;
        uint32 LavaStrikeTimer;

        bool HasCalledTenebron;
        bool HasCalledShadron;
        bool HasCalledVesperon;

        uint8 DrakeCount;

        uint32 CheckPlayerTimer;
        bool HasSoftEnraged;

        std::set<uint64> playersHitByLavaStrike;

        void Reset()
		{
            IsBerserk = false;
            IsSoftEnraged = false;
            HasSoftEnraged = false;

			Summons.DespawnAll();

            EnrageTimer = 15*MINUTE*IN_MILLISECONDS;

            TenebronTimer = 30000;
            ShadronTimer = 75000;
            VesperonTimer = 120000;

            FlameTsunamiTimer = 30000;
            FlameBreathTimer = 20000;
            TailSweepTimer = 20000;
            CleaveTimer = 7000;

            HasCalledTenebron = false;
            HasCalledShadron = false;
            HasCalledVesperon = false;

            LavaStrikeTimer = 15000;
            CheckPlayerTimer = 2000;

            playersHitByLavaStrike.clear();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            if (me->HasAura(SPELL_TWILIGHT_REVENGE))
                me->RemoveAurasDueToSpell(SPELL_TWILIGHT_REVENGE);

            me->ResetLootMode();
            me->SetHomePosition(3246.57f, 551.263f, 58.6164f, 4.66003f);

            DrakeCount = 0;

            // Drakes respawning system
			Creature* pTenebron = Unit::GetCreature(*me, instance->GetData64(DATA_TENEBRON));
			Creature* pShadron = Unit::GetCreature(*me, instance->GetData64(DATA_SHADRON));
			Creature* pVesperon = Unit::GetCreature(*me, instance->GetData64(DATA_VESPERON));
			if (pTenebron)
			{
				pTenebron->SetHomePosition(3239.07f, 657.235f, 86.8775f, 4.74729f);
				if (pTenebron->IsAlive())
				{
					if (pTenebron->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
						pTenebron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

					pTenebron->Relocate(3239.07f, 657.235f, 86.8775f, 4.74729f);
					pTenebron->MonsterMoveWithSpeed(3239.07f, 657.235f, 86.8775f, 0);

					if (pTenebron->AI())
						pTenebron->AI()->EnterEvadeMode();
				}
				else
				{
					if (instance->GetData(TYPE_TENEBRON_PREKILLED) != DONE)
					{
						pTenebron->Respawn(true);
						pTenebron->SetActive(true);

						pTenebron->Relocate(3239.07f, 657.235f, 86.8775f, 4.74729f);
						pTenebron->MonsterMoveWithSpeed(3239.07f, 657.235f, 86.8775f, 0);

						if (pTenebron->AI())
							pTenebron->AI()->EnterEvadeMode();
					}
				}
			}
			if (pShadron)
			{
				pShadron->SetHomePosition(3363.06f, 525.28f, 98.362f, 4.76475f);
				if (pShadron->IsAlive())
				{
					if (pShadron->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
						pShadron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

					pShadron->Relocate(3363.06f, 525.28f, 98.362f, 4.76475f);
					pShadron->MonsterMoveWithSpeed(3363.06f, 525.28f, 98.362f, 0);

					if (pShadron->AI())
						pShadron->AI()->EnterEvadeMode();
				}
				else
				{
					if (instance->GetData(TYPE_SHADRON_PREKILLED) != DONE)
					{
						pShadron->Respawn(true);
						pShadron->SetActive(true);

						pShadron->Relocate(3363.06f, 525.28f, 98.362f, 4.76475f);
						pShadron->MonsterMoveWithSpeed(3363.06f, 525.28f, 98.362f, 0);

						if (pShadron->AI())
							pShadron->AI()->EnterEvadeMode();
					}
				}
			}

			if (pVesperon)
			{
				pVesperon->SetHomePosition(3145.68f, 520.71f, 89.7f, 4.64258f);
				if (pVesperon->IsAlive())
				{
					if (pVesperon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
						pVesperon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

					pVesperon->Relocate(3145.68f, 520.71f, 89.7f, 4.64258f);
					pVesperon->MonsterMoveWithSpeed(3145.68f, 520.71f, 89.7f, 0);

					if (pVesperon->AI())
						pVesperon->AI()->EnterEvadeMode();
				}
				else
				{
					if (instance->GetData(TYPE_VESPERON_PREKILLED) != DONE)
					{
						pVesperon->Respawn(true);
						pVesperon->SetActive(true);

						pVesperon->Relocate(3145.68f, 520.71f, 89.7f, 4.64258f);
						pVesperon->MonsterMoveWithSpeed(3145.68f, 520.71f, 89.7f, 0);

						if (pVesperon->AI())
							pVesperon->AI()->EnterEvadeMode();
					}
				}
			}
		}

        void JustReachedHome()
        {
			instance->SetData(TYPE_SARTHARION_EVENT, NOT_STARTED);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == NPC_LAVA_BLAZE)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_SARTHARION_AGGRO, me);

            DoZoneInCombat();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			instance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
			FetchDragons();
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_SARTHARION_DEATH, me);

			Creature* pTenebron = Unit::GetCreature(*me, instance->GetData64(DATA_TENEBRON));
			Creature* pShadron = Unit::GetCreature(*me, instance->GetData64(DATA_SHADRON));
			Creature* pVesperon = Unit::GetCreature(*me, instance->GetData64(DATA_VESPERON));

			Summons.DespawnAll();

			if (pTenebron->IsAlive())
				pTenebron->DisappearAndDie();

			if (pShadron->IsAlive())
				pShadron->DisappearAndDie();

			if (pVesperon->IsAlive())
				pVesperon->DisappearAndDie();

                instance->SetData(TYPE_SARTHARION_EVENT, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SARTHARION_SLAY_1, SAY_SARTHARION_SLAY_2, SAY_SARTHARION_SLAY_3), me);
        }

        // me->ResetLootMode() is called from Reset()
        // AddDrakeLootMode() should only ever be called from FetchDragons(), which is called from Aggro()
        void AddDrakeLootMode()
        {
            if (me->HasLootMode(LOOT_MODE_HARD_MODE_2))      // Has two Drake loot modes
                me->AddLootMode(LOOT_MODE_HARD_MODE_3);      // Add 3rd Drake loot mode
            else if (me->HasLootMode(LOOT_MODE_HARD_MODE_1)) // Has one Drake loot mode
                me->AddLootMode(LOOT_MODE_HARD_MODE_2);      // Add 2nd Drake loot mode
            else                                             // Has no Drake loot modes
                me->AddLootMode(LOOT_MODE_HARD_MODE_1);      // Add 1st Drake loot mode
        }

        uint32 GetData(uint32 type)
        {
            if (type == TWILIGHT_ACHIEVEMENTS)
                return DrakeCount;

            return 0;
        }

        void FetchDragons()
        {
            if (!instance)
                return;

            me->ResetLootMode();
            DrakeCount = 0;

            Creature* pFetchTene = Unit::GetCreature(*me, instance->GetData64(DATA_TENEBRON));
            Creature* pFetchShad = Unit::GetCreature(*me, instance->GetData64(DATA_SHADRON));
            Creature* pFetchVesp = Unit::GetCreature(*me, instance->GetData64(DATA_VESPERON));

            //if at least one of the dragons are alive and are being called
            bool bCanUseWill = false;

            if (pFetchTene && pFetchTene->IsAlive() && !pFetchTene->GetVictim())
            {
                bCanUseWill = true;
                if (!pFetchTene->IsInCombatActive())
                {
                    AddDrakeLootMode();
                    ++DrakeCount;
                }
                pFetchTene->RemoveLootMode(1);

                if (pFetchTene->GetMotionMaster())
                    pFetchTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, Tene[0].GetPositionX(), Tene[0].GetPositionY(), Tene[0].GetPositionZ());

                if (!pFetchTene->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pFetchTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            if (pFetchShad && pFetchShad->IsAlive() && !pFetchShad->GetVictim())
            {
                bCanUseWill = true;
                if (!pFetchShad->IsInCombatActive())
                {
                    AddDrakeLootMode();
                    ++DrakeCount;
                }
                pFetchShad->RemoveLootMode(1);

                if (pFetchShad->GetMotionMaster())
                    pFetchShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, Shad[0].GetPositionX(), Shad[0].GetPositionY(), Shad[0].GetPositionZ());

                if (!pFetchShad->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pFetchShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            if (pFetchVesp && pFetchVesp->IsAlive() && !pFetchVesp->GetVictim())
            {
                bCanUseWill = true;
                if (!pFetchVesp->IsInCombatActive())
                {
                    AddDrakeLootMode();
                    ++DrakeCount;
                }
                pFetchVesp->RemoveLootMode(1);

                if (pFetchVesp->GetMotionMaster())
                    pFetchVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, Vesp[0].GetPositionX(), Vesp[0].GetPositionY(), Vesp[0].GetPositionZ());

                if (!pFetchVesp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pFetchVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            if (bCanUseWill)
                DoCast(me, SPELL_WILL_OF_SARTHARION, true);
        }

        void CheckPlayerPositions(Unit* who)
        {
            if (!who)
                return;

            if (who->IsInWater() && who->IsAlive())
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                {
                    who->ToPlayer()->TeleportTo(615, 3246.5f, 530.4f, 59.5f, 1.5f);
                }
                else
                {
                    who->Relocate(3246.5f, 530.4f, 59.5f, 1.5f);
                    who->MonsterMoveWithSpeed(3246.5f, 530.4f, 59.5f, 0);
                }
            }

            Map* map = me->GetMap();
            if (map && map->IsDungeon())
            {
                Map::PlayerList const &PlayerList = map->GetPlayers();

                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (i->getSource())
                    {
                        if (i->getSource()->IsAlive() && !i->getSource()->HasAura(SPELL_FLAME_TSUNAMI_DMG_AURA))
                        {
                            if (i->getSource()->GetPositionX() < 3182.0f ||
                                i->getSource()->GetPositionX() > 3318.0f ||
                                i->getSource()->GetPositionY() > 617.0f ||
                                i->getSource()->GetPositionY() < 483.0f ||
                                (i->getSource()->GetPositionX() < 3230.3f && i->getSource()->GetPositionY() > 565.8f) ||
                                (i->getSource()->GetPositionX() > 3233.7f && i->getSource()->GetPositionY() < 492.0f) ||
                                i->getSource()->GetPositionZ() < 12.0f ||
                                i->getSource()->GetPositionZ() > 170.0f)
                            {
                                i->getSource()->TeleportTo(615, 3246.5f, 530.4f, 59.5f, 1.5f);
                            }
                        }
                    }
               }
            }
        }

        void CallDragon(uint32 uiDataId)
        {
            if (instance)
            {
                if (Creature* creature = Unit::GetCreature(*me, instance->GetData64(uiDataId)))
                {
                    if (creature->IsAlive() && !creature->GetVictim())
                    {
                        if (creature->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
                            creature->SetWalk(false);

                        if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                        DoCast(me, SPELL_WILL_OF_SARTHARION, true);

                        int32 TextId = 0;

                        switch (creature->GetEntry())
                        {
                            case NPC_TENEBRON:
                                TextId = SAY_SARTHARION_CALL_TENEBRON;

                                if (!creature->HasAura(SPELL_POWER_OF_TENEBRON))
                                    creature->AddAura(SPELL_POWER_OF_TENEBRON, creature);

                                if (creature->GetMotionMaster())
                                    creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, Tene[1].GetPositionX(), Tene[1].GetPositionY(), Tene[1].GetPositionZ());

                                break;
                            case NPC_SHADRON:
                                TextId = SAY_SARTHARION_CALL_SHADRON;

                                if (!creature->HasAura(SPELL_POWER_OF_SHADRON))
                                    creature->AddAura(SPELL_POWER_OF_SHADRON, creature);

                                if (creature->GetMotionMaster())
                                    creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, Shad[1].GetPositionX(), Shad[1].GetPositionY(), Shad[1].GetPositionZ());

                                break;
                            case NPC_VESPERON:
                                TextId = SAY_SARTHARION_CALL_VESPERON;

                                if (!creature->HasAura(SPELL_POWER_OF_VESPERON))
                                    creature->AddAura(SPELL_POWER_OF_VESPERON, creature);

                                if (creature->GetMotionMaster())
                                    creature->GetMotionMaster()->MovePoint(POINT_ID_LAND, Vesp[1].GetPositionX(), Vesp[1].GetPositionY(), Vesp[1].GetPositionZ());

                                break;
                        }

                        DoSendQuantumText(TextId, me);
                    }
                }
            }
        }

        void SendFlameTsunami()
        {
			if (Map* map = me->GetMap())
			{
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (!PlayerList.isEmpty())
					{
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
						{
                            if (i->getSource() && i->getSource()->IsAlive())
                                DoSendQuantumText(WHISPER_LAVA_CHURN, me, i->getSource());
						}
					}
                }
			}
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_LAVA_STRIKE_EFFECT1)
                playersHitByLavaStrike.insert(target->GetGUID());
        }

        bool wasPlayerHitByLavaStrike(uint64 guid)
        {
            return playersHitByLavaStrike.count(guid) == 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            Unit* pTenebron = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_TENEBRON) : 0);
            Unit* pShadron = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_SHADRON) : 0);
            Unit* pVesperon = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_VESPERON) : 0);

            if (!IsBerserk && !HealthAbovePct(35))
            {
                bool OneDrakeAlive = false;

                if (pTenebron)
                {
                    if (pTenebron->IsAlive())
                    {
                        pTenebron->AddAura(SPELL_BERSERK, pTenebron);
                        OneDrakeAlive = true;
                    }
                }

                if (pShadron)
                {
                    if (pShadron->IsAlive())
                    {
                        pShadron->AddAura(SPELL_BERSERK, pShadron);
                        OneDrakeAlive = true;
                    }
                }

                if (pVesperon)
                {
                    if (pVesperon->IsAlive())
                    {
                        pVesperon->AddAura(SPELL_BERSERK, pVesperon);
                        OneDrakeAlive = true;
                    }
                }

                if (OneDrakeAlive)
                {
                    DoSendQuantumText(SAY_SARTHARION_BERSERK, me);
                    me->AddAura(SPELL_BERSERK, me);
                }

                IsBerserk = true;
            }

            if (EnrageTimer <= diff)
            {
                DoCast(me, SPELL_PYROBUFFET, true);
                EnrageTimer = 500;
            }
            else EnrageTimer -= diff;

            if (!IsSoftEnraged && HealthBelowPct(10) && !HasSoftEnraged)
            {
                IsSoftEnraged = true;
                LavaStrikeTimer = 1000;
            }

            if (LavaStrikeTimer <= diff)
            {
                if (IsSoftEnraged && !HasSoftEnraged)
                {
                    HasSoftEnraged = true;

                    for (uint8 i = 0; i < 9; i++)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            me->CastSpell(target, SPELL_LAVA_STRIKE, true);
                    }
                }
                else if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					me->CastSpell(target, SPELL_LAVA_STRIKE, true);

				if (urand(0, 5) == 0)
					DoSendQuantumText(RAND(SAY_SARTHARION_SPECIAL_1, SAY_SARTHARION_SPECIAL_2, SAY_SARTHARION_SPECIAL_3), me);

                LavaStrikeTimer = urand(5000, 20000);
            }
            else LavaStrikeTimer -= diff;

            if (FlameTsunamiTimer <= diff)
            {
                SendFlameTsunami();
                switch (urand(0, 1))
                {
                    case 0:
                    {
                        Creature* Right1 = me->SummonCreature(NPC_FLAME_TSUNAMI, FlameRight1Spawn.GetPositionX(), FlameRight1Spawn.GetPositionY(), FlameRight1Spawn.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 12000);
                        Creature* Right2 = me->SummonCreature(NPC_FLAME_TSUNAMI, FlameRight2Spawn.GetPositionX(), FlameRight2Spawn.GetPositionY(), FlameRight2Spawn.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 12000);
                        Creature* Right3 = me->SummonCreature(NPC_FLAME_TSUNAMI, FlameRight3Spawn.GetPositionX(), FlameRight3Spawn.GetPositionY(), FlameRight3Spawn.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 12000);

                        if (Right1 && Right2 && Right3)
                        {
                            if (Right1->GetMotionMaster() && Right2->GetMotionMaster() && Right3->GetMotionMaster())
                            {
                                if (Right1->AI() && Right2->AI() && Right3->AI())
                                {
                                    Right1->SetSpeed(MOVE_SWIM, 1.6f);
                                    Right2->SetSpeed(MOVE_SWIM, 1.6f);
                                    Right3->SetSpeed(MOVE_SWIM, 1.6f);
                                    Right1->GetMotionMaster()->MovePoint(1, FlameRight1Direction.GetPositionX(), FlameRight1Direction.GetPositionY(), FlameRight1Direction.GetPositionZ());
                                    Right2->GetMotionMaster()->MovePoint(1, FlameRight2Direction.GetPositionX(), FlameRight2Direction.GetPositionY(), FlameRight2Direction.GetPositionZ());
                                    Right3->GetMotionMaster()->MovePoint(1, FlameRight3Direction.GetPositionX(), FlameRight3Direction.GetPositionY(), FlameRight3Direction.GetPositionZ());
                                    Right1->AI()->SetData(1, 1);
                                    Right2->AI()->SetData(1, 1);
                                    Right3->AI()->SetData(1, 1);
                                }
                            }
                        }
                        break;
                    }
                    case 1:
                    {
                        Creature* Left1 = me->SummonCreature(NPC_FLAME_TSUNAMI, FlameLeft1Spawn.GetPositionX(), FlameLeft1Spawn.GetPositionY(), FlameLeft1Spawn.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 12000);
                        Creature* Left2 = me->SummonCreature(NPC_FLAME_TSUNAMI, FlameLeft2Spawn.GetPositionX(), FlameLeft2Spawn.GetPositionY(), FlameLeft2Spawn.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 12000);

                        if (Left1 && Left2)
                        {
                            if (Left1->GetMotionMaster() && Left2->GetMotionMaster())
                            {
                                if (Left1->AI() && Left2->AI())
                                {
                                    Left1->SetSpeed(MOVE_SWIM, 1.6f);
                                    Left2->SetSpeed(MOVE_SWIM, 1.6f);
                                    Left1->GetMotionMaster()->MovePoint(2, FlameLeft1Direction.GetPositionX(), FlameLeft1Direction.GetPositionY(), FlameLeft1Direction.GetPositionZ());
                                    Left2->GetMotionMaster()->MovePoint(2, FlameLeft2Direction.GetPositionX(), FlameLeft2Direction.GetPositionY(), FlameLeft2Direction.GetPositionZ());
                                    Left1->AI()->SetData(1, 2);
                                    Left2->AI()->SetData(1, 2);
                                }
                            }
                        }
                        break;
                    }
                }

                FlameTsunamiTimer = 30000;
            }
            else FlameTsunamiTimer -= diff;

            if (CheckPlayerTimer <= diff)
            {
                CheckPlayerPositions(me->GetVictim());
                CheckPlayerTimer = 1000;
            }
            else CheckPlayerTimer -= diff;

            if (FlameBreathTimer <= diff)
            {
                DoSendQuantumText(SAY_SARTHARION_BREATH, me);
                DoCastVictim(RAID_MODE(SPELL_FLAME_BREATH_10, SPELL_FLAME_BREATH_25));
                FlameBreathTimer = urand(25000, 35000);
            }
            else FlameBreathTimer -= diff;

            if (TailSweepTimer <= diff)
            {
                DoCastVictim(RAID_MODE(SPELL_TAIL_LASH_10, SPELL_TAIL_LASH_25));
                TailSweepTimer = urand(15000, 20000);
            }
            else TailSweepTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(7000, 10000);
            }
            else CleaveTimer -= diff;

            if (!HasCalledTenebron && TenebronTimer <= diff)
            {
                CallDragon(DATA_TENEBRON);
                HasCalledTenebron = true;
            }
            else TenebronTimer -= diff;

            if (!HasCalledShadron && ShadronTimer <= diff)
            {
                CallDragon(DATA_SHADRON);
                HasCalledShadron = true;
            }
            else ShadronTimer -= diff;

            if (!HasCalledVesperon && VesperonTimer <= diff)
            {
                CallDragon(DATA_VESPERON);
                HasCalledVesperon = true;
            }
            else VesperonTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_sartharionAI(creature);
    }
};

struct dummy_dragonAI : public QuantumBasicAI
{
    dummy_dragonAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    uint32 WaypointId;
    uint32 MoveNextTimer;
    bool CanMoveFree;

    void Reset()
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

		DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

        WaypointId = 0;
        MoveNextTimer = 500;
        CanMoveFree = false;
    }

    void MovementInform(uint32 uiType, uint32 PointId)
    {
        if (!instance || uiType != POINT_MOTION_TYPE)
            return;

        if (instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        if (PointId == POINT_ID_LAND)
        {
            if (me->GetMotionMaster())
                me->GetMotionMaster()->Clear();

            DoZoneInCombat();

            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0, true))
            {
                me->AddThreat(target, 1.0f);
                me->Attack(target, true);

                if (me->GetMotionMaster())
                    me->GetMotionMaster()->MoveChase(target);
            }

            CanMoveFree = false;
            return;
        }

        uint32 CommonWPCount = sizeof(DragonCommon)/sizeof(Position);

        WaypointId = PointId+1;

        if (WaypointId >= CommonWPCount)
        {
            if (!CanMoveFree)
                CanMoveFree = true;

            WaypointId = 0;
        }

        MoveNextTimer = 500;
    }

    void DoRaidWhisper(int32 TextId)
    {
        Map* map = me->GetMap();

        if (map && map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    DoSendQuantumText(TextId, me, i->getSource());
            }
        }
    }

    void OpenPortal()
    {
        int32 TextId = 0;

        Position spawnPos;

        switch (me->GetEntry())
        {
            case NPC_TENEBRON:
            {
                TextId = WHISPER_HATCH_EGGS;

                for(uint32 i = 0; i < 6; ++i)
                {
                    me->GetRandomNearPosition(spawnPos, 10.0f);
                    me->SummonCreature(NPC_TWILIGHT_EGG, spawnPos, TEMPSUMMON_CORPSE_DESPAWN);
                }
                break;
            }
            case NPC_SHADRON:
            {
                TextId = WHISPER_OPEN_PORTAL;

                me->GetRandomNearPosition(spawnPos, 10.0f);
                me->SummonCreature(NPC_ACOLYTE_OF_SHADRON, spawnPos, TEMPSUMMON_CORPSE_DESPAWN);
                break;
            }
            case NPC_VESPERON:
            {
                TextId = WHISPER_OPEN_PORTAL;

                me->GetRandomNearPosition(spawnPos, 10.0f);
                me->SummonCreature(NPC_ACOLYTE_OF_VESPERON, spawnPos, TEMPSUMMON_CORPSE_DESPAWN);
                break;
            }
        }

        DoRaidWhisper(TextId);

        if (!me->FindGameobjectWithDistance(GO_TWILIGHT_PORTAL, 65.0f))
        {
            if (instance)
            {
                if (instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                {
                    if (GameObject* pPortal = me->SummonGameObject(GO_TWILIGHT_PORTAL, 3234.3f, 544.7f, 58.7f, me->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 60*MINUTE*IN_MILLISECONDS))
                    {
                        me->RemoveGameObject(pPortal, false);
                        pPortal->SetSpawnedByDefault(false);
                        pPortal->SetPhaseMask(17, true);
                    }
                }
                else if (GameObject* pPortal = me->SummonGameObject(GO_TWILIGHT_PORTAL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 60*MINUTE*IN_MILLISECONDS))
                {
                    me->RemoveGameObject(pPortal, false);
                    pPortal->SetSpawnedByDefault(false);
                    pPortal->SetPhaseMask(17, true);
                }
            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        int32 TextId = 0;
        uint32 SpellId = 0;

        switch (me->GetEntry())
        {
            case NPC_TENEBRON:
                TextId = SAY_TENEBRON_DEATH;
                SpellId = SPELL_POWER_OF_TENEBRON;

				if (instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
					instance->SetData(TYPE_TENEBRON_PREKILLED, DONE);

                break;
            case NPC_SHADRON:
                TextId = SAY_SHADRON_DEATH;
                SpellId = SPELL_POWER_OF_SHADRON;

				if (instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
					instance->SetData(TYPE_SHADRON_PREKILLED, DONE);

                if (Creature* pAcolyte = me->FindCreatureWithDistance(NPC_ACOLYTE_OF_SHADRON, 100.0f))
                    pAcolyte->Kill(pAcolyte);

                break;
            case NPC_VESPERON:
                TextId = SAY_VESPERON_DEATH;
                SpellId = SPELL_POWER_OF_VESPERON;

				if (instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
					instance->SetData(TYPE_VESPERON_PREKILLED, DONE);

                if (Creature* pAcolyte = me->FindCreatureWithDistance(NPC_ACOLYTE_OF_VESPERON, 100.0f))
                    pAcolyte->Kill(pAcolyte);

                break;
        }

        DoSendQuantumText(TextId, me);

        me->RemoveAurasDueToSpell(SpellId);

		instance->DoRemoveAurasDueToSpellOnPlayers(SpellId);

		if (instance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
			return;

		if (Unit* pSartharion = Unit::GetUnit(*me, instance->GetData64(DATA_SARTHARION)))
		{
			if (pSartharion->IsAlive())
			{
				pSartharion->RemoveAurasDueToSpell(SpellId);

				if (Aura* RevengeAura = pSartharion->GetAura(SPELL_TWILIGHT_REVENGE))
				{
					RevengeAura->SetCharges(RevengeAura->GetCharges() + 1);
					RevengeAura->SetStackAmount(RevengeAura->GetStackAmount() + 1);
				}
				else
					me->AddAura(SPELL_TWILIGHT_REVENGE, pSartharion);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanMoveFree && MoveNextTimer)
        {
            if (MoveNextTimer <= diff)
            {
                if (WaypointId < MAX_WAYPOINT)
				{
                    if (me->GetMotionMaster())
					{
                        me->GetMotionMaster()->MovePoint(WaypointId, DragonCommon[WaypointId].GetPositionX(), DragonCommon[WaypointId].GetPositionY(), DragonCommon[WaypointId].GetPositionZ());
					}
				}
                MoveNextTimer = 0;
            }
            else
			{
                MoveNextTimer -= diff;
			}
        }
    }
};

class boss_tenebron : public CreatureScript
{
public:
    boss_tenebron() : CreatureScript("boss_tenebron") { }

    struct boss_tenebronAI : public dummy_dragonAI
    {
        boss_tenebronAI(Creature* creature) : dummy_dragonAI(creature) {}

        uint32 ShadowBreathTimer;
        uint32 ShadowFissureTimer;
        uint32 HatchEggTimer;

        bool HasPortalOpen;

        void Reset()
        {
            me->ResetLootMode();

            ShadowBreathTimer = 20000;
            ShadowFissureTimer = 5000;
            HatchEggTimer = 30000;

            HasPortalOpen = false;

            dummy_dragonAI::Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_TENEBRON_AGGRO, me);
            DoZoneInCombat();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_TENEBRON_SLAY_1, SAY_TENEBRON_SLAY_2), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                dummy_dragonAI::UpdateAI(diff);
                return;
            }

            if (ShadowFissureTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, RAID_MODE(SPELL_SHADOW_FISSURE_10, SPELL_SHADOW_FISSURE_25));
					ShadowFissureTimer = urand(15000, 20000);
				}
            }
            else ShadowFissureTimer -= diff;

            if (HatchEggTimer <= diff)
            {
                OpenPortal();
                HatchEggTimer = urand(45000, 70000);
            }
            else HatchEggTimer -= diff;

            if (ShadowBreathTimer <= diff)
            {
                DoSendQuantumText(SAY_TENEBRON_BREATH, me);
                DoCastVictim(RAID_MODE(SPELL_SHADOW_BREATH_10, SPELL_SHADOW_BREATH_25));
                ShadowBreathTimer = urand(20000, 25000);
            }
            else ShadowBreathTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_tenebronAI(creature);
    }
};

class boss_shadron : public CreatureScript
{
public:
    boss_shadron() : CreatureScript("boss_shadron") { }

    struct boss_shadronAI : public dummy_dragonAI
    {
        boss_shadronAI(Creature* creature) : dummy_dragonAI(creature) {}

        uint32 ShadowBreathTimer;
        uint32 ShadowFissureTimer;
        uint32 AcolyteShadronTimer;

        bool HasPortalOpen;

        void Reset()
        {
            me->ResetLootMode();

            ShadowBreathTimer = 20000;
            ShadowFissureTimer = 5000;
            AcolyteShadronTimer = 60000;

            if (me->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
                me->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);

            HasPortalOpen = false;

            dummy_dragonAI::Reset();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_SHADRON_AGGRO, me);
            DoZoneInCombat();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
        }

        void JustDied(Unit* killer)
        {
			if (Unit* Sartharion = Unit::GetUnit(*me, instance->GetData64(DATA_SARTHARION)))
			{
				if (Sartharion->HasAura(SPELL_GIFT_OF_TWILIGTH_SAR))
					Sartharion->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
			}

            if (killer)
                dummy_dragonAI::JustDied(killer);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SHADRON_SLAY_1, SAY_SHADRON_SLAY_2), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                dummy_dragonAI::UpdateAI(diff);
                return;
            }

            if (ShadowFissureTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, RAID_MODE(SPELL_SHADOW_FISSURE_10, SPELL_SHADOW_FISSURE_25));
					ShadowFissureTimer = urand(15000, 20000);
				}
            }
            else ShadowFissureTimer -= diff;

            if (AcolyteShadronTimer <= diff)
            {
                if (HasPortalOpen)
                    AcolyteShadronTimer = 10000;
                else
                {
                    OpenPortal();
                    HasPortalOpen = true;
                    AcolyteShadronTimer = urand(45000, 70000);
                }
            }
            else AcolyteShadronTimer -= diff;

            if (ShadowBreathTimer <= diff)
            {
                DoSendQuantumText(SAY_SHADRON_BREATH, me);
                DoCastVictim(RAID_MODE(SPELL_SHADOW_BREATH_10, SPELL_SHADOW_BREATH_25));
                ShadowBreathTimer = urand(20000, 25000);
            }
            else ShadowBreathTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_shadronAI(creature);
    }
};

class boss_vesperon : public CreatureScript
{
public:
    boss_vesperon() : CreatureScript("boss_vesperon") { }

    struct boss_vesperonAI : public dummy_dragonAI
    {
        boss_vesperonAI(Creature* creature) : dummy_dragonAI(creature) {}

        uint32 ShadowBreathTimer;
        uint32 ShadowFissureTimer;
        uint32 AcolyteVesperonTimer;

        bool HasPortalOpen;

        void Reset()
        {
            me->ResetLootMode();

            HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER, true, false, 0, false, false);
            HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER, true, false, 0, false, false);

            ShadowBreathTimer = 20000;
            ShadowFissureTimer = 5000;
            AcolyteVesperonTimer = 60000;

            HasPortalOpen = false;

            dummy_dragonAI::Reset();
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_VESPERON_AGGRO, me);
            DoZoneInCombat();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
        }

        void JustDied(Unit* killer)
        {
            HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER, true, false, 0, false, false);
            HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER, true, false, 0, false, false);

            if (killer)
                dummy_dragonAI::JustDied(killer);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_VESPERON_SLAY_1, SAY_VESPERON_SLAY_2), me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                dummy_dragonAI::UpdateAI(diff);
                return;
            }

            if (ShadowFissureTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, RAID_MODE(SPELL_SHADOW_FISSURE_10, SPELL_SHADOW_FISSURE_25));
					ShadowFissureTimer = urand(15000, 20000);
				}
            }
            else ShadowFissureTimer -= diff;

            if (AcolyteVesperonTimer <= diff)
            {
                if (HasPortalOpen)
                    AcolyteVesperonTimer = 10000;
                else
                {
                    OpenPortal();
                    HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER, true, true, 1, false, true);
                    AcolyteVesperonTimer = urand(45000, 70000);
                }
            }
            else AcolyteVesperonTimer -= diff;

            if (ShadowBreathTimer <= diff)
            {
                DoSendQuantumText(SAY_VESPERON_BREATH, me);
                DoCastVictim(RAID_MODE(SPELL_SHADOW_BREATH_10, SPELL_SHADOW_BREATH_25));
                ShadowBreathTimer = urand(20000, 25000);
            }
            else ShadowBreathTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vesperonAI(creature);
    }
};

class npc_acolyte_of_shadron : public CreatureScript
{
public:
    npc_acolyte_of_shadron() : CreatureScript("npc_acolyte_of_shadron") { }

    struct npc_acolyte_of_shadronAI : public QuantumBasicAI
    {
        npc_acolyte_of_shadronAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 DespawnTimer;
     
        void Reset()
        {
            DespawnTimer = 35000;
            if (instance)
            {
                Creature* target = NULL;
                if (instance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                {
                    if (target = ((Creature*)Unit::GetUnit(*me, instance->GetData64(DATA_SARTHARION))))
                        target->AddAura(SPELL_GIFT_OF_TWILIGTH_SAR, target);
                }
                else
                {
                    if (target = ((Creature*)Unit::GetUnit(*me, instance->GetData64(DATA_SHADRON))))
                        target->AddAura(SPELL_GIFT_OF_TWILIGTH_SHA, target);
                }
			}

            if (!me->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                me->AddAura(SPELL_TWILIGHT_SHIFT_ENTER, me);
        }

        void JustDied(Unit* killer)
        {
            if (instance)
            {
                Creature* Shadron = instance->instance->GetCreature(instance->GetData64(DATA_SHADRON));

                if (Shadron)
					(CAST_AI(boss_shadron::boss_shadronAI, Shadron->AI()))->HasPortalOpen = false;

                Creature* pDebuffTarget = NULL;

                if (killer)
                {
                    if (killer->GetTypeId() == TYPE_ID_PLAYER || (killer->GetOwner() && killer->GetOwner()->GetTypeId() == TYPE_ID_PLAYER))
                    {
                        pDebuffTarget = instance->instance->GetCreature(instance->GetData64(DATA_SARTHARION));
                        if (pDebuffTarget)
						{
                            if (pDebuffTarget->IsAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SAR))
                                pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
						}

                        pDebuffTarget = instance->instance->GetCreature(instance->GetData64(DATA_SHADRON));
                        if (pDebuffTarget)
						{
                            if (pDebuffTarget->IsAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
                                pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);
						}
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer < diff)
            {
                me->SetVisible(false);
                me->Kill(me);
                return;
            }
			else DespawnTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_acolyte_of_shadronAI(creature);
    }
};

class npc_acolyte_of_vesperon : public CreatureScript
{
public:
    npc_acolyte_of_vesperon() : CreatureScript("npc_acolyte_of_vesperon") { }

    struct npc_acolyte_of_vesperonAI : public QuantumBasicAI
    {
        npc_acolyte_of_vesperonAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 DespawnTimer;

        void Reset()
        {
            DespawnTimer = 35000;

            if (!me->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                me->AddAura(SPELL_TWILIGHT_SHIFT_ENTER, me);

            HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER, true, true, 1, true, false);
        }

        void JustDied(Unit* killer)
        {
            if (instance)
            {
                HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER, true, false, 0, false, false);

                Creature* pVesperon = instance->instance->GetCreature(instance->GetData64(DATA_VESPERON));
                if (pVesperon)
                {
                    if (pVesperon->IsInCombatActive())
                    {
                        (CAST_AI(boss_vesperon::boss_vesperonAI, pVesperon->AI()))->HasPortalOpen = false;

                        if (killer)
                        {
                            if (killer->GetTypeId() == TYPE_ID_PLAYER || (killer->GetOwner() && killer->GetOwner()->GetTypeId() == TYPE_ID_PLAYER))
                                HandleAuraOnRaidSartharion(me, SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER, true, false, 0, false, false);
                        }
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer < diff)
            {
                me->SetVisible(false);
                me->Kill(me);
                return;
            }
			else DespawnTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_acolyte_of_vesperonAI(creature);
    }
};

class mob_twilight_eggs : public CreatureScript
{
public:
    mob_twilight_eggs() : CreatureScript("mob_twilight_eggs") { }

    struct mob_twilight_eggsAI : public QuantumBasicAI
    {
        mob_twilight_eggsAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        uint32 HatchEggTimer;

        InstanceScript* instance;

        void Reset()
        {
            if (!me->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                me->AddAura(SPELL_TWILIGHT_SHIFT_ENTER, me);

            HatchEggTimer = 30000;
        }

        void MoveInLineOfSight(Unit* /*who*/){}

		void AttackStart(Unit* /*who*/){}

        void SpawnWhelps()
        {
            me->RemoveAllAuras();

            Position spawnPos;
            me->GetPosition(&spawnPos);

            if (Creature* whelp = me->SummonCreature(NPC_TWILIGHT_WHELP, spawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
				DoZoneInCombat(whelp);

            me->Kill(me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (HatchEggTimer <= diff)
            {
				Creature* Tenebron = instance->instance->GetCreature(instance->GetData64(DATA_TENEBRON));

				if (Tenebron)
					(CAST_AI(boss_tenebron::boss_tenebronAI, Tenebron->AI()))->HasPortalOpen = false;

				SpawnWhelps();
            }
            else HatchEggTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_eggsAI(creature);
    }
};

class npc_flame_tsunami : public CreatureScript
{
public:
    npc_flame_tsunami() : CreatureScript("npc_flame_tsunami") { }

    struct npc_flame_tsunamiAI : public QuantumBasicAI
    {
        npc_flame_tsunamiAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetDisplayId(MODEL_ID_INVISIBLE);

            if (!me->HasAura(SPELL_FLAME_TSUNAMI))
                me->AddAura(SPELL_FLAME_TSUNAMI, me);
        }

        uint32 TsunamiTimer;
        uint32 TsunamiBuffTimer;

        bool RightDirection;
        bool LeftDirection;

        void Reset()
        {
            TsunamiTimer = 100;
            me->SetCurrentFaction(103);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            TsunamiBuffTimer = 1000;

            RightDirection = false;
            LeftDirection = false;
        }

		void MoveInLineOfSight(Unit* /*who*/){}

		void AttackStart(Unit* /*who*/){}

        void SetData(uint32 type, uint32 value)
        {
            if (type == 1)
            {
                switch (value)
                {
                    case 1:
                        RightDirection = true;
                        break;
                    case 2:
                        LeftDirection = true;
                        break;
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (!target)
                return;

            if (target == me)
                return;

            if (target->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (spell)
            {
                if (spell->Id == SPELL_FLAME_TSUNAMI_DMG_AURA)
                {
                    if (RightDirection)
                    {
                        if (target->GetPositionX() > 3276.0)
                            return;

                        if (target->GetMotionMaster())
                            target->GetMotionMaster()->MoveJump(me->GetPositionX() + 4, me->GetPositionY(), 60.0f, 20, 5);
                    }

                    if (LeftDirection)
                    {
                        if (target->GetPositionX() < 3207.0)
                            return;

                        if (target->GetMotionMaster())
                            target->GetMotionMaster()->MoveJump(me->GetPositionX() - 4, me->GetPositionY(), 60.0f, 20, 5);
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (TsunamiTimer <= diff)
            {
                DoCast(me, SPELL_FLAME_TSUNAMI_DMG_AURA);
                TsunamiTimer = 500;
            }
			else TsunamiTimer -= diff;

            if (TsunamiBuffTimer <= diff)
            {
                if (Unit* LavaBlaze = GetClosestCreatureWithEntry(me, NPC_LAVA_BLAZE, 10.0f, true))
				{
                    LavaBlaze->CastSpell(LavaBlaze, SPELL_FLAME_TSUNAMI_BUFF, true);
					TsunamiBuffTimer = 1000;
				}
            }
			else TsunamiBuffTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flame_tsunamiAI(creature);
    }
};

class npc_twilight_fissure : public CreatureScript
{
public:
    npc_twilight_fissure() : CreatureScript("npc_twilight_fissure") { }

    struct npc_twilight_fissureAI : public QuantumBasicAI
    {
        npc_twilight_fissureAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
			SetCombatMovement(false);
        }

        uint32 VoidBlastTimer;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetCurrentFaction(103);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            VoidBlastTimer = 5000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (VoidBlastTimer <= diff)
            {
                DoCastAOE(RAID_MODE(SPELL_VOID_BLAST_10, SPELL_VOID_BLAST_25));
                me->RemoveAllAuras();
                me->Kill(me);
            }
			else VoidBlastTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_fissureAI(creature);
    }
};

class mob_twilight_whelp : public CreatureScript
{
public:
    mob_twilight_whelp() : CreatureScript("mob_twilight_whelp") { }

    struct mob_twilight_whelpAI : public QuantumBasicAI
    {
        mob_twilight_whelpAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
        }

        uint32 FadeArmorTimer;

        void Reset()
        {
            me->RemoveAllAuras();
            FadeArmorTimer = 1000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FadeArmorTimer <= diff)
            {
                DoCastVictim(SPELL_FADE_ARMOR);
                FadeArmorTimer = urand(5000, 10000);
            }
            else
                FadeArmorTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_whelpAI(creature);
    }
};

class achievement_twilight_assist : public AchievementCriteriaScript
{
    public:
        achievement_twilight_assist() : AchievementCriteriaScript("achievement_twilight_assist") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Sartharion = target->ToCreature())
                if (Sartharion->AI()->GetData(TWILIGHT_ACHIEVEMENTS) >= 1)
                    return true;

            return false;
        }
};

class achievement_twilight_duo : public AchievementCriteriaScript
{
    public:
        achievement_twilight_duo() : AchievementCriteriaScript("achievement_twilight_duo") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Sartharion = target->ToCreature())
                if (Sartharion->AI()->GetData(TWILIGHT_ACHIEVEMENTS) >= 2)
                    return true;

            return false;
        }
};

class achievement_twilight_zone : public AchievementCriteriaScript
{
    public:
        achievement_twilight_zone() : AchievementCriteriaScript("achievement_twilight_zone") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Sartharion = target->ToCreature())
                if (Sartharion->AI()->GetData(TWILIGHT_ACHIEVEMENTS) == 3)
                    return true;

            return false;
        }
};

class achievement_volcano_blows : public AchievementCriteriaScript
{
    public:
        achievement_volcano_blows() : AchievementCriteriaScript("achievement_volcano_blows") { }

        bool OnCheck(Player* player, Unit* target)
        {
            if (!target)
                return false;

            if (boss_sartharion::boss_sartharionAI* sartharionAI = CAST_AI(boss_sartharion::boss_sartharionAI, target->GetAI()))
                return sartharionAI->wasPlayerHitByLavaStrike(player->GetGUID());

            return false;
        }
};

class go_portal_sartharion : public GameObjectScript
{
public:
	go_portal_sartharion() : GameObjectScript("go_portal_sartharion") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!player || !go)
            return true;

        if (InstanceScript* instance = go->GetInstanceScript())
        {
            if (instance->instance)
            {
                if (player->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                {
                    player->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT);
                    player->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
                    player->CastSpell(player, SPELL_TWILIGHT_RESIDUE, true);

                    if (Creature* Vesperon = instance->instance->GetCreature(instance->GetData64(DATA_VESPERON)))
                    {
                        if (Vesperon->GetVictim() && Vesperon->IsInCombatActive())
                        {
                            if (player->HasAura(SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER))
                                player->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER);

                            if (Vesperon->GetVictim()->HasAura(SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER))
                            {
                                if (Aura* aur = Vesperon->AddAura(SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER, player))
                                {
                                    aur->SetStackAmount(1);
                                    aur->SetCharges(1);
                                }
                            }
                        }
                    }
                }
                else
                {
                    Creature* Sartharion = instance->instance->GetCreature(instance->GetData64(DATA_SARTHARION));
                    Creature* Vesperon = instance->instance->GetCreature(instance->GetData64(DATA_VESPERON));
                    Creature* Shadron = instance->instance->GetCreature(instance->GetData64(DATA_SHADRON));
                    Creature* Tenebron = instance->instance->GetCreature(instance->GetData64(DATA_TENEBRON));

                    if (Sartharion)
                    {
                        if (Sartharion->GetVictim())
                        {
                            if (Sartharion->GetVictim() == player)
                                return true;

                            if (Sartharion->GetVictim()->GetOwner())
                                if (Sartharion->GetVictim()->GetOwner() == player)
                                    return true;
                        }
                    }

                    if (Vesperon)
                    {
                        if (Vesperon->GetVictim())
                        {
                            if (Vesperon->GetVictim() == player)
                                return true;

                            if (Vesperon->GetVictim()->GetOwner())
                                if (Vesperon->GetVictim()->GetOwner() == player)
                                    return true;
                        }
                    }

                    if (Tenebron)
                    {
                        if (Tenebron->GetVictim())
                        {
                            if (Tenebron->GetVictim() == player)
                                return true;

                            if (Tenebron->GetVictim()->GetOwner())
                                if (Tenebron->GetVictim()->GetOwner() == player)
                                    return true;
                        }
                    }

                    if (Shadron)
                    {
                        if (Shadron->GetVictim())
                        {
                            if (Shadron->GetVictim() == player)
                                return true;

                            if (Shadron->GetVictim()->GetOwner())
                                if (Shadron->GetVictim()->GetOwner() == player)
                                    return true;
                        }
                    }

                    if (!player->HasAura(SPELL_TWILIGHT_RESIDUE))
                    {
                        player->CastSpell(player, SPELL_TWILIGHT_SHIFT_ENTER, true);

                        if (Vesperon)
                        {
                            if (Vesperon->IsInCombatActive())
                            {
                                if (go->FindCreatureWithDistance(NPC_ACOLYTE_OF_VESPERON, 100.0f, true))
                                {
                                    if (Aura* acoAur = player->AddAura(SPELL_TWILIGHT_TORMENT_VESP_ACO_ON_PLAYER, player))
                                    {
                                        acoAur->SetStackAmount(1);
                                        acoAur->SetCharges(1);
                                    }
                                }

                                if (player->HasAura(SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER))
                                    player->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP_ON_PLAYER);
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
};

void AddSC_boss_sartharion()
{
    new boss_sartharion();
    new boss_vesperon();
    new boss_shadron();
    new boss_tenebron();
    new npc_acolyte_of_shadron();
    new npc_acolyte_of_vesperon();
    new mob_twilight_eggs();
    new npc_flame_tsunami();
    new npc_twilight_fissure();
    new mob_twilight_whelp();
    new achievement_twilight_assist();
    new achievement_twilight_duo();
    new achievement_twilight_zone();
    new achievement_volcano_blows();
    new go_portal_sartharion();
}