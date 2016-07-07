/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellInfo.h"
#include "Group.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_INTRO_1             = -1659011,
    SAY_INTRO_2             = -1659012,
    SAY_INTRO_3             = -1659013,
    SAY_INTRO_4             = -1659014,
    SAY_INTRO_5             = -1659015,
    SAY_INTRO_6             = -1659016,
    SAY_INTRO_7             = -1659017,
    SAY_AGGRO               = -1659018,
    SAY_PHASE_2             = -1659019,
    EMOTE_PHASE_2           = -1659020,
    SAY_DOMINATE_MIND       = -1659021,
    SAY_DARK_EMPOWERMENT    = -1659022,
    SAY_DARK_TRANSFORMATION = -1659023,
    SAY_ANIMATE_DEAD        = -1659024,
    SAY_KILL_1              = -1659025,
	SAY_KILL_2              = -1659026,
    SAY_BERSERK             = -1659027,
    SAY_DEATH               = -1659028,

    SAY_DARNAVAN_AGGRO      = 0,
    SAY_DARNAVAN_RESCUED    = 1,
};

enum Spells
{
    SPELL_MANA_BARRIER              = 70842,

    SPELL_SHADOW_BOLT_10N           = 71254,
	SPELL_SHADOW_BOLT_25N           = 72008,
	SPELL_SHADOW_BOLT_10H           = 72503,
	SPELL_SHADOW_BOLT_25H           = 72504,
	SPELL_FROSTBOLT_10N             = 71420,
	SPELL_FROSTBOLT_25N             = 72007,
	SPELL_FROSTBOLT_10H             = 72501,
	SPELL_FROSTBOLT_25H             = 72502,
	SPELL_FROSTBOLT_VOLLEY_10N      = 72905,
	SPELL_FROSTBOLT_VOLLEY_25N      = 72906,
	SPELL_FROSTBOLT_VOLLEY_10H      = 72907,
	SPELL_FROSTBOLT_VOLLEY_25H      = 72908,
    SPELL_DEATH_AND_DECAY_10N       = 71001,
	SPELL_DEATH_AND_DECAY_25N       = 72108,
	SPELL_DEATH_AND_DECAY_10H       = 72109,
	SPELL_DEATH_AND_DECAY_25H       = 72110,
	SPELL_NECROTIC_STRIKE_10N       = 70659,
	SPELL_NECROTIC_STRIKE_25N       = 72490,
	SPELL_NECROTIC_STRIKE_10H       = 72491,
	SPELL_NECROTIC_STRIKE_25H       = 72492,
	SPELL_SHADOW_CLEAVE_10N         = 70670,
	SPELL_SHADOW_CLEAVE_25N         = 72006,
	SPELL_SHADOW_CLEAVE_10H         = 72493,
	SPELL_SHADOW_CLEAVE_25H         = 72494,
	SPELL_DEATHCHILL_BOLT_10N       = 70594,
	SPELL_DEATHCHILL_BOLT_25N       = 72005,
	SPELL_DEATHCHILL_BOLT_10H       = 72488,
	SPELL_DEATHCHILL_BOLT_25H       = 72489,
	SPELL_DEATHCHILL_BLAST_10N      = 70906,
	SPELL_DEATHCHILL_BLAST_25N      = 72485,
	SPELL_DEATHCHILL_BLAST_10H      = 72486,
	SPELL_DEATHCHILL_BLAST_25H      = 72487,
	SPELL_VENGEFUL_BLAST_10N        = 71544,
	SPELL_VENGEFUL_BLAST_25N        = 72010,
	SPELL_VENGEFUL_BLAST_10H        = 72011,
	SPELL_VENGEFUL_BLAST_25H        = 72012,
    SPELL_DOMINATE_MIND             = 71289,
    SPELL_TOUCH_OF_INSIGNIFICANCE   = 71204,
    SPELL_SUMMON_SHADE              = 71363,
    SPELL_SHADOW_CHANNELING         = 43897,
    SPELL_DARK_TRANSFORMATION_T     = 70895,
    SPELL_DARK_EMPOWERMENT_T        = 70896,
    SPELL_DARK_MARTYRDOM_T          = 70897,
    SPELL_FULL_HOUSE                = 72827, // does not exist in dbc but still can be used for criteria check
    SPELL_TELEPORT_VISUAL           = 41236,
    SPELL_DARK_TRANSFORMATION       = 70900,
    SPELL_VAMPIRIC_MIGHT            = 70674,
    SPELL_FANATIC_S_DETERMINATION   = 71235,
    SPELL_DARK_MARTYRDOM_FANATIC    = 71236,
    SPELL_DARK_EMPOWERMENT          = 70901,
    SPELL_FROST_FEVER               = 67767,
    SPELL_CURSE_OF_TORPOR           = 71237,
    SPELL_SHORUD_OF_THE_OCCULT      = 70768,
    SPELL_ADHERENT_S_DETERMINATION  = 71234,
    SPELL_DARK_MARTYRDOM_ADHERENT   = 70903,
    SPELL_VENGEFUL_BLAST_PASSIVE    = 71494,
    SPELL_BLADESTORM                = 65947,
    SPELL_CHARGE                    = 65927,
    SPELL_INTIMIDATING_SHOUT        = 65930,
    SPELL_MORTAL_STRIKE             = 65926,
    SPELL_SHATTERING_THROW          = 65940,
    SPELL_SUNDER_ARMOR              = 65936,
};

