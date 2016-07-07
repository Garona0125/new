/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
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
#include "magtheridons_lair.h"

struct Yell
{
    int32 id;
};

static Yell RandomTaunt[] =
{
    {-1544000},
    {-1544001},
    {-1544002},
    {-1544003},
    {-1544004},
    {-1544005},
};

const Position Pos[5] =
{
    {-55.6891f,  2.17024f, 0.714280f, 0.05236f},
    {-31.9479f,  40.2201f, 0.714251f, 4.93928f},
    { 10.4777f,  23.9914f, 0.714224f, 3.66519f},
    { 10.3369f, -19.7942f, 0.714241f, 2.53073f},
    {-31.7645f, -35.8374f, 0.714268f, 1.37881f}
};

enum Texts
{
	SAY_FREED                = -1544006,
	SAY_AGGRO                = -1544007,
	SAY_BANISH               = -1544008,
	SAY_CHAMBER_DESTROY      = -1544009,
	SAY_PLAYER_KILLED        = -1544010,
	SAY_DEATH                = -1544011,
	EMOTE_BERSERK            = -1544012,
	EMOTE_BLASTNOVA          = -1544013,
	EMOTE_BEGIN              = -1544014,
};

#define EMOTE_BONDS_WEAKEN "Magtheridon's bonds begin to weaken!"

enum Spells
{
	SPELL_BLASTNOVA             = 30616,
	SPELL_CLEAVE                = 30619,
	SPELL_QUAKE_TRIGGER         = 30657,
	SPELL_QUAKE_KNOCKBACK       = 30571,
	SPELL_BLAZE_TARGET          = 30541,
	SPELL_BLAZE_TRAP            = 30542,
	SPELL_DEBRIS_KNOCKDOWN      = 36449,
	SPELL_DEBRIS_VISUAL         = 30632,
	SPELL_DEBRIS_DAMAGE         = 30631,
	SPELL_CAMERA_SHAKE          = 36455,
	SPELL_BERSERK               = 27680,
	SPELL_SHADOW_CAGE           = 30168,
	SPELL_SHADOW_GRASP          = 30410,
	SPELL_SHADOW_GRASP_VISUAL   = 30166,
	SPELL_MIND_EXHAUSTION       = 44032,  //Casted by the cubes when channeling ends
	SPELL_SHADOW_CAGE_C         = 30205,
	SPELL_SHADOW_GRASP_C        = 30207,
	SPELL_SHADOW_BOLT_VOLLEY    = 30510,
	SPELL_DARK_MENDING          = 30528,
	SPELL_FEAR                  = 30530, //39176
	SPELL_BURNING_ABYSSAL       = 30511,
	SPELL_SOUL_TRANSFER         = 30531,
	SPELL_FIRE_BLAST            = 37110,
};

enum Misc
{
	CLICKERS_COUNT = 5,
};

typedef std::map<uint64, uint64> CubeMap;

class npc_burning_abyssal : public CreatureScript
{
public:
    npc_burning_abyssal() : CreatureScript("npc_burning_abyssal") { }

    struct npc_burning_abyssalAI : public QuantumBasicAI
    {
        npc_burning_abyssalAI(Creature* creature) : QuantumBasicAI(creature)
        {
            trigger = 0;
            DespawnTimer = 60000;
        }

        uint32 FireBlastTimer;
        uint32 DespawnTimer;
        uint32 trigger;

        void Reset()
        {
            FireBlastTimer = 6000;
        }

            void SpellHit(Unit*, const SpellInfo* spell)
        {
            if (trigger == 2 && spell->Id == SPELL_BLAZE_TARGET)
            {
                DoCast(me, SPELL_BLAZE_TRAP, true);
                me->SetVisible(false);
                DespawnTimer = 130000;
            }
        }

        void SetTrigger(uint32 _trigger)
        {
            trigger = _trigger;
            me->SetDisplayId(MODEL_ID_INVISIBLE);

            if (trigger == 1) //debris
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                DoCast(me, SPELL_DEBRIS_VISUAL, true);
                FireBlastTimer = 5000;
                DespawnTimer = 10000;
            }
        }

        void EnterToBattle(Unit * /*who*/)
        {
            DoZoneInCombat();
        }

