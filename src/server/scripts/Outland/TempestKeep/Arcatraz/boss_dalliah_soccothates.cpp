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
#include "arcatraz.h"

enum Spells
{
	SPELL_WHIRLWIND         = 36142,
	SPELL_HEAL_5N           = 36144,
	SPELL_HEAL_5H           = 39013,
	SPELL_GIFT_DOOMSAYER_5N = 36173,
	SPELL_GIFT_DOOMSAYER_5H = 39009,
	SPELL_SHADOW_WAVE       = 39016,
	SPELL_FEL_IMMOLATION_5N = 36051,
	SPELL_FEL_IMMOLATION_5H = 39007,
	SPELL_FELFIRE           = 35769,
	SPELL_KNOCK_BACK        = 36512,
	SPELL_LINEUP            = 35770,
	SPELL_FELFIRE_SHOCK_5N  = 35759,
	SPELL_FELFIRE_SHOCK_5H  = 39006,
};

#define SOUND_INTRO             11236
#define SAY_D_AGGRO             "It is unwise to anger me."
#define SOUND_D_AGGRO           11086
#define SAY_D_HEAL_1            "Ahh... That is much better." 
#define SOUND_D_HEAL_1          11091
#define SAY_D_HEAL_2            "Ahh... Just what I needed."
#define SOUND_D_HEAL_2          11092
#define SAY_D_KILL_1            "Completely ineffective. Just like someone else I know." 
#define SOUND_D_KILL_1          11087
#define SAY_D_KILL_2            "You chose the wrong opponent."
#define SOUND_D_KILL_2          11088    
#define SAY_D_WHIRL_1           "I'll cut you to pieces!" 
#define SOUND_D_WHIRL_1         11089
#define SAY_D_WHIRL_2           "Reap the Whirlwind!"
#define SOUND_D_WHIRL_2         11090
#define SAY_D_DEATH             "Now I'm really... angry..." 
#define SOUND_D_DEATH           11093

#define SAY_AGGRO_SOCCO         "Don't worry about me; kill that worthless dullard instead!"
#define SOUND_AGGRO_SOCCO       11085
#define SAY_25_SOCCO            "More than you can handle, scryer?"
#define SOUND_25_SOCCO          11094
#define SAY_DEATH_SOCCO         "Congratulations. I've wanted to do that for years." 
#define SOUND_DEATH_SOCCO       11097

class boss_dalliah : public CreatureScript
{
public:
    boss_dalliah() : CreatureScript("boss_dalliah") { }

    struct boss_dalliahAI : public QuantumBasicAI
    {
        boss_dalliahAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

            Once = false;
            OtherDead = false;
        }

        InstanceScript* instance;

        uint32 WhirlwindTimer;
        uint32 HealTimer;
        uint32 GiftTimer;
        uint32 ShadowWaveTimer;

		bool InCombat;
        bool Hp25;
        bool OtherHp25;
        bool OtherAggro;
        bool OtherDead;
        bool Once;
        bool IsHealing;

