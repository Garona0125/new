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
SDName: Boss_Dathrohan_Balnazzar
SD%Complete: 95
SDComment: Possibly need to fix/improve summons after death
SDCategory: Stratholme
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Balnazzar
{
    //Dathrohan spells
    SPELL_CRUSADERSHAMMER           = 17286,                //AOE stun
    SPELL_CRUSADERSTRIKE            = 17281,
    SPELL_HOLYSTRIKE                = 17284,                //weapon dmg +3
    SPELL_BALNAZZARTRANSFORM        = 17288,                //restore full HP/mana, trigger spell Balnazzar Transform Stun
    SPELL_SHADOWSHOCK               = 17399,
    SPELL_MINDBLAST                 = 17287,
    SPELL_PSYCHICSCREAM             = 13704,
    SPELL_SLEEP                     = 12098,
    SPELL_MINDCONTROL               = 15690,

    NPC_DATHROHAN                   = 10812,
    NPC_BALNAZZAR                   = 10813,
    NPC_ZOMBIE                      = 10698                 //probably incorrect
};

struct SummonDef
{
    float m_fX, m_fY, m_fZ, m_fOrient;
};

SummonDef m_aSummonPoint[]=
{
    {3444.156f, -3090.626f, 135.002f, 2.240f},                  //G1 front, left
    {3449.123f, -3087.009f, 135.002f, 2.240f},                  //G1 front, right
    {3446.246f, -3093.466f, 135.002f, 2.240f},                  //G1 back left
    {3451.160f, -3089.904f, 135.002f, 2.240f},                  //G1 back, right

    {3457.995f, -3080.916f, 135.002f, 3.784f},                  //G2 front, left
    {3454.302f, -3076.330f, 135.002f, 3.784f},                  //G2 front, right
    {3460.975f, -3078.901f, 135.002f, 3.784f},                  //G2 back left
    {3457.338f, -3073.979f, 135.002f, 3.784f}                   //G2 back, right
};

class boss_dathrohan_balnazzar : public CreatureScript
{
public:
    boss_dathrohan_balnazzar() : CreatureScript("boss_dathrohan_balnazzar") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_dathrohan_balnazzarAI (creature);
    }

    struct boss_dathrohan_balnazzarAI : public QuantumBasicAI
    {
        boss_dathrohan_balnazzarAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CrusadersHammerTimer;
        uint32 CrusaderStrikeTimer;
        uint32 MindBlastTimer;
        uint32 HolyStrikeTimer;
        uint32 ShadowShockTimer;
        uint32 PsychicScreamTimer;
        uint32 DeepSleepTimer;
        uint32 MindControlTimer;
        bool Transformed;

        void Reset()
        {
            CrusadersHammerTimer = 8000;
            CrusaderStrikeTimer = 12000;
            MindBlastTimer = 6000;
            HolyStrikeTimer = 18000;
            ShadowShockTimer = 4000;
            PsychicScreamTimer = 16000;
            DeepSleepTimer = 20000;
            MindControlTimer = 10000;
            Transformed = false;

            if (me->GetEntry() == NPC_BALNAZZAR)
                me->UpdateEntry(NPC_DATHROHAN);
        }

        void JustDied(Unit* /*Victim*/)
        {
            static uint32 uiCount = sizeof(m_aSummonPoint)/sizeof(SummonDef);

            for (uint8 i=0; i<uiCount; ++i)
                me->SummonCreature(NPC_ZOMBIE,
                m_aSummonPoint[i].m_fX, m_aSummonPoint[i].m_fY, m_aSummonPoint[i].m_fZ, m_aSummonPoint[i].m_fOrient,
                TEMPSUMMON_TIMED_DESPAWN, HOUR*IN_MILLISECONDS);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            //START NOT TRANSFORMED
            if (!Transformed)
            {
                //MindBlast
                if (MindBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_MINDBLAST);
                    MindBlastTimer = urand(15000, 20000);
                }
				else MindBlastTimer -= diff;

                //CrusadersHammer
                if (CrusadersHammerTimer <= diff)
                {
                    DoCastVictim(SPELL_CRUSADERSHAMMER);
                    CrusadersHammerTimer = 12000;
                }
				else CrusadersHammerTimer -= diff;

                //CrusaderStrike
                if (CrusaderStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_CRUSADERSTRIKE);
                    CrusaderStrikeTimer = 15000;
                }
				else CrusaderStrikeTimer -= diff;

                //HolyStrike
                if (HolyStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_HOLYSTRIKE);
                    HolyStrikeTimer = 15000;
                }
				else HolyStrikeTimer -= diff;

                //BalnazzarTransform
                if (HealthBelowPct(40))
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(false);

                    //restore hp, mana and stun
                    DoCast(me, SPELL_BALNAZZARTRANSFORM);
                    me->UpdateEntry(NPC_BALNAZZAR);
                    Transformed = true;
                }
            }
            else
            {
                if (MindBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_MINDBLAST);
                    MindBlastTimer = urand(15000, 20000);
                }
				else MindBlastTimer -= diff;

                if (ShadowShockTimer <= diff)
                {
                    DoCastVictim(SPELL_SHADOWSHOCK);
                    ShadowShockTimer = 11000;
                }
				else ShadowShockTimer -= diff;

                if (PsychicScreamTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_PSYCHICSCREAM);
						PsychicScreamTimer = 20000;
					}
                }
				else PsychicScreamTimer -= diff;

                if (DeepSleepTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_SLEEP);
						DeepSleepTimer = 15000;
					}
                }
				else DeepSleepTimer -= diff;

                if (MindControlTimer <= diff)
                {
                    DoCastVictim(SPELL_MINDCONTROL);
                    MindControlTimer = 15000;
                }
				else MindControlTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_dathrohan_balnazzar()
{
    new boss_dathrohan_balnazzar();
}