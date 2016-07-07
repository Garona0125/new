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
#include "QuantumGossip.h"
#include "Group.h"
#include "stratholme.h"

class go_gauntlet_gate : public GameObjectScript
{
public:
    go_gauntlet_gate() : GameObjectScript("go_gauntlet_gate") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
            return false;

        if (instance->GetData(TYPE_BARON_RUN) != NOT_STARTED)
            return false;

        if (Group* group = player->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* pGroupie = itr->getSource();
                if (!pGroupie)
                    continue;
				if (pGroupie->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE &&
					!pGroupie->HasAura(SPELL_BARON_ULTIMATUM) &&
					pGroupie->GetMap() == go->GetMap())
					pGroupie->CastSpell(pGroupie, SPELL_BARON_ULTIMATUM, true);
            }
        }
		else if (player->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE &&
			!player->HasAura(SPELL_BARON_ULTIMATUM) &&
			player->GetMap() == go->GetMap())
			player->CastSpell(player, SPELL_BARON_ULTIMATUM, true);
		
		instance->SetData(TYPE_BARON_RUN, IN_PROGRESS);
		return false;
    }
};

enum FreedSoul
{
	SAY_ZAPPED = 0,
};

class mob_freed_soul : public CreatureScript
{
public:
    mob_freed_soul() : CreatureScript("mob_freed_soul") { }

    struct mob_freed_soulAI : public QuantumBasicAI
    {
        mob_freed_soulAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
			Talk(SAY_ZAPPED);
        }

        void EnterToBattle(Unit* /*who*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_freed_soulAI(creature);
    }
};

enum SoulMisc
{
	SPELL_EGAN_BLASTER   = 17368,
	SPELL_SOUL_FREED     = 17370,
	QUEST_RESTLESS_SOUL  = 5282,
	ENTRY_RESTLESS       = 11122,
	ENTRY_FREED          = 11136,
};

class mob_restless_soul : public CreatureScript
{
public:
    mob_restless_soul() : CreatureScript("mob_restless_soul") { }

    struct mob_restless_soulAI : public QuantumBasicAI
    {
        mob_restless_soulAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 DieTimer;

        uint64 Tagger;

        bool Tagged;

        void Reset()
        {
            Tagger = 0;
            DieTimer = 5000;
            Tagged = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (!Tagged && spell->Id == SPELL_EGAN_BLASTER && CAST_PLR(caster)->GetQuestStatus(QUEST_RESTLESS_SOUL) == QUEST_STATUS_INCOMPLETE)
                {
                    Tagged = true;
                    Tagger = caster->GetGUID();
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->CastSpell(summoned, SPELL_SOUL_FREED, false);
        }

        void JustDied(Unit* /*Killer*/)
        {
            if (Tagged)
                me->SummonCreature(ENTRY_FREED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Tagged)
            {
                if (DieTimer <= diff)
                {
                    if (Unit* temp = Unit::GetUnit(*me, Tagger))
                    {
                        CAST_PLR(temp)->KilledMonsterCredit(ENTRY_RESTLESS, me->GetGUID());
                        me->Kill(me);
                    }
                }
				else DieTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_restless_soulAI(creature);
    }
};

enum GhostlyCitizenSpells
{
    SPELL_HAUNTING_PHANTOM  = 16336,
    SPELL_SLAP              = 6754,
};

class mobs_spectral_ghostly_citizen : public CreatureScript
{
public:
    mobs_spectral_ghostly_citizen() : CreatureScript("mobs_spectral_ghostly_citizen") { }

    struct mobs_spectral_ghostly_citizenAI : public QuantumBasicAI
    {
        mobs_spectral_ghostly_citizenAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DieTimer;
        bool Tagged;

        void Reset()
        {
            DieTimer = 5000;
            Tagged = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (!Tagged && spell->Id == SPELL_EGAN_BLASTER)
                Tagged = true;
        }

        void JustDied(Unit* /*Killer*/)
        {
            if (Tagged)
            {
                for (uint32 i = 1; i <= 4; ++i)
                {
                     //100%, 50%, 33%, 25% chance to spawn
                     if (urand(1, i) == 1)
                         DoSummon(ENTRY_RESTLESS, me, 20.0f, 600000);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Tagged)
            {
                if (DieTimer <= diff)
                    me->Kill(me);
                else DieTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            switch (emote)
            {
                case TEXT_EMOTE_DANCE:
                    EnterEvadeMode();
                    break;
                case TEXT_EMOTE_RUDE:
                    if (me->IsWithinDistInMap(player, 5))
                        DoCast(player, SPELL_SLAP, false);
                    else
                        me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                    break;
                case TEXT_EMOTE_WAVE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    break;
                case TEXT_EMOTE_BOW:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    break;
                case TEXT_EMOTE_KISS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
                    break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mobs_spectral_ghostly_citizenAI(creature);
    }
};

void AddSC_stratholme()
{
    new go_gauntlet_gate();
    new mob_freed_soul();
    new mob_restless_soul();
    new mobs_spectral_ghostly_citizen();
}