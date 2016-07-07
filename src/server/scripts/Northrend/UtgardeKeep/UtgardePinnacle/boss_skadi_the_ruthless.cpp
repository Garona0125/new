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
#include "Player.h"
#include "SpellInfo.h"

enum Texts
{
    SAY_AGGRO               = -1475000,
	SAY_DRAKE_BREATH_1      = -1475001,
    SAY_DRAKE_BREATH_2      = -1475002,
    SAY_DRAKE_BREATH_3      = -1475003,
	SAY_DRAKE_HARPOON_1     = -1475004,
	SAY_DRAKE_HARPOON_2     = -1475005,
	SAY_KILL_1              = -1475006,
    SAY_KILL_2              = -1475007,
	SAY_KILL_3              = -1475008,
	SAY_DEATH               = -1475009,
	SAY_DRAKE_DEATH         = -1475010,
	EMOTE_HARPOON_RANGE     = -1475011,
};

enum Spells
{
    SPELL_CRUSH_5N          = 50234,
	SPELL_CRUSH_5H          = 59330,
    SPELL_POISONED_SPEAR_5N = 50225,
	SPELL_POISONED_SPEAR_5H = 59331,
    SPELL_WHIRLWIND         = 50228,
    SPELL_RAPID_FIRE        = 56570,
    SPELL_HARPOON_DAMAGE    = 56578,
    SPELL_FREEZING_CLOUD    = 47579,
	SPELL_SUMMON_HARPOON    = 56789,
};

enum CombatPhase
{
    FLYING = 0,
    SKADI  = 1,
};

static Position SpawnLoc = {468.931f, -513.555f, 104.723f, 0};

static Position Location[] =
{
    // Boss
    {341.740997f, -516.955017f, 104.66900f, 0}, // 0
    {293.299f, -505.95f, 142.03f, 0},           // 1
    {301.664f, -535.164f, 146.097f, 0},         // 2
    {521.031006f, -544.667847f, 128.80064f, 0}, // 3
    {477.311981f, -509.296814f, 104.72308f, 0}, // 4
    {341.740997f, -516.955017f, 104.66900f, 0}, // 5
    {341.740997f, -516.955017f, 104.66900f, 0}, // 6
    {341.740997f, -516.955017f, 104.66900f, 0}, // 7
    // Triggers Left
    {469.661f, -484.546f, 104.712f, 0},         // 8
    {483.315f, -485.028f, 104.718f, 0},         // 9
    {476.87f, -487.994f, 104.735f, 0},          //10
    {477.512f, -497.772f, 104.728f, 0},         //11
    {486.287f, -500.759f, 104.722f, 0},         //12
    {480.1f, -503.895f, 104.722f, 0},           //13
    {472.391f, -505.103f, 104.723f, 0},         //14
    {478.885f, -510.803f, 104.723f, 0},         //15
    {489.529f, -508.615f, 104.723f, 0},         //16
    {484.272f, -508.589f, 104.723f, 0},         //17
    {465.328f, -506.495f, 104.427f, 0},         //18
    {456.885f, -508.104f, 104.447f, 0},         //19
    {450.177f, -507.989f, 105.247f, 0},         //20
    {442.273f, -508.029f, 104.813f, 0},         //21
    {434.225f, -508.19f, 104.787f, 0},          //22
    {423.902f, -508.525f, 104.274f, 0},         //23
    {414.551f, -508.645f, 105.136f, 0},         //24
    {405.787f, -508.755f, 104.988f, 0},         //25
    {398.812f, -507.224f, 104.82f, 0},          //26
    {389.702f, -506.846f, 104.729f, 0},         //27
    {381.856f, -506.76f, 104.756f, 0},          //28
    {372.881f, -507.254f, 104.779f, 0},         //29
    {364.978f, -508.182f, 104.673f, 0},         //30
    {357.633f, -508.075f, 104.647f, 0},         //31
    {350.008f, -506.826f, 104.588f, 0},         //32
    {341.69f, -506.77f, 104.499f, 0},           //33
    {335.31f, -505.745f, 105.18f, 0},           //34
    {471.178f, -510.74f, 104.723f, 0},          //35
    {461.759f, -510.365f, 104.199f, 0},         //36
    {424.07287f, -510.082916f, 104.711082f, 0}, //37
    // Triggers Right
    {489.46f, -513.297f, 105.413f, 0},          //38
    {485.706f, -517.175f, 104.724f, 0},         //39
    {480.98f, -519.313f, 104.724f, 0},          //40
    {475.05f, -520.52f, 104.724f, 0},           //41
    {482.97f, -512.099f, 104.724f, 0},          //42
    {477.082f, -514.172f, 104.724f, 0},         //43
    {468.991f, -516.691f, 104.724f, 0},         //44
    {461.722f, -517.063f, 104.627f, 0},         //45
    {455.88f, -517.681f, 104.707f, 0},          //46
    {450.499f, -519.099f, 104.701f, 0},         //47
    {444.889f, -518.963f, 104.82f, 0},          //48
    {440.181f, -518.893f, 104.861f, 0},         //49
    {434.393f, -518.758f, 104.891f, 0},         //50
    {429.328f, -518.583f, 104.904f, 0},         //51
    {423.844f, -518.394f, 105.004f, 0},         //52
    {418.707f, -518.266f, 105.135f, 0},         //53
    {413.377f, -518.085f, 105.153f, 0},         //54
    {407.277f, -517.844f, 104.893f, 0},         //55
    {401.082f, -517.443f, 104.723f, 0},         //56
    {394.933f, -514.64f, 104.724f, 0},          //57
    {388.917f, -514.688f, 104.734f, 0},         //58
    {383.814f, -515.834f, 104.73f, 0},          //59
    {377.887f, -518.653f, 104.777f, 0},         //60
    {371.376f, -518.289f, 104.781f, 0},         //61
    {365.669f, -517.822f, 104.758f, 0},         //62
    {359.572f, -517.314f, 104.706f, 0},         //63
    {353.632f, -517.146f, 104.647f, 0},         //64
    {347.998f, -517.038f, 104.538f, 0},         //65
    {341.803f, -516.98f, 104.584f, 0},          //66
    {335.879f, -516.674f, 104.628f, 0},         //67
    {329.871f, -515.92f, 104.711f, 0},          //68
    // Breach Zone
    {503.6110f, -527.6420f, 115.3011f, 0},      //69
    {435.1892f, -514.5232f, 118.6719f, 0},      //70
    {413.9327f, -540.9407f, 138.2614f, 0},      //71
};

