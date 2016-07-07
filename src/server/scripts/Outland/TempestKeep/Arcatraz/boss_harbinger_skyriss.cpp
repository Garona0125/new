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

enum Texts
{
    SAY_INTRO              = -1552000,
    SAY_AGGRO              = -1552001,
    SAY_KILL_1             = -1552002,
    SAY_KILL_2             = -1552003,
    SAY_MIND_1             = -1552004,
    SAY_MIND_2             = -1552005,
    SAY_FEAR_1             = -1552006,
    SAY_FEAR_2             = -1552007,
    SAY_IMAGE              = -1552008,
    SAY_DEATH              = -1552009,
};

enum Spells
{
    SPELL_FEAR               = 39415,
    SPELL_MIND_REND_5N       = 36924,
    SPELL_MIND_REND_5H       = 39017,
    SPELL_DOMINATION_5N      = 37162,
    SPELL_DOMINATION_5H      = 39019,
    SPELL_MANA_BURN          = 39020,
    SPELL_66_ILLUSION        = 36931,
    SPELL_33_ILLUSION        = 36932,
	// Need Implement
	SPELL_MIND_REND_IMAGE_5N = 36929,
	SPELL_MIND_REND_IMAGE_5H = 39021,
};

class boss_harbinger_skyriss : public CreatureScript
{
    public:
        boss_harbinger_skyriss() : CreatureScript("boss_harbinger_skyriss") { }

        struct boss_harbinger_skyrissAI : public QuantumBasicAI
        {
            boss_harbinger_skyrissAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
                Intro = false;
            }

            InstanceScript* instance;
			SummonList Summons;

			uint32 IntroPhase;
            uint32 IntroTimer;
            uint32 MindRendTimer;
            uint32 FearTimer;
            uint32 DominationTimer;
            uint32 ManaBurnTimer;

            bool Intro;
            bool IsImage33;
            bool IsImage66;

            void Reset()
            {
                if (!Intro)
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

                IsImage33 = false;
                IsImage66 = false;

				Summons.DespawnAll();

                IntroPhase = 1;
                IntroTimer = 5000;
                MindRendTimer = 3000;
                FearTimer = 15000;
                DominationTimer = 30000;
                ManaBurnTimer = 25000;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				instance->SetData(TYPE_HARBINGERSKYRISS, NOT_STARTED);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!Intro)
                    return;

                QuantumBasicAI::MoveInLineOfSight(who);
            }

            void EnterToBattle(Unit* /*who*/)
			{
				instance->SetData(TYPE_HARBINGERSKYRISS, IN_PROGRESS);
			}

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
				instance->SetData(TYPE_HARBINGERSKYRISS, DONE);
				Summons.DespawnAll();
				CompleteQuest();
            }

			void JustSummoned(Creature* summon)
			{
				if (!summon)
					return;

			    switch (summon->GetEntry())
				{
				    case NPC_SKYRISS_CLONE_1:
					case NPC_SKYRISS_CLONE_2:
						summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

						if (IsImage66)
							summon->SetHealth(summon->CountPctFromMaxHealth(33));
						else
							summon->SetHealth(summon->CountPctFromMaxHealth(66));

						if (me->GetVictim())
						{
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								summon->AI()->AttackStart(target);
						}
						Summons.Summon(summon);
						break;
					default:
						break;
				}
			}

            void KilledUnit(Unit* victim)
            {
                if (victim->GetEntry() == NPC_ALPHA_TARGET)
                    return;

                DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void DoSplit(uint32 value)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                DoSendQuantumText(SAY_IMAGE, me);

                if (value == 66)
                    DoCast(me, SPELL_66_ILLUSION);
                else
                    DoCast(me, SPELL_33_ILLUSION);
            }

			void CompleteQuest()
			{
				Unit* millhouse = Unit::GetUnit(*me, instance->GetData64(DATA_MILLHOUSE));
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();

				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (IsHeroic() && millhouse->IsAlive())
							player->CastSpell(player, SPELL_COMPLETE_QUEST_10886, true);
					}
				}
			}

            void UpdateAI(const uint32 diff)
            {
                if (!Intro)
                {
                    if (IntroTimer <= diff)
                    {
                        switch (IntroPhase)
                        {
                        case 1:
                            DoSendQuantumText(SAY_INTRO, me);
                            instance->HandleGameObject(instance->GetData64(DATA_SPHERE_SHIELD), true);
                            ++IntroPhase;
                            IntroTimer = 25000;
                            break;
                        case 2:
                            DoSendQuantumText(SAY_AGGRO, me);
                            if (Unit* mellichar = Unit::GetUnit(*me, instance->GetData64(DATA_MELLICHAR)))
                            {
                                mellichar->setDeathState(JUST_DIED);
                                mellichar->SetHealth(0);
                                instance->SetData(TYPE_SHIELD_OPEN, IN_PROGRESS);
                            }
                            ++IntroPhase;
                            IntroTimer = 3000;
                            break;
                        case 3:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                            Intro = true;
                            break;
                        }
                    }
                    else IntroTimer -= diff;
                }
                if (!UpdateVictim())
                    return;

                if (!IsImage66 && !HealthAbovePct(66))
                {
                    DoSplit(66);
                    IsImage66 = true;
                }
                if (!IsImage33 && !HealthAbovePct(33))
                {
                    DoSplit(33);
                    IsImage33 = true;
                }

                if (MindRendTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
					{
                        DoCast(target, DUNGEON_MODE(SPELL_MIND_REND_5N, SPELL_MIND_REND_5H));
						MindRendTimer = 8000;
					}
                }
                else MindRendTimer -= diff;

                if (FearTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoSendQuantumText(RAND(SAY_FEAR_1, SAY_FEAR_2), me);
                        DoCast(target, SPELL_FEAR);
						FearTimer = 25000;
					}
                }
                else FearTimer -= diff;

                if (DominationTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
					{
						DoSendQuantumText(RAND(SAY_MIND_1, SAY_MIND_2), me);
                        DoCast(target, DUNGEON_MODE(SPELL_DOMINATION_5N, SPELL_DOMINATION_5H));
						DominationTimer = 16000+rand()%16000;
					}
				}
                else DominationTimer -= diff;

				if (ManaBurnTimer <= diff && IsHeroic())
				{
					if (me->IsNonMeleeSpellCasted(false))
						return;

					if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
					{
						if (target && target->GetPowerType() == POWER_MANA)
						{
							DoCast(target, SPELL_MANA_BURN);
							ManaBurnTimer = 16000+rand()%16000;
						}
					}
				}
				else ManaBurnTimer -= diff;

				DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_harbinger_skyrissAI(creature);
        }
};

class boss_harbinger_skyriss_illusion : public CreatureScript
{
    public:
        boss_harbinger_skyriss_illusion() : CreatureScript("boss_harbinger_skyriss_illusion") { }

        struct boss_harbinger_skyriss_illusionAI : public QuantumBasicAI
        {
            boss_harbinger_skyriss_illusionAI(Creature* creature) : QuantumBasicAI(creature) { }

            void Reset(){}

            void EnterToBattle(Unit* /*who*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_harbinger_skyriss_illusionAI(creature);
        }
};

void AddSC_boss_harbinger_skyriss()
{
    new boss_harbinger_skyriss();
    new boss_harbinger_skyriss_illusion();
}