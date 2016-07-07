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
#include "Spell.h"
#include "black_temple.h"

enum Says
{
	SAY_GATHIOS_SLAY          = -1564085,
	SAY_GATHIOS_SLAY_COMNT    = -1564089,
	SAY_GATHIOS_DEATH         = -1564093,
	SAY_GATHIOS_SPECIAL1      = -1564077,
	SAY_GATHIOS_SPECIAL2      = -1564081,

	SAY_VERAS_SLAY            = -1564086,
	SAY_VERAS_COMNT           = -1564089,
	SAY_VERAS_DEATH           = -1564094,
	SAY_VERAS_SPECIAL1        = -1564078,
	SAY_VERAS_SPECIAL2        = -1564082,

	SAY_MALANDE_SLAY          = -1564087,
	SAY_MALANDE_COMNT         = -1564090,
	SAY_MALANDE_DEATH         = -1564095,
	SAY_MALANDE_SPECIAL1      = -1564079,
	SAY_MALANDE_SPECIAL2      = -1564083,

	SAY_ZEREVOR_SLAY          = -1564088,
	SAY_ZEREVOR_COMNT         = -1564091,
	SAY_ZEREVOR_DEATH         = -1564096,
	SAY_ZEREVOR_SPECIAL1      = -1564080,
	SAY_ZEREVOR_SPECIAL2      = -1564084,
};

enum Spells
{
	// High Nethermancer Zerevor's spells
	SPELL_FLAMESTRIKE           = 41481,
	SPELL_BLIZZARD              = 41482,
	SPELL_ARCANE_BOLT           = 41483,
	SPELL_ARCANE_EXPLOSION      = 41524,
	SPELL_DAMPEN_MAGIC          = 41478,
	// Lady Malande's spells
	SPELL_EMPOWERED_SMITE       = 41471,
	SPELL_CIRCLE_OF_HEALING     = 41455,
	SPELL_REFLECTIVE_SHIELD     = 41475,
	SPELL_REFLECTIVE_SHIELD_T   = 33619,
	SPELL_DIVINE_WRATH          = 41472,
	SPELL_HEAL_VISUAL           = 24171,
	// Gathios the Shatterer's spells
	SPELL_BLESS_PROTECTION      = 41450,
	SPELL_BLESS_SPELLWARD       = 41451,
	SPELL_CONSECRATION          = 41541,
	SPELL_HAMMER_OF_JUSTICE     = 41468,
	SPELL_SEAL_OF_COMMAND       = 41469,
	SPELL_JUDGEMENT_OF_COMMAND  = 41470,
	SPELL_SEAL_OF_BLOOD         = 41459,
	SPELL_JUDGEMENT_OF_BLOOD    = 41461,
	SPELL_CHROMATIC_AURA        = 41453,
	SPELL_DEVOTION_AURA         = 41452,
	// Veras Darkshadow's spells
	SPELL_DEADLY_POISON         = 41485,
	SPELL_DEADLY_POISON_TRIGGER = 41480,
	SPELL_ENVENOM               = 41487,
	SPELL_VANISH                = 41476,
	SPELL_BERSERK               = 45078,
};

struct CouncilYells
{
    int32 entry;
    uint32 timer;
};

static CouncilYells CouncilAggro[]=
{
    {-1564069, 5000},                                       // Gathios
    {-1564070, 5500},                                       // Veras
    {-1564071, 5000},                                       // Malande
    {-1564072, 0},                                          // Zerevor
};

// Need to get proper timers for this later
static CouncilYells CouncilEnrage[]=
{
    {-1564073, 2000},                                       // Gathios
    {-1564074, 6000},                                       // Veras
    {-1564075, 5000},                                       // Malande
    {-1564076, 0},                                          // Zerevor
};

