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
#include "mana_tombs.h"

/* ScriptData
SDName: Boss_Pandemonius
SD%Complete: 75
SDComment: Not known how void blast is done (amount of rapid cast seems to be related to players in party). All mobs remaining in surrounding area should aggro when engaged.
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

enum Texts
{
	SAY_AGGRO_1      = -1557008,
	SAY_AGGRO_2      = -1557009,
	SAY_AGGRO_3      = -1557010,
	SAY_KILL_1       = -1557011,
	SAY_KILL_2       = -1557012,
	SAY_DEATH        = -1557013,
	EMOTE_DARK_SHELL = -1557014,
};

enum Spells
{
	SPELL_VOID_BLAST_5N = 32325,
	SPELL_VOID_BLAST_5H = 38760,
	SPELL_DARK_SHELL_5N = 32358,
	SPELL_DARK_SHELL_5H = 38759,
};

class boss_pandemonius : public CreatureScript
{
public:
    boss_pandemonius() : CreatureScript("boss_pandemonius") { }

    struct boss_pandemoniusAI : public QuantumBasicAI
    {
        boss_pandemoniusAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 VoidBlastTimer;
        uint32 DarkShellTimer;
        uint32 VoidBlast_Counter;

        void Reset()
        {
            VoidBlastTimer = 8000+rand()%15000;
            DarkShellTimer = 20000;
            VoidBlast_Counter = 0;
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        }

		void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (VoidBlastTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, DUNGEON_MODE(SPELL_VOID_BLAST_5N, SPELL_VOID_BLAST_5H));
                    VoidBlastTimer = 500;
                    ++VoidBlast_Counter;
                }

                if (VoidBlast_Counter == 5)
                {
                    VoidBlastTimer = 15000+rand()%10000;
                    VoidBlast_Counter = 0;
                }
            }
			else VoidBlastTimer -= diff;

            if (!VoidBlast_Counter)
            {
                if (DarkShellTimer <= diff)
                {
                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(true);

                    DoSendQuantumText(EMOTE_DARK_SHELL, me);

                    DoCast(me, DUNGEON_MODE(SPELL_DARK_SHELL_5N, SPELL_DARK_SHELL_5H));
                    DarkShellTimer = 20000;
                }
				else DarkShellTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_pandemoniusAI(creature);
    }
};

void AddSC_boss_pandemonius()
{
    new boss_pandemonius();
}