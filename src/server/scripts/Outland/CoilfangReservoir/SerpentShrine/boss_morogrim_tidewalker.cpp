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

/* ScriptData
SDName: Boss_Morogrim_Tidewalker
SD%Complete: 90
SDComment: Water globules don't explode properly, remove hacks
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "serpent_shrine.h"

enum Morogrim
{
    SAY_AGGRO               = -1548030,
    SAY_SUMMON1             = -1548031,
    SAY_SUMMON2             = -1548032,
    SAY_SUMMON_BUBL1        = -1548033,
    SAY_SUMMON_BUBL2        = -1548034,
    SAY_SLAY1               = -1548035,
    SAY_SLAY2               = -1548036,
    SAY_SLAY3               = -1548037,
    SAY_DEATH               = -1548038,
	// Emotes
    EMOTE_WATERY_GRAVE      = -1548039,
    EMOTE_EARTHQUAKE        = -1548040,
    EMOTE_WATERY_GLOBULES   = -1548041,
	// Spells
    SPELL_TIDAL_WAVE        = 37730,
    SPELL_WATERY_GRAVE      = 38049,
    SPELL_EARTHQUAKE        = 37764,
    SPELL_WATERY_EXPLOSION  = 37852,
    SPELL_WATERY_GRAVE_1    = 38023,
    SPELL_WATERY_GRAVE_2    = 38024,
    SPELL_WATERY_GRAVE_3    = 38025,
    SPELL_WATERY_GRAVE_4    = 37850,
    SPELL_SUMMON_GLOBULE_1  = 37854,
    SPELL_SUMMON_GLOBULE_2  = 37858,
    SPELL_SUMMON_GLOBULE_3  = 37860,
    SPELL_SUMMON_GLOBULE_4  = 37861,
};

float MurlocCords[10][4] =
{
      {424.36f, -715.4f,  -7.14f,  0.124f},
      {425.13f, -719.3f,  -7.14f,  0.124f},
      {425.05f, -724.23f, -7.14f,  0.124f},
      {424.91f, -728.68f, -7.14f,  0.124f},
      {424.84f, -732.18f, -7.14f,  0.124f},
      {321.05f, -734.2f,  -13.15f, 0.124f},
      {321.05f, -729.4f,  -13.15f, 0.124f},
      {321.05f, -724.03f, -13.15f, 0.124f},
      {321.05f, -718.73f, -13.15f, 0.124f},
      {321.05f, -714.24f, -13.15f, 0.124f}
};

class boss_morogrim_tidewalker : public CreatureScript
{
public:
    boss_morogrim_tidewalker() : CreatureScript("boss_morogrim_tidewalker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_morogrim_tidewalkerAI(creature);
    }

    struct boss_morogrim_tidewalkerAI : public QuantumBasicAI
    {
        boss_morogrim_tidewalkerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        Map::PlayerList const* PlayerList;

        uint32 TidalWaveTimer;
        uint32 WateryGraveTimer;
        uint32 EarthquakeTimer;
        uint32 WateryGlobulesTimer;
        uint32 globulespell[4];
        int8 Playercount;
        int8 counter;

        bool Earthquake;
        bool Phase2;

        void Reset()
        {
            TidalWaveTimer = 10000;
            WateryGraveTimer = 30000;
            EarthquakeTimer = 40000;
            WateryGlobulesTimer = 0;
            globulespell[0] = SPELL_SUMMON_GLOBULE_1;
            globulespell[1] = SPELL_SUMMON_GLOBULE_2;
            globulespell[2] = SPELL_SUMMON_GLOBULE_3;
            globulespell[3] = SPELL_SUMMON_GLOBULE_4;

            Earthquake = false;
            Phase2 = false;

            if (instance)
                instance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, NOT_STARTED);
        }

        void StartEvent()
        {
            DoSendQuantumText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
        }

        void JustDied(Unit* /*victim*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, DONE);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            PlayerList = &me->GetMap()->GetPlayers();
            Playercount = PlayerList->getSize();
            StartEvent();
        }

        void ApplyWateryGrave(Unit* player, uint8 i)
        {
            if (player->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (player->GetGUID() == me->GetGUID())
                return;

            switch (i)
            {
                case 0:
					me->CastSpell(player, SPELL_WATERY_GRAVE_1, true);
					break;
                case 1:
					me->CastSpell(player, SPELL_WATERY_GRAVE_2, true);
					break;
                case 2:
					me->CastSpell(player, SPELL_WATERY_GRAVE_3, true);
					break;
                case 3:
					me->CastSpell(player, SPELL_WATERY_GRAVE_4, true);
					break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (EarthquakeTimer <= diff)
            {
                if (!Earthquake)
                {
                    DoCastVictim(SPELL_EARTHQUAKE);
                    Earthquake = true;
                    EarthquakeTimer = 10000;
                }
                else
                {
                    DoSendQuantumText(RAND(SAY_SUMMON1, SAY_SUMMON2), me);

                    for (uint8 i = 0; i < 10; ++i)
                    {
                        Unit* target = SelectTarget(TARGET_RANDOM, 0);
                        Creature* Murloc = me->SummonCreature(NPC_TIDEWALKER_LURKER, MurlocCords[i][0], MurlocCords[i][1], MurlocCords[i][2], MurlocCords[i][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                        if (target && Murloc)
                            Murloc->AI()->AttackStart(target);
                    }
                    DoSendQuantumText(EMOTE_EARTHQUAKE, me);
                    Earthquake = false;
                    EarthquakeTimer = 40000+rand()%5000;
                }
            }
			else EarthquakeTimer -= diff;

            if (TidalWaveTimer <= diff)
            {
                DoCastVictim(SPELL_TIDAL_WAVE);
                TidalWaveTimer = 20000;
            }
			else TidalWaveTimer -= diff;

            if (!Phase2)
            {
                //WateryGraveTimer
                if (WateryGraveTimer <= diff)
                {
                    //Teleport 4 players under the waterfalls
                    Unit* target;
                    std::set<uint64> list;
                    std::set<uint64>::const_iterator itr;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        counter = 0;
                        do
                        {
                            target = SelectTarget(TARGET_RANDOM, 1, 50, true);    //target players only
                            if (counter < Playercount)
                                break;
                            if (target)
                                itr = list.find(target->GetGUID());
                            ++counter;
                        }
						while (itr != list.end());

                        if (target)
                        {
                            list.insert(target->GetGUID());
                            ApplyWateryGrave(target, i);
                        }
                    }

                    DoSendQuantumText(RAND(SAY_SUMMON_BUBL1, SAY_SUMMON_BUBL2), me);
                    DoSendQuantumText(EMOTE_WATERY_GRAVE, me);
                    WateryGraveTimer = 30000;
                }
				else WateryGraveTimer -= diff;

                //Start Phase2
                if (HealthBelowPct(25))
                    Phase2 = true;
            }
            else
            {
                //WateryGlobulesTimer
                if (WateryGlobulesTimer <= diff)
                {
                    Unit* pGlobuleTarget;
                    std::set<uint64> globulelist;
                    std::set<uint64>::const_iterator itr;
                    for (uint8 g = 0; g < 4; g++)  //one unit can't cast more than one spell per update, so some players have to cast for us XD
                    {
                        counter = 0;
                        do
                        {
                            pGlobuleTarget = SelectTarget(TARGET_RANDOM, 0, 50, true);
                            if (pGlobuleTarget)
                                itr = globulelist.find(pGlobuleTarget->GetGUID());
                            if (counter > Playercount)
                                break;
                            ++counter;
                        }
						while (itr != globulelist.end());
                        if (pGlobuleTarget)
                        {
                            globulelist.insert(pGlobuleTarget->GetGUID());
                            pGlobuleTarget->CastSpell(pGlobuleTarget, globulespell[g], true);
                        }
                    }
                    DoSendQuantumText(EMOTE_WATERY_GLOBULES, me);
                    WateryGlobulesTimer = 25000;
                } else WateryGlobulesTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

enum WaterGlobule
{
	SPELL_GLOBULE_EXPLOSION = 37871,
};

class mob_water_globule : public CreatureScript
{
public:
    mob_water_globule() : CreatureScript("mob_water_globule") { }

    struct mob_water_globuleAI : public QuantumBasicAI
    {
        mob_water_globuleAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;

        void Reset()
        {
			CheckTimer = 1000;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            me->SetCurrentFaction(16);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || me->GetVictim())
                return;

            if (me->CanCreatureAttack(who))
            {
                //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(who);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CheckTimer <= diff)
            {
                if (me->IsWithinDistInMap(me->GetVictim(), 5))
                {
                    DoCastVictim(SPELL_GLOBULE_EXPLOSION);
                    me->DespawnAfterAction();
                    return;
                }
                CheckTimer = 500;
            }
			else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_water_globuleAI(creature);
    }
};

void AddSC_boss_morogrim_tidewalker()
{
    new boss_morogrim_tidewalker();
    new mob_water_globule();
}