class mob_blood_elf_council_voice_trigger : public CreatureScript
{
public:
    mob_blood_elf_council_voice_trigger() : CreatureScript("mob_blood_elf_council_voice_trigger") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_blood_elf_council_voice_triggerAI(creature);
    }

    struct mob_blood_elf_council_voice_triggerAI : public QuantumBasicAI
    {
        mob_blood_elf_council_voice_triggerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            for(uint8 i = 0; i < 4; ++i)
                Council[i] = 0;
        }

        uint64 Council[4];

        uint32 EnrageTimer;
        uint32 AggroYellTimer;

        uint8 YellCounter;                                      // Serves as the counter for both the aggro and enrage yells

        bool EventStarted;

        void Reset()
        {
            EnrageTimer = 900000;                               // 15 minutes
            AggroYellTimer = 500;

            YellCounter = 0;

            EventStarted = false;
        }

        // finds and stores the GUIDs for each Council member using instance data system.
        void LoadCouncilGUIDs()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
            {
                Council[0] = instance->GetData64(DATA_GATHIOSTHESHATTERER);
                Council[1] = instance->GetData64(DATA_VERASDARKSHADOW);
                Council[2] = instance->GetData64(DATA_LADYMALANDE);
                Council[3] = instance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!EventStarted)
                return;

            if (YellCounter > 3)
                return;

            if (AggroYellTimer)
            {
                if (AggroYellTimer <= diff)
            {
                if (Unit* member = Unit::GetUnit(*me, Council[YellCounter]))
                {
                    DoSendQuantumText(CouncilAggro[YellCounter].entry, member);
                    AggroYellTimer = CouncilAggro[YellCounter].timer;
                }
                ++YellCounter;
                if (YellCounter > 3)
                    YellCounter = 0;                            // Reuse for Enrage Yells
            }
				else AggroYellTimer -= diff;
            }

            if (EnrageTimer)
            {
                if (EnrageTimer <= diff)
            {
                if (Unit* member = Unit::GetUnit(*me, Council[YellCounter]))
                {
                    member->CastSpell(member, SPELL_BERSERK, true);
                    DoSendQuantumText(CouncilEnrage[YellCounter].entry, member);
                    EnrageTimer = CouncilEnrage[YellCounter].timer;
                }
				++YellCounter;
				}
				else EnrageTimer -= diff;
			}
		}
    };
};

class mob_illidari_council : public CreatureScript
{
public:
    mob_illidari_council() : CreatureScript("mob_illidari_council") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_illidari_councilAI(creature);
    }

    struct mob_illidari_councilAI : public QuantumBasicAI
    {
        mob_illidari_councilAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            for(uint8 i = 0; i < 4; ++i)
                Council[i] = 0;
        }

        InstanceScript* instance;

        uint64 Council[4];

        uint32 CheckTimer;
        uint32 EndEventTimer;

        uint8 DeathCount;

        bool EventBegun;

        void Reset()
        {
            CheckTimer    = 2000;
            EndEventTimer = 0;

            DeathCount = 0;

            Creature* member = NULL;
            for(uint8 i = 0; i < 4; ++i)
            {
                if (member = (Unit::GetCreature(*me, Council[i])))
                {
                    if (!member->IsAlive())
                    {
                        member->RemoveCorpse();
                        member->Respawn();
                    }
                    member->AI()->EnterEvadeMode();
                }
            }

            if (instance)
            {
                instance->SetData(DATA_ILLIDARICOUNCILEVENT, NOT_STARTED);
                if (Creature* VoiceTrigger = (Unit::GetCreature(*me, instance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE))))
                    VoiceTrigger->AI()->EnterEvadeMode();
            }

            EventBegun = false;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetDisplayId(MODEL_ID_INVISIBLE);
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void StartEvent(Unit* target)
        {
            if (!instance)
				return;

            if (target && target->IsAlive())
            {
                Council[0] = instance->GetData64(DATA_GATHIOSTHESHATTERER);
                Council[1] = instance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
                Council[2] = instance->GetData64(DATA_LADYMALANDE);
                Council[3] = instance->GetData64(DATA_VERASDARKSHADOW);

                // Start the event for the Voice Trigger
                if (Creature* VoiceTrigger = (Unit::GetCreature(*me, instance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE))))
                {
                    CAST_AI(mob_blood_elf_council_voice_trigger::mob_blood_elf_council_voice_triggerAI,VoiceTrigger->AI())->LoadCouncilGUIDs();
                    CAST_AI(mob_blood_elf_council_voice_trigger::mob_blood_elf_council_voice_triggerAI,VoiceTrigger->AI())->EventStarted = true;
                }

                for(uint8 i = 0; i < 4; ++i)
                {
                    Unit* Member = NULL;
                    if (Council[i])
                    {
                        Member = Unit::GetUnit(*me, Council[i]);

                        if (Member && Member->IsAlive())
                            ((Creature*)Member)->AI()->AttackStart(target);
                    }
                }

                instance->SetData(DATA_ILLIDARICOUNCILEVENT, IN_PROGRESS);
                EventBegun = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!EventBegun)
				return;

            if (EndEventTimer)
            {
                if (EndEventTimer <= diff)
                {
                    if (DeathCount > 3)
                    {
                        if (instance)
                        {
                            if (Creature* VoiceTrigger = (Unit::GetCreature(*me, instance->GetData64(DATA_BLOOD_ELF_COUNCIL_VOICE))))
                                VoiceTrigger->DealDamage(VoiceTrigger, VoiceTrigger->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                            instance->SetData(DATA_ILLIDARICOUNCILEVENT, DONE);
                        }
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        return;
                    }

                    Creature* member = (Unit::GetCreature(*me, Council[DeathCount]));
                    if (member && member->IsAlive())
					{
                        member->DealDamage(member, member->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
					}
                    ++DeathCount;
                    EndEventTimer = 1500;
                }
				else EndEventTimer -= diff;
            }

            if (CheckTimer)
            {
                if (CheckTimer <= diff)
                {
                    uint8 EvadeCheck = 0;
                    for(uint8 i = 0; i < 4; ++i)
                    {
                        if (Council[i])
                        {
                            if (Creature* Member = (Unit::GetCreature(*me, Council[i])))
                            {
                                // This is the evade/death check.
                                if (Member->IsAlive() && !Member->GetVictim())
								{
                                    ++EvadeCheck; // If all members evade, we reset so that players can properly reset the event
								}
                                else
									if (!Member->IsAlive()) // If even one member dies, kill the rest, set instance data, and kill self.
									{
										EndEventTimer = 1000;
										CheckTimer = 0;
										return;
									}
							}
						}
					}
					if (EvadeCheck > 3)
					{
						Reset();
					}

                    CheckTimer = 2000;
                }
				else CheckTimer -= diff;
            }
        }
    };
};