enum EventTypes
{
    EVENT_INTRO_2                       = 1,
    EVENT_INTRO_3                       = 2,
    EVENT_INTRO_4                       = 3,
    EVENT_INTRO_5                       = 4,
    EVENT_INTRO_6                       = 5,
    EVENT_INTRO_7                       = 6,
    EVENT_BERSERK                       = 7,
    EVENT_DEATH_AND_DECAY               = 8,
    EVENT_DOMINATE_MIND_H               = 9,
    EVENT_P1_SUMMON_WAVE                = 10,
    EVENT_P1_SHADOW_BOLT                = 11,
    EVENT_P1_EMPOWER_CULTIST            = 12,
    EVENT_P1_REANIMATE_CULTIST          = 13,
    EVENT_P2_SUMMON_WAVE                = 14,
    EVENT_P2_FROSTBOLT                  = 15,
    EVENT_P2_FROSTBOLT_VOLLEY           = 16,
    EVENT_P2_TOUCH_OF_INSIGNIFICANCE    = 17,
    EVENT_P2_SUMMON_SHADE               = 18,
    EVENT_CULTIST_DARK_MARTYRDOM        = 19,
    EVENT_FANATIC_NECROTIC_STRIKE       = 20,
    EVENT_FANATIC_SHADOW_CLEAVE         = 21,
    EVENT_FANATIC_VAMPIRIC_MIGHT        = 22,
    EVENT_ADHERENT_FROST_FEVER          = 23,
    EVENT_ADHERENT_DEATHCHILL           = 24,
    EVENT_ADHERENT_CURSE_OF_TORPOR      = 25,
    EVENT_ADHERENT_SHORUD_OF_THE_OCCULT = 26,
    EVENT_DARNAVAN_BLADESTORM           = 27,
    EVENT_DARNAVAN_CHARGE               = 28,
    EVENT_DARNAVAN_INTIMIDATING_SHOUT   = 29,
    EVENT_DARNAVAN_MORTAL_STRIKE        = 30,
    EVENT_DARNAVAN_SHATTERING_THROW     = 31,
    EVENT_DARNAVAN_SUNDER_ARMOR         = 32,
};

enum Phases
{
    PHASE_ALL       = 0,
    PHASE_INTRO     = 1,
    PHASE_ONE       = 2,
	PHASE_TWO       = 3,
};

enum DeprogrammingData
{
	ACTION_COMPLETE_QUEST = -384720,
    POINT_DESPAWN         = 384721,
};

enum Data
{
    DATA_CANCEL_MARTYRDOM_P2          = 1,
};

enum LadyAchievements
{
    ACHIEVEMENT_FULL_HOUSE_10 = 4535,
    ACHIEVEMENT_FULL_HOUSE_25 = 4611,
};

#define ACHIEVEMENT_FULL_HOUSE RAID_MODE<uint32>(ACHIEVEMENT_FULL_HOUSE_10, ACHIEVEMENT_FULL_HOUSE_25, ACHIEVEMENT_FULL_HOUSE_10, ACHIEVEMENT_FULL_HOUSE_25)
#define NPC_DARNAVAN        RAID_MODE<uint32>(NPC_DARNAVAN_10, NPC_DARNAVAN_25, NPC_DARNAVAN_10, NPC_DARNAVAN_25)
#define NPC_DARNAVAN_CREDIT RAID_MODE<uint32>(NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25, NPC_DARNAVAN_CREDIT_10, NPC_DARNAVAN_CREDIT_25)
#define QUEST_DEPROGRAMMING RAID_MODE<uint32>(QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25, QUEST_DEPROGRAMMING_10, QUEST_DEPROGRAMMING_25)