class boss_skadi_the_ruthless : public CreatureScript
{
public:
    boss_skadi_the_ruthless() : CreatureScript("boss_skadi_the_ruthless") { }

    struct boss_skadi_the_ruthlessAI : public QuantumBasicAI
    {
        boss_skadi_the_ruthlessAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;
        uint64 GraufGUID;
        std::vector<uint64> triggersGUID;

        uint32 CrushTimer;
        uint32 PoisonedSpearTimer;
        uint32 WhirlwindTimer;
        uint32 WaypointId;
        uint32 MovementTimer;
        uint32 MountTimer;
        uint32 SummonTimer;
        uint8  SpellHitCount;
        uint32 LoopCounter[3];

        CombatPhase Phase;

        void Reset()
        {
            triggersGUID.clear();

            CrushTimer = 8000;
            PoisonedSpearTimer = 10000;
            WhirlwindTimer = 20000;
            MountTimer = 3000;
            WaypointId = 0;
            SpellHitCount = 0;
            for(uint8 i = 0; i < 3; i++)
                LoopCounter[i] = 0;

            Phase = SKADI;

            Summons.DespawnAll();
            me->SetSpeed(MOVE_FLIGHT, 3.0f);
            me->SetDisableGravity(false);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if ((Unit::GetCreature(*me, GraufGUID) == NULL) && !me->IsMounted())
                 me->SummonCreature(NPC_GRAUF, Location[0].GetPositionX(), Location[0].GetPositionY(), Location[0].GetPositionZ(), 3.0f);

			instance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, NOT_STARTED);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
        }

        void JustReachedHome()
        {
			me->RemoveMount();
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            me->SetCanFly(false);
			me->SetDisableGravity(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (Unit::GetCreature(*me, GraufGUID) == NULL)
                me->SummonCreature(NPC_GRAUF, Location[0].GetPositionX(), Location[0].GetPositionY(), Location[0].GetPositionZ(), 3.0f);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            Phase = FLYING;

            MovementTimer = 1000;
            SummonTimer = 10000;
            me->SetInCombatWithZone();

			instance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, IN_PROGRESS);
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
			me->GetMotionMaster()->MoveJump(Location[0].GetPositionX(), Location[0].GetPositionY(), Location[0].GetPositionZ(), 5.0f, 10.0f);
			me->SetWalk(false);
			MountTimer = 1000;
			Summons.DespawnEntry(NPC_GRAUF);
        }

