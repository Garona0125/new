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
#include "Player.h"
#include "SpellInfo.h"
#include "naxxramas.h"

enum Texts
{
    EMOTE_AIR_PHASE         = 0,
    EMOTE_GROUND_PHASE      = 1,
    EMOTE_BREATH            = 2,
    EMOTE_ENRAGE            = 3,
};

enum Spells
{
    SPELL_FROST_AURA        = 28531,
    SPELL_CLEAVE            = 19983,
    SPELL_TAIL_SWEEP        = 55697,
    SPELL_SUMMON_BLIZZARD   = 28560,
    SPELL_LIFE_DRAIN        = 28542,
    SPELL_ICEBOLT           = 28522,
    SPELL_FROST_BREATH      = 29318,
    SPELL_FROST_EXPLOSION   = 28524,
    SPELL_FROST_MISSILE     = 30101,
    SPELL_BERSERK           = 26662,
    SPELL_DIES              = 29357,
    SPELL_CHILL             = 28547,
	SPELL_CHECK_RESISTS     = 60539,
};

enum Misc
{
    MAX_FROST_RESISTANCE = 100,
};

enum Phases
{
    PHASE_NULL = 0,
    PHASE_BIRTH,
    PHASE_GROUND,
    PHASE_FLIGHT,
};

enum Events
{
    EVENT_NONE          = 0,
    EVENT_BERSERK       = 1,
    EVENT_CLEAVE        = 2,
    EVENT_TAIL          = 3,
    EVENT_DRAIN         = 4,
    EVENT_BLIZZARD      = 5,
    EVENT_FLIGHT        = 6,
    EVENT_LIFTOFF       = 7,
    EVENT_ICEBOLT       = 8,
    EVENT_BREATH        = 9,
    EVENT_EXPLOSION     = 10,
    EVENT_LAND          = 11,
    EVENT_GROUND        = 12,
    EVENT_BIRTH         = 13,
	EVENT_CHECK_RESISTS = 14,
};

typedef std::map<uint64, uint64> IceBlockMap;

class boss_sapphiron : public CreatureScript
{
    public:
        boss_sapphiron() : CreatureScript("boss_sapphiron") { }

        struct boss_sapphironAI : public BossAI
        {
            boss_sapphironAI(Creature* creature) : BossAI(creature, BOSS_SAPPHIRON), IceboltCount(0), phase(PHASE_NULL)
            {
                map = me->GetMap();
            }

			Map* map;
			Phases phase;
			IceBlockMap Iceblocks;

            bool CanTheHundredClub;

			uint32 IceboltCount;

            void InitializeAI()
            {
                float x, y, z;
                me->GetPosition(x, y, z);
                me->SummonGameObject(GO_SAPPHIRON_BIRTH, x, y, z, 0, 0, 0, 0, 0, 0);
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);

                BossAI::InitializeAI();
            }

            void Reset()
            {
                _Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                if (phase == PHASE_FLIGHT)
				{
                    ClearIceBlock();
					me->SetReactState(REACT_AGGRESSIVE);
					if (me->IsHovering())
					{
						me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
						me->SetHover(false);
					}
					me->SetDisableGravity(false);
				}

				me->SetFullHealth();

                phase = PHASE_NULL;

                CanTheHundredClub = true;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                me->CastSpell(me, SPELL_FROST_AURA, true);
				DoCast(me, SPELL_CHECK_RESISTS, true);

				events.ScheduleEvent(EVENT_CHECK_RESISTS, 30*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BERSERK, 15*MINUTE*IN_MILLISECONDS);
                EnterPhaseGround();
            }

			void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                switch(spell->Id)
                {
                    case SPELL_ICEBOLT:
                    {
                        IceBlockMap::iterator itr = Iceblocks.find(target->GetGUID());
                        if (itr != Iceblocks.end() && !itr->second)
                        {
                            if (GameObject* iceblock = me->SummonGameObject(GO_ICEBLOCK, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, 0, 0, 0, 0, 25))
                                itr->second = iceblock->GetGUID();
                        }
                        break;
                    }
                    case SPELL_CHECK_RESISTS:
                        if (target && target->GetResistance(SPELL_SCHOOL_FROST) > MAX_FROST_RESISTANCE)
                            CanTheHundredClub = false;
                        break;
                }
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();

				me->CastSpell(me, SPELL_DIES, true);