        void Reset() 
        {
            WhirlwindTimer = 15000;
            HealTimer = 999999;
            GiftTimer = 10000;
            ShadowWaveTimer = 12500;

            InCombat = false;
            Hp25 = false;
            OtherHp25 = false;
            OtherAggro = false;
            IsHealing = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_DALLIAHTHEDOOMSAYEREVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (!Once && me->IsWithinDistInMap(who, 105.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
            {
				DoPlaySoundToSet(me, SOUND_INTRO);
                Once = true;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_DALLIAHTHEDOOMSAYEREVENT, IN_PROGRESS);

            DoPlaySoundToSet(me, SOUND_D_AGGRO);
            me->MonsterYell(SAY_D_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            switch(rand()%2)
			{
                case 0:
					DoPlaySoundToSet(me, SOUND_D_KILL_1);
					me->MonsterYell(SAY_D_KILL_1, LANG_UNIVERSAL, 0);
					break;
				case 1:
					DoPlaySoundToSet(me, SOUND_D_KILL_2);
					me->MonsterYell(SAY_D_KILL_2, LANG_UNIVERSAL, 0);
					break;
			}
        }
 
        void JustDied(Unit* /*killer*/)
        {
            DoPlaySoundToSet(me, SOUND_D_DEATH);
            me->MonsterYell(SAY_D_DEATH, LANG_UNIVERSAL, 0);

			instance->SetData(DATA_DALLIAHTHEDOOMSAYEREVENT, DONE);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            switch(spell->Id)
            {
                case SPELL_HEAL_5N:
                case SPELL_HEAL_5H:
                    switch(rand()%2)
                    {
                        case 0:
							DoPlaySoundToSet(me, SOUND_D_HEAL_1);
							me->MonsterYell(SAY_D_HEAL_1, LANG_UNIVERSAL, 0);
							break;
						case 1:
							DoPlaySoundToSet(me, SOUND_D_HEAL_2);
							me->MonsterYell(SAY_D_HEAL_2, LANG_UNIVERSAL, 0);
							break;
					}
					IsHealing = false;
					break;
            }

            if ((spell->Effects[0].Effect != SPELL_EFFECT_INTERRUPT_CAST && spell->Effects[1].Effect != SPELL_EFFECT_INTERRUPT_CAST && spell->Effects[2].Effect !=SPELL_EFFECT_INTERRUPT_CAST) || !me->IsNonMeleeSpellCasted(false))
                return;

            if (IsHealing)
            {
                me->InterruptNonMeleeSpells(false);
                IsHealing = false;
            }
        }
   
        void UpdateAI(const uint32 diff)
        {
            if (!OtherDead)
            {
                Unit* Temp = NULL; 
                Temp = Unit::GetUnit(*me, instance->GetData64(DATA_WRATH_SCRYERSOCCOTHRATES));
            
                if (Temp->IsDead())
                {
                    DoPlaySoundToSet(me, SOUND_DEATH_SOCCO);
                    me->MonsterYell(SAY_DEATH_SOCCO, LANG_UNIVERSAL, 0);
                    OtherDead = true;
                }

                if (!OtherAggro)
                {
                     if (instance->GetData(DATA_WRATH_SCRYERSOCCOTHRATESEVENT) == IN_PROGRESS)
                     {
                        DoPlaySoundToSet(me, SOUND_AGGRO_SOCCO);
                        me->MonsterYell(SAY_AGGRO_SOCCO, LANG_UNIVERSAL, 0);
                        OtherAggro = true;
                     }
                }

                if (!OtherHp25)
                {
                    if (instance->GetData(DATA_SOCCOTHRATES25) == 1) 
                    {
                        DoPlaySoundToSet(me, SOUND_25_SOCCO);
                        me->MonsterYell(SAY_25_SOCCO, LANG_UNIVERSAL, 0);
                        OtherHp25 = true;
                    }
                }
            }

            if (!UpdateVictim())
                return;

            if (WhirlwindTimer < diff)
            {
                switch (rand()%2)
                {
                    case 0:
						DoPlaySoundToSet(me, SOUND_D_WHIRL_1);
						me->MonsterYell(SAY_D_WHIRL_1 ,LANG_UNIVERSAL, 0);
						break;
					case 1:
						DoPlaySoundToSet(me, SOUND_D_WHIRL_2);
						me->MonsterYell(SAY_D_WHIRL_2, LANG_UNIVERSAL, 0);
						break;
				}
                DoCastAOE(SPELL_WHIRLWIND);
                WhirlwindTimer = 35000;
                HealTimer = 6000;
            }
			else WhirlwindTimer -= diff;

            if (HealTimer < diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    IsHealing = true;
                    DoCast(me, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
                    HealTimer = 900000;
                }
            }
			else HealTimer -= diff;

            if (GiftTimer < diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_GIFT_DOOMSAYER_5N, SPELL_GIFT_DOOMSAYER_5H));
                GiftTimer = 30000;
            }
			else GiftTimer -= diff;

            if (!Hp25)
            {
                if (me->GetHealth()*100 / me->GetMaxHealth() < 25)
                {
                    Hp25 = true;
                    instance->SetData(DATA_DALLIAH25, 1);
                }
            }

			if (ShadowWaveTimer < diff && IsHeroic())
			{
				if (!me->IsNonMeleeSpellCasted(false))
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_SHADOW_WAVE);
						ShadowWaveTimer = 10000;
					}
				}
			}
			else ShadowWaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_dalliahAI(creature);
    }
};

#define SAY_S_AGGRO             "At last, a target for my frustrations!" 
#define SOUND_S_AGGRO           11238
#define SAY_S_CHARGE_1          "On guard!"
#define SOUND_S_CHARGE_1        11241
#define SAY_S_CHARGE_2          "Defend yourself, for all the good it will do..."
#define SOUND_S_CHARGE_2        11242
#define SAY_S_KILL_1            "Yes, that was quite satisfying."
#define SOUND_S_KILL_1          11239
#define SAY_S_KILL_2            "Much better."
#define SOUND_S_KILL_2          11240
#define SAY_S_DEATH             "Knew this was... the only way out." 
#define SOUND_S_DEATH           11243

#define SAY_AGGRO_DALLI         "Have you come to kill Dalliah? Can I watch?"
#define SOUND_AGGRO_DALLI       11237
#define SAY_25_DALLI_1          "Having problems, Dalliah? How nice."
#define SOUND_25_DALLI_1        11244
#define SAY_25_DALLI_2          "I suggest a new strategy, you draw the attackers while I gather reinforcements. Hahaha!"
#define SOUND_25_DALLI_2        11246
#define SAY_DEATH_DALLI         "Finally! Well done!" 
#define SOUND_DEATH_DALLI       11247

class boss_soccothates : public CreatureScript
{
public:
    boss_soccothates() : CreatureScript("boss_soccothates") { }

    struct boss_soccothatesAI : public QuantumBasicAI
    {
        boss_soccothatesAI(Creature* creature) : QuantumBasicAI(creature) 
        {
            instance = creature->GetInstanceScript();
            Reset();
            OtherDead = false;
        }

