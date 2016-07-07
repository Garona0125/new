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
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "sunwell_plateau.h"

enum Quotes
{
    // Kalecgos dragon form
    SAY_EVIL_AGGRO          = -1580000,
    SAY_EVIL_SPELL_1        = -1580001,
    SAY_EVIL_SPELL_2        = -1580002,
    SAY_EVIL_SLAY_1         = -1580003,
    SAY_EVIL_SLAY_2         = -1580004,
    SAY_EVIL_ENRAGE         = -1580005,
    // Kalecgos humanoid form
    SAY_GOOD_AGGRO          = -1580006,
    SAY_GOOD_NEAR_DEATH_1   = -1580007,
    SAY_GOOD_NEAR_DEATH_2   = -1580008,
    SAY_GOOD_PLRWIN         = -1580009,
    // Shattrowar
    SAY_SATH_AGGRO          = -1580010,
    SAY_SATH_DEATH          = -1580011,
    SAY_SATH_SPELL_1        = -1580012,
    SAY_SATH_SPELL_2        = -1580013,
    SAY_SATH_SLAY_1         = -1580014,
    SAY_SATH_SLAY_2         = -1580015,
    SAY_SATH_ENRAGE         = -1580016,
};

enum Spells
{
    SPELL_SPECTRAL_EXHAUSTION   = 44867,
    SPELL_SPECTRAL_REALM        = 46021,
    SPELL_SPECTRAL_INVISIBILITY = 44801,
    SPELL_DEMONIC_VISUAL        = 44800,
    SPELL_SPECTRAL_BLAST        = 44869,
    SPELL_TELEPORT_SPECTRAL     = 46019,
    SPELL_ARCANE_BUFFET         = 45018,
    SPELL_FROST_BREATH          = 44799,
    SPELL_TAIL_LASH             = 45122,
    SPELL_BANISH                = 44836,
    SPELL_TRANSFORM_KALEC       = 44670,
    SPELL_ENRAGE                = 44807,
    SPELL_CORRUPTION_STRIKE     = 45029,
    SPELL_CURSE_OF_AGONY        = 45032,
    SPELL_SHADOW_BOLT_VOLLEY    = 45031,
    SPELL_HEROIC_STRIKE         = 45026,
    SPELL_REVITALIZE            = 45027,
};

#define GO_FAILED "You are unable to use this currently."

#define FLY_X     1679
#define FLY_Y     900
#define FLY_Z     82

#define CENTER_X  1705
#define CENTER_Y  930
#define RADIUS    30

#define DRAGON_REALM_Z 53.079f
#define DEMON_REALM_Z  -74.558f

uint32 WildMagic[] =
{
	44978,
	45001,
	45002,
	45004,
	45006,
	45010
};

class boss_kalecgos_dragon_form : public CreatureScript
{
public:
    boss_kalecgos_dragon_form() : CreatureScript("boss_kalecgos_dragon_form") {}

    struct boss_kalecgos_dragon_formAI : public QuantumBasicAI
    {
        boss_kalecgos_dragon_formAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            SathGUID = 0;
        }

        InstanceScript* instance;

        uint32 ArcaneBuffetTimer;
        uint32 FrostBreathTimer;
        uint32 WildMagicTimer;
        uint32 SpectralBlastTimer;
        uint32 TailLashTimer;
        uint32 CheckTimer;
        uint32 TalkTimer;
        uint32 TalkSequence;
        uint32 CloseTimer;

        bool IsFriendly;
        bool IsEnraged;
        bool IsBanished;
        bool DoorClosed;

        uint64 SathGUID;