        void JustSummoned(Creature* summoned)
        {
            switch (summoned->GetEntry())
            {
                case NPC_GRAUF:
                    GraufGUID = summoned->GetGUID();
                    break;
                case NPC_YMIRJAR_WARRIOR:
                case NPC_YMIRJAR_WITCH_DOCTOR:
                case NPC_YMIRJAR_HARPOONER:
                    summoned->SetActive(true);
                    summoned->SetInCombatWithZone();
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        summoned->AI()->AttackStart(target);
                    break;
                case NPC_FLAME_BREATH_TRIGGER:
                    summoned->CastSpell((Unit*)NULL, SPELL_FREEZING_CLOUD, true);
                    summoned->DespawnAfterAction(10*IN_MILLISECONDS);
                    break;
            }
            Summons.Summon(summoned);
        }

        void SummonedCreatureDies(Creature* creature, Unit* killer)
        {
            if (creature->GetEntry() == NPC_YMIRJAR_HARPOONER)
				killer->CastSpell(creature, SPELL_SUMMON_HARPOON, true);
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_GRAUF)
			{
                GraufGUID = 0;
			}
            Summons.Despawn(summoned);
        }

        void DoAction(const int32 action)
        {
            if (action == 1)
            {
                if (SpellHitCount == 0)
                    LoopCounter[1] = LoopCounter[0];
                me->SetHealth(me->GetMaxHealth());
                SpellHitCount++;
                if (SpellHitCount >= 3)
                {
                    LoopCounter[2] = LoopCounter[0];
                    Phase = SKADI;

					me->RemoveMount();
					me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
					me->SetCanFly(false);
					me->SetDisableGravity(false);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                    if (Creature* Grauf = me->SummonCreature(NPC_GRAUF, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS))
                    {
                        Grauf->GetMotionMaster()->MoveFall();
                        Grauf->HandleEmoteCommand(EMOTE_ONESHOT_FLY_DEATH);
                    }

                    me->GetMotionMaster()->MoveJump(Location[4].GetPositionX(), Location[4].GetPositionY(), Location[4].GetPositionZ(), 5.0f, 10.0f);
                    DoSendQuantumText(SAY_DRAKE_DEATH, me);
                    CrushTimer = 8000;
                    PoisonedSpearTimer = 10000;
                    WhirlwindTimer = 20000;

                    if (Unit* target = SelectTarget(TARGET_RANDOM))
                    {
                        me->GetMotionMaster()->MoveChase(target);
                        me->AI()->AttackStart(target);
                    }
                }
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_HARPOON_DAMAGE)
			{
				if (!(rand()%5))
					DoSendQuantumText(RAND(SAY_DRAKE_HARPOON_1, SAY_DRAKE_HARPOON_2), me);
				DoAction(1);
			}
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (Phase != FLYING)
                return;

            if (id == 3)
            {
                LoopCounter[0]++;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (Creature* trigger = me->SummonCreature(NPC_WORLD_TRIGGER, Location[4].GetPositionX(), Location[4].GetPositionY(), Location[4].GetPositionZ()))
				{
					DoSendQuantumText(EMOTE_HARPOON_RANGE, trigger, me);
				}
				DoSendQuantumText(EMOTE_HARPOON_RANGE, me);
            }
			else
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            switch (Phase)
            {
                case FLYING:
                    if (!UpdateVictim())
                        return;

                    if (MountTimer && MountTimer <= diff)
                    {
                        me->Mount(NPC_MOUNT);
						me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
                        me->SetCanFly(true);
						me->SetDisableGravity(true);
                        MountTimer = 0;
                    }
					else MountTimer -= diff;

                    if (SummonTimer <= diff)
                    {
                        SpawnMobs();
                        SummonTimer = 25000;
                    }
					else SummonTimer -= diff;

                    if (MovementTimer <= diff)
                    {
                        switch (WaypointId)
                        {
                            case 0:
                                me->GetMotionMaster()->MovePoint(0, Location[1].GetPositionX(), Location[1].GetPositionY(), Location[1].GetPositionZ());
                                MovementTimer = 5000;
                                break;
                            case 1:
                                me->GetMotionMaster()->MovePoint(2, Location[2].GetPositionX(), Location[2].GetPositionY(), Location[2].GetPositionZ());
                                MovementTimer = 2000;
                                break;
                            case 2:
                                me->GetMotionMaster()->MovePoint(3, Location[3].GetPositionX(), Location[3].GetPositionY(), Location[3].GetPositionZ());
                                MovementTimer = 11000;
                                break;
                            case 3:
                                me->GetMotionMaster()->MovePoint(4, Location[69].GetPositionX(), Location[69].GetPositionY(), Location[69].GetPositionZ());
                                DoSendQuantumText(RAND(SAY_DRAKE_BREATH_1, SAY_DRAKE_BREATH_2), me);
                                MovementTimer = 2500;
                                break;
                            case 4:
                                me->GetMotionMaster()->MovePoint(5, Location[70].GetPositionX(), Location[70].GetPositionY(), Location[70].GetPositionZ());
                                MovementTimer = 2000;
                                SpawnTrigger();
                                break;
                            case 5:
                                me->GetMotionMaster()->MovePoint(6, Location[71].GetPositionX(), Location[71].GetPositionY(), Location[71].GetPositionZ());
                                MovementTimer = 3000;
                                break;
                            case 6:
                                me->GetMotionMaster()->MovePoint(3, Location[3].GetPositionX(), Location[3].GetPositionY(), Location[3].GetPositionZ());
                                WaypointId = 2;
                                MovementTimer = 15000;
                                break;
                        }
                        WaypointId++;
                    }
					else
						MovementTimer -= diff;
					break;
                case SKADI:

                    if (!UpdateVictim())
                        return;

                    if (CrushTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_CRUSH_5N, SPELL_CRUSH_5H));
                        CrushTimer = 8000;
                    }
					else CrushTimer -= diff;

                    if (PoisonedSpearTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_POISONED_SPEAR_5N, SPELL_POISONED_SPEAR_5H));
							PoisonedSpearTimer = 10000;
						}
                    }
					else PoisonedSpearTimer -= diff;

                    if (WhirlwindTimer <= diff)
                    {
                        DoCastAOE(SPELL_WHIRLWIND);
                        WhirlwindTimer = 20000;
                    }
					else WhirlwindTimer -= diff;

                    DoMeleeAttackIfReady();
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
            Summons.DespawnAll();

			instance->SetData(DATA_SKADI_THE_RUTHLESS_EVENT, DONE);

			if (LoopCounter[1] == LoopCounter[2])
				instance->DoCompleteAchievement(ACHIEVEMENT_GIRL_LOVES_SKADI);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3), me);
        }

        void SpawnMobs()
        {
            for (uint8 i = 0; i < DUNGEON_MODE(5, 6); ++i)
            {
                switch (urand(0, 2))
                {
                    case 0:
						me->SummonCreature(NPC_YMIRJAR_WARRIOR, SpawnLoc.GetPositionX()+rand()%5, SpawnLoc.GetPositionY()+rand()%5, SpawnLoc.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
						break;
                    case 1:
						me->SummonCreature(NPC_YMIRJAR_WITCH_DOCTOR, SpawnLoc.GetPositionX()+rand()%5, SpawnLoc.GetPositionY()+rand()%5, SpawnLoc.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
						break;
                    case 2:
						me->SummonCreature(NPC_YMIRJAR_HARPOONER, SpawnLoc.GetPositionX()+rand()%5, SpawnLoc.GetPositionY()+rand()%5, SpawnLoc.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
						break;
                }
            }
        }

        void SpawnTrigger()
        {
            uint8 iStart = 0, iEnd = 0;
            switch (urand(0, 1))
            {
                case 0:
                    iStart = 8;
                    iEnd = 37;
                    break;
                case 1:
                    iStart = 38;
                    iEnd = 68;
                    break;
                default:
                    break;
            }
            for (uint32 i = iStart; i < iEnd; ++i)
                me->SummonCreature(NPC_FLAME_BREATH_TRIGGER, Location[i]);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_skadi_the_ruthlessAI(creature);
    }
};

class go_harpoon_launcher : public GameObjectScript
{
public:
    go_harpoon_launcher() : GameObjectScript("go_harpoon_launcher") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
			return false;

        if (Creature* skadi = Unit::GetCreature((*go), instance->GetData64(DATA_SKADI_THE_RUTHLESS)))
        {
            if (!skadi->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                player->CastSpell(skadi, SPELL_RAPID_FIRE, true);
                
        }
        return false;
    }
};

class go_skadi_harpoon : public GameObjectScript
{
public:
	go_skadi_harpoon() : GameObjectScript("go_skadi_harpoon") {}

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->CastSpell(player, 56790, true);
		go->RemoveFromWorld();
		return true;
	}
};

void AddSC_boss_skadi_the_ruthless()
{
    new boss_skadi_the_ruthless();
    new go_harpoon_launcher();
    new go_skadi_harpoon();
}