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
#include "QuantumGossip.h"
#include "Vehicle.h"

#define GOSSIP_ITEM_ROYAL   "I am ready to listen"
#define GOSSIP_ITEM_ROYAL_1 "That is tragic. How did this happen?"
#define GOSSIP_ITEM_ROYAL_2 "Interesting, continue please."
#define GOSSIP_ITEM_ROYAL_3 "Unbelievable! How dare they??"
#define GOSSIP_ITEM_ROYAL_4 "Of course I will help!"

class npc_royal_historian_archesonus : public CreatureScript
{
public:
    npc_royal_historian_archesonus() : CreatureScript("npc_royal_historian_archesonus") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(3702) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(2235, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

		return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(2236, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(2237, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(2238, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(2239, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(3702);
                break;
        }
        return true;
    }
};

enum Spells
{
    SPELL_CITIZEN_AURA                  = 74034,
    SPELL_PETACT_AURA                   = 74071,
    SPELL_QUEST_CREDIT                  = 73960,
    SPELL_MOTIVATE_1                    = 73943,
    SPELL_MOTIVATE_2                    = 74080,
    SPELL_TURNIN                        = 75078,
    SPELL_AOE_TURNIN                    = 73955,
    // Basic Orders
    SPELL_SALUTE_CREDIT                 = 73771,
    SPELL_DANCE_CREDIT                  = 73830,
    SPELL_ROAR_CREDIT                   = 73832,
    SPELL_CHEER_CREDIT                  = 73833,
    // Press Fire
    SPELL_SHOOT_CREDIT                  = 74184,
    SPELL_SHOOT_VISUAL                  = 74179,
    // Prepping the Speech
    SPELL_CREATE_TELEPORTER             = 74206,
    SPELL_CREDIT_OZZIE                  = 74154,
    SPELL_CREDIT_MILLI                  = 74155,
    SPELL_CREDIT_TOG                    = 74156,
};

enum Creatures
{
    NPC_SPARKNOZZLE                     = 39675,
    NPC_CITIZEN_1                       = 39253,
    NPC_CITIZEN_2                       = 39623,
    NPC_MOTIVATED_CITIZEN_1             = 39466,
    NPC_MOTIVATED_CITIZEN_2             = 39624,
    NPC_TRAINEE                         = 39349,
    NPC_TARGET                          = 39711,
    NPC_OZZIE                           = 1268,
    NPC_MILLI                           = 7955,
    NPC_TOG                             = 6119,
    NPC_SUMMONING_PAD                   = 39817,
};

enum Points
{
    POINT_SPARKNOZZLE                   = 4026500,
};

enum Texts
{
    SAY_CITIZEN_START                   = 0,
    SAY_CITIZEN_END                     = 1,
};

class npc_gnome_citizen : public CreatureScript
{
    public:
        npc_gnome_citizen() : CreatureScript("npc_gnome_citizen") { }

        struct npc_gnome_citizenAI : public QuantumBasicAI
        {
            npc_gnome_citizenAI(Creature* creature) : QuantumBasicAI(creature)
            {
                Reset();
                Player* player = me->GetOwner()->ToPlayer();

                switch (urand(1, 4))
                {
                    case 1:
                        _mountModel = 6569;
                        break;
                    case 2:
                        _mountModel = 9473;
                        break;
                    case 3:
                        _mountModel = 9474;
                        break;
                    case 4:
                        _mountModel = 9475;
                        break;
                }
                if (player)
                    me->GetMotionMaster()->MoveFollow(player, 5.0f, float(rand_norm() + 1.0f) * M_PI / 3.0f * 4.0f);
            }

            void Reset()
            {
                _complete = false;
                me->AddAura(SPELL_CITIZEN_AURA, me);
                DoCast(me, SPELL_PETACT_AURA);
                me->SetReactState(REACT_PASSIVE);
                Talk(SAY_CITIZEN_START);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_SPARKNOZZLE)
                    me->DespawnAfterAction();
            }

            void SpellHit(Unit* caster, SpellEntry const* spell)
            {
                if (spell->Id == SPELL_AOE_TURNIN && caster->GetEntry() == NPC_SPARKNOZZLE && !_complete)
                {
                    _complete = true;    // Preventing from giving credit twice
                    DoCast(me, SPELL_TURNIN);
                    DoCast(me, SPELL_QUEST_CREDIT);
                    Talk(SAY_CITIZEN_END);
                    me->GetMotionMaster()->MovePoint(POINT_SPARKNOZZLE, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                }
            }

            void UpdateAI(const uint32 diff)
            {
                Unit* owner = me->GetOwner();

                if (!owner)
                    return;

                if (owner->IsMounted() && !me->IsMounted())
                    me->Mount(_mountModel);
                else if (!owner->IsMounted() && me->IsMounted())
                    me->RemoveMount();

                me->SetSpeed(MOVE_RUN, owner->GetSpeedRate(MOVE_RUN));
                me->SetSpeed(MOVE_WALK, owner->GetSpeedRate(MOVE_WALK));
            }

        private:
            uint32 _mountModel;
            bool _complete;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gnome_citizenAI(creature);
        }
};

#define STEAM_0     "��, ����������� ���� ����������, �������-�� ����������� �� ������!"
#define STEAM_1     "� ����� ��� �����, ��� ������ ����� ��������� ������!"
#define STEAM_2     "������ �����, ��� ���������� ������ �������� ����������."
#define STEAM_3     "�� ������� �������� ���, ��� ���� �������������� ��������� �� ������!"
#define STEAM_4     "����, �������, ������� ����� ������ ���������!"
#define STEAM_5     "�������� ������!"
#define STEAM_6     "�� ���� ��� ����� ��������� ����� ������� ������ �����!"
#define STEAM_7     "��� ������ � ��� ������, �������� ���, ��� ����� ��������� ���������!"
#define STEAM_8     "�������� ��� ��������� ���������!"
#define STEAM_9     "�� ��, ������!"
#define STEAM_10    "�������, ��� ��������� ������ � ����� ����� - ��� ���!"
#define STEAM_11    "������������� �������� ��� ��� ������ ������ ���� ��� ������!"
#define STEAM_12    "�������! �������� ���� �������!"
#define STEAM_13    "����������!"
#define STEAM_14    "������, ����� ������ � �������� - ����� ��������� �������� ������������ ����� � ������ ������!"
#define STEAM_15    "��������� ��� ���� ���� ����� ������ �������� �����! ��������� �� �������!"
#define STEAM_16    "� ������ - ���������!"
#define STEAM_17    "�����������!"
#define STEAM_18    "�� - ������ ����� �����������, ������� � �����-���� �����! �������-�� �� ��������!"

class npc_steamcrank : public CreatureScript
{
    public:
        npc_steamcrank() : CreatureScript("npc_steamcrank") {}

        struct npc_steamcrankAI : public QuantumBasicAI
        {
            npc_steamcrankAI(Creature* creature) : QuantumBasicAI(creature) {}

            void Reset(){}

            void JumpToNextStep(uint32 timer)
            {
                StepTimer = timer;
                ++_step;
                if (_step > 26)
                {
                    _step = 0;
                    StepTimer = 2000;
                }
            }

            void ReceiveEmote(Player* player, uint32 TextEmote)
            {
                switch(TextEmote)
                {
                    case TEXT_EMOTE_SALUTE:
                        if (_step >= 5 && _step < 8)
                            me->CastSpell(player, SPELL_SALUTE_CREDIT, true);
                        break;
                    case TEXT_EMOTE_ROAR:
                        if (_step >= 11 && _step < 14)
                            me->CastSpell(player, SPELL_ROAR_CREDIT, true);
                        break;
                    case TEXT_EMOTE_CHEER:
                        if (_step >= 17 && _step < 20)
                            me->CastSpell(player, SPELL_CHEER_CREDIT, true);
                        break;
                    case TEXT_EMOTE_DANCE:
                        if (_step >= 23 && _step < 26)
                            me->CastSpell(player, SPELL_DANCE_CREDIT, true);
                        break;
                }
            }

            void ForceEmote(uint32 emote)
            {
                std::list<Creature*> Trainees;
                GetCreatureListWithEntryInGrid(Trainees, me, NPC_TRAINEE, 15.0f);
                if (!Trainees.empty())
                {
                    for (std::list<Creature*>::iterator itr = Trainees.begin(); itr != Trainees.end(); ++itr)
                        (*itr)->SetUInt32Value(UNIT_NPC_EMOTE_STATE, emote);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (StepTimer <= diff)
                {
                    switch (_step)
                    {
                        case 0:
                            me->MonsterSay(STEAM_0, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 1:
                            me->MonsterSay(STEAM_1, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 2:
                            me->MonsterSay(STEAM_2, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 3:
                            me->MonsterSay(STEAM_3, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 4:
                            me->MonsterSay(STEAM_4, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(1000);
                            break;
                        case 5:
                            ForceEmote(EMOTE_ONESHOT_SALUTE);
                            JumpToNextStep(1500);
                            break;
                        case 6:
                            ForceEmote(EMOTE_ONESHOT_NONE);
                            JumpToNextStep(3000);
                        case 7:
                            me->MonsterSay(STEAM_5, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 8:
                            me->MonsterSay(STEAM_6, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 9:
                            me->MonsterSay(STEAM_7, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 10:
                            me->MonsterSay(STEAM_8, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(1000);
                            break;
                        case 11:
                            ForceEmote(EMOTE_ONESHOT_ROAR);
                            JumpToNextStep(2000);
                            break;
                        case 12:
                            ForceEmote(EMOTE_ONESHOT_NONE);
                            JumpToNextStep(3000);
                            break;
                        case 13:
                            me->MonsterSay(STEAM_9, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 14:
                            me->MonsterSay(STEAM_10, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 15:
                            me->MonsterSay(STEAM_11, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 16:
                            me->MonsterSay(STEAM_12, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(1000);
                            break;
                        case 17:
                            ForceEmote(EMOTE_ONESHOT_CHEER);
                            JumpToNextStep(1500);
                            break;
                        case 18:
                            ForceEmote(EMOTE_ONESHOT_NONE);
                            JumpToNextStep(3000);
                            break;
                        case 19:
                            me->MonsterSay(STEAM_13, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 20:
                            me->MonsterSay(STEAM_14, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 21:
                            me->MonsterSay(STEAM_15, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 22:
                            me->MonsterSay(STEAM_16, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(1000);
                            break;
                        case 23:
                            ForceEmote(EMOTE_ONESHOT_DANCE);
                            JumpToNextStep(2500);
                            break;
                        case 24:
                            ForceEmote(EMOTE_ONESHOT_NONE);
                            JumpToNextStep(3000);
                            break;
                        case 25:
                            me->MonsterSay(STEAM_17, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                        case 26:
                            me->MonsterSay(STEAM_18, LANG_UNIVERSAL, NULL);
                            JumpToNextStep(5000);
                            break;
                    }
                }
                else
                    StepTimer -= diff;
            }

        private:
            uint32 _step;
            uint32 StepTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_steamcrankAI(creature);
        }
};

#define MEK_1_0 "����� ��� ������� ���� �����, �� �� ������� �� �������..."
#define MEK_1_1 "...���� �����������������!"
#define LIS_1_0 "���? � ������� �� ���� � ��� �� �������! ��� �������!"
#define MEK_2_0 "�� �� �������� ���� ����������! �� �� �������� ��� ���!"
#define MEK_2_1 "�� ����� ����! �� ����� ���������� ����! ������� �� ���������..."
#define MEK_2_2 "...��� ���� �������������!"
#define LIS_2_0 "������! ����... ����� � ������, ���� ������� ����������."
#define MEK_3_0 "�� ������ ��������� �������� ������ ��������, ��� ����, ������, �����, ������� � ������ � ���� ������."
#define MEK_3_1 "��� ������� - ������� �������������."
#define MEK_3_2 "����� � ��������� ���."
#define LIS_3_0 "�������, ������ ��������. ���� ����-�� ��� ���� �� �������."

class npc_mekkatorque : public CreatureScript
{
    public:
        npc_mekkatorque() : CreatureScript("npc_mekkatorque") { }

        struct npc_mekkatorqueAI : public QuantumBasicAI
        {
            npc_mekkatorqueAI(Creature* creature) : QuantumBasicAI(creature)
            {
                if (Creature* ozzie = me->FindCreatureWithDistance(NPC_OZZIE, 15.0f, true))
                    _listener = ozzie;
                else if (Creature* milli = me->FindCreatureWithDistance(NPC_MILLI, 15.0f, true))
                    _listener = milli;
                else if(Creature* tog = me->FindCreatureWithDistance(NPC_TOG, 15.0f, true))
                    _listener = tog;
                else
                {
                    me->DespawnAfterAction();
                    return;
                }
                _variation = urand (1,3);
                me->CastSpell(me, SPELL_CREATE_TELEPORTER, true);
            }

            void Reset()
            {
            }

            void JumpToNextStep(uint32 timer)
            {
                StepTimer = timer;
                ++_step;
            }

            void CastCredit()
            {
                Unit* owner = me->GetOwner();
                switch (_listener->GetEntry())
                {
                    case NPC_OZZIE:
                        me->CastSpell(owner, SPELL_CREDIT_OZZIE, true);
                        break;
                    case NPC_MILLI:
                        me->CastSpell(owner, SPELL_CREDIT_MILLI, true);
                        break;
                    case NPC_TOG:
                        me->CastSpell(owner, SPELL_CREDIT_TOG, true);
                        break;
                }
                me->DespawnAfterAction();
            }

            void UpdateAI(const uint32 diff)
            {
                if (StepTimer <= diff)
                {
                    switch (_variation)
                    {
                        case 1:
                            switch (_step)
                            {
                                case 0:
                                    me->MonsterSay(MEK_1_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(5000);
                                    break;
                                case 1:
                                    me->MonsterSay(MEK_1_1, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(5000);
                                    break;
                                case 2:
                                    _listener->MonsterSay(LIS_1_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(3000);
                                    break;
                                case 3:
                                    if (Creature* Pad = me->FindCreatureWithDistance(NPC_SUMMONING_PAD, 1.0f, true))
                                        Pad->DespawnAfterAction();
                                    CastCredit();
                                    break;
                            }
                            break;
                        case 2:
                            switch (_step)
                            {
                                case 0:
                                    me->MonsterSay(MEK_2_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(5000);
                                    break;
                                case 1:
                                    me->MonsterSay(MEK_2_1, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(5000);
                                    break;
                                case 2:
                                    me->MonsterSay(MEK_2_2, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(5000);
                                    break;
                                case 3:
                                    _listener->MonsterSay(LIS_2_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(3000);
                                    break;
                                case 4:
                                    if (Creature* Pad = me->FindCreatureWithDistance(NPC_SUMMONING_PAD, 1.0f, true))
                                        Pad->DespawnAfterAction();
                                    CastCredit();
                                    break;
                            }
                            break;
                        case 3:
                            switch (_step)
                            {
                                case 0:
                                    me->MonsterSay(MEK_3_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(7000);
                                    break;
                                case 1:
                                    me->MonsterSay(MEK_3_1, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(3000);
                                    break;
                                case 2:
                                    me->MonsterSay(MEK_3_2, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(3000);
                                    break;
                                case 3:
                                    _listener->MonsterSay(LIS_3_0, LANG_UNIVERSAL, NULL);
                                    JumpToNextStep(3000);
                                    break;
                                case 4:
                                    if (Creature* Pad = me->FindCreatureWithDistance(NPC_SUMMONING_PAD, 1.0f, true))
                                        Pad->DespawnAfterAction();
                                    CastCredit();
                                    break;
                            }
                            break;
                    }
                }
                else
                    StepTimer -= diff;
            }

        private:
            uint32 _step;
            uint32 StepTimer;
            uint32 _variation;
            Creature* _listener;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mekkatorqueAI(creature);
        }
};

class npc_shoot_bunny : public CreatureScript
{
public:
	npc_shoot_bunny() : CreatureScript("npc_shoot_bunny") { }

	struct npc_shoot_bunnyAI : public QuantumBasicAI
	{
		npc_shoot_bunnyAI(Creature* creature) : QuantumBasicAI(creature) { }

		void Reset()
		{
			if (me->FindCreatureWithDistance(NPC_TARGET, 3.0f, true))
			{
				if (Unit* vehSummoner = me->ToTempSummon()->GetSummoner())
				{
					if (Vehicle* vehicle = vehSummoner->GetVehicleKit())
					{
						if (Unit* driver = vehicle->GetPassenger(0))
							driver->CastSpell(driver, SPELL_SHOOT_CREDIT, true);
					}
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shoot_bunnyAI(creature);
	}
};

class spell_motivate_a_tron : public SpellScriptLoader
{
    public:
        spell_motivate_a_tron() : SpellScriptLoader("spell_motivate_a_tron") {}

        class spell_motivate_a_tron_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_motivate_a_tron_SpellScript)
            bool Validate(SpellEntry const* /*spellEntry*/)
            {
                if (!sSpellStore.LookupEntry(SPELL_MOTIVATE_1))
                    return false;
                if (!sSpellStore.LookupEntry(SPELL_MOTIVATE_2))
                    return false;
               return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    uint32 motivate = 0;
                    if (target->GetEntry() == NPC_CITIZEN_1)
                        motivate = SPELL_MOTIVATE_1;
                    else if (target->GetEntry() == NPC_CITIZEN_2)
                        motivate = SPELL_MOTIVATE_2;
                    if (motivate)
                        caster->CastSpell(target, motivate, true, NULL, NULL, caster->GetGUID());
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_motivate_a_tron_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_motivate_a_tron_SpellScript();
        }
};

void AddSC_ironforge()
{
    new npc_royal_historian_archesonus();
	new npc_gnome_citizen();
	new npc_steamcrank();
	new npc_mekkatorque();
	new npc_shoot_bunny();
	new spell_motivate_a_tron();
}