struct boss_illidari_councilAI : public QuantumBasicAI
{
    boss_illidari_councilAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();

        for (uint8 i = 0; i < 4; ++i)
            Council[i] = 0;
        LoadedGUIDs = false;
    }

    uint64 Council[4];

    uint32 pulseTimer;

    InstanceScript* instance;

    bool LoadedGUIDs;

    void EnterToBattle(Unit* who)
    {
        if (instance)
        {
            Creature* Controller = (Unit::GetCreature(*me, instance->GetData64(DATA_ILLIDARICOUNCIL)));
            if (Controller)
                CAST_AI(mob_illidari_council::mob_illidari_councilAI,Controller->AI())->StartEvent(who);
        }
        else
        {
            EnterEvadeMode();
            return;
        }
        DoZoneInCombat();

        if (!LoadedGUIDs)
            LoadGUIDs();
    }

    void EnterEvadeMode()
    {
        for(uint8 i = 0; i < 4; ++i)
        {
            if (Unit* unit = Unit::GetUnit(*me, Council[i]))
			{
                if (unit != me && unit->GetVictim())
                {
                    AttackStart(unit->GetVictim());
                    return;
                }
			}
        }
        QuantumBasicAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* DoneBy, uint32 &damage)
    {
        if (DoneBy == me)
            return;

        damage /= 4;
        for (uint8 i = 0; i < 4; ++i)
        {
            if (Creature* unit = Unit::GetCreature(*me, Council[i]))
			{
                if (unit != me && unit->IsAlive())
                {
                    if (damage <= unit->GetHealth())
                    {
                        unit->LowerPlayerDamageReq(damage);
                        unit->SetHealth(unit->GetHealth() - damage);
                    }
                    else
                    {
                        unit->LowerPlayerDamageReq(damage);
                        unit->Kill(unit, false);
                    }
				}
			}
		}
	}

    void LoadGUIDs()
    {
        if (!instance)
			return;

        Council[0] = instance->GetData64(DATA_LADYMALANDE);
        Council[1] = instance->GetData64(DATA_HIGHNETHERMANCERZEREVOR);
        Council[2] = instance->GetData64(DATA_GATHIOSTHESHATTERER);
        Council[3] = instance->GetData64(DATA_VERASDARKSHADOW);

        LoadedGUIDs = true;
    }
};