        void Reset()
        {
			SathGUID = instance->GetData64(DATA_SATHROVARR_THE_CORRUPTOR);
			instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_1), true);
			instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_2), true);

			Unit* Sath = Unit::GetUnit(*me, SathGUID);
            if (Sath)
				((Creature*)Sath)->AI()->EnterEvadeMode();


            me->SetCurrentFaction(16);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);
            me->SetVisible(true);
            me->SetStandState(UNIT_STAND_STATE_SLEEP);

            ArcaneBuffetTimer = 8000;
            FrostBreathTimer = 15000;
            WildMagicTimer = 10000;
            TailLashTimer = 25000;
            SpectralBlastTimer = 20000+(rand()%5000);
            CheckTimer = SpectralBlastTimer+20000; //after spectral blast
            DoorClosed = false;
            CloseTimer = 10000;

            TalkTimer = 0;
            TalkSequence = 0;
            IsFriendly = false;
            IsEnraged = false;
            IsBanished = false;
        }

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (damage >= me->GetHealth() && doneby != me)
                damage = 0;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoSendQuantumText(SAY_EVIL_AGGRO, me);
            DoZoneInCombat();

			instance->SetData(DATA_KALECGOS_EVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_EVIL_SLAY_1, SAY_EVIL_SLAY_2), me);
        }

        void MovementInform(uint32 type,uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id != 1)
                return;

            me->SetVisible(false);
            if (IsFriendly)
            {
                me->setDeathState(JUST_DIED);

                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        Player* player = itr->getSource();
                        if (player)
                            me->GetMap()->ToInstanceMap()->PermBindAllPlayers(player);
                    }
                }
            }
            else
            {
                me->GetMotionMaster()->MoveTargetedHome();
                TalkTimer = 30000;
            }
        }

        void GoodEnding()
        {
            switch(TalkSequence)
            {
            case 1:
                me->SetCurrentFaction(35);
                TalkTimer = 1000;
                break;
            case 2:
                DoSendQuantumText(SAY_GOOD_PLRWIN, me);
                TalkTimer = 10000;
                break;
            case 3:
                me->AddUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, FLY_X, FLY_Y, FLY_Z);
                TalkTimer = 600000;
                break;
            default:
                break;
            }
        }

        void BadEnding()
        {
            switch(TalkSequence)
            {
            case 1:
                DoSendQuantumText(SAY_EVIL_ENRAGE, me);
                TalkTimer = 3000;
                break;
            case 2:
                me->AddUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, FLY_X, FLY_Y, FLY_Z);
                TalkTimer = 600000;
                break;
            case 3:
                EnterEvadeMode();
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 /*diff*/);
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kalecgos_dragon_formAI (creature);
    }
};

class boss_sathrovarr_the_corruptor : public CreatureScript
{
public:
    boss_sathrovarr_the_corruptor() : CreatureScript("boss_sathrovarr_the_corruptor") {}

    struct boss_sathrovarr_the_corruptorAI : public QuantumBasicAI
    {
        boss_sathrovarr_the_corruptorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            KalecgosHumanGUID = 0;
            KalecgosDragonGUID = 0;
        }

        InstanceScript* instance;

        uint32 CorruptionStrikeTimer;
        uint32 CurseOfAgonyTimer;
        uint32 ShadowBoltVolleyTimer;
        uint32 CheckTimer;

        uint64 KalecgosHumanGUID;
        uint64 KalecgosDragonGUID;

        bool IsEnraged;
        bool IsBanished;
		bool Summoned;

        void Reset()
        {
            me->SetFullHealth();
            me->SetActive(true);

			KalecgosDragonGUID = instance->GetData64(DATA_KALECGOS_DRAGON);

			Summoned = true;

            if (KalecgosHumanGUID)
            {
                if (Unit* kalec = Unit::GetUnit(*me, KalecgosHumanGUID))
                    kalec->setDeathState(JUST_DIED);

                KalecgosHumanGUID = 0;
            }

            ShadowBoltVolleyTimer = 3000;
            CurseOfAgonyTimer = 20000;
            CorruptionStrikeTimer = 13000;
            CheckTimer = 1000;
            IsEnraged = false;
            IsBanished = false;

			instance->SetData(DATA_KALECGOS_EVENT, NOT_STARTED);

            DoCast(me, SPELL_SPECTRAL_INVISIBILITY, true);
        }

		void SummonKalecgosHuman()
		{
			Creature* kalec = me->SummonCreature(NPC_KALECGOS_HUMAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000);
			KalecgosHumanGUID = kalec->GetGUID();
			kalec->CastSpell(kalec, SPELL_SPECTRAL_INVISIBILITY, true);
			me->CombatStart(kalec);
			me->AddThreat(kalec, 100.0f);
			Summoned = false;
		}

