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
#include "ahnkahet.h"

enum Yells
{
    SAY_AGGRO       = -1619030,
    SAY_SLAY_1      = -1619031,
    SAY_SLAY_2      = -1619032,
    SAY_SLAY_3      = -1619033,
    SAY_DEATH_1     = -1619034,
    SAY_DEATH_2     = -1619035,
    SAY_INSANITY    = -1619036,
};

enum Spells
{
    SPELL_INSANITY                     = 57496,
    SPELL_INSANITY_VISUAL              = 57561,
    SPELL_INSANITY_TARGET              = 57508,
    SPELL_MIND_FLAY_5N                 = 57941,
	SPELL_MIND_FLAY_5H                 = 59974,
    SPELL_SHADOW_BOLT_VOLLEY_5N        = 57942,
	SPELL_SHADOW_BOLT_VOLLEY_5H        = 59975,
    SPELL_SHIVER_5N                    = 57949,
	SPELL_SHIVER_5H                    = 59978,
    SPELL_CLONE_PLAYER                 = 57507,
	SPELL_COPY_WEAPON                  = 45785,
    SPELL_INSANITY_PHASING_1           = 57508,
    SPELL_INSANITY_PHASING_2           = 57509,
    SPELL_INSANITY_PHASING_3           = 57510,
    SPELL_INSANITY_PHASING_4           = 57511,
    SPELL_INSANITY_PHASING_5           = 57512,
	SPELL_TWISTED_VISAGE_SPAWN         = 57551,
	SPELL_TWISTED_VISAGE_DEATH         = 57555,
};

class boss_volazj : public CreatureScript
{
public:
    boss_volazj() : CreatureScript("boss_volazj") {}

    struct boss_volazjAI : public QuantumBasicAI
    {
        boss_volazjAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 MindFlayTimer;
        uint32 ShadowBoltVolleyTimer;
        uint32 ShiverTimer;
        uint32 InsanityHandled;
        SummonList Summons;

        uint32 GetHealthPct(uint32 damage)
        {
            if (damage > me->GetHealth())
                return 0;
            return 100*(me->GetHealth()-damage)/me->GetMaxHealth();
        }

		void Reset()
        {
            MindFlayTimer = 8*IN_MILLISECONDS;
            ShadowBoltVolleyTimer = 5*IN_MILLISECONDS;
            ShiverTimer = 15*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_HERALD_VOLAZJ, NOT_STARTED);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);

            me->SetPhaseMask((1|16|32|64|128|256), true);
            InsanityHandled = 0;

            ResetPlayersPhaseMask();