class boss_gathios_the_shatterer : public CreatureScript
{
public:
    boss_gathios_the_shatterer() : CreatureScript("boss_gathios_the_shatterer") { }

    struct boss_gathios_the_shattererAI : public boss_illidari_councilAI
    {
        boss_gathios_the_shattererAI(Creature* creature) : boss_illidari_councilAI(creature) {}

        uint32 ConsecrationTimer;
        uint32 HammerOfJusticeTimer;
        uint32 SealTimer;
        uint32 AuraTimer;
        uint32 BlessingTimer;
        uint32 JudgeTimer;

        void Reset()
        {
            ConsecrationTimer = 40000;
            HammerOfJusticeTimer = 10000;
            SealTimer = 40000;
            AuraTimer = 90000;
            BlessingTimer = 60000;
            JudgeTimer = 45000;

            pulseTimer = 10000;
        }

        void KilledUnit(Unit *victim)
        {
            DoSendQuantumText(SAY_GATHIOS_SLAY, me);
        }

        void JustDied(Unit *victim)
        {
            DoSendQuantumText(SAY_GATHIOS_DEATH, me);
        }

        Unit* SelectCouncilMember()
        {
            Unit* unit = me;
            uint32 member = 0;                                  // He chooses Lady Malande most often

            if (rand()%10 == 0)                                  // But there is a chance he picks someone else.
                member = urand(1, 3);

            if (member != 2)                                     // No need to create another pointer to us using Unit::GetUnit
                unit = Unit::GetUnit(*me, Council[member]);
            return unit;
        }

        void CastAuraOnCouncil()
        {
            uint32 spellid = 0;
            switch(rand()%2)
            {
                case 0: spellid = SPELL_DEVOTION_AURA;
					break;
                case 1: spellid = SPELL_CHROMATIC_AURA;
					break;
            }
            for(uint8 i = 0; i < 4; ++i)
            {
                Unit* unit = Unit::GetUnit(*me, Council[i]);
                if (unit)
                    unit->CastSpell(unit, spellid, true, 0, 0, me->GetGUID());
            }
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

            if (BlessingTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* unit = SelectCouncilMember())
                    {
						switch(rand()%2)
                        {
                            case 0: DoCast(unit, SPELL_BLESS_SPELLWARD);
								break;
                            case 1: DoCast(unit, SPELL_BLESS_PROTECTION);
								break;
                        }
                    }
                    BlessingTimer = 15000;
                }
            }
			else BlessingTimer -= diff;

            if (JudgeTimer <= diff)
            {
               if (!me->IsNonMeleeSpellCasted(false))
               {
                    if (me->HasAura(SPELL_SEAL_OF_COMMAND, 0))
                    {
						DoCastVictim(SPELL_JUDGEMENT_OF_COMMAND);
						me->RemoveAurasDueToSpell(SPELL_SEAL_OF_COMMAND);
						JudgeTimer = 45000;
                    }
                    if (me->HasAura(SPELL_SEAL_OF_BLOOD,0))
                    {
						DoCastVictim(SPELL_JUDGEMENT_OF_BLOOD);
						me->RemoveAurasDueToSpell(SPELL_SEAL_OF_BLOOD);
						JudgeTimer = 45000;
					}
                    JudgeTimer = 15000;
               }
            }
			else JudgeTimer -= diff;

            if (ConsecrationTimer <= diff)
            {
				DoCast(me, SPELL_CONSECRATION);
				ConsecrationTimer = 30000;
            }
			else ConsecrationTimer -= diff;

            if (HammerOfJusticeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
                {
                    // is in ~10-40 yd range
                    if (me->GetDistance2d(target) > 10)
                    {
						DoCast(target, SPELL_HAMMER_OF_JUSTICE);
						HammerOfJusticeTimer = 20000;
                    }
                }
            }
			else HammerOfJusticeTimer -= diff;