        void EnterToBattle(Unit* /*who*/)
        {
			SummonKalecgosHuman();

            DoSendQuantumText(SAY_SATH_AGGRO, me);
        }

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (damage >= me->GetHealth() && doneby != me)
                damage = 0;
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetGUID() == KalecgosHumanGUID)
            {
                TeleportAllPlayersBack();
                if (Unit* kalecgos = Unit::GetUnit(*me, KalecgosDragonGUID))
                {
                    CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->TalkTimer = 1;
                    CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->IsFriendly = false;
                }
                EnterEvadeMode();
                return;
            }
			DoSendQuantumText(RAND(SAY_SATH_SLAY_1, SAY_SATH_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_SATH_DEATH, me);
            me->SetPosition(me->GetPositionX(), me->GetPositionY(), DRAGON_REALM_Z, me->GetOrientation());
            me->RemoveAurasDueToSpell(SPELL_SPECTRAL_INVISIBILITY);
            TeleportAllPlayersBack();
            if (Unit* kalecgos = Unit::GetUnit(*me, KalecgosDragonGUID))
            {
                CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->TalkTimer = 1;
                CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->IsFriendly = true;
            }

			instance->SetData(DATA_KALECGOS_EVENT, DONE);
        }

        void TeleportAllPlayersBack()
        {
            Map* map = me->GetMap();
            if (!map->IsDungeon())
				return;
            Map::PlayerList const &PlayerList = map->GetPlayers();
            Map::PlayerList::const_iterator i;
            for(i = PlayerList.begin(); i != PlayerList.end(); ++i)
			{
                if (Player* player = i->getSource())
				{
					if (player->HasAuraEffect(SPELL_SPECTRAL_REALM, 0))
						player->RemoveAurasDueToSpell(SPELL_SPECTRAL_REALM);
				}
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CheckTimer <= diff)
            {
                if (((me->GetHealth()*100 / me->GetMaxHealth()) < 10) && !IsEnraged)
                {
                    Unit* kalecgos = Unit::GetUnit(*me, KalecgosDragonGUID);
                    if (kalecgos)
                    {
                        kalecgos->CastSpell(kalecgos, SPELL_ENRAGE, true);
                        CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->IsEnraged = true;
                    }
                    DoCast(me, SPELL_ENRAGE, true);
                    IsEnraged = true;
                }

                if (!IsBanished && (me->GetHealth()*100)/me->GetMaxHealth() < 1)
                {
                    if (Unit* kalecgos = Unit::GetUnit(*me, KalecgosDragonGUID))
                    {
                        if (CAST_AI(boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI,((Creature*)kalecgos)->AI())->IsBanished)
                        {
                            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            return;
                        }
                        else
                        {
                            DoCast(me, SPELL_BANISH);
                            IsBanished = true;
                        }
                    }
                    else
                    {
                        sLog->OutErrorConsole("Sathrovarr is unable to find Kalecgos");
                        EnterEvadeMode();
                        return;
                    }
                }
                CheckTimer = 1000;
            }
			else CheckTimer -= diff;

            if (ShadowBoltVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoSendQuantumText(SAY_SATH_SPELL_1, me);
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = 7000+(rand()%3000);
				}
            }
			else ShadowBoltVolleyTimer -= diff;

            if (CurseOfAgonyTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_AGONY);
					CurseOfAgonyTimer = 20000;
				}
            }
			else CurseOfAgonyTimer -= diff;

            if (CorruptionStrikeTimer <= diff)
            {
				DoSendQuantumText(SAY_SATH_SPELL_2, me);
				DoCastVictim(SPELL_CORRUPTION_STRIKE);
				CorruptionStrikeTimer = 13000;
            }
			else CorruptionStrikeTimer -= diff;

            DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_sathrovarr_the_corruptorAI (creature);
    }
};

    void boss_kalecgos_dragon_form::boss_kalecgos_dragon_formAI::UpdateAI(const uint32 diff)
    {
        if (TalkTimer)
        {
            if (!TalkSequence)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop();
                TalkSequence++;

				instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_1), true);
				instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_2), true);
            }

            if (TalkTimer <= diff)
            {
                if (IsFriendly)
                    GoodEnding();
                else
                    BadEnding();
                TalkSequence++;
            }
			else TalkTimer -= diff;
        }
        else
        {
            if (!UpdateVictim())
                return;

            if (!DoorClosed)
            {
                if (CloseTimer <= diff)
                {
					instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_1), false);
					instance->HandleGameObject(instance->GetData64(DATA_GO_FORECEFIELD_COLL_2), false);
					DoorClosed = true;
					CloseTimer = 10000;
                }
				else CloseTimer -= diff;
            }

            if (CheckTimer <= diff)
			{
                {
                    if (((me->GetHealth()*100 / me->GetMaxHealth()) < 10) && !IsEnraged)
                    {
                        Unit* Sath = Unit::GetUnit(*me, SathGUID);
                        if (Sath)
                        {
                            Sath->CastSpell(Sath, SPELL_ENRAGE, true);
                            CAST_AI(boss_sathrovarr_the_corruptor::boss_sathrovarr_the_corruptorAI,((Creature*)Sath)->AI())->IsEnraged = true;
                        }
                        DoCast(me, SPELL_ENRAGE, true);
                        IsEnraged = true;
                    }

                    if (!IsBanished && (me->GetHealth()*100)/me->GetMaxHealth() < 1)
                    {
                        if (Unit* Sath = Unit::GetUnit(*me, SathGUID))
                        {
                            if (CAST_AI(boss_sathrovarr_the_corruptor::boss_sathrovarr_the_corruptorAI,((Creature*)Sath)->AI())->IsBanished)
                            {
                                Sath->DealDamage(Sath, Sath->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                                return;
                            }
                            else
                            {
                                DoCast(me, SPELL_BANISH);
                                IsBanished = true;
                            }
                        }
                        else
                        {
                            sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Didn't find Shathrowar. Kalecgos event reseted.");
                            EnterEvadeMode();
                            return;
                        }
                    }
                    CheckTimer = 1000;
				}
            }
			else CheckTimer -= diff;

            if (ArcaneBuffetTimer <= diff)
            {
				DoCastAOE(SPELL_ARCANE_BUFFET);
				ArcaneBuffetTimer = 8000;
            }
			else ArcaneBuffetTimer -= diff;

            if (FrostBreathTimer <= diff)
            {
				DoCastAOE(SPELL_FROST_BREATH);
				FrostBreathTimer = 15000;
            }
			else FrostBreathTimer -= diff;

            if (TailLashTimer <= diff)
            {
				DoCastAOE(SPELL_TAIL_LASH);
				TailLashTimer = 15000;
            }
			else TailLashTimer -= diff;

            if (WildMagicTimer <= diff)
            {
				DoCastAOE(WildMagic[rand()%6]);
				WildMagicTimer = 20000;
            }
			else WildMagicTimer -= diff;

            if (SpectralBlastTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
				{
					if (target && target->IsAlive() && !(target->HasAuraEffect(SPELL_SPECTRAL_EXHAUSTION, 0)))
					{
						DoCast(target, SPELL_SPECTRAL_BLAST);
						SpectralBlastTimer = 20000+(rand()%5000);
					}
					else SpectralBlastTimer = 1000;
				}
				else SpectralBlastTimer -= diff;
			}

			DoMeleeAttackIfReady();
     }
};