uint32 const SummonEntries[2] = {NPC_CULT_FANATIC, NPC_CULT_ADHERENT};

#define GUID_CULTIST    1

Position const SummonPositions[7] =
{
    {-578.7066f, 2154.167f, 51.01529f, 1.692969f}, // 1 Left Door 1 (Cult Fanatic)
    {-598.9028f, 2155.005f, 51.01530f, 1.692969f}, // 2 Left Door 2 (Cult Adherent)
    {-619.2864f, 2154.460f, 51.01530f, 1.692969f}, // 3 Left Door 3 (Cult Fanatic)
    {-578.6996f, 2269.856f, 51.01529f, 4.590216f}, // 4 Right Door 1 (Cult Adherent)
    {-598.9688f, 2269.264f, 51.01529f, 4.590216f}, // 5 Right Door 2 (Cult Fanatic)
    {-619.4323f, 2268.523f, 51.01530f, 4.590216f}, // 6 Right Door 3 (Cult Adherent)
    {-524.2480f, 2211.920f, 62.90960f, 3.141592f}, // 7 Upper (Random Cultist)
};

class DaranavanMoveEvent : public BasicEvent
{
    public:
        DaranavanMoveEvent(Creature& darnavan) : _darnavan(darnavan) { }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _darnavan.GetMotionMaster()->MovePoint(POINT_DESPAWN, SummonPositions[6]);
            return true;
        }

    private:
        Creature& _darnavan;
};

class boss_lady_deathwhisper : public CreatureScript
{
    public:
        boss_lady_deathwhisper() : CreatureScript("boss_lady_deathwhisper") { }

        struct boss_lady_deathwhisperAI : public BossAI
        {
            boss_lady_deathwhisperAI(Creature* creature) : BossAI(creature, DATA_LADY_DEATHWHISPER), _dominateMindCount(RAID_MODE<uint8>(0, 1, 1, 3)){}

			uint64 _nextVengefulShadeTargetGUID;
            uint64 _darnavanGUID;
            std::deque<uint64> _reanimationQueue;
            uint32 _waveCounter;
            uint8 const _dominateMindCount;
            bool intro;

            void Reset()
            {
                _Reset();
				intro = false;
                me->SetPower(POWER_MANA, me->GetMaxPower(POWER_MANA));
                events.SetPhase(PHASE_ONE);
                _waveCounter = 0;
                _nextVengefulShadeTargetGUID = 0;
                _darnavanGUID = 0;

                DoCast(me, SPELL_SHADOW_CHANNELING);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);

                me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
            }

            void MoveInLineOfSight(Unit* who)
            {
				if (intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 100.0f))
					return;