        InstanceScript* instance;

		uint32 FlameShockTimer;
        uint32 FlameTimer;
        uint32 LineupTimer;

        bool InCombat;
        bool Hp25;
        bool OtherHp25;
        bool OtherAggro;
        bool OtherDead;

        void Reset() 
        {
            FlameShockTimer = 1000;
            FlameTimer = 10000;
            LineupTimer = 0;

            InCombat = false;
            Hp25 = false;
            OtherHp25 = false;
            OtherAggro = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_WRATH_SCRYERSOCCOTHRATESEVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->RemoveAllAuras();

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
            DoCast(me, DUNGEON_MODE(SPELL_FEL_IMMOLATION_5N, SPELL_FEL_IMMOLATION_5H), true);
			instance->SetData(DATA_WRATH_SCRYERSOCCOTHRATESEVENT, IN_PROGRESS);
            DoPlaySoundToSet(me, SOUND_S_AGGRO);
            me->MonsterYell(SAY_S_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* /*victim*/)
        {    
            switch(rand()%2)
            {
                case 0:
					DoPlaySoundToSet(me, SOUND_S_KILL_1);
					me->MonsterYell(SAY_S_KILL_1,LANG_UNIVERSAL, 0);
					break;
				case 1:
					DoPlaySoundToSet(me, SOUND_S_KILL_2);
					me->MonsterYell(SAY_S_KILL_2, LANG_UNIVERSAL, 0);
					break;
            }
        }
 
        void JustDied(Unit* /*killer*/)
        {
            DoPlaySoundToSet(me, SOUND_S_DEATH);

            me->MonsterYell(SAY_S_DEATH, LANG_UNIVERSAL,NULL);

			instance->SetData(DATA_WRATH_SCRYERSOCCOTHRATESEVENT, DONE);
        }
   
        void UpdateAI(const uint32 diff)
        {
            if (!OtherDead)
            {
                Unit* Temp = NULL; 
                Temp = Unit::GetUnit(*me, instance->GetData64(DATA_DALLIAHTHEDOOMSAYER));
            
                if (Temp->IsDead())
                {
                    DoPlaySoundToSet(me, SOUND_DEATH_DALLI);
                    me->MonsterYell(SAY_DEATH_DALLI, LANG_UNIVERSAL, 0);
                    OtherDead = true;
                }

                if (!OtherAggro)
                {
                     if (instance->GetData(DATA_DALLIAHTHEDOOMSAYEREVENT) == IN_PROGRESS)
                     {
                        DoPlaySoundToSet(me, SOUND_AGGRO_DALLI);
                        me->MonsterYell(SAY_AGGRO_DALLI, LANG_UNIVERSAL, 0);
                        OtherAggro = true;
                     }
                }

                if (!OtherHp25)
                {
                    if (instance->GetData(DATA_DALLIAH25) == 1) 
                    {
                        switch(rand()%2)
                        {
                            case 0:
								DoPlaySoundToSet(me,SOUND_25_DALLI_1);
								me->MonsterYell(SAY_25_DALLI_1, LANG_UNIVERSAL, 0);
								break;
							case 1:
								DoPlaySoundToSet(me,SOUND_25_DALLI_2);
								me->MonsterYell(SAY_25_DALLI_2, LANG_UNIVERSAL, 0);
								break;
						}                
						OtherHp25 = true;
                    }
                }
            }

            if (!UpdateVictim())
                return;

            if (FlameShockTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_FELFIRE_SHOCK_5N, SPELL_FELFIRE_SHOCK_5H));
					FlameShockTimer = 15000;
				}
            }
			else FlameShockTimer -= diff;

            if (FlameTimer < diff)
            {
                switch (rand()%2)
                {
                    case 0:
                        DoPlaySoundToSet(me,SOUND_S_CHARGE_1);
                        me->MonsterYell(SAY_S_CHARGE_1, LANG_UNIVERSAL, 0);
                        break;
                    case 1:
                        DoPlaySoundToSet(me,SOUND_S_CHARGE_2);
                        me->MonsterYell(SAY_S_CHARGE_2, LANG_UNIVERSAL, 0);
                        break;
                }

                DoCast(me, SPELL_KNOCK_BACK);

                LineupTimer = 5000;
                FlameTimer = 30000;
            }
			else FlameTimer -= diff;

            if (LineupTimer)
            {
                if (LineupTimer < diff)
                {
                    if (!me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        DoCast(me, SPELL_FELFIRE, true);
                        LineupTimer = 200;
                    }
					else LineupTimer = 0;
                }
				else LineupTimer -= diff;
            }

            if (!Hp25)
            {
                if (me->GetHealth()*100 / me->GetMaxHealth() < 25) 
                {
                    Hp25 = true;
                    instance->SetData(DATA_SOCCOTHRATES25, 1);
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_soccothatesAI(creature);
    }
};

void AddSC_boss_dalliah_soccothrates()
{
    new boss_dalliah();
    new boss_soccothates();
}