        void AttackStart(Unit* who)
        {
            if (!trigger)
                QuantumBasicAI::AttackStart(who);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!trigger)
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (trigger)
            {
                if (trigger == 1)
                {
                    if (FireBlastTimer <= diff)
                    {
                        DoCast(me, SPELL_DEBRIS_DAMAGE, true);
                        trigger = 3;
                    }
					else FireBlastTimer -= diff;
                }
                else if (trigger >= 2)
                {
                    if (DespawnTimer <= diff)
                    {
                        me->DealDamage(me, me->GetHealth());
                        me->RemoveCorpse();
                    }
					else DespawnTimer -= diff;
                }
                return;
            }

            if (!UpdateVictim())
                return;

            if (FireBlastTimer <= diff)
            {
                DoCastVictim(SPELL_FIRE_BLAST);
                FireBlastTimer = urand(5000, 15000);
            }
			else FireBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burning_abyssalAI(creature);
    }
};

class boss_magtheridon : public CreatureScript
{
public:
    boss_magtheridon() : CreatureScript("boss_magtheridon") { }

    struct boss_magtheridonAI : public QuantumBasicAI
    {
        boss_magtheridonAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_STUNNED);
            DoCast(me, SPELL_SHADOW_CAGE_C, true);
        }

        CubeMap Cube;
        SummonList Summons;
        InstanceScript* instance;

        uint32 BerserkTimer;
        uint32 QuakeTimer;
        uint32 CleaveTimer;
        uint32 BlastNovaTimer;
        uint32 BlazeTimer;
        uint32 DebrisTimer;
        uint32 RandChatTimer;
        uint32 ReleaseTimer;

        uint8 Phase;
        bool NeedCheckCube;

        void Reset()
        {
            if (instance)
            {
                if (Phase == 3)
                    instance->SetData(DATA_COLLAPSE, false);

                instance->SetData(DATA_MAGTHERIDON_EVENT, NOT_STARTED);
            }

            BerserkTimer = 1320000;
            QuakeTimer = 40000;
            DebrisTimer = 10000;
            BlazeTimer = 10000+rand()%20000;
            BlastNovaTimer = 60000;
            CleaveTimer = 15000;
            RandChatTimer = 90000;
            ReleaseTimer = 120000;

            Phase = 1;
            NeedCheckCube = false;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_UNK_6);

            for (uint8 i = 0; i < 5; i++)
                me->SummonCreature(NPC_CHANNELLER, Pos[i]);
        }

        void EnterEvadeMode()
        {
            me->ClearUnitState(UNIT_STATE_STUNNED);
            me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_C);
            Summons.DespawnAll();
            _EnterEvadeMode();
            me->GetMotionMaster()->MoveTargetedHome();
            Reset();
        }

        void JustReachedHome()
        {
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_STUNNED);
            DoCast(me, SPELL_SHADOW_CAGE_C, true);
        }

        void JustSummoned(Creature* summon)
        {
            Summons.Summon(summon);
        }

        void SetClicker(uint64 cubeGUID, uint64 clickerGUID)
        {
            // to avoid multiclicks from 1 cube
            if (uint64 guid = Cube[cubeGUID])
                DebuffClicker(Unit::GetUnit(*me, guid));
            Cube[cubeGUID] = clickerGUID;
            NeedCheckCube = true;
        }

        //function to interrupt channeling and debuff clicker with mind exh(used if second person clicks with same cube or after dispeling/ending shadow grasp DoT)
        void DebuffClicker(Unit* clicker)
        {
            if (!clicker || !clicker->IsAlive())
                return;

            clicker->RemoveAurasDueToSpell(SPELL_SHADOW_GRASP); // cannot interrupt triggered spells
            clicker->InterruptNonMeleeSpells(false);
            clicker->CastSpell(clicker, SPELL_MIND_EXHAUSTION, true);
        }

        void NeedCheckCubeStatus()
        {
            uint32 ClickerNum = 0;
            // now checking if every clicker has debuff from manticron(it is dispelable atm rev 6110 : S)
            // if not - apply mind exhaustion and delete from clicker's list
            for (CubeMap::iterator i = Cube.begin(); i != Cube.end(); ++i)
            {
                Unit* clicker = Unit::GetUnit(*me, i->second);
                if (!clicker || !clicker->FindCurrentSpellBySpellId(SPELL_SHADOW_GRASP_VISUAL))
                {
                    DebuffClicker(clicker);
                    i->second = 0;
                }
				else ++ClickerNum;
            }

            // if 5 clickers from other cubes apply shadow cage
            if (ClickerNum >= CLICKERS_COUNT && !me->HasAura(SPELL_SHADOW_CAGE))
            {
                DoSendQuantumText(SAY_BANISH, me);
                DoCast(me, SPELL_SHADOW_CAGE, true);
            }
            else if (ClickerNum < CLICKERS_COUNT && me->HasAura(SPELL_SHADOW_CAGE))
                me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE);

            if (!ClickerNum)
                NeedCheckCube = false;
        }

        void KilledUnit(Unit * /*victim*/)
        {
            DoSendQuantumText(SAY_PLAYER_KILLED, me);
        }

        void JustDied(Unit * /*killer*/)
        {
            Summons.DespawnAll();

            if (instance)
                instance->SetData(DATA_MAGTHERIDON_EVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);
        }

        void StartEvent()
        {
            if (instance)
            {
                if (instance->GetData(DATA_MAGTHERIDON_EVENT) != NOT_STARTED)
                    return;

                instance->SetData(DATA_MAGTHERIDON_EVENT, IN_PROGRESS);
            }

            me->MonsterTextEmote(EMOTE_BONDS_WEAKEN, 0);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetInCombatWithZone();
            Summons.DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase == 1)
            {
                if (RandChatTimer <= diff)
                {
                    DoSendQuantumText(RandomTaunt[rand()%6].id, me);
                    RandChatTimer = 90000;
                }
				else RandChatTimer -= diff;

                if (!me->IsInCombatActive())
                    return;

                if (ReleaseTimer <= diff)
                {
                    me->ClearUnitState(UNIT_STATE_STUNNED);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->RemoveAurasDueToSpell(SPELL_SHADOW_CAGE_C);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoResetThreat();
                    DoZoneInCombat();
                    if (Unit* target = me->GetVictim())
                        me->GetMotionMaster()->MoveChase(target);
                    DoSendQuantumText(SAY_FREED, me);
                    Phase = 2;
                }
				else ReleaseTimer -= diff;
				
				return;
			}

            if (!UpdateVictim())
                return;

            if (NeedCheckCube)
                NeedCheckCubeStatus();

            if (BerserkTimer <= diff)
            {
                DoCast(me, SPELL_BERSERK, true);
                DoSendQuantumText(EMOTE_BERSERK, me);
                BerserkTimer = 60000;
            }
			else BerserkTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 10000;
            }
			else CleaveTimer -= diff;

            if (BlastNovaTimer <= diff)
            {
                // to avoid earthquake interruption
                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                {
                    DoSendQuantumText(EMOTE_BLASTNOVA, me);
                    DoCast(me, SPELL_BLASTNOVA);
                    BlastNovaTimer = 60000;
                }
            }
			else BlastNovaTimer -= diff;

            if (QuakeTimer <= diff)
            {
                // to avoid blastnova interruption
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCast(me, SPELL_QUAKE_TRIGGER);
                    QuakeTimer = 50000;
                }
            }
			else QuakeTimer -= diff;

            if (BlazeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    float x, y, z;
                    target->GetPosition(x, y, z);
                    if (Creature* summon = me->SummonCreature(NPC_ABYSSAL, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
                        CAST_AI(npc_burning_abyssal::npc_burning_abyssalAI, summon->AI())->SetTrigger(2);
                        DoCast(me, SPELL_BLAZE_TARGET, true);
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
                BlazeTimer = 20000 + rand()%20000;
            }
			else BlazeTimer -= diff;

            if (Phase != 3 && HealthBelowPct(30) && !me->IsNonMeleeSpellCasted(false) && !me->HasUnitState(UNIT_STATE_STUNNED)) 
            {
                Phase = 3;
                DoSendQuantumText(SAY_CHAMBER_DESTROY, me);
                DoCast(me, SPELL_CAMERA_SHAKE, true);
                DoCast(me, SPELL_DEBRIS_KNOCKDOWN, true);

                if (instance)
                    instance->SetData(DATA_COLLAPSE, true);
            }

            if (Phase == 3)
            {
                if (DebrisTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        if (Creature* summon = me->SummonCreature(NPC_ABYSSAL, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            CAST_AI(npc_burning_abyssal::npc_burning_abyssalAI, summon->AI())->SetTrigger(1);
                    }
                    DebrisTimer = 10000;
                }
				else DebrisTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magtheridonAI(creature);
    }
};

class npc_hellfire_channeler : public CreatureScript
{
public:
    npc_hellfire_channeler() : CreatureScript("npc_hellfire_channeler") { }

    struct npc_hellfire_channelerAI : public QuantumBasicAI
    {
        npc_hellfire_channelerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ShadowBoltVolleyTimer;
        uint32 DarkMendingTimer;
        uint32 FearTimer;
        uint32 InfernalTimer;
        uint32 CheckTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 8000 + rand()%2000;
            DarkMendingTimer = 10000;
            FearTimer = 15000 + rand()%5000;
            InfernalTimer = 10000 + rand()%40000;
            CheckTimer = 5000;
            DoCast(me, SPELL_SHADOW_GRASP_C, false);
        }

        void MoveInLineOfSight(Unit* /*who*/) {}

        void EnterToBattle(Unit * /*who*/)
        {
            Creature* Magtheridon = Unit::GetCreature(*me, instance->GetData64(DATA_MAGTHERIDON));
            if (Magtheridon && Magtheridon->IsAlive())
                CAST_AI(boss_magtheridon::boss_magtheridonAI, Magtheridon->AI())->StartEvent();

            me->InterruptNonMeleeSpells(false);
            me->SetInCombatWithZone();

            if (Unit* target = SelectTarget(TARGET_NEAREST))
                AttackStart(target);
        }

        void EnterEvadeMode()
        {
            Creature* Magtheridon = Unit::GetCreature(*me, instance->GetData64(DATA_MAGTHERIDON));
            if (Magtheridon && Magtheridon->IsAlive())
                Magtheridon->AI()->EnterEvadeMode();

            _EnterEvadeMode();
            me->GetMotionMaster()->MoveTargetedHome();
            Reset();
        }

        void JustSummoned(Creature* summon)
        {
            summon->AI()->AttackStart(me->GetVictim());
        }

        void DamageTaken(Unit * /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth())
                DoCast(me, SPELL_SOUL_TRANSFER, true);
        }

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowBoltVolleyTimer <= diff)
            {
                DoCast(SPELL_SHADOW_BOLT_VOLLEY);
                ShadowBoltVolleyTimer = 10000 + rand()%10000;
            }
			else ShadowBoltVolleyTimer -= diff;

            if (DarkMendingTimer <= diff)
            {
				if (HealthBelowPct(50))
				{
					DoCast(me, SPELL_DARK_MENDING);
					DarkMendingTimer = 10000 +(rand() % 10000);
				}
            }
			else DarkMendingTimer -= diff;

            if (FearTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_FEAR);
					FearTimer = 25000 + rand()%15000;
				}
            }
			else FearTimer -= diff;

            if (InfernalTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BURNING_ABYSSAL, true);
					InfernalTimer = 30000 + rand()%10000;
				}
            }
			else InfernalTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hellfire_channelerAI(creature);
    }
};

class go_manticron_cube : public GameObjectScript
{
public:
    go_manticron_cube() : GameObjectScript("go_manticron_cube") { }

    bool OnGossipHello (Player* player, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
            return true;

        Creature* Magtheridon = Unit::GetCreature(*go, instance->GetData64(DATA_MAGTHERIDON));
        if (!Magtheridon || !Magtheridon->IsAlive() || !Magtheridon->IsInCombatActive())
            return true;

        // if exhausted or already channeling return
        if (player->HasAura(SPELL_MIND_EXHAUSTION) || player->FindCurrentSpellBySpellId(SPELL_SHADOW_GRASP_VISUAL))
            return true;

        player->InterruptNonMeleeSpells(false);
        player->AddAura(SPELL_SHADOW_GRASP, player);
        player->CastSpell(player, SPELL_SHADOW_GRASP_VISUAL, false);
        CAST_AI(boss_magtheridon::boss_magtheridonAI, Magtheridon->AI())->SetClicker(go->GetGUID(), player->GetGUID());
        return true;
    }
};

void AddSC_boss_magtheridon()
{
    new boss_magtheridon();
    new npc_hellfire_channeler();
    new go_manticron_cube();
    new npc_burning_abyssal();
}