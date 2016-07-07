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
	SAY_TAUNT_1           = -1564018,
	SAY_TAUNT_2           = -1564019,
	SAY_TAUNT_3           = -1564020,
	SAY_AGGRO             = -1564021,
	SAY_SPELL_1           = -1564022,
	SAY_SPELL_2           = -1564023,
	SAY_SPELL_3           = -1564024,
	SAY_SLAY_1            = -1564025,
	SAY_SLAY_2            = -1564026,
	SAY_ENRAGE            = -1564027,
	SAY_DEATH             = -1564028,
};

enum Spells
{
	SPELL_BEAM_SINISTER         = 40859,
	SPELL_BEAM_SINISTER_TRIGGER = 40863,
	SPELL_BEAM_VILE             = 40860,
	SPELL_BEAM_VILE_TRIGGER     = 40865,
	SPELL_BEAM_WICKED           = 40861,
	SPELL_BEAM_WICKED_TRIGGER   = 40866,
	SPELL_BEAM_SINFUL           = 40827,
	SPELL_BEAM_SINFUL_TRIGGER   = 40862,
	SPELL_ATTRACTION            = 40871,
	SPELL_ATTRACTION_VIS        = 41001,
	SPELL_SILENCING_SHRIEK      = 40823,
	SPELL_ENRAGE                = 23537,
	SPELL_SABER_LASH            = 40810,
	SPELL_SABER_LASH_TRIGGER    = 40816,
	SPELL_SABER_LASH_IMM        = 43690,
	SPELL_TELEPORT_VISUAL       = 40869,
	SPELL_BERSERK               = 45078,
	SPELL_PRISMATIC_SHIELD      = 40879,
};

struct Locations
{
    float x, y, z;
};

static Locations TeleportPoint[] =
{
    {959.996f, 212.576f, 193.843f},
    {932.537f, 231.813f, 193.838f},
    {958.675f, 254.767f, 193.822f},
    {946.955f, 201.316f, 192.535f},
    {944.294f, 149.676f, 197.551f},
    {930.548f, 284.888f, 193.367f},
    {965.997f, 278.398f, 195.777f}
};

class boss_mother_shahraz : public CreatureScript
{
public:
    boss_mother_shahraz() : CreatureScript("boss_mother_shahraz") { }

    struct boss_shahrazAI : public QuantumBasicAI
    {
        boss_shahrazAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint64 TargetGUID[3];
        uint32 BeamTimer;
        uint32 BeamCount;
        uint32 CurrentBeam;
        uint32 FatalAttractionTimer;
        uint32 FatalAttractionExplodeTimer;
        uint32 ShriekTimer;
        uint32 SaberTimer;
        uint32 RandomYellTimer;
        uint32 EnrageTimer;

        uint32 pulseTimer;

        bool Enraged;

        void Reset()
        {
			instance->SetData(DATA_MOTHERSHAHRAZEVENT, NOT_STARTED);

            for(uint8 i = 0; i<3; i++)
                TargetGUID[i] = 0;

            BeamTimer = 5000; // Timers may be incorrect
            BeamCount = 1;
            CurrentBeam = rand()%4;                                    // 0 - Sinister, 1 - Vile, 2 - Wicked, 3 - Sinful
            FatalAttractionTimer = 60000;
            FatalAttractionExplodeTimer = 70000;
            ShriekTimer = 20000;
            SaberTimer = 5000;
            RandomYellTimer = urand(70, 111) * 1000;
            EnrageTimer = 600000;

            pulseTimer = 10000;

            Enraged = false;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_MOTHERSHAHRAZEVENT, IN_PROGRESS);

