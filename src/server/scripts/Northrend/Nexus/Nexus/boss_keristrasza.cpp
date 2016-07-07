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
#include "nexus.h"

enum Texts
{
    SAY_AGGRO        = -1576040,
    SAY_SLAY         = -1576041,
    SAY_ENRAGE       = -1576042,
    SAY_DEATH        = -1576043,
    SAY_CRYSTAL_NOVA = -1576044,
};

enum Spells
{
    SPELL_FROZEN_PRISON          = 47854,
    SPELL_TAIL_SWEEP             = 50155,
    SPELL_CRYSTAL_CHAINS         = 50997,
    SPELL_ENRAGE                 = 8599,
    SPELL_CRYSTALFIRE_BREATH_5N  = 48096,
    SPELL_CRYSTALFIRE_BREATH_5H  = 57091,
    SPELL_CRYSTALIZE             = 48179,
    SPELL_INTENSE_COLD           = 48094,
    SPELL_INTENSE_COLD_TRIGGERED = 48095,
};

enum Misc
{
    DATA_CONTAINMENT_SPHERES = 3,
};

class boss_keristrasza : public CreatureScript
{
public:
    boss_keristrasza() : CreatureScript("boss_keristrasza") { }

    struct boss_keristraszaAI : public QuantumBasicAI
    {
        boss_keristraszaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CrystalfireBreathTimer;
        uint32 CrystalChainsTimer;
        uint32 CrystalizeTimer;
        uint32 TailSweepTimer;
        bool Enrage;

        std::set<uint64> lIntenseColdPlayers;
        uint64 auiContainmentSphereGUIDs[DATA_CONTAINMENT_SPHERES];

        void Reset()
        {
            CrystalfireBreathTimer = 10*IN_MILLISECONDS;
            CrystalChainsTimer = 20*IN_MILLISECONDS;
            CrystalizeTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            TailSweepTimer = 5*IN_MILLISECONDS;
            Enrage = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

			lIntenseColdPlayers.clear();

            RemovePrison(CheckContainmentSpheres());

            if (instance)
                instance->SetData(DATA_KERISTRASZA_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
            DoCast(SPELL_INTENSE_COLD);

            if (instance)
            {
                instance->SetData(DATA_KERISTRASZA_EVENT, IN_PROGRESS);

                Map::PlayerList const &players = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    lIntenseColdPlayers.insert(itr->getSource()->GetGUID());
            }
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
            {
                AchievementEntry const *achievIntenseCold = sAchievementStore.LookupEntry(ACHIEVEMENT_INTENSE_COLD);
                if (achievIntenseCold && IsHeroic())
                {
                    for (std::set<uint64>::const_iterator itr = lIntenseColdPlayers.begin(); itr != lIntenseColdPlayers.end(); ++itr)
                    {
                        Player* player = Unit::GetPlayer(*me, *itr);

                        if (player && player->IsAlive() && (player->GetDistance2d(me) < 50))
                            player->CompletedAchievement(achievIntenseCold);
                    }
                }

                instance->SetData(DATA_KERISTRASZA_EVENT, DONE);
            }
        }

        bool CheckContainmentSpheres(bool remove_prison = false)
        {
            if (!instance)
                return false;

            auiContainmentSphereGUIDs[0] = instance->GetData64(ANOMALUS_CONTAINMET_SPHERE);
            auiContainmentSphereGUIDs[1] = instance->GetData64(ORMOROKS_CONTAINMET_SPHERE);
            auiContainmentSphereGUIDs[2] = instance->GetData64(TELESTRAS_CONTAINMET_SPHERE);

            GameObject* ContainmentSpheres[DATA_CONTAINMENT_SPHERES];

            for (uint8 i = 0; i < DATA_CONTAINMENT_SPHERES; ++i)
            {
                ContainmentSpheres[i] = instance->instance->GetGameObject(auiContainmentSphereGUIDs[i]);
                if (!ContainmentSpheres[i])
                    return false;
                if (ContainmentSpheres[i]->GetGoState() != GO_STATE_ACTIVE)
                    return false;
            }
            if (remove_prison)
                RemovePrison(true);
            return true;
        }

        void RemovePrison(bool remove)
        {
            if (remove)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

                if (me->HasAura(SPELL_FROZEN_PRISON))
					me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                DoCast(me, SPELL_FROZEN_PRISON, false);
            }
        }

        void SpellHitTarget(Unit *target, const SpellInfo *spell)
        {
            if (target->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (spell->Id == SPELL_INTENSE_COLD_TRIGGERED)
			{
                 if (Aura* aura = target->GetAura(SPELL_INTENSE_COLD_TRIGGERED))
				 {
                     if (aura->GetStackAmount() > 2)
                         lIntenseColdPlayers.erase(target->GetGUID());
				 }
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (!Enrage && HealthBelowPct(25))
            {
                DoSendQuantumText(SAY_ENRAGE, me);
                DoCast(me, SPELL_ENRAGE);
                Enrage = true;
            }

            if (CrystalfireBreathTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_CRYSTALFIRE_BREATH_5N, SPELL_CRYSTALFIRE_BREATH_5H));
				CrystalfireBreathTimer = 14*IN_MILLISECONDS;
            }
			else CrystalfireBreathTimer -= diff;

            if (TailSweepTimer <= diff)
            {
                DoCast(me, SPELL_TAIL_SWEEP);
                TailSweepTimer = 5*IN_MILLISECONDS;
            }
			else TailSweepTimer -= diff;

            if (CrystalChainsTimer <= diff)
            {
                if (Unit *target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_CRYSTAL_CHAINS);
					CrystalChainsTimer = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
				}
            }
			else CrystalChainsTimer -= diff;

            if (IsHeroic())
			{
                if (CrystalizeTimer <= diff)
                {
                    DoSendQuantumText(SAY_CRYSTAL_NOVA, me);
                    DoCast(me, SPELL_CRYSTALIZE);

                    CrystalizeTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                }
				else CrystalizeTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_keristraszaAI(creature);
    }
};

class go_containment_sphere : public GameObjectScript
{
public:
    go_containment_sphere() : GameObjectScript("go_containment_sphere") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        Creature* keristrasza = Unit::GetCreature(*go, instance ? instance->GetData64(DATA_KERISTRASZA) : 0);
        if (keristrasza && keristrasza->IsAlive())
        {
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            go->SetGoState(GO_STATE_ACTIVE);
            CAST_AI(boss_keristrasza::boss_keristraszaAI, keristrasza->AI())->CheckContainmentSpheres(true);
        }
        return true;
    }
};

void AddSC_boss_keristrasza()
{
    new boss_keristrasza();
    new go_containment_sphere();
}