            if (SealTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    switch(rand()%2)
                    {
                        case 0: DoCast(me, SPELL_SEAL_OF_COMMAND);
							break;
                        case 1: DoCast(me, SPELL_SEAL_OF_BLOOD);
							break;
                    }
                    SealTimer = 30000;
                }
            }
			else SealTimer -= diff;

            if (AuraTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    CastAuraOnCouncil();
                    AuraTimer = 60000;
                }
            }
			else AuraTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gathios_the_shattererAI(creature);
    }
};

class boss_high_nethermancer_zerevor : public CreatureScript
{
public:
    boss_high_nethermancer_zerevor() : CreatureScript("boss_high_nethermancer_zerevor") { }

    struct boss_high_nethermancer_zerevorAI : public boss_illidari_councilAI
    {
        boss_high_nethermancer_zerevorAI(Creature* creature) : boss_illidari_councilAI(creature) {}

        uint32 BlizzardTimer;
        uint32 FlamestrikeTimer;
        uint32 ArcaneBoltTimer;
        uint32 DampenMagicTimer;
        uint32 ArcaneExplosionTimer;

        void Reset()
        {
            BlizzardTimer = urand(30, 91) * 1000;
            FlamestrikeTimer = urand(30, 91) * 1000;
            ArcaneBoltTimer = 500;
            DampenMagicTimer = 200;
            ArcaneExplosionTimer = 14000;

            pulseTimer = 10000;
        }

        void KilledUnit(Unit *victim)
        {
            DoSendQuantumText(SAY_ZEREVOR_SLAY, me);
        }

        void JustDied(Unit *victim)
        {
            DoSendQuantumText(SAY_ZEREVOR_DEATH, me);
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

            if (DampenMagicTimer <= diff)
            {
               if (!me->IsNonMeleeSpellCasted(false))
               {
                    DoCast(me, SPELL_DAMPEN_MAGIC);
                    DampenMagicTimer = 30000;
                    ArcaneBoltTimer += 2000;                        // Give the Mage some time to spellsteal Dampen.
               }
            }
			else DampenMagicTimer -= diff;

            if (ArcaneExplosionTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (me->GetDistance2d(me->GetVictim()) <= 5)
                    {
                        DoCastVictim(SPELL_ARCANE_EXPLOSION);
                        ArcaneExplosionTimer = 14000;
                    }
                }
            }
			else ArcaneExplosionTimer -= diff;

            if (BlizzardTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_BLIZZARD);
                        BlizzardTimer = 30000 + rand()%15 * 1000;
                    }
                }
            }
			else BlizzardTimer -= diff;

            if (FlamestrikeTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_FLAMESTRIKE);
                        FlamestrikeTimer = 30000 + rand()%15 * 1000;
                    }
                }
            }
			else FlamestrikeTimer -= diff;

            if (ArcaneBoltTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_ARCANE_BOLT);
                    ArcaneBoltTimer = 2000;
                }
            }
			else ArcaneBoltTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_nethermancer_zerevorAI(creature);
    }
};

class boss_lady_malande : public CreatureScript
{
public:
    boss_lady_malande() : CreatureScript("boss_lady_malande") { }

    struct boss_lady_malandeAI : public boss_illidari_councilAI
    {
        boss_lady_malandeAI(Creature* creature) : boss_illidari_councilAI(creature) {}

        uint32 EmpoweredSmiteTimer;
        uint32 CircleOfHealingTimer;
        uint32 DivineWrathTimer;
        uint32 ReflectiveShieldTimer;

        void Reset()
        {
            EmpoweredSmiteTimer = 18000;
            CircleOfHealingTimer = 20000;
            DivineWrathTimer = 40000;
            ReflectiveShieldTimer = 15000;

            pulseTimer = 10000;
        }

        void KilledUnit(Unit *victim)
        {
            DoSendQuantumText(SAY_MALANDE_SLAY, me);
        }

        void JustDied(Unit *victim)
        {
            DoSendQuantumText(SAY_MALANDE_DEATH, me);
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

            if (EmpoweredSmiteTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    DoCastVictim(SPELL_EMPOWERED_SMITE);
                    EmpoweredSmiteTimer = 30000;
                }
            }
			else EmpoweredSmiteTimer -= diff;

            if (CircleOfHealingTimer <= diff)
            {
				DoCast(me, SPELL_CIRCLE_OF_HEALING);
				CircleOfHealingTimer = 30000;
            }
			else CircleOfHealingTimer -= diff;