            DoZoneInCombat();
            DoSendQuantumText(SAY_AGGRO, me);
            DoCast(me, SPELL_PRISMATIC_SHIELD, true);
            DoCast(me, SPELL_SABER_LASH_TRIGGER, true);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_MOTHERSHAHRAZEVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);
        }

        void TeleportPlayers()
        {
            uint32 random = rand()%7;
            float X = TeleportPoint[random].x;
            float Y = TeleportPoint[random].y;
            float Z = TeleportPoint[random].z;
            for(uint8 i = 0; i < 3; i++)
            {
                Unit* unit = SelectTarget(TARGET_RANDOM, 1, 300,true);
                if (unit && unit->IsAlive() && (unit->GetTypeId() == TYPE_ID_PLAYER) && !unit->HasAura(SPELL_SABER_LASH_IMM,0))
                {
                    TargetGUID[i] = unit->GetGUID();
                    unit->CastSpell(unit, SPELL_TELEPORT_VISUAL, true);
                    unit->NearTeleportTo(X, Y, Z, unit->GetOrientation());
                }
            }
        }

        void CastBeam(const uint32 diff)
        {
            //Randomly cast one beam.
            if (BeamTimer <= diff)
            {
                Unit* target = SelectTarget(TARGET_RANDOM, 0, 200, true);
                if (!target || !target->IsAlive())
                    return;

                BeamTimer = 8000;

                switch (CurrentBeam)
                {
                    case 0:
                        DoCast(target, SPELL_BEAM_SINISTER,true);
                        break;
                    case 1:
                        DoCast(target, SPELL_BEAM_VILE,true);
                        break;
                    case 2:
                        DoCast(target, SPELL_BEAM_WICKED,true);
                        break;
                    case 3:
                        DoCast(target, SPELL_BEAM_SINFUL,true);
                        break;
                }
                BeamCount++;
                uint32 Beam = CurrentBeam;
                if (BeamCount > 3)
                {
                    while(CurrentBeam == Beam)
                        CurrentBeam = rand()%4;
                    BeamCount = 1;
                }
            }
			else BeamTimer -= diff;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (pulseTimer <= diff)
            {
                DoAttackerAreaInCombat(me->GetVictim(),50);
                pulseTimer = 10000;
            }
			else pulseTimer -= diff;

            if (((me->GetHealth()*100 / me->GetMaxHealth()) < 10) && !Enraged)
            {
                Enraged = true;
                DoCast(me, SPELL_ENRAGE, true);
                DoSendQuantumText(SAY_ENRAGE, me);
            }

            CastBeam(diff);

            //if (BeamTimer <= diff)
            //{
            //    if (!me->IsNonMeleeSpellCasted(false))
            //    {
            //        switch(CurrentBeam)
            //        {
            //            case 0:
            //                DoCast(me, SPELL_BEAM_SINISTER_TRIGGER);
            //                break;
            //            case 1:
            //                DoCast(me, SPELL_BEAM_VILE_TRIGGER);
            //                break;
            //            case 2:
            //                DoCast(me, SPELL_BEAM_WICKED_TRIGGER);
            //                break;
            //            case 3:
            //                DoCast(me, SPELL_BEAM_SINFUL_TRIGGER);
            //                break;
            //        }
            //        uint32 Beam = CurrentBeam;
            //        while(CurrentBeam == Beam)
            //            CurrentBeam = rand()%4;

            //        BeamTimer = 30000;
            //    }
            //}
			//else BeamTimer -= diff;

            // Select 3 random targets (can select same target more than once), teleport to a random location then make them cast explosions until they get away from each other.
            if (FatalAttractionTimer <= diff)
            {
                TeleportPlayers();
				DoSendQuantumText(RAND(SAY_SPELL_1, SAY_SPELL_2, SAY_SPELL_3), me);
                FatalAttractionExplodeTimer = 2000;
                FatalAttractionTimer = 30000;
            }
			else FatalAttractionTimer -= diff;

            if (FatalAttractionExplodeTimer <= diff)
            {
                Player* targets[3];
                for(uint8 i = 0; i < 3; ++i)
                {
                    if (TargetGUID[i])
                        targets[i] = Unit::GetPlayer(*me,TargetGUID[i]);
                    else
                        targets[i] = NULL;
                }

                if (targets[0] && targets[0]->IsAlive())
                {
                    bool isNear = false;
                    if (targets[1] && targets[1]->IsAlive() && targets[0]->GetDistance2d(targets[1]) < 25)
                        isNear = true;

                    if (!isNear)
                        if (targets[2] && targets[2]->IsAlive() && targets[0]->GetDistance2d(targets[2]) < 25)
                            isNear = true;
                
                    if (isNear)
                        targets[0]->CastSpell(targets[0],SPELL_ATTRACTION,true);
                    else
                    {
                        targets[0]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                        TargetGUID[0] = 0;
                        targets[0] = NULL;
                    }
                }

                if (targets[1] && targets[1]->IsAlive())
                {
                    bool isNear = false;
                    if (targets[0] && targets[0]->IsAlive() && targets[1]->GetDistance2d(targets[0]) < 25)
                        isNear = true;

                    if (!isNear)
                        if (targets[2] && targets[2]->IsAlive() && targets[1]->GetDistance2d(targets[2]) < 25)
                            isNear = true;
                
                    if (isNear)
                        targets[1]->CastSpell(targets[1],SPELL_ATTRACTION,true);
                    else
                    {
                        targets[1]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                        TargetGUID[1] = 0;
                        targets[1] = NULL;
                    }
                }

                if (targets[2] && targets[2]->IsAlive())
                {
                    bool isNear = false;
                    if (targets[0] && targets[0]->IsAlive() && targets[2]->GetDistance2d(targets[0]) < 25)
                        isNear = true;

                    if (!isNear)
                        if (targets[1] && targets[1]->IsAlive() && targets[2]->GetDistance2d(targets[1]) < 25)
                            isNear = true;

                    if (isNear)
                        targets[2]->CastSpell(targets[1],SPELL_ATTRACTION,true);
                    else
                    {
                        targets[2]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                        TargetGUID[2] = 0;
                        targets[2] = NULL;
                    }
                }

                bool AllClear = true;

                for(uint8 i = 0; i < 3; i++)
                {
                    if (TargetGUID[i] != 0)
                        AllClear = false;
                }

                if (AllClear)
                    FatalAttractionExplodeTimer = 60000;
                else
                    FatalAttractionExplodeTimer = 1000;

            }
			else FatalAttractionExplodeTimer -= diff;

            if (ShriekTimer <= diff)
            {
				DoCastVictim(SPELL_SILENCING_SHRIEK);
				ShriekTimer = 20000;
            }
			else ShriekTimer -= diff;

            //if (SaberTimer <= diff)
            //{
            //    DoCastVictim(SPELL_SABER_LASH);
            //    SaberTimer = 10000;
            //}else SaberTimer -= diff;

            //Enrage
            if (!me->HasAura(SPELL_BERSERK, 0))
                if (EnrageTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK);
                    DoSendQuantumText(SAY_ENRAGE, me);
                }
				else EnrageTimer -= diff;

            //Random taunts
            if (RandomYellTimer <= diff)
            {
				DoSendQuantumText(RAND(SAY_TAUNT_1, SAY_TAUNT_2, SAY_TAUNT_3), me);
            }
			else RandomYellTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_shahrazAI(creature);
    }
};

void AddSC_boss_mother_shahraz()
{
    new boss_mother_shahraz();
}