				if (intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 100.0f))
                {
					DoSendQuantumText(SAY_INTRO_1, me);
                    events.SetPhase(PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_2, 11000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_3, 21000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_4, 29000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_5, 44000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_6, 62000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_7, 71000, 0, PHASE_INTRO);
					intro = true;
                }
            }

            void AttackStart(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (who && me->Attack(who, true) && !events.IsInPhase(PHASE_ONE))
                    me->GetMotionMaster()->MoveChase(who);
            }

            void EnterToBattle(Unit* who)
            {
                if (!instance->CheckRequiredBosses(DATA_LADY_DEATHWHISPER, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                me->SetActive(true);
                DoZoneInCombat();

                events.Reset();
                events.SetPhase(PHASE_ONE);
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_DEATH_AND_DECAY, 10000);
                events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, 5000, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5500, 6000), 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(20000, 30000), 0, PHASE_ONE);
                if (GetDifficulty() != RAID_DIFFICULTY_10MAN_NORMAL)
                    events.ScheduleEvent(EVENT_DOMINATE_MIND_H, 27000);

                DoSendQuantumText(SAY_AGGRO, me);
                DoStartNoMovement(who);
                me->RemoveAurasDueToSpell(SPELL_SHADOW_CHANNELING);
                DoCast(me, SPELL_MANA_BARRIER, true);

                instance->SetBossState(DATA_LADY_DEATHWHISPER, IN_PROGRESS);
            }

            void JustDied(Unit* killer)
            {
                DoSendQuantumText(SAY_DEATH, me);

                std::set<uint32> livingAddEntries;
                // Full House achievement
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
				{
                    if (Unit* unit = ObjectAccessor::GetUnit(*me, *itr))
					{
                        if (unit->IsAlive() && unit->GetEntry() != NPC_VENGEFUL_SHADE)
                            livingAddEntries.insert(unit->GetEntry());
					}
				}

                if (livingAddEntries.size() >= 5)
                    instance->DoCompleteAchievement(ACHIEVEMENT_FULL_HOUSE);

                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    if (darnavan->IsAlive())
                    {
                        darnavan->SetCurrentFaction(35);
                        darnavan->CombatStop(true);
                        darnavan->GetMotionMaster()->MoveIdle();
                        darnavan->SetReactState(REACT_PASSIVE);
                        darnavan->m_Events.AddEvent(new DaranavanMoveEvent(*darnavan), darnavan->m_Events.CalculateTime(10000));
                        darnavan->AI()->Talk(SAY_DARNAVAN_RESCUED);
                        if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
                            if (Group* group = player->GetGroup())
                            {
                                for (GroupReference* itr = group->GetFirstMember(); itr != 0; itr = itr->next())
                                    if (Player* player = itr->getSource())
                                        player->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, 0);
                            }
                            else player->KilledMonsterCredit(NPC_DARNAVAN_CREDIT, 0);
                        }
                    }
                }

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}

                _JustDied();
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                instance->SetBossState(DATA_LADY_DEATHWHISPER, FAIL);

                summons.DespawnAll();
                if (Creature* darnavan = ObjectAccessor::GetCreature(*me, _darnavanGUID))
                {
                    darnavan->DespawnAfterAction();
                    _darnavanGUID = 0;
                }
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void DamageTaken(Unit* /*damageDealer*/, uint32& damage)
            {
                if (events.IsInPhase(PHASE_ONE) && damage > me->GetPower(POWER_MANA))
                {
                    DoSendQuantumText(SAY_PHASE_2, me);
                    DoSendQuantumText(EMOTE_PHASE_2, me);
                    DoStartMovement(me->GetVictim());
                    damage -= me->GetPower(POWER_MANA);
                    me->SetPower(POWER_MANA, 0);
                    me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
					{
						EntryCheckPredicate pred(NPC_CULT_FANATIC);
						summons.DoAction(DATA_CANCEL_MARTYRDOM_P2, pred);
					}

					EntryCheckPredicate pred(NPC_CULT_ADHERENT);
					summons.DoAction(DATA_CANCEL_MARTYRDOM_P2, pred);

                    events.SetPhase(PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10000, 12000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(19000, 21000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(6000, 9000), 0, PHASE_TWO);
                    events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(12000, 15000), 0, PHASE_TWO);
                    if (IsHeroic())
                    {
                        me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                        events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45000, 0, PHASE_TWO);
                    }
                }
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_DARNAVAN)
                    _darnavanGUID = summon->GetGUID();
                else
                    summons.Summon(summon);

                Unit* target = 0;
                if (summon->GetEntry() == NPC_VENGEFUL_SHADE)
                {
                    target = ObjectAccessor::GetUnit(*me, _nextVengefulShadeTargetGUID);   // Vengeful Shade
                    _nextVengefulShadeTargetGUID = 0;
                }
                else
                    target = SelectTarget(TARGET_RANDOM);                        // Wave adds

                summon->AI()->AttackStart(target);                                      // CAN be 0
                if (summon->GetEntry() == NPC_REANIMATED_FANATIC)
                    summon->CastSpell(summon, SPELL_FANATIC_S_DETERMINATION, true);
                else if (summon->GetEntry() == NPC_REANIMATED_ADHERENT)
                    summon->CastSpell(summon, SPELL_ADHERENT_S_DETERMINATION, true);
            }

            void UpdateAI(uint32 const diff)
            {
				if ((!UpdateVictim() && !events.IsInPhase(PHASE_INTRO)) || !CheckInRoom())
                    return;

                events.Update(diff);

				if (me->HasUnitState(UNIT_STATE_CASTING) && !events.IsInPhase(PHASE_INTRO))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_2:
                            DoSendQuantumText(SAY_INTRO_2, me);
                            break;
                        case EVENT_INTRO_3:
                            DoSendQuantumText(SAY_INTRO_3, me);
                            break;
                        case EVENT_INTRO_4:
                            DoSendQuantumText(SAY_INTRO_4, me);
                            break;
                        case EVENT_INTRO_5:
                            DoSendQuantumText(SAY_INTRO_5, me);
                            break;
                        case EVENT_INTRO_6:
                            DoSendQuantumText(SAY_INTRO_6, me);
                            break;
                        case EVENT_INTRO_7:
                            DoSendQuantumText(SAY_INTRO_7, me);
                            break;
                        case EVENT_DEATH_AND_DECAY:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_DEATH_AND_DECAY_10N, SPELL_DEATH_AND_DECAY_25N, SPELL_DEATH_AND_DECAY_10H, SPELL_DEATH_AND_DECAY_25H));
                            events.ScheduleEvent(EVENT_DEATH_AND_DECAY, urand(22000, 30000));
                            break;
                        case EVENT_DOMINATE_MIND_H:
                            DoSendQuantumText(SAY_DOMINATE_MIND, me);
                            for (uint8 i = 0; i < _dominateMindCount; i++)
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, -SPELL_DOMINATE_MIND))
                                    DoCast(target, SPELL_DOMINATE_MIND);
                            events.ScheduleEvent(EVENT_DOMINATE_MIND_H, urand(40000, 45000));
                            break;
                        case EVENT_P1_SUMMON_WAVE:
                            SummonWaveP1();
                            events.ScheduleEvent(EVENT_P1_SUMMON_WAVE, IsHeroic() ? 45000 : 60000, 0, PHASE_ONE);
                            break;
                        case EVENT_P1_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_SHADOW_BOLT_10N, SPELL_SHADOW_BOLT_25N, SPELL_SHADOW_BOLT_10H, SPELL_SHADOW_BOLT_25H));
                            events.ScheduleEvent(EVENT_P1_SHADOW_BOLT, urand(5000, 8000), 0, PHASE_ONE);
                            break;
                        case EVENT_P1_REANIMATE_CULTIST:
                            ReanimateCultist();
                            break;
                        case EVENT_P1_EMPOWER_CULTIST:
                            EmpowerCultist();
                            events.ScheduleEvent(EVENT_P1_EMPOWER_CULTIST, urand(18000, 25000), 0, PHASE_ONE);
                            break;
                        case EVENT_P2_FROSTBOLT:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_FROSTBOLT_10N, SPELL_FROSTBOLT_25N, SPELL_FROSTBOLT_10H, SPELL_FROSTBOLT_25H));
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT, urand(10000, 11000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_FROSTBOLT_VOLLEY:
                            DoCastAOE(RAID_MODE(SPELL_FROSTBOLT_VOLLEY_10N, SPELL_FROSTBOLT_VOLLEY_25N, SPELL_FROSTBOLT_VOLLEY_10H, SPELL_FROSTBOLT_VOLLEY_25H));
                            events.ScheduleEvent(EVENT_P2_FROSTBOLT_VOLLEY, urand(13000, 15000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_TOUCH_OF_INSIGNIFICANCE:
                            DoCastVictim(SPELL_TOUCH_OF_INSIGNIFICANCE);
                            events.ScheduleEvent(EVENT_P2_TOUCH_OF_INSIGNIFICANCE, urand(9000, 13000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_SHADE:
                            if (Unit* shadeTarget = SelectTarget(TARGET_RANDOM, 1))
                            {
                                _nextVengefulShadeTargetGUID = shadeTarget->GetGUID();
                                DoCast(shadeTarget, SPELL_SUMMON_SHADE);
                            }
                            events.ScheduleEvent(EVENT_P2_SUMMON_SHADE, urand(18000, 23000), 0, PHASE_TWO);
                            break;
                        case EVENT_P2_SUMMON_WAVE:
                            SummonWaveP2();
                            events.ScheduleEvent(EVENT_P2_SUMMON_WAVE, 45000, 0, PHASE_TWO);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                    }
                }

                if (me->HasAura(SPELL_MANA_BARRIER))
                    return;

                DoMeleeAttackIfReady();
            }

            void SummonWaveP1()
            {
                uint8 addIndex = _waveCounter & 1;
                uint8 addIndexOther = uint8(addIndex ^ 1);

                if (_waveCounter)
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                else
                    Summon(NPC_DARNAVAN, SummonPositions[addIndex * 3]);

                Summon(SummonEntries[addIndexOther], SummonPositions[addIndex * 3 + 1]);
                Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3 + 2]);
                if (Is25ManRaid())
                {
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndexOther * 3 + 1]);
                    Summon(SummonEntries[addIndexOther], SummonPositions[addIndexOther * 3 + 2]);
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);
                }

                ++_waveCounter;
            }

            void SummonWaveP2()
            {
                if (Is25ManRaid())
                {
                    uint8 addIndex = _waveCounter & 1;
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3]);
                    Summon(SummonEntries[addIndex ^ 1], SummonPositions[addIndex * 3 + 1]);
                    Summon(SummonEntries[addIndex], SummonPositions[addIndex * 3+ 2]);
                }
                else
                    Summon(SummonEntries[urand(0, 1)], SummonPositions[6]);

                ++_waveCounter;
            }

            void Summon(uint32 entry, const Position& pos)
            {
                if (TempSummon* summon = me->SummonCreature(entry, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                    summon->AI()->DoCast(summon, SPELL_TELEPORT_VISUAL);
            }

            void SetGUID(uint64 guid, int32 id/* = 0*/)
            {
                if (id != GUID_CULTIST)
                    return;

                _reanimationQueue.push_back(guid);
                events.ScheduleEvent(EVENT_P1_REANIMATE_CULTIST, 3000, 0, PHASE_ONE);
            }

            void ReanimateCultist()
            {
                if (_reanimationQueue.empty())
                    return;

                uint64 cultistGUID = _reanimationQueue.front();
                Creature* cultist = ObjectAccessor::GetCreature(*me, cultistGUID);
                _reanimationQueue.pop_front();
                if (!cultist)
                    return;

                DoSendQuantumText(SAY_ANIMATE_DEAD, me);
                if (cultist->IsDead())
                    cultist->Respawn();

                DoCast(cultist, SPELL_DARK_MARTYRDOM_T);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_DARK_MARTYRDOM_T)
                {
                    Position pos;
                    target->GetPosition(&pos);
                    if (target->GetEntry() == NPC_CULT_FANATIC)
                        me->SummonCreature(NPC_REANIMATED_FANATIC, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    else
                        me->SummonCreature(NPC_REANIMATED_ADHERENT, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);

                    if (TempSummon* summon = target->ToTempSummon())
                        summon->UnSummon();
                }
            }

            void EmpowerCultist()
            {
                if (summons.empty())
                    return;

                std::list<Creature*> temp;
                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, *itr))
                        if (creature->IsAlive() && (creature->GetEntry() == NPC_CULT_FANATIC || creature->GetEntry() == NPC_CULT_ADHERENT))
                            temp.push_back(creature);

                // noone to empower
                if (temp.empty())
                    return;

                // select random cultist
                Creature* cultist = Quantum::DataPackets::SelectRandomContainerElement(temp);
                DoCast(cultist, cultist->GetEntry() == NPC_CULT_FANATIC ? SPELL_DARK_TRANSFORMATION_T : SPELL_DARK_EMPOWERMENT_T, true);
                DoSendQuantumText(cultist->GetEntry() == NPC_CULT_FANATIC ? SAY_DARK_TRANSFORMATION : SAY_DARK_EMPOWERMENT, me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_lady_deathwhisperAI>(creature);
        }
};