            if (DivineWrathTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
					DoCast(target, SPELL_DIVINE_WRATH);
					DivineWrathTimer = 20000 + rand()%20 * 1000;
                }
            }
			else DivineWrathTimer -= diff;

            if (ReflectiveShieldTimer <= diff)
            {
				DoCast(me, SPELL_REFLECTIVE_SHIELD);
				ReflectiveShieldTimer = 45000;
            }
			else ReflectiveShieldTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lady_malandeAI(creature);
    }
};

class boss_veras_darkshadow : public CreatureScript
{
public:
    boss_veras_darkshadow() : CreatureScript("boss_veras_darkshadow") { }

    struct boss_veras_darkshadowAI : public boss_illidari_councilAI
    {
        boss_veras_darkshadowAI(Creature* creature) : boss_illidari_councilAI(creature) {}

        uint64 EnvenomTargetGUID;

        uint32 DeadlyPoisonTimer;
        uint32 VanishTimer;
        uint32 AppearEnvenomTimer;
        uint32 EnvenomTimer;

        bool HasVanished;

        void Reset()
        {
            EnvenomTargetGUID = 0;

            DeadlyPoisonTimer = 20000;
            VanishTimer = 10000;
            AppearEnvenomTimer = 150000;
            EnvenomTimer = 3000;

            HasVanished = false;
            me->SetVisible(true);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            pulseTimer = 10000;
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id != SPELL_DEADLY_POISON)
            {
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                {
                    EnvenomTargetGUID = target->GetGUID();
                    EnvenomTimer = 3000;
                }
            }
        }

        void KilledUnit(Unit *victim)
        {
            DoSendQuantumText(SAY_VERAS_SLAY, me);
        }

        void JustDied(Unit *victim)
        {
            DoSendQuantumText(SAY_VERAS_DEATH, me);
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

            if (!me->HasAura(SPELL_VANISH,0))
            {
                if (VanishTimer <= diff)                          // Disappear and stop attacking, but follow a random unit
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(me,SPELL_DEADLY_POISON_TRIGGER, true);
                        DoCast(me, SPELL_VANISH, false);
                        AppearEnvenomTimer = 4000;
                        VanishTimer = 25000;
                        DoResetThreat();
                        return;
                    }
                }
				else VanishTimer -= diff;

                DoMeleeAttackIfReady();
            }
            else
            {
                if (EnvenomTimer <= diff)                   // Cast Envenom. This is cast 4 seconds after Vanish is over
                {
                    if (EnvenomTargetGUID)
                    {
                        if (Unit* target = Unit::GetUnit(*me,EnvenomTargetGUID))
                        {
                            if (target->HasAura(SPELL_DEADLY_POISON,0))
                            {
								if (rand()%3 == 0)
									DoCast(target,SPELL_ENVENOM);
                            }
                        }
                    }
                }
				else EnvenomTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_veras_darkshadowAI(creature);
    }
};

class spell_boss_lady_malande_shield : public SpellScriptLoader
{
public:
    spell_boss_lady_malande_shield() : SpellScriptLoader("spell_boss_lady_malande_shield") { }

    class spell_boss_lady_malande_shield_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_boss_lady_malande_shield_AuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return sSpellMgr->GetSpellInfo(SPELL_REFLECTIVE_SHIELD_T);
        }

        void Trigger(AuraEffect* aurEff, DamageInfo & dmgInfo, uint32 & absorbAmount)
        {
            Unit * target = GetTarget();
            if (dmgInfo.GetAttacker() == target)
                return;
            int32 bp = absorbAmount / 2;
            target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_REFLECTIVE_SHIELD_T, &bp, NULL, NULL, true, NULL, aurEff);
        }

        void Register()
        {
             AfterEffectAbsorb += AuraEffectAbsorbFn(spell_boss_lady_malande_shield_AuraScript::Trigger, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_boss_lady_malande_shield_AuraScript();
    }
};

void AddSC_boss_illidari_council()
{
    new mob_illidari_council();
    new mob_blood_elf_council_voice_trigger();
    new boss_gathios_the_shatterer();
    new boss_lady_malande();
    new boss_veras_darkshadow();
    new boss_high_nethermancer_zerevor();
    new spell_boss_lady_malande_shield();
}