class boss_kalecgos_human_form : public CreatureScript
{
public:
    boss_kalecgos_human_form() : CreatureScript("boss_kalecgos_human_form") { }

    struct boss_kalecgos_human_formAI : public QuantumBasicAI
    {
        InstanceScript* instance;

        uint32 RevitalizeTimer;
        uint32 HeroicStrikeTimer;
        uint32 YellTimer;
        uint32 YellSequence;

        uint64 SathGUID;

        bool IsEnraged;

        boss_kalecgos_human_formAI(Creature* creature) : QuantumBasicAI(creature)
		{
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
			SathGUID = instance->GetData64(DATA_SATHROVARR_THE_CORRUPTOR);

            RevitalizeTimer = 5000;
            HeroicStrikeTimer = 3000;
            YellTimer = 5000;
            YellSequence = 0;

            IsEnraged = false;
        }

        void EnterToBattle(Unit* who) {}

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (doneby->GetGUID() != SathGUID)
                damage = 0;
			else if (IsEnraged)
                damage *= 3;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (YellTimer <= diff)
            {
                switch(YellSequence)
                {
                case 0:
                    DoSendQuantumText(SAY_GOOD_AGGRO, me);
                    YellSequence++;
                    break;
                case 1:
                    if ((me->GetHealth()*100)/me->GetMaxHealth() < 50)
                    {
                        DoSendQuantumText(SAY_GOOD_NEAR_DEATH_1, me);
                        YellSequence++;
                    }
                    break;
                case 2:
                    if ((me->GetHealth()*100)/me->GetMaxHealth() < 10)
                    {
                        DoSendQuantumText(SAY_GOOD_NEAR_DEATH_2, me);
                        YellSequence++;
                    }
                    break;
                default:
                    break;
                }
                YellTimer = 5000;
            }

            if (RevitalizeTimer <= diff)
            {
                DoCast(me, SPELL_REVITALIZE);
                RevitalizeTimer = 5000;
            }
			else RevitalizeTimer -= diff;

            if (HeroicStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_HEROIC_STRIKE);
                HeroicStrikeTimer = 2000;
            }
			else HeroicStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kalecgos_human_formAI(creature);
    }
};

class go_spectral_rift : GameObjectScript
{
public:
    go_spectral_rift() : GameObjectScript("go_spectral_rift") {}

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->HasAuraEffect(SPELL_SPECTRAL_EXHAUSTION, 0))
            player->GetSession()->SendNotification(GO_FAILED);
        else
            player->CastSpell(player, SPELL_TELEPORT_SPECTRAL, true);
        return true;
    }
};

void AddSC_boss_kalecgos()
{
    new boss_kalecgos_dragon_form();
    new boss_sathrovarr_the_corruptor();
    new boss_kalecgos_human_form();
    new go_spectral_rift();
}