typedef boss_lady_deathwhisper::boss_lady_deathwhisperAI DeathwisperAI;

class npc_cult_fanatic : public CreatureScript
{
    public:
        npc_cult_fanatic() : CreatureScript("npc_cult_fanatic") { }

        struct npc_cult_fanaticAI : public QuantumBasicAI
        {
            npc_cult_fanaticAI(Creature* creature) : QuantumBasicAI(creature) {}

			EventMap Events;

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(10000, 12000));
                Events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20000, 27000));
                if (me->GetEntry() == NPC_CULT_FANATIC)
                    Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, urand(18000, 32000));
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_DARK_TRANSFORMATION)
                    me->UpdateEntry(NPC_DEFORMED_FANATIC);
                else if (spell->Id == SPELL_DARK_TRANSFORMATION_T)
                {
                    Events.CancelEvent(EVENT_CULTIST_DARK_MARTYRDOM);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(me, SPELL_DARK_TRANSFORMATION);
                }
            }

            void DoAction(int32 const action)
            {
                if (action == DATA_CANCEL_MARTYRDOM_P2)
                    Events.CancelEvent(EVENT_CULTIST_DARK_MARTYRDOM);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FANATIC_NECROTIC_STRIKE:
                            DoCastVictim(RAID_MODE(SPELL_NECROTIC_STRIKE_10N, SPELL_NECROTIC_STRIKE_25N, SPELL_NECROTIC_STRIKE_10H, SPELL_NECROTIC_STRIKE_25H));
                            Events.ScheduleEvent(EVENT_FANATIC_NECROTIC_STRIKE, urand(11000, 13000));
                            break;
                        case EVENT_FANATIC_SHADOW_CLEAVE:
                            DoCastVictim(RAID_MODE(SPELL_SHADOW_CLEAVE_10N, SPELL_SHADOW_CLEAVE_25N, SPELL_SHADOW_CLEAVE_10H, SPELL_SHADOW_CLEAVE_25H));
                            Events.ScheduleEvent(EVENT_FANATIC_SHADOW_CLEAVE, urand(9500, 11000));
                            break;
                        case EVENT_FANATIC_VAMPIRIC_MIGHT:
                            DoCast(me, SPELL_VAMPIRIC_MIGHT);
                            Events.ScheduleEvent(EVENT_FANATIC_VAMPIRIC_MIGHT, urand(20000, 27000));
                            break;
                        case EVENT_CULTIST_DARK_MARTYRDOM:
                            DoCast(me, SPELL_DARK_MARTYRDOM_FANATIC);
                            Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, urand(16000, 21000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_cult_fanaticAI>(creature);
        }
};

