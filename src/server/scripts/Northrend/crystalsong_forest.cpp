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

enum Warmage
{
	SPELL_TRANSITUS_SHIELD_BEAM = 48310,
    NPC_TRANSITUS_SHIELD_DUMMY  = 27306,
    NPC_WARMAGE_SARINA          = 32369,
    NPC_WARMAGE_HALISTER        = 32371,
    NPC_WARMAGE_ILSUDRIA        = 32372,
};

class npc_warmage_violetstand : public CreatureScript
{
public:
    npc_warmage_violetstand() : CreatureScript("npc_warmage_violetstand") { }

    struct npc_warmage_violetstandAI : public QuantumBasicAI
    {
        npc_warmage_violetstandAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint64 TargetGUID;

        void Reset()
        {
            TargetGUID = 0;
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (me->IsNonMeleeSpellCasted(false))
                return;

            if (me->GetEntry() == NPC_WARMAGE_SARINA)
            {
                if (!TargetGUID)
                {
                    std::list<Creature*> orbList;
                    GetCreatureListWithEntryInGrid(orbList, me, NPC_TRANSITUS_SHIELD_DUMMY, 32.0f);
                    if (!orbList.empty())
                    {
                        for (std::list<Creature*>::const_iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
                        {
                            if (Creature* orb = *itr)
                            {
                                if (orb->GetPositionY() < 1000)
                                {
                                    TargetGUID = orb->GetGUID();
                                    break;
                                }
                            }
                        }
                    }
                }
            }
			else
            {
				if (!TargetGUID)
				{
					if (Creature* orb = GetClosestCreatureWithEntry(me, NPC_TRANSITUS_SHIELD_DUMMY, 32.0f))
						TargetGUID = orb->GetGUID();
				}
            }

            if (Creature* orb = me->GetCreature(*me, TargetGUID))
				DoCast(orb, SPELL_TRANSITUS_SHIELD_BEAM);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warmage_violetstandAI(creature);
    }
};

enum Maloric
{
    SPELL_INCAPACITATE_MALORIC              = 63124,
	SPELL_SEARCH_MALORIC                    = 63125,
	SPELL_SEARCH_MALORIC_CREDIT             = 63126,

    QUEST_THERES_SOMETHING_ABOUT_THE_SQUIRE = 13654,
};
 
class npc_maloric : public CreatureScript
{
public:
    npc_maloric() : CreatureScript("npc_maloric") {}
 
    struct npc_maloricAI : public QuantumBasicAI
    {
        npc_maloricAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ResetTimer;
       
        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->RemoveAurasDueToSpell(SPELL_INCAPACITATE_MALORIC);
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);
		}
 
        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			if (spell->Id == SPELL_INCAPACITATE_MALORIC)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_THERES_SOMETHING_ABOUT_THE_SQUIRE) == QUEST_STATUS_COMPLETE
						|| player->GetQuestStatus(QUEST_THERES_SOMETHING_ABOUT_THE_SQUIRE) == QUEST_STATUS_REWARDED)
						return;

					if (player->GetQuestStatus(QUEST_THERES_SOMETHING_ABOUT_THE_SQUIRE) == QUEST_STATUS_INCOMPLETE)
					{
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);

						ResetTimer = 30*IN_MILLISECONDS;
					}
                }        
            }          
        }

		void UpdateAI(uint32 const diff)
        {
			if (ResetTimer <= diff && !me->IsInCombatActive())
			{
				Reset();
				ResetTimer = 30*IN_MILLISECONDS;
			}
			else ResetTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };
 
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_maloricAI(creature);
    }
};

void AddSC_crystalsong_forest()
{
    new npc_warmage_violetstand();
	new npc_maloric();
}