            Summons.DespawnAll();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetControlled(false, UNIT_STATE_STUNNED);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_HERALD_VOLAZJ, IN_PROGRESS);
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_QUICK_DEMISE_START_EVENT);
        }

		void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
			{
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);

                who->RemoveAurasDueToSpell(GetSpellForPhaseMask(who->GetPhaseMask()));
			}
        }

		void EnterEvadeMode()
        {
            me->RemoveAllAuras();
            me->SetControlled(false, UNIT_STATE_STUNNED);
            QuantumBasicAI::EnterEvadeMode();
        }

        void DamageTaken(Unit * /*pAttacker*/, uint32 &damage)
        {
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                damage = 0;

            if ((GetHealthPct(0) >= 66 && GetHealthPct(damage) < 66)|| (GetHealthPct(0) >= 33 && GetHealthPct(damage) < 33))
            {
                me->InterruptNonMeleeSpells(false);
				DoSendQuantumText(SAY_INSANITY, me);
                DoCast(me, SPELL_INSANITY, false);
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_INSANITY)
            {
                if (target->GetTypeId() != TYPE_ID_PLAYER || InsanityHandled > 4)
                    return;

                if (!InsanityHandled)
                {
                    DoCast(me, SPELL_INSANITY_VISUAL, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetControlled(true, UNIT_STATE_STUNNED);
                }

                target->CastSpell(target, SPELL_INSANITY_TARGET + InsanityHandled, true);

                Map::PlayerList const &players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                {
                    Player* player = i->getSource();
                    if (!player || !player->IsAlive())
                        continue;

                    if (Creature* clone = me->SummonCreature(NPC_TWISTED_VISAGE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                    {
                        player->CastSpell(clone, SPELL_CLONE_PLAYER, true);
						player->CastSpell(player, SPELL_COPY_WEAPON, true);
                        clone->SetPhaseMask((1<<(4+InsanityHandled)), true);
						clone->CastSpell(clone, SPELL_TWISTED_VISAGE_SPAWN);
						clone->AI()->DoZoneInCombat();
                    }
                }
                ++InsanityHandled;
            }
        }

		void JustDied(Unit* /*killer*/)
        {
			DoSendQuantumText(RAND(SAY_DEATH_1, SAY_DEATH_2), me);

			instance->SetData(DATA_HERALD_VOLAZJ, DONE);

            Summons.DespawnAll();
            ResetPlayersPhaseMask();
        }

        void ResetPlayersPhaseMask()
        {
            Map::PlayerList const &players = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                Player* player = i->getSource();
                player->RemoveAurasDueToSpell(GetSpellForPhaseMask(player->GetPhaseMask()));
            }
        }

        void JustSummoned(Creature* summon)
        {
            Summons.Summon(summon);
        }

        uint32 GetSpellForPhaseMask(uint32 phase)
        {
            uint32 spell = 0;
            switch (phase)
            {
                case 16:
                    spell = SPELL_INSANITY_PHASING_1;
                    break;
                case 32:
                    spell = SPELL_INSANITY_PHASING_2;
                    break;
                case 64:
                    spell = SPELL_INSANITY_PHASING_3;
                    break;
                case 128:
                    spell = SPELL_INSANITY_PHASING_4;
                    break;
                case 256:
                    spell = SPELL_INSANITY_PHASING_5;
                    break;
            }
            return spell;
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            uint32 phase= summon->GetPhaseMask();
            uint32 nextPhase = 0;
            Summons.Despawn(summon);

            // Check if all summons in this phase killed
            for (SummonList::const_iterator iter = Summons.begin(); iter != Summons.end(); ++iter)
            {
                if (Creature* visage = Unit::GetCreature(*me, *iter))
                {
                    // Not all are dead
                    if (phase == visage->GetPhaseMask())
                        return;
                    else
                        nextPhase = visage->GetPhaseMask();
                }
            }

            uint32 spell = GetSpellForPhaseMask(phase);
            uint32 spell2 = GetSpellForPhaseMask(nextPhase);
            Map* map = me->GetMap();
            if (!map)
                return;

            Map::PlayerList const &PlayerList = map->GetPlayers();
            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        if (player->HasAura(spell))
                        {
                            player->RemoveAurasDueToSpell(spell);
                            if (spell2) // if there is still some different mask cast spell for it
                                player->CastSpell(player, spell2, true);
                        }
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (InsanityHandled)
            {
                if (!Summons.empty())
                    return;

                InsanityHandled = 0;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetControlled(false, UNIT_STATE_STUNNED);
                me->RemoveAurasDueToSpell(SPELL_INSANITY_VISUAL);
            }

            if (MindFlayTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_FLAY_5N, SPELL_MIND_FLAY_5H));
					MindFlayTimer = 20*IN_MILLISECONDS;
				}
            }
			else MindFlayTimer -= diff;

            if (ShadowBoltVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_VOLLEY_5N, SPELL_SHADOW_BOLT_VOLLEY_5H));
					ShadowBoltVolleyTimer = 5*IN_MILLISECONDS;
				}
            }
			else ShadowBoltVolleyTimer -= diff;

            if (ShiverTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SHIVER_5N, SPELL_SHIVER_5H));
					ShiverTimer = 15*IN_MILLISECONDS;
				}
            }
			else ShiverTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_volazjAI(creature);
    }
};

class npc_twisted_visage : public CreatureScript
{
public:
    npc_twisted_visage() : CreatureScript("npc_twisted_visage") {}

    struct npc_twisted_visageAI : public QuantumBasicAI
    {
        npc_twisted_visageAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			//me->SetPhaseMask(1, true);
			DoCast(me, SPELL_TWISTED_VISAGE_DEATH, true);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twisted_visageAI(creature);
    }
};

void AddSC_boss_volazj()
{
    new boss_volazj();
	new npc_twisted_visage();
}