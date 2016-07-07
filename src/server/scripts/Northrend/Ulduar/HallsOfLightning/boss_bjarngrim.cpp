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
#include "halls_of_lightning.h"

enum Texts
{
    SAY_AGGRO                               = -1602000,
    SAY_SLAY_1                              = -1602001,
    SAY_SLAY_2                              = -1602002,
    SAY_SLAY_3                              = -1602003,
    SAY_DEATH                               = -1602004,
    SAY_BATTLE_STANCE                       = -1602005,
    EMOTE_BATTLE_STANCE                     = -1602006,
    SAY_BERSEKER_STANCE                     = -1602007,
    EMOTE_BERSEKER_STANCE                   = -1602008,
    SAY_DEFENSIVE_STANCE                    = -1602009,
    EMOTE_DEFENSIVE_STANCE                  = -1602010,
};

enum Spells
{
    SPELL_PUMMEL                            = 12555,
    SPELL_IRONFORM                          = 52022,
    SPELL_SLAM                              = 52026,
    SPELL_CHARGE_UP                         = 56458,
    SPELL_TEMPORARY_ELECTRICAL_CHARGE       = 52092,
    SPELL_DEFENSIVE_STANCE                  = 53790,
    SPELL_SPELL_REFLECTION                  = 36096,
    SPELL_INTERCEPT                         = 58769,
    SPELL_KNOCK_AWAY                        = 52029,
    SPELL_BERSEKER_STANCE                   = 53791,
    SPELL_WHIRLWIND                         = 52027,
    SPELL_MORTAL_STRIKE                     = 16856,
    SPELL_BATTLE_STANCE                     = 53792,
    SPELL_CLEAVE                            = 15284,
    SPELL_ARC_WELD                          = 59085,
    SPELL_RENEW_STEEL_5N                    = 52774,
    SPELL_RENEW_STEEL_5H                    = 59160,
};

enum Equips
{
    EQUIP_SWORD                             = 37871,
    EQUIP_SHIELD                            = 35642,
    EQUIP_MACE                              = 43623,
};

enum Stances
{
    STANCE_DEFENSIVE                        = 0,
    STANCE_BERSERKER                        = 1,
    STANCE_BATTLE                           = 2,
};

enum Events
{
    EVENT_CHANGE_STANCE                     = 1,
    EVENT_REFLECTION                        = 2,
    EVENT_KNOCK_AWAY                        = 3,
    EVENT_PUMMEL                            = 4,
    EVENT_IRONFORM                          = 5,
    EVENT_INTERCEPT                         = 6,
    EVENT_WHIRLWIND                         = 7,
    EVENT_CLEAVE                            = 8,
    EVENT_MORTAL_STRIKE                     = 9,
    EVENT_SLAM                              = 10,
    EVENT_ARC_WELD                          = 11,
    EVENT_RENEW_STEEL                       = 12,
};

enum Datas
{
    DATA_LIGHTNING_STRUCK                   = 1,
};

class boss_bjarngrim : public CreatureScript
{
    public:
        boss_bjarngrim() : CreatureScript("boss_bjarngrim") { }

        struct boss_bjarngrimAI : public QuantumBasicAI
        {
            boss_bjarngrimAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

                Stance = STANCE_DEFENSIVE;
            }

			InstanceScript* instance;
            EventMap events;

            uint8 Stance;

            bool Charged;