				if (CanTheHundredClub)
				{
					AchievementEntry const* AchievTheHundredClub = sAchievementStore.LookupEntry(RAID_MODE(ACHIEVEMENT_THE_HUNDRED_CLUB_10, ACHIEVEMENT_THE_HUNDRED_CLUB_25));
					if (AchievTheHundredClub)
					{
						if (map && map->IsDungeon())
						{
							Map::PlayerList const &players = map->GetPlayers();
							for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
								itr->getSource()->CompletedAchievement(AchievTheHundredClub);
						}
					}
				}
			}

            void MovementInform(uint32 /*type*/, uint32 id)
            {
                if (id == 1)
                    events.ScheduleEvent(EVENT_LIFTOFF, 0);
            }

            void DoAction(int32 const param)
            {
                if (param == DATA_SAPPHIRON_BIRTH)
                {
                    phase = PHASE_BIRTH;
                    events.ScheduleEvent(EVENT_BIRTH, 23*IN_MILLISECONDS);
                }
            }

            void EnterPhaseGround()
            {
                phase = PHASE_GROUND;
                me->SetReactState(REACT_AGGRESSIVE);
                events.SetPhase(PHASE_GROUND);
                events.ScheduleEvent(EVENT_CLEAVE, urand(5, 15)*IN_MILLISECONDS, 0, PHASE_GROUND);
                events.ScheduleEvent(EVENT_TAIL, urand(5, 15)*IN_MILLISECONDS, 0, PHASE_GROUND);
                events.ScheduleEvent(EVENT_DRAIN, 24*IN_MILLISECONDS, 0, PHASE_GROUND);
                events.ScheduleEvent(EVENT_BLIZZARD, urand(5, 10)*IN_MILLISECONDS, 0, PHASE_GROUND);
                events.ScheduleEvent(EVENT_FLIGHT, 45*IN_MILLISECONDS);
            }

            void ClearIceBlock()
            {
                for (IceBlockMap::const_iterator itr = Iceblocks.begin(); itr != Iceblocks.end(); ++itr)
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, itr->first))
                        player->RemoveAura(SPELL_ICEBOLT);

					if (GameObject* go = ObjectAccessor::GetGameObject(*me, itr->second))
                        go->Delete();
                }
                Iceblocks.clear();
            }

            uint32 GetData(uint32 data) const
            {
                if (data == DATA_THE_HUNDRED_CLUB)
                    return CanTheHundredClub;

                return 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!phase)
                    return;

                events.Update(diff);

                if ((phase != PHASE_BIRTH && !UpdateVictim()) || !CheckInRoom())
                    return;

                if (phase == PHASE_GROUND)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
							case EVENT_CHECK_RESISTS:
								DoCast(me, SPELL_CHECK_RESISTS);
								events.ScheduleEvent(EVENT_CHECK_RESISTS, 30*IN_MILLISECONDS);
								return;
                            case EVENT_BERSERK:
                                DoSendQuantumText(EMOTE_ENRAGE, me);
                                DoCast(me, SPELL_BERSERK);
                                return;
                            case EVENT_CLEAVE:
                                DoCastVictim(SPELL_CLEAVE);
                                events.ScheduleEvent(EVENT_CLEAVE, urand(5, 15)*IN_MILLISECONDS, 0, PHASE_GROUND);
                                return;
                            case EVENT_TAIL:
                                DoCastAOE(SPELL_TAIL_SWEEP);
                                events.ScheduleEvent(EVENT_TAIL, urand(5, 15)*IN_MILLISECONDS, 0, PHASE_GROUND);
                                return;
                            case EVENT_DRAIN:
                                DoCastAOE(SPELL_LIFE_DRAIN);
                                events.ScheduleEvent(EVENT_DRAIN, 24*IN_MILLISECONDS, 0, PHASE_GROUND);
                                return;
                            case EVENT_BLIZZARD:
                            {
                                if (Creature* summon = DoSummon(NPC_BLIZZARD, me, 0.0f, urand(25, 30)*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                                    summon->GetMotionMaster()->MoveRandom(40);
                                events.ScheduleEvent(EVENT_BLIZZARD, RAID_MODE(20, 7)*IN_MILLISECONDS, 0, PHASE_GROUND);
                                break;
                            }
                            case EVENT_FLIGHT:
                                if (HealthAbovePct(HEALTH_PERCENT_10))
                                {
                                    phase = PHASE_FLIGHT;
                                    events.SetPhase(PHASE_FLIGHT);
                                    me->SetReactState(REACT_PASSIVE);
                                    me->AttackStop();
                                    float x, y, z, o;
                                    me->GetHomePosition(x, y, z, o);
                                    me->GetMotionMaster()->MovePoint(1, x, y, z);
                                    return;
                                }
                                break;
                        }
                    }

                    DoMeleeAttackIfReady();
                }
                else
                {
                    if (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
							case EVENT_CHECK_RESISTS:
								DoCast(me, SPELL_CHECK_RESISTS);
								events.ScheduleEvent(EVENT_CHECK_RESISTS, 30*IN_MILLISECONDS);
								return;
                            case EVENT_LIFTOFF:
                                Talk(EMOTE_AIR_PHASE);
                                me->SetDisableGravity(true);
								me->SetHover(true);
                                me->SendMovementFlagUpdate();
                                events.ScheduleEvent(EVENT_ICEBOLT, 1500);
                                IceboltCount = RAID_MODE(2, 3);
                                return;
                            case EVENT_ICEBOLT:
                            {
                                std::vector<Unit*> targets;
                                std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                                for (; i != me->getThreatManager().getThreatList().end(); ++i)
                                    if ((*i)->getTarget()->GetTypeId() == TYPE_ID_PLAYER && !(*i)->getTarget()->HasAura(SPELL_ICEBOLT))
                                        targets.push_back((*i)->getTarget());

                                if (targets.empty())
                                    IceboltCount = 0;
                                else
                                {
                                    std::vector<Unit*>::const_iterator itr = targets.begin();
                                    advance(itr, rand()%targets.size());
                                    Iceblocks.insert(std::make_pair((*itr)->GetGUID(), 0));
                                    DoCast(*itr, SPELL_ICEBOLT);
                                    --IceboltCount;
                                }

                                if (IceboltCount)
                                    events.ScheduleEvent(EVENT_ICEBOLT, 1*IN_MILLISECONDS);
                                else
                                    events.ScheduleEvent(EVENT_BREATH, 1*IN_MILLISECONDS);
                                return;
                            }
                            case EVENT_BREATH:
                            {
                                Talk(EMOTE_BREATH);
                                DoCastAOE(SPELL_FROST_MISSILE);
                                events.ScheduleEvent(EVENT_EXPLOSION, 8*IN_MILLISECONDS);
                                return;
                            }
                            case EVENT_EXPLOSION:
                                CastExplosion();
                                ClearIceBlock();
                                events.ScheduleEvent(EVENT_LAND, 3*IN_MILLISECONDS);
                                return;
                            case EVENT_LAND:
                                me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                                Talk(EMOTE_GROUND_PHASE);
								me->SetHover(false);
                                me->SetDisableGravity(false);
                                me->SendMovementFlagUpdate();
                                events.ScheduleEvent(EVENT_GROUND, 1500);
                                return;
                            case EVENT_GROUND:
                                EnterPhaseGround();
                                return;
                            case EVENT_BIRTH:
                                me->SetVisible(true);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                me->SetReactState(REACT_AGGRESSIVE);
                                return;
                        }
                    }
                }
            }

            void CastExplosion()
            {
                DoZoneInCombat(); // make sure everyone is in threatlist
                std::vector<Unit*> targets;
                std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                for (; i != me->getThreatManager().getThreatList().end(); ++i)
                {
                    Unit* target = (*i)->getTarget();
                    if (target->GetTypeId() != TYPE_ID_PLAYER)
                        continue;

                    if (target->HasAura(SPELL_ICEBOLT))
                    {
                        target->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, true);
                        targets.push_back(target);
                        continue;
                    }

                    for (IceBlockMap::const_iterator itr = Iceblocks.begin(); itr != Iceblocks.end(); ++itr)
                    {
                        if (GameObject* go = GameObject::GetGameObject(*me, itr->second))
                        {
                            if (go->IsInBetween(me, target, 2.0f)
                                && me->GetExactDist2d(target->GetPositionX(), target->GetPositionY()) - me->GetExactDist2d(go->GetPositionX(), go->GetPositionY()) < 5.0f)
                            {
                                target->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, true);
                                targets.push_back(target);
                                break;
                            }
                        }
                    }
                }

                me->CastSpell(me, SPELL_FROST_EXPLOSION, true);

                for (std::vector<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    (*itr)->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_sapphironAI (creature);
        }
};

class achievement_the_hundred_club : public AchievementCriteriaScript
{
public:
	achievement_the_hundred_club() : AchievementCriteriaScript("achievement_the_hundred_club") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		return target && target->GetAI()->GetData(DATA_THE_HUNDRED_CLUB);
	}
};

void AddSC_boss_sapphiron()
{
    new boss_sapphiron();
    new achievement_the_hundred_club();
}