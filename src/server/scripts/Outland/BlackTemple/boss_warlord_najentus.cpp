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
#include "black_temple.h"

enum Texts
{
	SAY_AGGRO                   = -1564000,
	SAY_IMPALE_1                = -1564001,
	SAY_IMPALE_2                = -1564002,
	SAY_SLAY_1                  = -1564003,
	SAY_SLAY_2                  = -1564004,
	SAY_SPECIAL_1               = -1564005,
	SAY_SPECIAL_2               = -1564006,
	SAY_ENRAGE_1                = -1564007,
	SAY_ENRAGE_2                = -1564008,
	SAY_DEATH                   = -1564009,
};

enum Spells
{
	SPELL_NEEDLE_SPINE          = 39992,
	SPELL_NEEDLE_SPINE_DMG      = 39835,
	SPELL_NEEDLE_SPINE_AOE      = 39968,
	SPELL_TIDAL_BURST           = 39878,
	SPELL_TIDAL_SHIELD          = 39872,
	SPELL_IMPALING_SPINE        = 39837,
	SPELL_CREATE_NAJENTUS_SPINE = 39956,
	SPELL_HURL_SPINE            = 39948,
	SPELL_BERSERK               = 45078,
};

class boss_najentus : public CreatureScript
{
public:
    boss_najentus() : CreatureScript("boss_najentus") { }

    struct boss_najentusAI : public QuantumBasicAI
    {
        boss_najentusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 NeedleSpineTimer;
        uint32 EnrageTimer;
        uint32 SpecialYellTimer;
        uint32 TidalShieldTimer;
        uint32 ImpalingSpineTimer;
		uint32 PulseTimer;

        uint64 SpineTargetGUID;

        void Reset()
        {
            EnrageTimer = 480000;
            SpecialYellTimer = 45000;
            TidalShieldTimer = 60000;

            ResetTimer();

            PulseTimer = 10000;

            SpineTargetGUID = 0;

			instance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, NOT_STARTED);

            DeleteSpine();
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);

            DeleteSpine();
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_HURL_SPINE && me->HasAura(SPELL_TIDAL_SHIELD, 0))
            {
                me->RemoveAurasDueToSpell(SPELL_TIDAL_SHIELD);
                me->CastSpell(me, SPELL_TIDAL_BURST, true);
                ResetTimer();
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
			if (spell->Id == SPELL_NEEDLE_SPINE && target->GetTypeId() == TYPE_ID_PLAYER)
				me->CastSpell(target, SPELL_NEEDLE_SPINE_DMG, true);
		}

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, IN_PROGRESS);

            DoSendQuantumText(SAY_AGGRO, me);
            DoZoneInCombat();
        }

        bool RemoveImpalingSpine()
        {
            if (!SpineTargetGUID)
				return false;

			if (Unit* target = Unit::GetUnit(*me, SpineTargetGUID))
			{
				if (target->HasAura(SPELL_IMPALING_SPINE))
					target->RemoveAurasDueToSpell(SPELL_IMPALING_SPINE);
			}

            SpineTargetGUID = 0;
            return true;
        }

        void ResetTimer(uint32 inc = 0)
        {
            NeedleSpineTimer = 10000 + inc;
            ImpalingSpineTimer = 20000 + inc;
        }

        void DeleteSpine()
        {
            InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
            InstanceMap::PlayerList::const_iterator it;

            Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                Player* player = i->getSource();
                if (player)
                    player->DestroyItemCount(32408, player->GetItemCount(32408), true);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (PulseTimer <= diff)
            {
                DoAttackerAreaInCombat(me->GetVictim(), 50);
                PulseTimer = 10000;
            }
			else PulseTimer -= diff;

            if (TidalShieldTimer <= diff)
            {
				DoCast(me, SPELL_TIDAL_SHIELD, true);
				ResetTimer(45000);
				TidalShieldTimer = 60000;
            }
			else TidalShieldTimer -= diff;

            if (!me->HasAura(SPELL_BERSERK, 0))
            {
                if (EnrageTimer <= diff)
                {
                    DoSendQuantumText(RAND(SAY_ENRAGE_1, SAY_ENRAGE_2), me);
                    DoCast(me, SPELL_BERSERK, true);
                }
				else EnrageTimer -= diff;
            }

            if (SpecialYellTimer <= diff)
            {
				DoSendQuantumText(RAND(SAY_SPECIAL_1, SAY_SPECIAL_2), me);
                SpecialYellTimer = 25000 + (rand()%76)*1000;
            }
			else SpecialYellTimer -= diff;

            if (ImpalingSpineTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_IMPALING_SPINE);
					SpineTargetGUID = target->GetGUID();

					target->SummonGameObject(GO_SPINE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 0);
					DoSendQuantumText(RAND(SAY_IMPALE_1, SAY_IMPALE_2), me);
					ImpalingSpineTimer = 21000;
                }
            }
			else ImpalingSpineTimer -= diff;

            if (NeedleSpineTimer <= diff)
            {
				DoCast(me, SPELL_NEEDLE_SPINE, true);
				NeedleSpineTimer = 3000;
            }
			else NeedleSpineTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_najentusAI(creature);
    }
};

class go_najentus_spine : public GameObjectScript
{
public:
    go_najentus_spine() : GameObjectScript("go_najentus_spine") { }

    bool OnGossipHello (Player* player, GameObject* go)
    {
        if (InstanceScript* instance = go->GetInstanceScript())
		{
			if (Creature* najentus = Unit::GetCreature(*go, instance->GetData64(DATA_HIGHWARLORDNAJENTUS)))
			{
                if (CAST_AI(boss_najentus::boss_najentusAI, najentus->AI())->RemoveImpalingSpine())
                {
                    player->CastSpell(player, SPELL_CREATE_NAJENTUS_SPINE, true);
                    go->AddObjectToRemoveList();
				}
			}
		}
		return true;
	}
};

void AddSC_boss_najentus()
{
    new boss_najentus();
    new go_najentus_spine();
}