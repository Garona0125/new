 /*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "LFGMgr.h"
#include "shadowfang_keep.h"

enum Texts
{
    SAY_AGGRO_1                      = -1033020,
    SAY_AGGRO_2                      = -1033021,
    SAY_AGGRO_3                      = -1033022,
    SAY_CALL_BAXTER                  = -1033023,
    SAY_CALL_FRYE                    = -1033024,
    SAY_DEATH                        = -1033025,
};

enum Spells
{
    SPELL_ALLURING_PERFUME           = 68589,
    SPELL_ALLURING_PERFUME_SPRAY     = 68607,
    SPELL_IRRESISTIBLE_COLOGNE       = 68946,
    SPELL_IRRESISTIBLE_COLOGNE_SPRAY = 68948,
    SPELL_TABLE_APPEAR               = 69216,
    SPELL_SUMMON_TABLE               = 69218,
    SPELL_CHAIN_REACTION             = 68821,
    SPELL_UNSTABLE_REACTION          = 68957,
    SPELL_THROW_PERFUME              = 68799,
    SPELL_THROW_COLOGNE              = 68841,
    SPELL_ALLURING_PERFUME_SPILL     = 68798,
    SPELL_IRRESISTIBLE_COLOGNE_SPILL = 68614,
};

enum Action
{
    START_INTRO     = 0,
    START_FIGHT     = 1,
    APOTHECARY_DIED = 2,
    SPAWN_CRAZED    = 3,
};

enum Phase
{
    PHASE_NORMAL = 0,
    PHASE_INTRO  = 1,
};

static Position Loc[] =
{
    // spawn points
    {-215.776443f, 2242.365479f, 79.769257f, 0.0f},
    {-169.500702f, 2219.286377f, 80.613045f, 0.0f},
    {-200.056641f, 2152.635010f, 79.763107f, 0.0f},
    {-238.448242f, 2165.165283f, 89.582985f, 0.0f},
    // moveto points
    {-210.784164f, 2219.004150f, 79.761803f, 0.0f},
    {-198.453400f, 2208.410889f, 79.762474f, 0.0f},
    {-208.469910f, 2167.971924f, 79.764969f, 0.0f},
    {-228.251511f, 2187.282471f, 79.762840f, 0.0f}
};

#define GOSSIP_ITEM_START "Begin the battle."

void SetInCombat(Creature* self)
{
    self->AI()->DoZoneInCombat(self, 150.0f);

    if (!self->IsInCombatActive())
        self->AI()->EnterEvadeMode();
}

class npc_apothecary_hummel : public CreatureScript
{
    public:
        npc_apothecary_hummel() : CreatureScript("npc_apothecary_hummel") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(START_INTRO);

            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        struct npc_apothecary_hummelAI : public QuantumBasicAI
        {
            npc_apothecary_hummelAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            SummonList summons;
            uint8 _deadCount;
            uint8 phase;
            uint8 step;
            uint32 AggroTimer;
            uint32 StepTimer;
            uint32 SprayTimer;
            uint32 ChainReactionTimer;
            bool FirstCrazed;

            void Reset()
            {
                me->RestoreFaction();
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                phase = PHASE_NORMAL;
                step = 0;
                _deadCount = 0;
                StepTimer = 1500;
                AggroTimer = 5000;
                SprayTimer = urand(4000, 7000);
                ChainReactionTimer = urand(10000, 25000);
                FirstCrazed = false;

                me->SetCorpseDelay(900); // delay despawn while still fighting baxter or frye
                summons.DespawnAll();

                if (!instance)
                    return;

                instance->SetData(TYPE_CROWN, NOT_STARTED);

                if (Creature* baxter = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_BAXTER)))
                {
                    if (baxter->IsAlive())
                        baxter->AI()->EnterEvadeMode();
                    else
                        baxter->Respawn();
                }

                if (Creature* frye = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_FRYE)))
                {
                    if (frye->IsAlive())
                        frye->AI()->EnterEvadeMode();
                    else
                        frye->Respawn();
                }

                if (GameObject* door = instance->instance->GetGameObject(instance->GetData64(DATA_DOOR)))
                    instance->HandleGameObject(NULL, true, door);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case START_INTRO:
                    {
                        if (Creature* baxter = ObjectAccessor::GetCreature(*me, instance? instance->GetData64(DATA_BAXTER) : 0))
                            baxter->AI()->DoAction(START_INTRO);
                        if (Creature* frye = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_FRYE) : 0))
                            frye->AI()->DoAction(START_INTRO);

                        phase = PHASE_INTRO;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        SetInCombat(me);
                        break;
                    }
                    case START_FIGHT:
                    {
                        phase = PHASE_NORMAL;
                        me->SetCurrentFaction(16);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        SetInCombat(me);
                        instance->SetData(TYPE_CROWN, IN_PROGRESS);
                        break;
                    }
                    case APOTHECARY_DIED:
                    {
                        ++_deadCount;
                        if (_deadCount > 2) // all 3 apothecarys dead, set lootable
                        {
                            summons.DespawnAll();
                            me->SetCorpseDelay(90); // set delay
                            me->setDeathState(JUST_DIED); // update delay
                            instance->SetData(TYPE_CROWN, DONE);
                            me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                            Map* map = me->GetMap();
                            if (map && map->IsDungeon())
                            {
                                Map::PlayerList const& players = map->GetPlayers();
                                if (!players.isEmpty())
								{
                                    for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
									{
                                        if (Player* player = i->getSource())
										{
                                            if (player->GetDistance(me) < 120.0f)
                                                sLFGMgr->RewardDungeonDoneFor(288, player);
										}
									}
								}
                            }
                        }
                        else
                        {
                            if (me->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
                                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        }
                        break;
                    }
                    case SPAWN_CRAZED:
                    {
                        uint8 i = urand(0, 3);
                        if (Creature* crazed = me->SummonCreature(NPC_CRAZED_APOTHECARY, Loc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS))
                        {
                            crazed->SetCurrentFaction(16);
                            crazed->SetReactState(REACT_PASSIVE);
                            crazed->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            crazed->GetMotionMaster()->MovePoint(1, Loc[i + 4]);
                        }

                        if (!FirstCrazed)
                            FirstCrazed = true;

                        break;
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (phase == PHASE_INTRO)
                {
                    if (StepTimer <= diff)
                    {
                        ++step;
                        switch (step)
                        {
                            case 1:
                            {
                                DoSendQuantumText(SAY_AGGRO_1, me);
                                StepTimer = 4000;
                                break;
                            }
                            case 2:
                            {
                                DoSendQuantumText(SAY_AGGRO_2, me);
                                StepTimer = 5500;
                                break;
                            }
                            case 3:
                            {
                                DoSendQuantumText(SAY_AGGRO_3, me);
                                StepTimer = 1000;
                                break;
                            }
                            case 4:
                            {
                                DoAction(START_FIGHT);
                                break;
                            }
                        }
                    }
                    else StepTimer -= diff;
                }
                else // PHASE_NORMAL
                {
                    if (!UpdateVictim())
                        return;

                    if (AggroTimer <= diff)
                    {
                        SetInCombat(me);
                        AggroTimer = 5000;
                    }
                    else AggroTimer -= diff;

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    if (ChainReactionTimer <= diff)
                    {
                        DoCast(me, SPELL_TABLE_APPEAR, true);
                        DoCast(me, SPELL_SUMMON_TABLE, true);
                        DoCast(SPELL_CHAIN_REACTION);
                        ChainReactionTimer = urand(15000, 25000);
                    }
                    else ChainReactionTimer -= diff;

                    if (SprayTimer <= diff)
                    {
                        DoCastVictim(SPELL_ALLURING_PERFUME_SPRAY);
                        SprayTimer = urand(8000, 15000);
                    }
                    else
                        SprayTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoAction(APOTHECARY_DIED);
                DoSendQuantumText(SAY_DEATH, me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_apothecary_hummelAI(creature);
        }
};

class npc_apothecary_baxter : public CreatureScript
{
    public:
        npc_apothecary_baxter() : CreatureScript("npc_apothecary_baxter") { }

        struct npc_apothecary_baxterAI : public QuantumBasicAI
        {
            npc_apothecary_baxterAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 ChainReactionTimer;
            uint32 SprayTimer;
            uint32 AggroTimer;
            uint32 WaitTimer;
            uint8 phase;

            void Reset()
            {
                me->RestoreFaction();
                AggroTimer = 5000;
                WaitTimer = 20000;
                SprayTimer = urand(4000, 7000);
                ChainReactionTimer = urand (10000, 25000);
                phase = PHASE_NORMAL;

                if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                {
                    if (hummel->IsAlive())
                        hummel->AI()->EnterEvadeMode();
                    else
                        hummel->Respawn();
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case START_INTRO:
                    {
                        phase = PHASE_INTRO;
                        break;
                    }
                    case START_FIGHT:
                    {
                        if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                            DoSendQuantumText(SAY_CALL_BAXTER, hummel);

                        phase = PHASE_NORMAL;
                        me->SetCurrentFaction(16);
                        SetInCombat(me);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (phase == PHASE_INTRO)
                {
                    if (WaitTimer <= diff)
                    {
                        DoAction(START_FIGHT);
                    }
                    else WaitTimer -= diff;
                }
                else // PHASE_NORMAL
                {
                    if (!UpdateVictim())
                        return;

                    if (AggroTimer <= diff)
                    {
                        SetInCombat(me);
                        AggroTimer = 5000;
                    }
                    else AggroTimer -= diff;

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    if (ChainReactionTimer <= diff)
                    {
                        DoCast(me, SPELL_TABLE_APPEAR, true);
                        DoCast(me, SPELL_SUMMON_TABLE, true);
                        DoCast(SPELL_CHAIN_REACTION);
                        ChainReactionTimer = urand(15000, 25000);
                    }
                    else ChainReactionTimer -= diff;

                    if (SprayTimer <= diff)
                    {
                        DoCastVictim(SPELL_IRRESISTIBLE_COLOGNE_SPRAY);
                        SprayTimer = urand(8000, 15000);
                    }
                    else SprayTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                    hummel->AI()->DoAction(APOTHECARY_DIED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_apothecary_baxterAI(creature);
        }
};

class npc_apothecary_frye : public CreatureScript
{
    public:
        npc_apothecary_frye() : CreatureScript("npc_apothecary_frye") { }

        struct npc_apothecary_fryeAI : public QuantumBasicAI
        {
            npc_apothecary_fryeAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 TargetSwitchTimer;
            uint32 ThrowTimer;
            uint32 AggroTimer;
            uint32 WaitTimer;
            uint8 phase;

            void Reset()
            {
                me->RestoreFaction();
                AggroTimer = 5000;
                WaitTimer = 28000;
                ThrowTimer = urand(2000, 4000);
                TargetSwitchTimer = urand(1000, 2000);
                phase = PHASE_NORMAL;

                if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                {
                    if (hummel->IsAlive())
                        hummel->AI()->EnterEvadeMode();
                    else
                        hummel->Respawn();
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case START_INTRO:
                    {
                        phase = PHASE_INTRO;
                        break;
                    }
                    case START_FIGHT:
                    {
                        if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                            DoSendQuantumText(SAY_CALL_FRYE, hummel);

                        phase = PHASE_NORMAL;
                        me->SetCurrentFaction(16);
                        SetInCombat(me);
                        break;
                    }
                }
            }

            void SummonBunny(Unit* target, bool perfume)
            {
                if (!target)
                    return;

                if (Creature* bunny = me->SummonCreature(NPC_VIAL_BUNNY, *target, TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS))
                {
                    bunny->SetCurrentFaction(16);
                    bunny->SetReactState(REACT_PASSIVE);
                    bunny->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    bunny->CastSpell(bunny, perfume ? SPELL_ALLURING_PERFUME_SPILL : SPELL_IRRESISTIBLE_COLOGNE_SPILL, true, NULL, NULL, me->GetGUID());
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_THROW_PERFUME:
                        SummonBunny(target, true);
                        break;
                    case SPELL_THROW_COLOGNE:
                        SummonBunny(target, false);
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (phase == PHASE_INTRO)
                {
                    if (WaitTimer <= diff)
                    {
                        DoAction(START_FIGHT);
                    }
                    else WaitTimer -= diff;
                }
                else // PHASE_NORMAL
                {
                    if (!UpdateVictim())
                        return;

                    if (ThrowTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, urand(0, 1) ? SPELL_THROW_PERFUME : SPELL_THROW_COLOGNE);
							ThrowTimer = urand(5000, 7500);
						}
                    }
                    else ThrowTimer -= diff;

                    if (TargetSwitchTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                        {
                            me->getThreatManager().modifyThreatPercent(me->GetVictim(), -100);
                            me->AddThreat(target, 9999999.9f);
                        }
                        TargetSwitchTimer = urand(5000, 10000);
                    }
                    else TargetSwitchTimer -= diff;

                    if (AggroTimer <= diff)
                    {
                        SetInCombat(me);
                        AggroTimer = 5000;
                    }
                    else AggroTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* hummel = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_HUMMEL) : 0))
                    hummel->AI()->DoAction(APOTHECARY_DIED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_apothecary_fryeAI(creature);
        }
};

class npc_crazed_apothecary : public CreatureScript
{
    public:
        npc_crazed_apothecary() : CreatureScript("npc_crazed_apothecary") {}

        struct npc_crazed_apothecaryAI : public QuantumBasicAI
        {
            npc_crazed_apothecaryAI(Creature* creature) : QuantumBasicAI(creature) { }

			uint32 ExplodeTimer;

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                DoZoneInCombat(me, 150.0f);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
                {
                    ExplodeTimer = urand (2500, 5000);
                    me->GetMotionMaster()->MoveFollow(target, 0.0f, float(2*M_PI*rand_norm()));
                }
                else me->DespawnAfterAction();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (ExplodeTimer <= diff)
                {
                    DoCast(me, SPELL_UNSTABLE_REACTION);
                }
                else ExplodeTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_crazed_apothecaryAI(creature);
        }
};

void AddSC_boss_apothecary_trio()
{
    new npc_apothecary_hummel();
    new npc_apothecary_baxter();
    new npc_apothecary_frye();
    new npc_crazed_apothecary();
}