class npc_cult_adherent : public CreatureScript
{
    public:
        npc_cult_adherent() : CreatureScript("npc_cult_adherent") { }

        struct npc_cult_adherentAI : public QuantumBasicAI
        {
            npc_cult_adherentAI(Creature* creature) : QuantumBasicAI(creature) {}

            void Reset()
            {
                Events.Reset();
                Events.ScheduleEvent(EVENT_ADHERENT_FROST_FEVER, urand(10000, 12000));
                Events.ScheduleEvent(EVENT_ADHERENT_DEATHCHILL, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(14000, 16000));
                Events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(32000, 39000));
                if (me->GetEntry() == NPC_CULT_ADHERENT)
                    Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, urand(18000, 32000));
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_DARK_EMPOWERMENT)
                    me->UpdateEntry(NPC_EMPOWERED_ADHERENT);
                else if (spell->Id == SPELL_DARK_EMPOWERMENT_T)
                {
                    Events.CancelEvent(EVENT_CULTIST_DARK_MARTYRDOM);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(me, SPELL_DARK_EMPOWERMENT);
                }
            }

            void DoAction(int32 const action)
            {
                if (action == DATA_CANCEL_MARTYRDOM_P2)
                    Events.CancelEvent(EVENT_CULTIST_DARK_MARTYRDOM);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ADHERENT_FROST_FEVER:
                            DoCastVictim(SPELL_FROST_FEVER);
                            Events.ScheduleEvent(EVENT_ADHERENT_FROST_FEVER, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_DEATHCHILL:
                            if (me->GetEntry() == NPC_EMPOWERED_ADHERENT)
                                DoCastVictim(RAID_MODE(SPELL_DEATHCHILL_BLAST_10N, SPELL_DEATHCHILL_BLAST_25N, SPELL_DEATHCHILL_BLAST_10H, SPELL_DEATHCHILL_BLAST_25H));
                            else
                                DoCastVictim(RAID_MODE(SPELL_DEATHCHILL_BOLT_10N, SPELL_DEATHCHILL_BOLT_25N, SPELL_DEATHCHILL_BOLT_10H, SPELL_DEATHCHILL_BOLT_25H));
                            Events.ScheduleEvent(EVENT_ADHERENT_DEATHCHILL, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_CURSE_OF_TORPOR:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                                DoCast(target, SPELL_CURSE_OF_TORPOR);
                            Events.ScheduleEvent(EVENT_ADHERENT_CURSE_OF_TORPOR, urand(9000, 13000));
                            break;
                        case EVENT_ADHERENT_SHORUD_OF_THE_OCCULT:
                            DoCast(me, SPELL_SHORUD_OF_THE_OCCULT);
                            Events.ScheduleEvent(EVENT_ADHERENT_SHORUD_OF_THE_OCCULT, urand(27000, 32000));
                            break;
                        case EVENT_CULTIST_DARK_MARTYRDOM:
                            DoCast(me, SPELL_DARK_MARTYRDOM_ADHERENT);
                            Events.ScheduleEvent(EVENT_CULTIST_DARK_MARTYRDOM, urand(16000, 21000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        protected:
            EventMap Events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_cult_adherentAI>(creature);
        }
};

class npc_vengeful_shade : public CreatureScript
{
    public:
        npc_vengeful_shade() : CreatureScript("npc_vengeful_shade") { }

        struct npc_vengeful_shadeAI : public QuantumBasicAI
        {
            npc_vengeful_shadeAI(Creature* creature) : QuantumBasicAI(creature)
            {
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset()
            {
                me->AddAura(SPELL_VENGEFUL_BLAST_PASSIVE, me);
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_VENGEFUL_BLAST_10N:
                    case SPELL_VENGEFUL_BLAST_25N:
                    case SPELL_VENGEFUL_BLAST_10H:
                    case SPELL_VENGEFUL_BLAST_25H:
                        me->Kill(me);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_vengeful_shadeAI>(creature);
        }
};

class npc_darnavan : public CreatureScript
{
    public:
        npc_darnavan() : CreatureScript("npc_darnavan") { }

        struct npc_darnavanAI : public QuantumBasicAI
        {
            npc_darnavanAI(Creature* creature) : QuantumBasicAI(creature){}

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, 10000);
                _events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(20000, 25000));
                _events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(25000, 30000));
                _events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(5000, 8000));
                _canCharge = true;
                _canShatter = true;
            }

            void JustDied(Unit* killer)
            {
                _events.Reset();

                if (Player* owner = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (Group* group = owner->GetGroup())
                    {
                        for (GroupReference* itr = group->GetFirstMember(); itr != 0; itr = itr->next())
                            if (Player* member = itr->getSource())
                                member->FailQuest(QUEST_DEPROGRAMMING);
                    }
                    else
                        owner->FailQuest(QUEST_DEPROGRAMMING);
                }

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != POINT_DESPAWN)
                    return;

                me->DespawnAfterAction();
            }

            void EnterToBattle(Unit* /*victim*/)
            {
                Talk(SAY_DARNAVAN_AGGRO);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (_canShatter && me->GetVictim() && me->GetVictim()->IsImmunedToDamage(SPELL_SCHOOL_MASK_NORMAL))
                {
                    DoCastVictim(SPELL_SHATTERING_THROW);
                    _canShatter = false;
                    _events.ScheduleEvent(EVENT_DARNAVAN_SHATTERING_THROW, 30000);
                    return;
                }

                if (_canCharge && !me->IsWithinMeleeRange(me->GetVictim()))
                {
                    DoCastVictim(SPELL_CHARGE);
                    _canCharge = false;
                    _events.ScheduleEvent(EVENT_DARNAVAN_CHARGE, 20000);
                    return;
                }

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DARNAVAN_BLADESTORM:
                            DoCast(SPELL_BLADESTORM);
                            _events.ScheduleEvent(EVENT_DARNAVAN_BLADESTORM, urand(90000, 100000));
                            break;
                        case EVENT_DARNAVAN_CHARGE:
                            _canCharge = true;
                            break;
                        case EVENT_DARNAVAN_INTIMIDATING_SHOUT:
                            DoCast(SPELL_INTIMIDATING_SHOUT);
                            _events.ScheduleEvent(EVENT_DARNAVAN_INTIMIDATING_SHOUT, urand(90000, 120000));
                            break;
                        case EVENT_DARNAVAN_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            _events.ScheduleEvent(EVENT_DARNAVAN_MORTAL_STRIKE, urand(15000, 30000));
                            break;
                        case EVENT_DARNAVAN_SHATTERING_THROW:
                            _canShatter = true;
                            break;
                        case EVENT_DARNAVAN_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_DARNAVAN_SUNDER_ARMOR, urand(3000, 7000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            bool _canCharge;
            bool _canShatter;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_darnavanAI>(creature);
        }
};

class spell_deathwhisper_mana_barrier : public SpellScriptLoader
{
    public:
        spell_deathwhisper_mana_barrier() : SpellScriptLoader("spell_deathwhisper_mana_barrier") { }

        class spell_deathwhisper_mana_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathwhisper_mana_barrier_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                {
                    int32 missingHealth = int32(caster->GetMaxHealth() - caster->GetHealth());
                    caster->ModifyHealth(missingHealth);
                    caster->ModifyPower(POWER_MANA, -missingHealth);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathwhisper_mana_barrier_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_deathwhisper_mana_barrier_AuraScript();
        }
};

class spell_cultist_dark_martyrdom : public SpellScriptLoader
{
    public:
        spell_cultist_dark_martyrdom() : SpellScriptLoader("spell_cultist_dark_martyrdom") { }

        class spell_cultist_dark_martyrdom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cultist_dark_martyrdom_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->IsSummon())
				{
                    if (Unit* owner = GetCaster()->ToTempSummon()->GetSummoner())
                        owner->GetAI()->SetGUID(GetCaster()->GetGUID(), GUID_CULTIST);
				}

                GetCaster()->Kill(GetCaster());
                GetCaster()->SetDisplayId(uint32(GetCaster()->GetEntry() == NPC_CULT_FANATIC ? NPC_REANIMATED_FANATIC : NPC_REANIMATED_ADHERENT));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_cultist_dark_martyrdom_SpellScript::HandleEffect, EFFECT_2, SPELL_EFFECT_FORCE_DESELECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cultist_dark_martyrdom_SpellScript();
        }
};

void AddSC_boss_lady_deathwhisper()
{
    new boss_lady_deathwhisper();
    new npc_cult_fanatic();
    new npc_cult_adherent();
    new npc_vengeful_shade();
    new npc_darnavan();
    new spell_deathwhisper_mana_barrier();
    new spell_cultist_dark_martyrdom();
}