            void Reset()
            {
                events.Reset();
                Charged = false;

                if (Creature* stormforgedLieutenant = me->FindCreatureWithDistance(NPC_STORMFORGED_LIEUTENANT, 250.0f))
					stormforgedLieutenant->Respawn();

				if (!me->HasAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE))
					me->AddAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE, me);

                if (Stance != STANCE_DEFENSIVE)
                    RemoveStance(Stance);

                DoCast(me, SPELL_DEFENSIVE_STANCE, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_SHIELD, EQUIP_NO_CHANGE);
                Stance = STANCE_DEFENSIVE;

				instance->SetData(TYPE_BJARNGRIM, NOT_STARTED);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetInCombatWithZone();
                DoSendQuantumText(SAY_AGGRO, me);

				me->SetPowerType(POWER_RAGE);
				me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

                me->CallForHelp(60.0f);

				if (!me->HasAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE))
					me->AddAura(SPELL_TEMPORARY_ELECTRICAL_CHARGE, me);
				Charged = true;

                events.ScheduleEvent(EVENT_CHANGE_STANCE, urand(20, 25) * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SLAM, 15*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_PUMMEL, 10*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_IRONFORM, 25*IN_MILLISECONDS);

                events.ScheduleEvent(EVENT_REFLECTION, 8*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(10, 20) * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_INTERCEPT, urand(5, 10) * IN_MILLISECONDS);

				instance->SetData(TYPE_BJARNGRIM, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();

                DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(TYPE_BJARNGRIM, DONE);
            }

            void RemoveStance(uint8 stance)
            {
                switch (stance)
                {
                    case STANCE_DEFENSIVE:
                        me->RemoveAurasDueToSpell(SPELL_DEFENSIVE_STANCE);
                        events.CancelEvent(EVENT_REFLECTION);
                        events.CancelEvent(EVENT_KNOCK_AWAY);
                        events.CancelEvent(EVENT_INTERCEPT);
                        break;
                    case STANCE_BERSERKER:
                        me->RemoveAurasDueToSpell(SPELL_BERSEKER_STANCE);
                        events.CancelEvent(EVENT_WHIRLWIND);
                        events.CancelEvent(EVENT_MORTAL_STRIKE);
                        break;
                    case STANCE_BATTLE:
                        me->RemoveAurasDueToSpell(SPELL_BATTLE_STANCE);
                        events.CancelEvent(EVENT_CLEAVE);
                        break;
                }
            }

            void SwitchStance()
            {
                uint8 tempStance = urand(0, 2);

                if (tempStance == Stance)
                    ++tempStance;

                if (tempStance > 2)
                    tempStance = 0;

                Stance = tempStance;

                switch (Stance)
                {
                    case STANCE_DEFENSIVE:
                        DoSendQuantumText(SAY_DEFENSIVE_STANCE, me);
                        DoSendQuantumText(EMOTE_DEFENSIVE_STANCE, me);
                        DoCast(me, SPELL_DEFENSIVE_STANCE);
                        events.ScheduleEvent(EVENT_REFLECTION, 7*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(5, 20) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_INTERCEPT, urand(5, 10) * IN_MILLISECONDS);
                        SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_SHIELD, EQUIP_NO_CHANGE);
                        break;
                    case STANCE_BERSERKER:
                        DoSendQuantumText(SAY_BERSEKER_STANCE, me);
                        DoSendQuantumText(EMOTE_BERSEKER_STANCE, me);
                        DoCast(me, SPELL_BERSEKER_STANCE);
                        events.ScheduleEvent(EVENT_WHIRLWIND, 8*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 5*IN_MILLISECONDS);
                        SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_SWORD, EQUIP_NO_CHANGE);
                        break;
                    case STANCE_BATTLE:
                        DoSendQuantumText(SAY_BATTLE_STANCE, me);
                        DoSendQuantumText(EMOTE_BATTLE_STANCE, me);
                        DoCast(me, SPELL_BATTLE_STANCE);
                        events.ScheduleEvent(EVENT_CLEAVE, 5*IN_MILLISECONDS);
                        SetEquipmentSlots(false, EQUIP_MACE, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                        break;
                }
            }
            
            uint32 GetData(uint32 type)
            {
                if (type == DATA_LIGHTNING_STRUCK)
                    return Charged ? 1 : 0;

                return 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHANGE_STANCE:
                            RemoveStance(Stance);
                            SwitchStance();
                            events.ScheduleEvent(EVENT_CHANGE_STANCE, urand(20, 25) * IN_MILLISECONDS);
                            return;
                        case EVENT_SLAM:
                            DoCastVictim(SPELL_SLAM);
                            events.ScheduleEvent(EVENT_SLAM, urand(10, 14) * IN_MILLISECONDS);
                            break;
                        case EVENT_PUMMEL:
                            DoCastVictim(SPELL_PUMMEL);
                            events.ScheduleEvent(EVENT_PUMMEL, urand(5, 10) * IN_MILLISECONDS);
                            break;
                        case EVENT_IRONFORM:
                            DoCast(me, SPELL_IRONFORM);
                            events.ScheduleEvent(EVENT_IRONFORM, urand(20, 25) * IN_MILLISECONDS);
                            break;
                        case EVENT_REFLECTION:
                            DoCast(me, SPELL_SPELL_REFLECTION);
                            events.ScheduleEvent(EVENT_REFLECTION, urand(8, 9) * IN_MILLISECONDS);
                            break;
                        case EVENT_KNOCK_AWAY:
                            DoCast(me, SPELL_KNOCK_AWAY);
                            events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(10, 20) * IN_MILLISECONDS);
                            break;
                        case EVENT_INTERCEPT:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -8.0f))
                                DoCast(target, SPELL_INTERCEPT);
                            events.ScheduleEvent(EVENT_INTERCEPT, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_WHIRLWIND:
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_WHIRLWIND, urand(10, 11) * IN_MILLISECONDS);
                            break;
                        case EVENT_MORTAL_STRIKE:
                            DoCastVictim(SPELL_MORTAL_STRIKE);
                            events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(7, 10) * IN_MILLISECONDS);
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(5, 10) * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_bjarngrimAI(creature);
        }
};

class npc_stormforged_lieutenant : public CreatureScript
{
    public:
        npc_stormforged_lieutenant() : CreatureScript("npc_stormforged_lieutenant") { }

        struct npc_stormforged_lieutenantAI : public QuantumBasicAI
        {
            npc_stormforged_lieutenantAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterToBattle(Unit* who)
            {
				if (Creature* bjarngrim = instance->instance->GetCreature(instance->GetData64(DATA_BJARNGRIM)))
				{
					if (bjarngrim->IsAlive() && !bjarngrim->GetVictim())
						bjarngrim->AI()->AttackStart(who);
				}

                events.ScheduleEvent(EVENT_ARC_WELD, urand(5, 20) * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_RENEW_STEEL, urand(10, 25) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARC_WELD:
                            DoCastVictim(SPELL_ARC_WELD);
                            events.ScheduleEvent(EVENT_ARC_WELD, urand(15, 20) * IN_MILLISECONDS);
                            break;
                        case EVENT_RENEW_STEEL:
							if (Creature* bjarngrim = instance->instance->GetCreature(instance->GetData64(DATA_BJARNGRIM)))
							{
								if (bjarngrim->IsAlive())
									DoCast(bjarngrim, DUNGEON_MODE(SPELL_RENEW_STEEL_5N, SPELL_RENEW_STEEL_5H));
							}
                            events.ScheduleEvent(EVENT_RENEW_STEEL, urand(15, 25) * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_stormforged_lieutenantAI(creature);
        }
};

class achievement_lightning_struck : public AchievementCriteriaScript
{
public:
	achievement_lightning_struck() : AchievementCriteriaScript("achievement_lightning_struck") {}

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* bjarngrim = target->ToCreature())
			if (bjarngrim->AI()->GetData(DATA_LIGHTNING_STRUCK))
				return true;

		return false;
	}
};

void AddSC_boss_bjarngrim()
{
    new boss_bjarngrim();
    new npc_stormforged_lieutenant();
    new achievement_lightning_struck();
}