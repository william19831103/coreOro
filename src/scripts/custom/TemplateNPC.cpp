
#include "GameEventMgr.h"
#include "Bag.h"
#include "Opcodes.h"
#include "TemplateNPC.h"

enum TemplateNPCGossip
{
    GOSSIP_SENDER_START                 = 10000000,
    GOSSIP_SENDER_TEMP_DELETE           = 10000001,
    GOSSIP_SENDER_TEMP_ADD              = 10000002,
    GOSSIP_SENDER_TEMP_EQUIP            = 10000003,
    GOSSIP_SENDER_PET_DELETE            = 10000004,
    GOSSIP_SENDER_PET_ADD               = 10000005,
    GOSSIP_SENDER_PET_SELECT            = 10000006,
    GOSSIP_SENDER_TEMP_CONFIRM          = 10000007,
    GOSSIP_SENDER_TEMP_SAVE_PREMADENAME = 10000008,
    GOSSIP_SENDER_TEMP_SPECS            = 10000009,
    GOSSIP_SENDER_TEMP_PATCH            = 10000010,
    GOSSIP_SENDER_TEMP_DELETE_PATCH     = 10000011,
    GOSSIP_SENDER_TEMP_DELETE_CONFIRM   = 10000012
};

enum TemplatePatchGossip
{
    GOSSIP_SENDER_PATCH_12 = 0,
    GOSSIP_SENDER_PATCH_13 = 1,
    GOSSIP_SENDER_PATCH_14 = 2,
    GOSSIP_SENDER_PATCH_15 = 3,
    GOSSIP_SENDER_PATCH_16 = 4,
    GOSSIP_SENDER_PATCH_17 = 5,
    GOSSIP_SENDER_PATCH_18 = 6,
    GOSSIP_SENDER_PATCH_19 = 7,
    GOSSIP_SENDER_PATCH_110 = 8,
    GOSSIP_SENDER_PATCH_111 = 9,
    GOSSIP_SENDER_PATCH_112 = 10
};

enum TemplateNPCSpells
{
    COOL_VISUAL_SPELL       = 17451,
    COOL_VISUAL_SPELL_2     = 17321,
    COOL_VISUAL_SPELL_3     = 29215,
    COOL_VISUAL_SPELL_4     = 27741
};

enum TemplateNPCItems
{
    BOTTOMLESS_BAG                 = 14156,
    ANCIENT_SINEW_WRAPPED_LAMINA   = 18714,
    DOOMSHOT                       = 12654,
    CORE_FELCLOTH_BAG              = 21342,
    SOUL_SHARD                     = 6265,
    EARTH_TOTEM                    = 5175,
    AIR_TOTEM                      = 5178,
    WATER_TOTEM                    = 5177,
    FIRE_TOTEM                     = 5176
};

enum TemplateNPCNpcs
{
    MAGE_NPC       = 600003,
    HUNTER_NPC     = 600001,
    ROGUE_NPC      = 600006,
    WARRIOR_NPC    = 600011,
    WARLOCK_NPC    = 600012,
    PALADIN_NPC    = 600010,
    PRIEST_NPC     = 600015,
    SHAMAN_NPC     = 600014,
    DRUID_NPC      = 600016
};

enum TemplateNPCWarlockPets
{
    WARLOCK_PET_IMP        = 416,
    WARLOCK_PET_SUCCUBUS   = 1863,
    WARLOCK_PET_VOIDWALKER = 1860,
    WARLOCK_PET_FELHUNTER  = 417
};

enum TemplateNPCGeneric
{
    GET_LVL_60                  = GOSSIP_ACTION_INFO_DEF + 1,
    MAKE_PET_HAPPY              = GOSSIP_ACTION_INFO_DEF + 2,
    SHOW_PETS                   = GOSSIP_ACTION_INFO_DEF + 3,
    SAVE_PET                    = GOSSIP_ACTION_INFO_DEF + 4,
    DELETE_PET                  = GOSSIP_ACTION_INFO_DEF + 5,
    RESET_TALENTS               = GOSSIP_ACTION_INFO_DEF + 6,
    RESET_COOLDOWNS_AND_CHARGES = GOSSIP_ACTION_INFO_DEF + 7,
    DELETE_GEAR                 = GOSSIP_ACTION_INFO_DEF + 8,
    ALL_BUFFS                   = GOSSIP_ACTION_INFO_DEF + 9,
    SHOW_TEMP                   = GOSSIP_ACTION_INFO_DEF + 10,
    SAVE_TEMP                   = GOSSIP_ACTION_INFO_DEF + 11,
    DELETE_TEMP                 = GOSSIP_ACTION_INFO_DEF + 12,
    TEACH_WARLOCK_PET           = GOSSIP_ACTION_INFO_DEF + 13,
    SHOW_ILVL                   = GOSSIP_ACTION_INFO_DEF + 14,
    FIX_DB                      = GOSSIP_ACTION_INFO_DEF + 15,
    SHOW_SPECS                  = GOSSIP_ACTION_INFO_DEF + 16,
    UPGRADE_TALENTS             = GOSSIP_ACTION_INFO_DEF + 17,
    RESET_BUFFS                 = GOSSIP_ACTION_INFO_DEF + 18,
    TRAIN_RIDE                  = GOSSIP_ACTION_INFO_DEF + 19,
    SHOW_MY_NPC					= GOSSIP_ACTION_INFO_DEF + 20,
    FIX_QUEST_ITEM_LEVEL        = GOSSIP_ACTION_INFO_DEF + 21
};

std::string DoColor(std::string cColor)
{
    std::string color = "cFFC79C6E";

    if (cColor == "RED")			color = "cFFC79C6E";
    else if (cColor == "YELLOW")	color = "cFF8787ED";
    else if (cColor == "BLUE")		color = "cFF40C7EB";
    else if (cColor == "GREEN")		color = "cFFFF7D0A";
    else if (cColor == "PURPLE")	color = "cFFFFFFFF";
    else if (cColor == "ORANGE")	color = "cFFABD473";
    else if (cColor == "WHITE")		color = "cFFFFF569";
    else if (cColor == "GREY")		color = "cFFF58CBA";
    else if (cColor == "BROWN")		color = "cFF0070DE";

    return color;
}

std::string GetClassString(Player* pPlayer)
{
    switch (pPlayer->GetClass())
    {
        case CLASS_WARRIOR: return "WARRIOR"; break;
        case CLASS_PALADIN: return "PALADIN"; break;
        case CLASS_HUNTER:  return "HUNTER";  break;
        case CLASS_ROGUE:   return "ROGUE";   break;
        case CLASS_PRIEST:  return "PRIEST";  break;
        case CLASS_SHAMAN:  return "SHAMAN";  break;
        case CLASS_MAGE:    return "MAGE";    break;
        case CLASS_WARLOCK: return "WARLOCK"; break;
        case CLASS_DRUID:   return "DRUID";   break;
        default: return "WARRIOR"; break;
    }
}

std::string GetPetFamily(uint32 Entry)
{
    switch (Entry)
    {
        case 1: return "Wolf"; break;
        case 2: return "Cat"; break;
        case 3: return "Spider"; break;
        case 4: return "Bear"; break;
        case 5: return "Boar"; break;
        case 6: return "Crocolisk"; break;
        case 7: return "Carrion Bird"; break;
        case 8: return "Crab"; break;
        case 9: return "Gorilla"; break;
        case 11: return "Raptor"; break;
        case 12: return "Tallstrider"; break;
        case 15: return "Felhunter"; break;
        case 16: return "Voidwalker"; break;
        case 17: return "Succubus"; break;
        case 19: return "Doomguard"; break;
        case 20: return "Scorpid"; break;
        case 21: return "Turtle"; break;
        case 23: return "Imp"; break;
        case 24: return "Bat"; break;
        case 25: return "Hyena"; break;
        case 26: return "Owl"; break;
        case 27: return "Wind Serpent"; break;
        case 28: return "Remote Control"; break;
        default: return "Wolf"; break;
    }
}

std::string GetMySpellFamilyName(Player* pPlayer)
{
    switch (pPlayer->GetClass())
    {
        case CLASS_PRIEST:
            return "SPELLFAMILY_PRIEST";
            break;
        case CLASS_PALADIN:
            return "SPELLFAMILY_PALADIN";
            break;
        case CLASS_WARRIOR:
            return "SPELLFAMILY_WARRIOR";
            break;
        case CLASS_MAGE:
            return "SPELLFAMILY_MAGE";
            break;
        case CLASS_WARLOCK:
            return "SPELLFAMILY_WARLOCK";
            break;
        case CLASS_SHAMAN:
            return "SPELLFAMILY_SHAMAN";
            break;
        case CLASS_DRUID:
            return "SPELLFAMILY_DRUID";
            break;
        case CLASS_HUNTER:
            return "SPELLFAMILY_HUNTER";
            break;
        case CLASS_ROGUE:
            return "SPELLFAMILY_ROGUE";
            break;
        default: return "SPELLFAMILY_WARRIOR"; break;
    }
}

uint16 AverageItemLevel(Player* pPlayer)
{
    float ilevel = 0;
    uint32 counter = 0;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            uint32 itemId = equippedItem->GetEntry();
            ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemId);
            if (item_proto->ItemLevel)
            {
                ilevel = ilevel + item_proto->ItemLevel;
                ++counter;
            }
        }
    }

    ilevel = ilevel / counter; //calculate average itemlevel.
    double ilvl = floor(ilevel * 100.0 + .5) / 100.0;

    if (ilvl)
        return ilvl;
    else
        return 0;

    /* was for testing.

    std::ostringstream ss;
    ss << "Your average itemlevel is " << ilvl << ".";
    ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
    pPlayer->GetSession()->SendAreaTriggerMessage(ss.str().c_str());
    */
}

std::string DefineMageSpec(Player* pPlayer)
{
    enum MageSpells
    {
        IGNITE_RANK1 = 11119,
        IGNITE_RANK2 = 11120,
        IGNITE_RANK3 = 12846,
        IGNITE_RANK4 = 12847,
        IGNITE_RANK5 = 12848,
        COMBUSTION = 11129,
        ICE_BARRIER_RANK1 = 11426,
        ICE_BARRIER_RANK2 = 13031,
        ICE_BARRIER_RANK3 = 13032,
        ICE_BARRIER_RANK4 = 13033,
        WINTERS_CHILL_RANK1 = 11180,
        WINTERS_CHILL_RANK2 = 28592,
        WINTERS_CHILL_RANK3 = 28593,
        WINTERS_CHILL_RANK4 = 28594,
        WINTERS_CHILL_RANK5 = 28595,
        ARCANE_POWER = 12042,
        PYROBLAST = 18809,
        COLD_SNAP = 12472,
        ICE_BLOCK = 11958,
        FROST_CHANNELING_RANK1 = 11160,
        FROST_CHANNELING_RANK2 = 12518,
        FROST_CHANNELING_RANK3 = 12519,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(ARCANE_POWER))
    {
        if (pPlayer->HasSpell(PYROBLAST))
            GetSpells << "(Arcane Power/Pyroblast) ";
        if (pPlayer->HasSpell(FROST_CHANNELING_RANK1) || pPlayer->HasSpell(FROST_CHANNELING_RANK2) || pPlayer->HasSpell(FROST_CHANNELING_RANK3))
            GetSpells << "(Arcane Power/Frost) ";
    };

    if (pPlayer->HasSpell(PYROBLAST) && !pPlayer->HasSpell(ARCANE_POWER))
    {
        if (pPlayer->HasSpell(ICE_BLOCK))
            GetSpells << "(Pyroblast/Ice Block) ";
    };

    if (pPlayer->HasSpell(IGNITE_RANK1) || pPlayer->HasSpell(IGNITE_RANK2) || pPlayer->HasSpell(IGNITE_RANK3) || pPlayer->HasSpell(IGNITE_RANK4) || pPlayer->HasSpell(IGNITE_RANK5))
    {
        if (pPlayer->HasSpell(COMBUSTION))
            GetSpells << "(Combustion/Ignite) ";
    };

    if (pPlayer->HasSpell(ICE_BARRIER_RANK4))
    {
        if (pPlayer->HasSpell(WINTERS_CHILL_RANK5))
            GetSpells << "(Ice Barrier/Winters Chill) ";
        if (pPlayer->HasSpell(COLD_SNAP))
            GetSpells << "(Ice Barrier/Cold Snap) ";
    };

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineWarlockSpec(Player* pPlayer)
{
    enum WarlockSpells
    {
        SHADOW_MASTERY_RANK1 = 18271,
        SHADOW_MASTERY_RANK2 = 18272,
        SHADOW_MASTERY_RANK3 = 18273,
        SHADOW_MASTERY_RANK4 = 18274,
        SHADOW_MASTERY_RANK5 = 18275,
        RUIN = 17959,
        DEMONIC_SACRIFICE = 18788,
        CONFLAGRATE_RANK1 = 17962,
        CONFLAGRATE_RANK2 = 18930,
        CONFLAGRATE_RANK3 = 18931,
        CONFLAGRATE_RANK4 = 18932,
        SOUL_LINK = 19028,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(SOUL_LINK))
        GetSpells << "(Soul Link) ";

    if (pPlayer->HasSpell(RUIN))
    {
        if (pPlayer->HasSpell(SHADOW_MASTERY_RANK1) || pPlayer->HasSpell(SHADOW_MASTERY_RANK2) || pPlayer->HasSpell(SHADOW_MASTERY_RANK3) || pPlayer->HasSpell(SHADOW_MASTERY_RANK4) || pPlayer->HasSpell(SHADOW_MASTERY_RANK5))
            GetSpells << "(SM/Ruin) ";
        else if (pPlayer->HasSpell(DEMONIC_SACRIFICE))
            GetSpells << "(DS/Ruin) ";
        else if (pPlayer->HasSpell(CONFLAGRATE_RANK1) || pPlayer->HasSpell(CONFLAGRATE_RANK2) || pPlayer->HasSpell(CONFLAGRATE_RANK3) || pPlayer->HasSpell(CONFLAGRATE_RANK4))
            GetSpells << "(Ruin/Conflag) ";
    };

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineDruidSpec(Player* pPlayer)
{
    enum DruidSpells
    {
        THICK_HIDE_RANK1 = 16929,
        THICK_HIDE_RANK2 = 16930,
        THICK_HIDE_RANK3 = 16931,
        THICK_HIDE_RANK4 = 16932,
        THICK_HIDE_RANK5 = 16933,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(THICK_HIDE_RANK1) || pPlayer->HasSpell(THICK_HIDE_RANK2) || pPlayer->HasSpell(THICK_HIDE_RANK3) || pPlayer->HasSpell(THICK_HIDE_RANK4) || pPlayer->HasSpell(THICK_HIDE_RANK5))
        GetSpells << "Bear ";
    else
        GetSpells << "Cat ";

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineHunterSpec(Player* pPlayer)
{
    enum HunterSpells
    {
        TRUESHOT_AURA_RANK1 = 19506,
        TRUESHOT_AURA_RANK2 = 20905,
        TRUESHOT_AURA_RANK3 = 20906,
        BESTIAL_WRATH = 19574,
        LIGHTNING_REFLEXES_RANK1 = 19168,
        LIGHTNING_REFLEXES_RANK2 = 19180,
        LIGHTNING_REFLEXES_RANK3 = 19181,
        LIGHTNING_REFLEXES_RANK4 = 24296,
        LIGHTNING_REFLEXES_RANK5 = 24297,
        AIMED_SHOT_RANK1 = 19434,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(LIGHTNING_REFLEXES_RANK1) || pPlayer->HasSpell(LIGHTNING_REFLEXES_RANK2) || pPlayer->HasSpell(LIGHTNING_REFLEXES_RANK3) || pPlayer->HasSpell(LIGHTNING_REFLEXES_RANK4) || pPlayer->HasSpell(LIGHTNING_REFLEXES_RANK5))
        GetSpells << "(Lightning Reflexes) ";
    else if (pPlayer->HasSpell(BESTIAL_WRATH))
        GetSpells << "(Bestial Wrath) ";
    else if (pPlayer->HasSpell(TRUESHOT_AURA_RANK1) || pPlayer->HasSpell(TRUESHOT_AURA_RANK2) || pPlayer->HasSpell(TRUESHOT_AURA_RANK3))
        GetSpells << "(Trueshot) ";

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineShamanSpec(Player* pPlayer)
{
    enum ShamanSpells
    {
        ELEMENTAL_FURY = 16089,
        STORMSTRIKE = 17364,
        NATURES_GUIDANCE_RANK1 = 16180,
        NATURES_GUIDANCE_RANK2 = 16196,
        NATURES_GUIDANCE_RANK3 = 16198,
        TWO_HANDED_AXES_AND_MACES = 16269,
        MANA_TIDE_TOTEM = 16190,
        ELEMENTAL_MASTERY = 16166,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(TWO_HANDED_AXES_AND_MACES))
        GetSpells << "(Two-Handed, ";
    else
        GetSpells << "(";

    if (pPlayer->HasSpell(ELEMENTAL_FURY))
        GetSpells << "Elemental Fury) ";
    else if (pPlayer->HasSpell(STORMSTRIKE))
        GetSpells << "Stormstrike) ";
    else if (pPlayer->HasSpell(MANA_TIDE_TOTEM))
        GetSpells << "Mana Tide) ";
    else if (pPlayer->HasSpell(ELEMENTAL_MASTERY))
        GetSpells << "Elemental Mastery) ";
    else if (pPlayer->HasSpell(NATURES_GUIDANCE_RANK1) || pPlayer->HasSpell(NATURES_GUIDANCE_RANK2) || pPlayer->HasSpell(NATURES_GUIDANCE_RANK3))
        GetSpells << "Nature's Guidance) ";
    else
        GetSpells << ") ";

    ChatHandler(pPlayer->GetSession()).PSendSysMessage(GetSpells.str().c_str());

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineRogueSpec(Player* pPlayer)
{
    enum RogueSpells
    {
        LETHALITY_RANK1 = 14128,
        LETHALITY_RANK2 = 14132,
        LETHALITY_RANK3 = 14135,
        LETHALITY_RANK4 = 14136,
        LETHALITY_RANK5 = 14137,
        IMPROVED_BACKSTAB_RANK1 = 13733,
        IMPROVED_BACKSTAB_RANK2 = 13865,
        IMPROVED_BACKSTAB_RANK3 = 13866,
        OPPORTUNITY_RANK1 = 14057,
        OPPORTUNITY_RANK2 = 14072,
        OPPORTUNITY_RANK3 = 14073,
        OPPORTUNITY_RANK4 = 14074,
        OPPORTUNITY_RANK5 = 14075,
        DAGGER_SPECIALIZATION_RANK1 = 13706,
        DAGGER_SPECIALIZATION_RANK2 = 13804,
        DAGGER_SPECIALIZATION_RANK3 = 13805,
        DAGGER_SPECIALIZATION_RANK4 = 13806,
        DAGGER_SPECIALIZATION_RANK5 = 13807,
        SWORD_SPECIALIZATION_RANK1 = 13960,
        SWORD_SPECIALIZATION_RANK2 = 13961,
        SWORD_SPECIALIZATION_RANK3 = 13962,
        SWORD_SPECIALIZATION_RANK4 = 13963,
        SWORD_SPECIALIZATION_RANK5 = 13964,
        SEAL_FATE_RANK1 = 14186,
        SEAL_FATE_RANK2 = 14190,
        SEAL_FATE_RANK3 = 14193,
        SEAL_FATE_RANK4 = 14194,
        SEAL_FATE_RANK5 = 14195,
        COLD_BLOOD = 14177,
        PREPARATION = 14185,
    };

    std::ostringstream GetSpells;

    if (pPlayer->HasSpell(DAGGER_SPECIALIZATION_RANK1) || pPlayer->HasSpell(DAGGER_SPECIALIZATION_RANK2) || pPlayer->HasSpell(DAGGER_SPECIALIZATION_RANK3) || pPlayer->HasSpell(DAGGER_SPECIALIZATION_RANK4) || pPlayer->HasSpell(DAGGER_SPECIALIZATION_RANK5))
        if (pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK1) || pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK2) || pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK3) && pPlayer->HasSpell(OPPORTUNITY_RANK1) || pPlayer->HasSpell(OPPORTUNITY_RANK2) || pPlayer->HasSpell(OPPORTUNITY_RANK3) || pPlayer->HasSpell(OPPORTUNITY_RANK4) || pPlayer->HasSpell(OPPORTUNITY_RANK5) && pPlayer->HasSpell(LETHALITY_RANK1) || pPlayer->HasSpell(LETHALITY_RANK2) || pPlayer->HasSpell(LETHALITY_RANK3) || pPlayer->HasSpell(LETHALITY_RANK4) || pPlayer->HasSpell(LETHALITY_RANK5))
        {
            GetSpells << "(Dagger/Backstab) ";
        }
    if (pPlayer->HasSpell(SWORD_SPECIALIZATION_RANK1) || pPlayer->HasSpell(SWORD_SPECIALIZATION_RANK2) || pPlayer->HasSpell(SWORD_SPECIALIZATION_RANK3) || pPlayer->HasSpell(SWORD_SPECIALIZATION_RANK4) || pPlayer->HasSpell(SWORD_SPECIALIZATION_RANK5))
        GetSpells << "(Sword) ";
    if (pPlayer->HasSpell(SEAL_FATE_RANK1) || pPlayer->HasSpell(SEAL_FATE_RANK2) || pPlayer->HasSpell(SEAL_FATE_RANK3) || pPlayer->HasSpell(SEAL_FATE_RANK4) || pPlayer->HasSpell(SEAL_FATE_RANK5))
    {
        if ((pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK1) || pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK2) || pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK3)) && (pPlayer->HasSpell(OPPORTUNITY_RANK1) || pPlayer->HasSpell(OPPORTUNITY_RANK2) || pPlayer->HasSpell(OPPORTUNITY_RANK3) || pPlayer->HasSpell(OPPORTUNITY_RANK4) || pPlayer->HasSpell(OPPORTUNITY_RANK5)) && (pPlayer->HasSpell(LETHALITY_RANK1) || pPlayer->HasSpell(LETHALITY_RANK2) || pPlayer->HasSpell(LETHALITY_RANK3) || pPlayer->HasSpell(LETHALITY_RANK4) || pPlayer->HasSpell(LETHALITY_RANK5)))
            GetSpells << "(Dagger/Seal Fate) ";
        else if (!pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK1) && !pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK2) && !pPlayer->HasSpell(IMPROVED_BACKSTAB_RANK3) && !pPlayer->HasSpell(OPPORTUNITY_RANK1) && !pPlayer->HasSpell(OPPORTUNITY_RANK2) && !pPlayer->HasSpell(OPPORTUNITY_RANK3) && !pPlayer->HasSpell(OPPORTUNITY_RANK4) && !pPlayer->HasSpell(OPPORTUNITY_RANK5))
            GetSpells << "(Sword/Seal Fate) ";
    }
    else if (pPlayer->HasSpell(COLD_BLOOD) && pPlayer->HasSpell(PREPARATION))
        GetSpells << "(Cold Blood/Preparation) ";

    if (GetSpells)
        return GetSpells.str().c_str();
    else
        return "";
}

std::string DefineMainHand(Player* pPlayer)
{
    Item* MainHandItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    std::ostringstream MainweaponSub;

    if (MainHandItem)
    {
        uint32 itemId = MainHandItem->GetEntry();
        ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemId);

        switch (item_proto->InventoryType)
        {
            case INVTYPE_2HWEAPON:
            {
                if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_AXE2)
                    MainweaponSub << ("2H Axe");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_MACE2)
                    MainweaponSub << ("2H Mace");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2)
                    MainweaponSub << ("2H Sword");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM)
                    MainweaponSub << ("Polearm");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_STAFF)
                    MainweaponSub << ("Staff");
                break;
            }
            case INVTYPE_WEAPON:
            {
                if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_AXE)
                    MainweaponSub << ("Axe");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_MACE)
                    MainweaponSub << ("Mace");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD)
                    MainweaponSub << ("Sword");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                    MainweaponSub << ("Dagger");
                break;
            }
        }
    }
    if (MainweaponSub)
        return MainweaponSub.str().c_str();
    else
        return "";
}

std::string DefineOffHand(Player* pPlayer)
{
    Item* OffHandItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    std::ostringstream OffweaponSub;

    if (OffHandItem)
    {
        uint32 itemId = OffHandItem->GetEntry();
        ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemId);

        switch (item_proto->InventoryType)
        {
            case INVTYPE_SHIELD:
            {
                OffweaponSub << ("Shield");
                break;
            }
            case INVTYPE_WEAPON:
            {
                if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_AXE)
                    OffweaponSub << ("Axe");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_MACE)
                    OffweaponSub << ("Mace");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_SWORD)
                    OffweaponSub << ("Sword");
                else if (item_proto->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                    OffweaponSub << ("Dagger");
                break;
            }
        }
    }
    if (OffweaponSub)
        return OffweaponSub.str().c_str();
    else
        return "";
}

std::string GetWeaponsString(Player* pPlayer)
{
    std::ostringstream GetWeapons;
    if (DefineMainHand(pPlayer) != "" && DefineOffHand(pPlayer) == "")
        GetWeapons << "(" << DefineMainHand(pPlayer) << ")";
    if (DefineMainHand(pPlayer) != "" && DefineOffHand(pPlayer) != "")
        GetWeapons << "(" << DefineMainHand(pPlayer) << "/" << DefineOffHand(pPlayer) << ")";

    if (GetWeapons)
        return GetWeapons.str().c_str();
    else
        return "";
}

std::string TemplateExportNameString(Player* pPlayer)
{
    uint32 curtalent_spent = 0;
    uint32 WarriorProtectionPoints = 0;
    uint32 WarriorFuryPoints = 0;
    uint32 WarriorArmsPoints = 0;
    uint32 WarlockDemonologyPoints = 0;
    uint32 WarlockDestructionPoints = 0;
    uint32 WarlockAfflictionPoints = 0;
    uint32 ShamanRestorationPoints = 0;
    uint32 ShamanEnhancementPoints = 0;
    uint32 ShamanElementalCombatPoints = 0;
    uint32 RogueSubtletyPoints = 0;
    uint32 RogueCombatPoints = 0;
    uint32 RogueAssassinationPoints = 0;
    uint32 PriestShadowPoints = 0;
    uint32 PriestHolyPoints = 0;
    uint32 PriestDisciplinePoints = 0;
    uint32 PaladinProtectionPoints = 0;
    uint32 PaladinHolyPoints = 0;
    uint32 PaladinRetributionPoints = 0;
    uint32 MageFrostPoints = 0;
    uint32 MageFirePoints = 0;
    uint32 MageArcanePoints = 0;
    uint32 HunterSurvivalPoints = 0;
    uint32 HunterMarksmanshipPoints = 0;
    uint32 HunterBeastMasteryPoints = 0;
    uint32 DruidRestorationPoints = 0;
    uint32 DruidFeralCombatPoints = 0;
    uint32 DruidBalancePoints = 0;

    for (uint32 i = 1; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo) continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if (talentTabInfo->ClassMask != pPlayer->GetClassMask())
            continue;

        uint32 spentPoints = 0;
        for (int j = 0; j < MAX_TALENT_RANK; ++j)
        {
            if (talentInfo->RankID[j] != 0)
            {
                switch (talentInfo->TalentTab)
                {
                    //  WARRIOR
                    case WarriorProtection:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorProtection)
                        {
                            spentPoints += j + 1;
                            WarriorProtectionPoints += spentPoints;
                        }
                        break;
                    }
                    case WarriorFury:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorFury)
                        {
                            spentPoints += j + 1;
                            WarriorFuryPoints += spentPoints;
                        }
                        break;
                    }
                    case WarriorArms:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorArms)
                        {
                            spentPoints += j + 1;
                            WarriorArmsPoints += spentPoints;
                        }
                        break;
                    }
                    // WARLOCK
                    case WarlockDemonology:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDemonology)
                        {
                            spentPoints += j + 1;
                            WarlockDemonologyPoints += spentPoints;
                        }
                        break;
                    }
                    case WarlockDestruction:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDestruction)
                        {
                            spentPoints += j + 1;
                            WarlockDestructionPoints += spentPoints;
                        }
                        break;
                    }
                    case WarlockAffliction:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockAffliction)
                        {
                            spentPoints += j + 1;
                            WarlockAfflictionPoints += spentPoints;
                        }
                        break;
                    }
                    // SHAMAN
                    case ShamanElementalCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanElementalCombat)
                        {
                            spentPoints += j + 1;
                            ShamanElementalCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case ShamanEnhancement:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanEnhancement)
                        {
                            spentPoints += j + 1;
                            ShamanEnhancementPoints += spentPoints;
                        }
                        break;
                    }
                    case ShamanRestoration:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanRestoration)
                        {
                            spentPoints += j + 1;
                            ShamanRestorationPoints += spentPoints;
                        }
                        break;
                    }
                    // ROGUE
                    case RogueAssassination:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueAssassination)
                        {
                            spentPoints += j + 1;
                            RogueAssassinationPoints += spentPoints;
                        }
                        break;
                    }
                    case RogueCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueCombat)
                        {
                            spentPoints += j + 1;
                            RogueCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case RogueSubtlety:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueSubtlety)
                        {
                            spentPoints += j + 1;
                            RogueSubtletyPoints += spentPoints;
                        }
                        break;
                    }
                    // PRIEST
                    case PriestDiscipline:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestDiscipline)
                        {
                            spentPoints += j + 1;
                            PriestDisciplinePoints += spentPoints;
                        }
                        break;
                    }
                    case PriestHoly:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestHoly)
                        {
                            spentPoints += j + 1;
                            PriestHolyPoints += spentPoints;
                        }
                        break;
                    }
                    case PriestShadow:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestShadow)
                        {
                            spentPoints += j + 1;
                            PriestShadowPoints += spentPoints;
                        }
                        break;
                    }
                    // PALADIN
                    case PaladinHoly:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinHoly)
                        {
                            spentPoints += j + 1;
                            PaladinHolyPoints += spentPoints;
                        }
                        break;
                    }
                    case PaladinProtection:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinProtection)
                        {
                            spentPoints += j + 1;
                            PaladinProtectionPoints += spentPoints;
                        }
                        break;
                    }
                    case PaladinRetribution:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinRetribution)
                        {
                            spentPoints += j + 1;
                            PaladinRetributionPoints += spentPoints;
                        }
                        break;
                    }
                    // MAGE
                    case MageArcane:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageArcane)
                        {
                            spentPoints += j + 1;
                            MageArcanePoints += spentPoints;
                        }
                        break;
                    }
                    case MageFire:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFire)
                        {
                            spentPoints += j + 1;
                            MageFirePoints += spentPoints;
                        }
                        break;
                    }
                    case MageFrost:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFrost)
                        {
                            spentPoints += j + 1;
                            MageFrostPoints += spentPoints;
                        }
                        break;
                    }
                    // HUNTER
                    case HunterBeastMastery:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterBeastMastery)
                        {
                            spentPoints += j + 1;
                            HunterBeastMasteryPoints += spentPoints;
                        }
                        break;
                    }
                    case HunterMarksmanship:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterMarksmanship)
                        {
                            spentPoints += j + 1;
                            HunterMarksmanshipPoints += spentPoints;
                        }
                        break;
                    }
                    case HunterSurvival:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterSurvival)
                        {
                            spentPoints += j + 1;
                            HunterSurvivalPoints += spentPoints;
                        }
                        break;
                    }
                    // DRUID
                    case DruidBalance:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidBalance)
                        {
                            spentPoints += j + 1;
                            DruidBalancePoints += spentPoints;
                        }
                        break;
                    }
                    case DruidFeralCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidFeralCombat)
                        {
                            spentPoints += j + 1;
                            DruidFeralCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case DruidRestoration:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidRestoration)
                        {
                            spentPoints += j + 1;
                            DruidRestorationPoints += spentPoints;
                        }
                        break;
                    }
                }
            }
        }
        /* for debugging
        std::ostringstream ss;
        ss << "talentpoints spent " << talentInfo->TalentTab << " / " << curtalent_spent << " / " << spentPoints << ".";
        ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
        */
    }

    std::ostringstream PointsStream;
    DefineMainHand(pPlayer);
    //WARRIOR
    std::ostringstream GetWeapons;
    if (DefineMainHand(pPlayer) != "" && DefineOffHand(pPlayer) == "")
        GetWeapons << "(" << DefineMainHand(pPlayer) << ")";
    if (DefineMainHand(pPlayer) != "" && DefineOffHand(pPlayer) != "")
        GetWeapons << "(" << DefineMainHand(pPlayer) << "/" << DefineOffHand(pPlayer);

    if (WarriorProtectionPoints > WarriorFuryPoints && WarriorProtectionPoints > WarriorArmsPoints)
    {
        if (GetWeaponsString(pPlayer) != "")
            PointsStream << "Protection " << GetWeaponsString(pPlayer) << " " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
        else
            PointsStream << "Protection " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
    }
    else if (WarriorFuryPoints > WarriorProtectionPoints && WarriorFuryPoints > WarriorArmsPoints)
    {
        if (GetWeaponsString(pPlayer) != "")
            PointsStream << "Fury " << GetWeaponsString(pPlayer) << " " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
        else
            PointsStream << "Fury " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
    }
    else if (WarriorArmsPoints > WarriorFuryPoints && WarriorArmsPoints > WarriorProtectionPoints)
    {
        if (GetWeaponsString(pPlayer) != "")
            PointsStream << "Arms " << GetWeaponsString(pPlayer) << " " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
        else
            PointsStream << "Arms " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints;
    }

    //WARLOCK
    if (WarlockAfflictionPoints > WarlockDemonologyPoints && WarlockAfflictionPoints > WarlockDestructionPoints)
    {
        PointsStream << "Affliction " << DefineWarlockSpec(pPlayer) << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints;
    }
    else if (WarlockDemonologyPoints > WarlockAfflictionPoints && WarlockDemonologyPoints > WarlockDestructionPoints)
    {
        PointsStream << "Demonology " << DefineWarlockSpec(pPlayer) << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints;
    }
    else if (WarlockDestructionPoints > WarlockDemonologyPoints && WarlockDestructionPoints > WarlockAfflictionPoints)
    {
        PointsStream << "Destruction " << DefineWarlockSpec(pPlayer) << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints;
    }

    //SHAMAN
    if (ShamanElementalCombatPoints > ShamanEnhancementPoints && ShamanElementalCombatPoints > ShamanRestorationPoints)
    {
        PointsStream << "Elemental " << DefineShamanSpec(pPlayer) << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints;
    }
    else if (ShamanEnhancementPoints > ShamanElementalCombatPoints && ShamanEnhancementPoints > ShamanRestorationPoints)
    {
        PointsStream << "Enhancement " << DefineShamanSpec(pPlayer) << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints;
    }
    else if (ShamanRestorationPoints > ShamanEnhancementPoints && ShamanRestorationPoints > ShamanElementalCombatPoints)
    {
        PointsStream << "Restoration " << DefineShamanSpec(pPlayer) << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints;
    }

    //ROGUE
    if (RogueAssassinationPoints > RogueCombatPoints && RogueAssassinationPoints > RogueSubtletyPoints)
    {
        PointsStream << "Assassination " << DefineRogueSpec(pPlayer) << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints;
    }
    else if (RogueCombatPoints > RogueAssassinationPoints && RogueCombatPoints > RogueSubtletyPoints)
    {
        PointsStream << "Combat " << DefineRogueSpec(pPlayer) << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints;
    }
    else if (RogueSubtletyPoints > RogueCombatPoints && RogueSubtletyPoints > RogueAssassinationPoints)
    {
        PointsStream << "Subtlety " << DefineRogueSpec(pPlayer) << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints;
    }

    //PRIEST
    if (PriestDisciplinePoints > PriestHolyPoints && PriestDisciplinePoints > PriestShadowPoints)
    {
        PointsStream << "Discipline " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints;
    }
    else if (PriestHolyPoints > PriestDisciplinePoints && PriestHolyPoints > PriestShadowPoints)
    {
        PointsStream << "Holy " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints;
    }
    else if (PriestShadowPoints > PriestDisciplinePoints && PriestShadowPoints > PriestHolyPoints)
    {
        PointsStream << "Shadow " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints;
    }

    //PALADIN
    if (PaladinHolyPoints > PaladinProtectionPoints && PaladinHolyPoints > PaladinRetributionPoints)
    {
        PointsStream << "Holy " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints;
    }
    else if (PaladinProtectionPoints > PaladinHolyPoints && PaladinProtectionPoints > PaladinRetributionPoints)
    {
        PointsStream << "Protection " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints;
    }
    else if (PaladinRetributionPoints > PaladinHolyPoints && PaladinRetributionPoints > PaladinProtectionPoints)
    {
        PointsStream << "Retribution " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints;
    }

    //MAGE
    if (MageArcanePoints > MageFirePoints && MageArcanePoints > MageFrostPoints)
    {
        PointsStream << "Arcane " << DefineMageSpec(pPlayer) << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints;
    }
    else if (MageFirePoints > MageArcanePoints && MageFirePoints > MageFrostPoints)
    {
        PointsStream << "Fire " << DefineMageSpec(pPlayer) << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints;
    }
    else if (MageFrostPoints > MageArcanePoints && MageFrostPoints > MageFirePoints)
    {
        PointsStream << "Frost " << DefineMageSpec(pPlayer) << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints;
    }

    //HUNTER
    if (HunterBeastMasteryPoints > HunterMarksmanshipPoints && HunterBeastMasteryPoints > HunterSurvivalPoints)
    {
        PointsStream << "Beast Mastery " << DefineHunterSpec(pPlayer) << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints;
    }
    else if (HunterMarksmanshipPoints > HunterBeastMasteryPoints && HunterMarksmanshipPoints > HunterSurvivalPoints)
    {
        PointsStream << "Marksmanship " << DefineHunterSpec(pPlayer) << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints;
    }
    else if (HunterSurvivalPoints > HunterBeastMasteryPoints && HunterSurvivalPoints > HunterMarksmanshipPoints)
    {
        PointsStream << "Survival " << DefineHunterSpec(pPlayer) << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints;
    }

    //DRUID
    if (DruidBalancePoints > DruidFeralCombatPoints && DruidBalancePoints > DruidRestorationPoints)
    {
        PointsStream << "Balance " << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints;
    }
    else if (DruidFeralCombatPoints > DruidBalancePoints && DruidFeralCombatPoints > DruidRestorationPoints)
    {
        PointsStream << "Feral " << DefineDruidSpec(pPlayer) << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints;
    }
    else if (DruidRestorationPoints > DruidBalancePoints && DruidRestorationPoints > DruidFeralCombatPoints)
    {
        PointsStream << "Restoration " << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints;
    }

    uint32 ilvl = AverageItemLevel(pPlayer);

        std::ostringstream exportStream;
        exportStream << (PointsStream.str().c_str()) << ", iLvL " << ilvl;
        return (exportStream.str().c_str());

}

uint16 GetSpecIDByTalentPoints(Player* pPlayer)
{
    uint32 curtalent_spent = 0;
    uint32 WarriorProtectionPoints = 0;
    uint32 WarriorFuryPoints = 0;
    uint32 WarriorArmsPoints = 0;
    uint32 WarlockDemonologyPoints = 0;
    uint32 WarlockDestructionPoints = 0;
    uint32 WarlockAfflictionPoints = 0;
    uint32 ShamanRestorationPoints = 0;
    uint32 ShamanEnhancementPoints = 0;
    uint32 ShamanElementalCombatPoints = 0;
    uint32 RogueSubtletyPoints = 0;
    uint32 RogueCombatPoints = 0;
    uint32 RogueAssassinationPoints = 0;
    uint32 PriestShadowPoints = 0;
    uint32 PriestHolyPoints = 0;
    uint32 PriestDisciplinePoints = 0;
    uint32 PaladinProtectionPoints = 0;
    uint32 PaladinHolyPoints = 0;
    uint32 PaladinRetributionPoints = 0;
    uint32 MageFrostPoints = 0;
    uint32 MageFirePoints = 0;
    uint32 MageArcanePoints = 0;
    uint32 HunterSurvivalPoints = 0;
    uint32 HunterMarksmanshipPoints = 0;
    uint32 HunterBeastMasteryPoints = 0;
    uint32 DruidRestorationPoints = 0;
    uint32 DruidFeralCombatPoints = 0;
    uint32 DruidBalancePoints = 0;

    for (uint32 i = 1; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo) continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if (talentTabInfo->ClassMask != pPlayer->GetClassMask())
            continue;

        uint32 spentPoints = 0;
        for (int j = 0; j < MAX_TALENT_RANK; ++j)
        {
            if (talentInfo->RankID[j] != 0)
            {
                switch (talentInfo->TalentTab)
                {
                    //  WARRIOR
                    case WarriorProtection:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorProtection)
                        {
                            spentPoints += j + 1;
                            WarriorProtectionPoints += spentPoints;
                        }
                        break;
                    }
                    case WarriorFury:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorFury)
                        {
                            spentPoints += j + 1;
                            WarriorFuryPoints += spentPoints;
                        }
                        break;
                    }
                    case WarriorArms:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorArms)
                        {
                            spentPoints += j + 1;
                            WarriorArmsPoints += spentPoints;
                        }
                        break;
                    }
                    // WARLOCK
                    case WarlockDemonology:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDemonology)
                        {
                            spentPoints += j + 1;
                            WarlockDemonologyPoints += spentPoints;
                        }
                        break;
                    }
                    case WarlockDestruction:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDestruction)
                        {
                            spentPoints += j + 1;
                            WarlockDestructionPoints += spentPoints;
                        }
                        break;
                    }
                    case WarlockAffliction:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockAffliction)
                        {
                            spentPoints += j + 1;
                            WarlockAfflictionPoints += spentPoints;
                        }
                        break;
                    }
                    // SHAMAN
                    case ShamanElementalCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanElementalCombat)
                        {
                            spentPoints += j + 1;
                            ShamanElementalCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case ShamanEnhancement:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanEnhancement)
                        {
                            spentPoints += j + 1;
                            ShamanEnhancementPoints += spentPoints;
                        }
                        break;
                    }
                    case ShamanRestoration:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanRestoration)
                        {
                            spentPoints += j + 1;
                            ShamanRestorationPoints += spentPoints;
                        }
                        break;
                    }
                    // ROGUE
                    case RogueAssassination:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueAssassination)
                        {
                            spentPoints += j + 1;
                            RogueAssassinationPoints += spentPoints;
                        }
                        break;
                    }
                    case RogueCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueCombat)
                        {
                            spentPoints += j + 1;
                            RogueCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case RogueSubtlety:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueSubtlety)
                        {
                            spentPoints += j + 1;
                            RogueSubtletyPoints += spentPoints;
                        }
                        break;
                    }
                    // PRIEST
                    case PriestDiscipline:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestDiscipline)
                        {
                            spentPoints += j + 1;
                            PriestDisciplinePoints += spentPoints;
                        }
                        break;
                    }
                    case PriestHoly:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestHoly)
                        {
                            spentPoints += j + 1;
                            PriestHolyPoints += spentPoints;
                        }
                        break;
                    }
                    case PriestShadow:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestShadow)
                        {
                            spentPoints += j + 1;
                            PriestShadowPoints += spentPoints;
                        }
                        break;
                    }
                    // PALADIN
                    case PaladinHoly:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinHoly)
                        {
                            spentPoints += j + 1;
                            PaladinHolyPoints += spentPoints;
                        }
                        break;
                    }
                    case PaladinProtection:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinProtection)
                        {
                            spentPoints += j + 1;
                            PaladinProtectionPoints += spentPoints;
                        }
                        break;
                    }
                    case PaladinRetribution:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinRetribution)
                        {
                            spentPoints += j + 1;
                            PaladinRetributionPoints += spentPoints;
                        }
                        break;
                    }
                    // MAGE
                    case MageArcane:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageArcane)
                        {
                            spentPoints += j + 1;
                            MageArcanePoints += spentPoints;
                        }
                        break;
                    }
                    case MageFire:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFire)
                        {
                            spentPoints += j + 1;
                            MageFirePoints += spentPoints;
                        }
                        break;
                    }
                    case MageFrost:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFrost)
                        {
                            spentPoints += j + 1;
                            MageFrostPoints += spentPoints;
                        }
                        break;
                    }
                    // HUNTER
                    case HunterBeastMastery:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterBeastMastery)
                        {
                            spentPoints += j + 1;
                            HunterBeastMasteryPoints += spentPoints;
                        }
                        break;
                    }
                    case HunterMarksmanship:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterMarksmanship)
                        {
                            spentPoints += j + 1;
                            HunterMarksmanshipPoints += spentPoints;
                        }
                        break;
                    }
                    case HunterSurvival:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterSurvival)
                        {
                            spentPoints += j + 1;
                            HunterSurvivalPoints += spentPoints;
                        }
                        break;
                    }
                    // DRUID
                    case DruidBalance:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidBalance)
                        {
                            spentPoints += j + 1;
                            DruidBalancePoints += spentPoints;
                        }
                        break;
                    }
                    case DruidFeralCombat:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidFeralCombat)
                        {
                            spentPoints += j + 1;
                            DruidFeralCombatPoints += spentPoints;
                        }
                        break;
                    }
                    case DruidRestoration:
                    {
                        if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidRestoration)
                        {
                            spentPoints += j + 1;
                            DruidRestorationPoints += spentPoints;
                        }
                        break;
                    }
                }
            }
        }
        /* for debugging
        std::ostringstream ss;
        ss << "talentpoints spent " << talentInfo->TalentTab << " / " << curtalent_spent << " / " << spentPoints << ".";
        ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
        */
    }

    uint16 SpecID = 0;

    //WARRIOR
    if (WarriorProtectionPoints > WarriorFuryPoints && WarriorProtectionPoints > WarriorArmsPoints)
    {
        SpecID = 163;
    }
    else if (WarriorFuryPoints > WarriorProtectionPoints && WarriorFuryPoints > WarriorArmsPoints)
    {
        SpecID = 164;
    }
    else if (WarriorArmsPoints > WarriorFuryPoints && WarriorArmsPoints > WarriorProtectionPoints)
    {
        SpecID = 161;
    }

    //WARLOCK
    if (WarlockAfflictionPoints > WarlockDemonologyPoints && WarlockAfflictionPoints > WarlockDestructionPoints)
    {
        SpecID = 302;
    }
    else if (WarlockDemonologyPoints > WarlockAfflictionPoints && WarlockDemonologyPoints > WarlockDestructionPoints)
    {
        SpecID = 303;
    }
    else if (WarlockDestructionPoints > WarlockDemonologyPoints && WarlockDestructionPoints > WarlockAfflictionPoints)
    {
        SpecID = 301;
    }

    //SHAMAN
    if (ShamanElementalCombatPoints > ShamanEnhancementPoints && ShamanElementalCombatPoints > ShamanRestorationPoints)
    {
        SpecID = 261;
    }
    else if (ShamanEnhancementPoints > ShamanElementalCombatPoints && ShamanEnhancementPoints > ShamanRestorationPoints)
    {
        SpecID = 263;
    }
    else if (ShamanRestorationPoints > ShamanEnhancementPoints && ShamanRestorationPoints > ShamanElementalCombatPoints)
    {
        SpecID = 262;
    }

    //ROGUE
    if (RogueAssassinationPoints > RogueCombatPoints && RogueAssassinationPoints > RogueSubtletyPoints)
    {
        SpecID = 182;
    }
    else if (RogueCombatPoints > RogueAssassinationPoints && RogueCombatPoints > RogueSubtletyPoints)
    {
        SpecID = 181;
    }
    else if (RogueSubtletyPoints > RogueCombatPoints && RogueSubtletyPoints > RogueAssassinationPoints)
    {
        SpecID = 183;
    }

    //PRIEST
    if (PriestDisciplinePoints > PriestHolyPoints && PriestDisciplinePoints > PriestShadowPoints)
    {
        SpecID = 201;
    }
    else if (PriestHolyPoints > PriestDisciplinePoints && PriestHolyPoints > PriestShadowPoints)
    {
        SpecID = 202;
    }
    else if (PriestShadowPoints > PriestDisciplinePoints && PriestShadowPoints > PriestHolyPoints)
    {
        SpecID = 203;
    }

    //PALADIN
    if (PaladinHolyPoints > PaladinProtectionPoints && PaladinHolyPoints > PaladinRetributionPoints)
    {
        SpecID = 382;
    }
    else if (PaladinProtectionPoints > PaladinHolyPoints && PaladinProtectionPoints > PaladinRetributionPoints)
    {
        SpecID = 383;
    }
    else if (PaladinRetributionPoints > PaladinHolyPoints && PaladinRetributionPoints > PaladinProtectionPoints)
    {
        SpecID = 381;
    }

    //MAGE
    if (MageArcanePoints > MageFirePoints && MageArcanePoints > MageFrostPoints)
    {
        SpecID = 81;
    }
    else if (MageFirePoints > MageArcanePoints && MageFirePoints > MageFrostPoints)
    {
        SpecID = 41;
    }
    else if (MageFrostPoints > MageArcanePoints && MageFrostPoints > MageFirePoints)
    {
        SpecID = 61;
    }

    //HUNTER
    if (HunterBeastMasteryPoints > HunterMarksmanshipPoints && HunterBeastMasteryPoints > HunterSurvivalPoints)
    {
        SpecID = 361;
    }
    else if (HunterMarksmanshipPoints > HunterBeastMasteryPoints && HunterMarksmanshipPoints > HunterSurvivalPoints)
    {
        SpecID = 363;
    }
    else if (HunterSurvivalPoints > HunterBeastMasteryPoints && HunterSurvivalPoints > HunterMarksmanshipPoints)
    {
        SpecID = 362;
    }

    //DRUID
    if (DruidBalancePoints > DruidFeralCombatPoints && DruidBalancePoints > DruidRestorationPoints)
    {
        SpecID = 283;
    }
    else if (DruidFeralCombatPoints > DruidBalancePoints && DruidFeralCombatPoints > DruidRestorationPoints)
    {
        SpecID = 281;
    }
    else if (DruidRestorationPoints > DruidBalancePoints && DruidRestorationPoints > DruidFeralCombatPoints)
    {
        SpecID = 282;
    }

    return SpecID;
}

enum QuestSpellsProficiencies
{
    HUNTER_DISMISS_PET = 2641,
    HUNTER_CALL_PET = 883,
    HUNTER_TAME_BEAST = 1515,
    HUNTER_BEAST_TRAINING = 5149,
    HUNTER_FEED_PET = 6991,
    HUNTER_REVIVE_PET = 982,
    HUNTER_TRANQUILIZING_SHOT = 19801,

    ROGUE_POISONS = 2842,
    ROGUE_DEADLY_POISON_V = 25347,

    WARRIOR_DEFENSIVE_STANCE = 71,
    WARRIOR_SUNDER_ARMOR = 7386,
    WARRIOR_TAUNT = 355,
    WARRIOR_BERSERKER_STANCE = 2458,
    WARRIOR_INTERCEPT = 20252,

    WARLOCK_SUMMON_VOIDWALKER = 697,
    WARLOCK_SUMMON_SUCCUBUS = 712,
    WARLOCK_SUMMON_IMP = 688,
    WARLOCK_SUMMON_FELHUNTER = 691,
    WARLOCK_INFERNO = 1122,
    WARLOCK_SUMMON_FELSTEED = 5784,
    WARLOCK_SUMMON_DREADSTEED = 23161,

    MAGE_POLYMORPH_PIG = 28272,
    MAGE_POLYMORPH_TURTLE = 28271,
    MAGE_POLYMORPH_COW = 28270,
    MAGE_ARCANE_BRILLIANCE = 23028,

    SHAMAN_SEARING_TOTEM = 3599,
    SHAMAN_HEALING_STREAM_TOTEM = 5394,
    SHAMAN_STONESKIN_TOTEM = 8071,

    PRIEST_DEVOURING_PLAGUE = 2944,
    PRIEST_HEX_OF_WEAKNESS = 9035,
    PRIEST_TOUCH_OF_WEAKNESS = 2652,
    PRIEST_STARSHARDS = 10797,
    PRIEST_ELUNES_GRACE = 2651,
    PRIEST_SHADOWGUARD = 18137,
    PRIEST_FEAR_WARD = 6346,
    PRIEST_FEEDBACK = 13896,
    PRIEST_DESPERATE_PRAYER = 13908,
    PRIEST_PRAYER_OF_SHADOW_PROTECTION = 27683,
    PRIEST_PRAYER_OF_FORTITUDE = 21564,

    PALADIN_REDEMPTION = 7328,
    PALADIN_SUMMON_CHARGER = 23214,

    DRUID_CURE_POISON = 8946,
    DRUID_AQUATIC_FORM = 1066,
    DRUID_BEAR_FORM = 5487,
    DRUID_GROWL = 6795,
    DRUID_MAUL = 6807,
    DRUID_GIFT_OF_THE_WILD = 21850,
};

void MoveToNPC(Player* pPlayer, uint32 npc_id)
{
    if (!pPlayer)
        return;

    if (!npc_id)
        return;

    if (Creature* NPC = pPlayer->FindNearestCreature(npc_id, 200.0f))
    {
        float fX, fY, fZ;
        NPC->GetPosition(fX, fY, fZ);
        pPlayer->CastSpell(pPlayer, 23978, true);
        pPlayer->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_PATHFINDING);
        pPlayer->SetTargetGuid(NPC->GetObjectGuid());
    }
}

void ApplyBonus(Player* pPlayer, Item* item, EnchantmentSlot slot, uint32 bonusEntry, uint32 duration, uint32 charges)
{
    if (!pPlayer)
        return;

    if (!item)
        return;

    if (!bonusEntry || bonusEntry == 0)
        return;

    pPlayer->_ApplyItemMods(item, slot, false);
    item->SetEnchantment(slot, bonusEntry, duration, charges);
    pPlayer->_ApplyItemMods(item, slot, true);
}

void LearnQuestSpells(Player* pPlayer)
{
    if (!pPlayer)
        return;

    uint32 reqLevel = pPlayer->GetLevel();

    std::vector<uint32> questspells;

    uint32 temp[] = {
        HUNTER_DISMISS_PET, HUNTER_CALL_PET, HUNTER_TAME_BEAST, HUNTER_BEAST_TRAINING, HUNTER_FEED_PET, HUNTER_REVIVE_PET, HUNTER_TRANQUILIZING_SHOT,
        ROGUE_POISONS, ROGUE_DEADLY_POISON_V,
        WARRIOR_DEFENSIVE_STANCE, WARRIOR_SUNDER_ARMOR, WARRIOR_TAUNT, WARRIOR_BERSERKER_STANCE, WARRIOR_INTERCEPT,
        WARLOCK_SUMMON_VOIDWALKER, WARLOCK_SUMMON_SUCCUBUS, WARLOCK_SUMMON_IMP, WARLOCK_SUMMON_FELHUNTER, WARLOCK_INFERNO, WARLOCK_SUMMON_FELSTEED, WARLOCK_SUMMON_DREADSTEED,
        MAGE_POLYMORPH_PIG, MAGE_POLYMORPH_TURTLE, MAGE_POLYMORPH_COW, MAGE_ARCANE_BRILLIANCE,
        SHAMAN_SEARING_TOTEM, SHAMAN_HEALING_STREAM_TOTEM, SHAMAN_STONESKIN_TOTEM,
        PRIEST_DEVOURING_PLAGUE, PRIEST_HEX_OF_WEAKNESS, PRIEST_TOUCH_OF_WEAKNESS, PRIEST_STARSHARDS, PRIEST_ELUNES_GRACE, PRIEST_SHADOWGUARD, PRIEST_FEAR_WARD, PRIEST_FEEDBACK, PRIEST_DESPERATE_PRAYER, PRIEST_PRAYER_OF_SHADOW_PROTECTION, PRIEST_PRAYER_OF_FORTITUDE,
        PALADIN_REDEMPTION, PALADIN_SUMMON_CHARGER,
        DRUID_CURE_POISON, DRUID_AQUATIC_FORM, DRUID_BEAR_FORM, DRUID_GROWL, DRUID_MAUL, DRUID_GIFT_OF_THE_WILD,
    };

    questspells = std::vector<uint32>(temp, temp + sizeof(temp) / sizeof(temp[0]));

    for (std::vector<uint32>::const_iterator itr = questspells.begin(); itr != questspells.end(); ++itr)
    {

        SpellEntry const* spell = sSpellMgr.GetSpellEntry(*itr);

        if (!spell)
            continue;

        if (!pPlayer->IsSpellFitByClassAndRace(spell->Id))
            continue;

        uint32 spellLevel = spell->spellLevel;
        if (pPlayer->GetLevel() < spellLevel)
            continue;

        if (pPlayer->HasSpell(spell->Id))
            continue;

        pPlayer->LearnSpell(spell->Id, false);
    }
}

uint32 GetTemplateID()
{
    QueryResult* TalentIdResult = CharacterDatabase.PQuery("SELECT MAX(temp_id) FROM template_npc_talents");
    QueryResult* GearIdResult = CharacterDatabase.PQuery("SELECT MAX(temp_id) FROM template_npc_gear");

    uint32 TalentId{};
    uint32 GearId{};

    if (TalentIdResult)
    {
        Field* fields = TalentIdResult->Fetch();
        TalentId = fields[0].GetInt32();
        delete TalentIdResult;
    }
    if (GearIdResult)
    {
        Field* fields = GearIdResult->Fetch();
        GearId = fields[0].GetInt32();
        delete GearIdResult;
    }
    if (GearId = TalentId)
        return GearId + 1;
    else
        return 50;
}

void perform_npc_vendor_template()
{
    //QueryResult *result = WorldDatabase.PQuery("SELECT item FROM npc_vendor_template WHERE entry='15127'");
    QueryResult* result = WorldDatabase.PQuery("SELECT item FROM npc_vendor WHERE entry='15127'");

    //12799 15127

    if (result)
    {
        std::string alliance_id_name;

        do
        {
            Field *fields = result->Fetch();

            uint32 entry = fields[0].GetUInt32();

            if (entry)
            {
                if (ItemPrototype const* alliance_id_proto = ObjectMgr::GetItemPrototype(entry))
                    alliance_id_name = alliance_id_proto->Name1;

                QueryResult *founditem = WorldDatabase.PQuery("SELECT alliance_id FROM player_factionchange_items WHERE alliance_id='%u'", entry);

                if (!founditem)
                    WorldDatabase.PExecute("INSERT INTO player_factionchange_items (`alliance_id`, `horde_id`, `comment`) VALUES ('%u', '%u', '%s');", entry, 0, "todo");
            }

        } while (result->NextRow());

        delete result;
    }
}

void perform_player_factionchange_items()
{
    QueryResult* result = WorldDatabase.PQuery("SELECT alliance_id, horde_id, comment FROM player_factionchange_items");

    if (result)
    {
        std::string alliance_id_name;
        std::string horde_id_name;

        do
        {
            Field *fields = result->Fetch();

            uint32 alliance_id = fields[0].GetUInt32();
            uint32 horde_id = fields[1].GetUInt32();

            if (alliance_id && horde_id)
            {
                if (ItemPrototype const* alliance_id_proto = ObjectMgr::GetItemPrototype(alliance_id))
                    alliance_id_name = alliance_id_proto->Name1;

                if (ItemPrototype const* horde_id_proto = ObjectMgr::GetItemPrototype(horde_id))
                    horde_id_name = horde_id_proto->Name1;

                std::replace(alliance_id_name.begin(), alliance_id_name.end(), '\'', '\''); // replace quotes or Server sql will crash.
                std::replace(horde_id_name.begin(), horde_id_name.end(), '\'', '"');

                std::ostringstream QueryText;

                QueryText << alliance_id_name << " / " << horde_id_name;

                WorldDatabase.PExecute("UPDATE player_factionchange_items SET comment = '%s' WHERE alliance_id='%u'", QueryText.str().c_str(), alliance_id);
            }

        } while (result->NextRow());

        delete result;
    }
}

void sTemplateNPC::LoadTalentsContainer()
{
    for (TemplateNpcTalentsMap::const_iterator itr = m_TemplateNpcTalentsMap.begin(); itr != m_TemplateNpcTalentsMap.end(); ++itr)
        delete* itr;

    m_TemplateNpcTalentsMap.clear();

    uint32 oldMSTime = WorldTimer::getMSTime();
    uint32 count = 0;

    QueryResult* result = CharacterDatabase.Query("SELECT * FROM template_npc_talents;");

    if (!result)
    {
        sLog.outError(">> Loaded 0 talent templates. DB table `template_npc_talents` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        template_npc_talents* pTalent = new template_npc_talents;

        pTalent->temp_id    = fields[0].GetUInt32();
        pTalent->class_name = fields[1].GetString();
        pTalent->talent_id  = fields[2].GetUInt32();
        pTalent->rank       = fields[2].GetUInt32();

        m_TemplateNpcTalentsMap.push_back(pTalent);
        ++count;
    } while (result->NextRow());
    sLog.outString(">> Loaded %u talent templates.", count);
}

void ExtractGearTemplateToDB(Player* pPlayer, std::string& gossipTempText)
{
    if (!pPlayer)
        return;

    uint32 TempID = GetTemplateID();
    uint32 patch = 0;
    uint32 item_entry_human = 0;
    uint32 item_entry_orc = 0;
    uint32 item_entry_dwarf = 0;
    uint32 item_entry_troll = 0;
    std::string gossipStr = "empty";

    if (gossipTempText != "")
        gossipStr = gossipTempText;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            uint32 itemId = equippedItem->GetEntry();

            QueryResult *getpatch = WorldDatabase.PQuery("SELECT patch FROM item_template "
                "WHERE entry = '%u'", itemId);

            if (getpatch)
            {
                Field* fields = getpatch->Fetch();
                uint32 newpatch = fields[0].GetInt32();
                patch = newpatch;
                delete getpatch;
            }

            if (pPlayer->GetRace() == RACE_ORC)
                item_entry_orc = equippedItem->GetEntry();

            if (pPlayer->GetRace() == RACE_HUMAN)
                item_entry_human = equippedItem->GetEntry();

            if (pPlayer->GetRace() == RACE_DWARF)
                item_entry_dwarf = equippedItem->GetEntry();

            if (pPlayer->GetRace() == RACE_TROLL)
                item_entry_troll = equippedItem->GetEntry();

            CharacterDatabase.PExecute("INSERT INTO template_npc_gear (`temp_id`, `class`, `gossip_text`, `item_slot`, `item_entry`, `item_entry_human`, `item_entry_orc`, `item_entry_dwarf`, `item_entry_troll`, `item_enchant`, `randomPropertyId`, `talent_tab_id`, `patch`) VALUES ('%u', '%s', '%s', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');"
                , TempID, GetClassString(pPlayer).c_str(), gossipStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), item_entry_human, item_entry_orc, item_entry_dwarf, item_entry_troll, equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT), equippedItem->GetItemRandomPropertyId(), GetSpecIDByTalentPoints(pPlayer), patch);
        }

        //CharacterDatabase.PExecute("UPDATE template_npc_gear SET patch = '%u' WHERE temp_id='%u'", patch, TempID);
    }
}

bool IsIgnoredSpell(uint32 spellID)
{
    std::vector<uint32> ignoreSpells;

    uint32 temp[] = {
        877, //Elemental Fury
        1868 //zzOLDHoly Flurry
    };

    ignoreSpells = std::vector<uint32>(temp, temp + sizeof(temp) / sizeof(temp[0]));

    for (std::vector<uint32>::const_iterator itr = ignoreSpells.begin(); itr != ignoreSpells.end(); ++itr)
        if (spellID == (*itr))
            return true;
    return false;
}

void LearnSkillRecipes(Player* pPlayer, uint32 skill_id)
{
    uint32 classmask = pPlayer->GetClassMask();

    for (uint32 j = 0; j < sObjectMgr.GetMaxSkillLineAbilityId(); ++j)
    {
        SkillLineAbilityEntry const *skillLine = sObjectMgr.GetSkillLineAbility(j);
        if (!skillLine)
            continue;

        // wrong skill
        if (skillLine->skillId != skill_id)
            continue;

        // not high rank
        if (skillLine->forward_spellid)
            continue;

        // skip racial skills
        if (skillLine->racemask != 0)
            continue;

        // skip wrong class skills
        if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
            continue;

        SpellEntry const* spellEntry = sSpellMgr.GetSpellEntry(skillLine->spellId);
        if (!spellEntry || !SpellMgr::IsSpellValid(spellEntry, pPlayer, false))
            continue;

        pPlayer->LearnSpell(skillLine->spellId, false);
    }
}

bool LearnAllRecipesProfession(Player *pPlayer, SkillType skill)
{
    ChatHandler handler(pPlayer->GetSession());
    char* skill_name;

    SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
    skill_name = SkillInfo->name[sWorld.GetDefaultDbcLocale()];

    if (!SkillInfo)
    {
        sLog.outError("Profession NPC: received non-valid skill ID");
        return false;
    }

    pPlayer->SetSkill(SkillInfo->id, 300, 300);
    LearnSkillRecipes(pPlayer, SkillInfo->id);
    ChatHandler(pPlayer->GetSession()).PSendSysMessage("All recipes for %s learned", skill_name);

    return true;
}

void LearnProfession(Player *pPlayer, SkillType skill)
{
    if (pPlayer->GetFreePrimaryProfessionPoints() == 0 && !(skill == SKILL_COOKING || skill == SKILL_FIRST_AID))
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("You already know two primary professions.");
    else
    {
        if (!LearnAllRecipesProfession(pPlayer, skill))
            ChatHandler(pPlayer->GetSession()).PSendSysMessage("Internal error.");
    }
}

void LearnAllWildCreaturesPetSpell(Player* pPlayer)
{
    if (pPlayer->GetClass() != CLASS_HUNTER)
        return;

    SkillLineAbilityMapBounds bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySkillId(SKILL_BEAST_TRAINING);

    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (SkillLineAbilityEntry const* pAbility = itr->second)
        {
            if (SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(pAbility->spellId))
                if (pSpellEntry->spellLevel <= pPlayer->GetLevel() && !pPlayer->HasSpell(pAbility->spellId))
                    pPlayer->LearnSpell(pAbility->spellId, false);
        }
    }
}

void LearnAllWarlockPetSpellforSkillID(Pet* pPet, SkillType SkillID)
{
    if (pPet->GetOwner()->GetClass() != CLASS_WARLOCK)
        return;

    SkillLineAbilityMapBounds bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySkillId(SkillID);

    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (SkillLineAbilityEntry const* pAbility = itr->second)
        {
            if (SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(pAbility->spellId))
                if (pSpellEntry->spellLevel <= pPet->GetLevel() && !pPet->HasSpell(pAbility->spellId))
                    pPet->LearnSpell(pAbility->spellId);
        }
    }
}

void LearnWarlockPetSpells(Player* pPlayer)
{
    if (!pPlayer)
        return;

    if (pPlayer->GetClass() != CLASS_WARLOCK)
        return;

    if (Pet* pPet = pPlayer->GetPet())
    {
        if (pPet->getPetType() != SUMMON_PET)
            return;

        uint32 petentry = pPet->GetEntry();

        if (!petentry)
            return;

        switch (petentry)
        {
            case WARLOCK_PET_IMP:
            {
                LearnAllWarlockPetSpellforSkillID(pPet, SKILL_PET_IMP);
                break;
            }
            case WARLOCK_PET_SUCCUBUS:
            {
                LearnAllWarlockPetSpellforSkillID(pPet, SKILL_PET_SUCCUBUS);
                break;
            }
            case WARLOCK_PET_VOIDWALKER:
            {
                LearnAllWarlockPetSpellforSkillID(pPet, SKILL_PET_VOIDWALKER);
                break;
            }
            case WARLOCK_PET_FELHUNTER:
            {
                LearnAllWarlockPetSpellforSkillID(pPet, SKILL_PET_FELHUNTER);
                break;
            }
        }
        pPet->SendSpellGo(pPet, COOL_VISUAL_SPELL_2);
    }
}

void ResetPetFromPlayer(Player* pPlayer)
{
    Pet* pet = pPlayer->GetPet();

    if (!pet)
        return;

    if (pet->getPetType() != HUNTER_PET || pet->m_petSpells.size() <= 1)
        return;

    pet->GivePetLevel(pPlayer->GetLevel());

    if (pet->m_petSpells.size() <= 1)
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetUnlearnOpcode: %s is considered pet-like but doesn't have a charminfo!", pet->GetGuidStr().c_str());
        return;
    }

    for (PetSpellMap::iterator itr = pet->m_petSpells.begin(); itr != pet->m_petSpells.end();)
    {
        uint32 spell_id = itr->first;                       // Pet::RemoveSpell can invalidate iterator at erase NEW spell
        ++itr;
        pet->unlearnSpell(spell_id, false);
    }

    pet->SetTP(pet->GetLevel() * (pet->GetLoyaltyLevel() - 1));

    for (int i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
        if (UnitActionBarEntry const* ab = charmInfo->GetActionBarEntry(i))
            if (ab->GetAction() && ab->IsActionBarForSpell())
                charmInfo->SetActionBar(i, 0, ACT_DISABLED);

    // relearn pet passives
    pet->LearnPetPassives();

    pet->m_resetTalentsTime = time(NULL);

    pPlayer->PetSpellInitialize();
}

void CreateHunterPet(Player* pPlayer, Creature* pCreature, uint32 entry)
{
    if (!pPlayer)
        return;

    if (!pCreature)
        return;

    if (pPlayer->GetClass() != CLASS_HUNTER)
        return;

    if (Pet* pet = pPlayer->GetPet())
    {
        if (pet)
        {
            //pPlayer->CastSpell(player, 883, true); // Call Pet. If you don't do this and your pet is dismissed the Server will crash. Need to find a fix.
            pPlayer->GetSession()->SendAreaTriggerMessage("You already have a Pet.");
            return;
        }

        if (pet->getPetType() == HUNTER_PET)
        {
            pet->Unsummon(PET_SAVE_AS_DELETED, pPlayer);
        }
    }

    if (Creature* pBeast = pCreature->SummonCreature(entry, pPlayer->GetPositionX(), pPlayer->GetPositionY() + 2, pPlayer->GetPositionZ(), pPlayer->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN))
    {

        if (!pBeast)
            return;

        Pet* pet = new Pet(HUNTER_PET);

        pet->SetReactState(REACT_DEFENSIVE);

        if (!pet->CreateBaseAtCreature(pBeast))
        {
            delete pet;
            return;
        }

        pet->SetOwnerGuid(pPlayer->GetObjectGuid());
        pet->SetCreatorGuid(pPlayer->GetObjectGuid());
        pet->SetFactionTemplateId(pPlayer->GetFactionTemplateId());

        if (pPlayer->IsPvP())
            pet->SetPvP(true);

        if (!pet->InitStatsForLevel(pPlayer->GetLevel()))
        {
            sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!", pBeast->GetEntry());
            delete pet;
            return;
        }

        pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
        // this enables pet details window (Shift+P)
        pet->AIM_Initialize();
        pet->InitPetCreateSpells();
        pet->SetHealth(pet->GetMaxHealth());

        // "kill" original creature
        pBeast->ForcedDespawn();

        while (pet->GetLoyaltyLevel() != BEST_FRIEND)
            pet->ModifyLoyalty(pet->GetStartLoyaltyPoints(BEST_FRIEND));

        // add to world
        pet->GetMap()->Add((Creature*)pet);

        // caster have pet now
        pPlayer->SetPet(pet);

        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        pPlayer->PetSpellInitialize();
        pet->SendSpellGo(pet, COOL_VISUAL_SPELL_3);
    }
}

// Cause of the AQ ClassBooks etc.
void UpgradePlayerSpellsToMax(Player* pPlayer)
{
    if (!pPlayer)
        return;

    ChrClassesEntry const* clsEntry = sChrClassesStore.LookupEntry(pPlayer->GetClass());
    if (!clsEntry)
        return;

    uint32 family = clsEntry->spellfamily;

    PlayerSpellMap m_spells = pPlayer->GetSpellMap();

    for (PlayerSpellMap::iterator itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
        //Check for higher ranks that are not from Trainer (AQ Class books etc.)
        SpellChainMapNext const& nextMap = sSpellMgr.GetSpellChainNext();
        for (SpellChainMapNext::const_iterator i = nextMap.lower_bound(itr->first); i != nextMap.upper_bound(itr->first); ++i)
        {
            PlayerSpellMap::iterator iter = pPlayer->GetSpellMap().find(i->second);

            SpellEntry const* Highspell = sSpellMgr.GetSpellEntry(i->second);
            if (!Highspell)
                continue;

            // known spell
            if (pPlayer->HasSpell(Highspell->Id))
                continue;

            if (IsIgnoredSpell(Highspell->Id))
                continue;

            // check race/class requirement
            if (!pPlayer->IsSpellFitByClassAndRace(Highspell->Id))
                continue;

            // check level requirement
            uint32 spellLevel = Highspell->spellLevel;
            if (pPlayer->GetLevel() < spellLevel)
                continue;

            if (!SpellMgr::IsSpellValid(Highspell, pPlayer, false))
                continue;

            // skip other spell families
            if (Highspell->SpellFamilyName != family)
                continue;

            // skip spells learned as talent
            if (GetTalentSpellCost(Highspell->Id) > 0)
                continue;

            if (SpellChainNode const* spell_chain = sSpellMgr.GetSpellChainNode(Highspell->Id))
            {
                // check prev.rank requirement
                if (spell_chain->prev && !pPlayer->HasSpell(spell_chain->prev))
                    continue;

                // check additional spell requirement
                if (spell_chain->req && !pPlayer->HasSpell(spell_chain->req))
                    continue;
            }

            pPlayer->LearnSpell(Highspell->Id, false);
        }
    }
}

void LearnAllTrainerSpells(Player* pPlayer, uint32 TrainerID)
{
    if (!pPlayer)
        return;

    if (!TrainerID)
        return;

    bool AllSpellsKnown = true;
    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(TrainerID);

    if (!cInfo)
        return;

    if (TrainerSpellData const* cSpells = cInfo->trainer_id ? sObjectMgr.GetNpcTrainerTemplateSpells(cInfo->trainer_id) : sObjectMgr.GetNpcTrainerSpells(TrainerID))
    {
        for (TrainerSpellMap::const_iterator itr = cSpells->spellList.begin(); itr != cSpells->spellList.end(); ++itr)
        {
            TrainerSpell const* tSpell = &itr->second;

            if (pPlayer->GetTrainerSpellState(tSpell) > TRAINER_SPELL_GREEN)
            {
                if (pPlayer->HasSpell(tSpell->spell))
                    pPlayer->RemoveSpell(tSpell->spell, false, true);
                continue;
            }

            SpellEntry const* spell = sSpellMgr.GetSpellEntry(tSpell->spell);
            SpellEntry const* TriggerSpell = sSpellMgr.GetSpellEntry(spell->EffectTriggerSpell[0]);

            if (TriggerSpell)
            {
                AllSpellsKnown = false;
                pPlayer->LearnSpell(TriggerSpell->Id, false);
            }
        }
    }
    else
    {
        sLog.outBasic("[UterusOne::LearnAllTrainerSpells] No TrainerSpellData for %u", TrainerID);
        return;
    }

    // Repeat until player learned all spells.
    if (!AllSpellsKnown)
    {
        sLog.outBasic("[UterusOne::LearnAllTrainerSpells] !AllSpellsKnown");
        LearnAllTrainerSpells(pPlayer, TrainerID);
    }
    else
        UpgradePlayerSpellsToMax(pPlayer); // Upgrade all class book spells!
}

void LearnAllSpells(Player* pPlayer)
{
    if (!pPlayer)
        return;

    enum TrainerIDS
    {
        RIDING_TRAINER          = 4752,     // Kildar <Riding Trainer>
        WEAPON_MASTER_IRONFORGE = 11865,    // Buliwyf Stonehand <Weapon Master>
        WEAPON_MASTER_DARNASSUS = 11866,    // Ilyenia Moonfire <Weapon Master>
        WEAPON_MASTER_STORMWIND = 11867,    // Woo Ping <Weapon Master>
        WARRIOR_TRAINER         = 914,      // Ander Germaine <Warrior Trainer>
        PRIEST_TRAINER          = 3046,     // Father Cobb <Priest Trainer>
        PALADIN_TRAINER         = 928,      // Lord Grayson Shadowbreaker <Paladin Trainer>
        ROGUE_TRAINER           = 13283,    // Lord Tony Romano <Rogue Trainer>
        MAGE_TRAINER            = 3047,     // Archmage Shymm <Mage Trainer>
        SHAMAN_TRAINER          = 3032,     // Beram Skychaser <Shaman Trainer>
        HUNTER_TRAINER          = 5115,     // Daera Brightspear <Hunter Trainer>
        HUNTERPET_TRAINER       = 10090,    // Belia Thundergranite <Pet Trainer>
        DRUID_TRAINER           = 9465,     // Golhine the Hooded <Druid Trainer>
        WARLOCK_TRAINER         = 5173,     // Alexander Calder <Warlock Trainer>
        PET_TRAINER             = 10088,    // Xao'tsu <Pet Trainer>
        PORTAL_TRAINER_TB       = 5957,     // Birgitte Cranston <Portal Trainer>
        PORTAL_TRAINER_DA       = 4165,     // Elissa Dumas <Portal Trainer>
        PORTAL_TRAINER_SW       = 2485,     // Larimaine Purdue <Portal Trainer>
        PORTAL_TRAINER_UC       = 2492,     // Lexington Mortaim <Portal Trainer>
        PORTAL_TRAINER_IF       = 2489,     // Milstaff Stormeye <Portal Trainer>
        PORTAL_TRAINER_OG       = 5958      // Thuul <Portal Trainer>
    };

    uint32 TrainerID = RIDING_TRAINER; // Just in case some random ID first

    switch (pPlayer->GetClass())
    {
        case CLASS_WARRIOR:
            TrainerID = WARRIOR_TRAINER;
            break;
        case CLASS_PRIEST:
            TrainerID = PRIEST_TRAINER;
            break;
        case CLASS_PALADIN:
            TrainerID = PALADIN_TRAINER;
            break;
        case CLASS_ROGUE:
            TrainerID = ROGUE_TRAINER;
            break;
        case CLASS_MAGE:
        {
            TrainerID = MAGE_TRAINER;
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_DA);
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_IF);
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_SW);
            }
            else
            {
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_TB);
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_OG);
                LearnAllTrainerSpells(pPlayer, PORTAL_TRAINER_UC);
            }
            break;
        }
        case CLASS_SHAMAN:
            TrainerID = SHAMAN_TRAINER;
            break;
        case CLASS_HUNTER:
            TrainerID = HUNTER_TRAINER;
            LearnAllTrainerSpells(pPlayer, HUNTERPET_TRAINER);
            LearnAllWildCreaturesPetSpell(pPlayer);
            break;
        case CLASS_DRUID:
            TrainerID = DRUID_TRAINER;
            break;
        case CLASS_WARLOCK:
            TrainerID = WARLOCK_TRAINER;
            break;
        default: TrainerID = RIDING_TRAINER; break;
    }

    LearnAllTrainerSpells(pPlayer, TrainerID);
    LearnAllTrainerSpells(pPlayer, WEAPON_MASTER_IRONFORGE);
    LearnAllTrainerSpells(pPlayer, WEAPON_MASTER_DARNASSUS);
    LearnAllTrainerSpells(pPlayer, WEAPON_MASTER_STORMWIND);
    LearnAllTrainerSpells(pPlayer, RIDING_TRAINER);
    pPlayer->LearnSpell(33389, false);
    pPlayer->LearnSpell(33392, false);
}

void LearnTalentsFromDB(Player* pPlayer, uint32 temp_id)
{
    if (!pPlayer)
        return;

    if (!temp_id)
        return;

    QueryResult* select = CharacterDatabase.PQuery("SELECT * FROM template_npc_talents WHERE class = '%s' AND "
        "temp_id = '%u';", GetClassString(pPlayer).c_str(), temp_id);

    if (!select)
        return;

        do
        {
            Field* fields       = select->Fetch();
            uint32 talentID     = fields[2].GetUInt32();

            if (talentID)
            {
                uint32 const firstRankId = sSpellMgr.GetFirstSpellInChain(talentID);
                if (firstRankId && firstRankId != talentID && GetTalentSpellPos(firstRankId))
                    pPlayer->LearnSpell(firstRankId, false, true);
                pPlayer->LearnSpell(talentID, false, (firstRankId == talentID && GetTalentSpellPos(firstRankId)));
            }

        } while (select->NextRow());
        delete select;
}

void DeleteEquippedGear(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (int i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
    }
}

void DeleteBagsAndContent(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (int i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
    {
        if (Bag* pBag = (Bag*)pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
            {
                if (Item* pItem = pBag->GetItemByPos(j))
                    pPlayer->DestroyItem(i, j, true);
            }
        }
    }
}

void AddBags(Player* pPlayer)
{
    if (!pPlayer)
        return;

    bool hasQuiver = false;
    bool hasFelcloth = false;
    bool hasTotems = false;

    for (int bagslot = INVENTORY_SLOT_BAG_START; bagslot < INVENTORY_SLOT_BAG_END; ++bagslot)
    {
        Item* bBag = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, bagslot);

        if (bBag && bBag->IsBag() && ((Bag*)bBag)->IsEmpty())
            pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, bagslot, true);

        if (pPlayer->GetClass() == CLASS_SHAMAN && !hasTotems)
        {
            pPlayer->AddItem(FIRE_TOTEM, 1);
            pPlayer->AddItem(WATER_TOTEM, 1);
            pPlayer->AddItem(AIR_TOTEM, 1);
            pPlayer->AddItem(EARTH_TOTEM, 1);
            hasTotems = true;
        }
        if (pPlayer->GetClass() == CLASS_HUNTER)
        {
            if (!hasQuiver)
            {
                pPlayer->EquipNewItem(bagslot, ANCIENT_SINEW_WRAPPED_LAMINA, true);
                pPlayer->AddItem(DOOMSHOT, 3600);
                pPlayer->SetAmmo(DOOMSHOT);
                hasQuiver = true;
            }
            else
                pPlayer->EquipNewItem(bagslot, BOTTOMLESS_BAG, true);
        }
        else if (pPlayer->GetClass() == CLASS_WARLOCK)
        {
            if (!hasFelcloth)
            {
                pPlayer->EquipNewItem(bagslot, CORE_FELCLOTH_BAG, true);
                pPlayer->AddItem(SOUL_SHARD, 28);
                hasFelcloth = true;
            }
            else
                pPlayer->EquipNewItem(bagslot, BOTTOMLESS_BAG, true);
        }
        else
            pPlayer->EquipNewItem(bagslot, BOTTOMLESS_BAG, true);
    }
}

void EquipItemsFromDB(Player* pPlayer, uint32 temp_id)
{
    if (!pPlayer)
        return;

    if (!temp_id)
        return;

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT item_entry, item_entry_human, item_entry_orc, item_entry_dwarf, item_entry_troll, item_enchant, randomPropertyId "
        "FROM template_npc_gear WHERE class = '%s' AND temp_id = '%u';", GetClassString(pPlayer).c_str(), temp_id));

    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        uint32 item_entry       = fields[0].GetUInt32();
        uint32 item_entry_human = fields[1].GetUInt32();
        uint32 item_entry_orc   = fields[2].GetUInt32();
        uint32 item_entry_dwarf = fields[3].GetUInt32();
        uint32 item_entry_troll = fields[4].GetUInt32();
        uint32 item_enchant     = fields[5].GetUInt32();
        uint32 item_property_Id = fields[6].GetUInt32();

        for (auto it = sObjectMgr.factionchange_items.begin(); it != sObjectMgr.factionchange_items.end(); ++it)
            if (it->second == item_entry || it->first == item_entry)
                item_entry = pPlayer->GetTeam() == ALLIANCE ? it->first : it->second;

        // Looking for race specific Items.
        if (pPlayer->GetRace() == RACE_HUMAN && item_entry_human > 0)
            item_entry = item_entry_human;

        if (pPlayer->GetRace() == RACE_ORC && item_entry_orc > 0)
            item_entry = item_entry_orc;

        if (pPlayer->GetRace() == RACE_DWARF && item_entry_dwarf > 0)
            item_entry = item_entry_dwarf;

        if (pPlayer->GetRace() == RACE_TROLL && item_entry_troll > 0)
            item_entry = item_entry_troll;

        if (ItemPrototype const* pItem = ObjectMgr::GetItemPrototype(item_entry))
        {
            if (pItem->MaxCount)
                pPlayer->DeleteItemInBag(item_entry);
            pPlayer->SatisfyItemRequirements(pItem);
            pPlayer->StoreNewItemInBestSlots(item_entry, 1, item_enchant, item_property_Id);
        }
    } while (result->NextRow());
}

void ApplyTemplateToPlayer(Player* pPlayer, Creature* pCreature, uint32 temp_id) // Merge
{
    if (!pPlayer)
        return;

    pPlayer->ResetTalents(true);
    LearnTalentsFromDB(pPlayer, temp_id);
    DeleteEquippedGear(pPlayer);
    EquipItemsFromDB(pPlayer, temp_id);
    LearnAllSpells(pPlayer);
    pPlayer->UpdateSkillsToMaxSkillsForLevel();
    pPlayer->RemoveAllSpellCooldown();
    pPlayer->ModifyHealth(pPlayer->GetMaxHealth());
    if (pPlayer->GetPower(POWER_MANA) != pPlayer->GetMaxPower(POWER_MANA))
        pPlayer->ModifyPower(POWER_MANA, pPlayer->GetMaxPower(POWER_MANA));
    pPlayer->SendSpellGo(pPlayer, COOL_VISUAL_SPELL_3);
    pPlayer->SaveInventoryAndGoldToDB();
}

void BuildPetGossipPageSQL(Player* pPlayer, Creature* pCreature, uint32 start)
{
    uint32 count = NULL;
    uint32 max = start + 10;
    uint32 last = start - 10;
    uint32 currentPet = NULL;

    Pet* pPet = pPlayer->GetPet();

    if (pPlayer->GetClass() == CLASS_HUNTER && pPet)
        if (pPet->getPetType() == HUNTER_PET)
            currentPet = pPet->GetEntry();

    QueryResult* result = WorldDatabase.PQuery("SELECT * FROM (SELECT * FROM (SELECT entry, name, pet_family, base_attack_time FROM (SELECT * FROM creature_template WHERE level_max <= '%u' AND pet_family != 0 AND type_flags & 1 AND rank > 0) p GROUP BY display_id1) p ORDER BY pet_family ASC) p ORDER BY base_attack_time;", pPlayer->GetLevel());

    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        const char* name = fields[1].GetString();
        uint32 pet_family = fields[2].GetUInt32();
        uint32 base_attack_time = fields[3].GetUInt32();

        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(entry);

        if (!cInfo)
            continue;

        if (cInfo->level_min > pPlayer->GetLevel())
            continue;

        if (!cInfo->isTameable())
            continue;

        ++count;

        if (count < start)
            continue;

        std::ostringstream gstring;
        std::ostringstream attack_time;

        attack_time << float(cInfo->base_attack_time * 0.0010f);
        std::string str = attack_time.str();

        if (str.length() < 2)
            attack_time << ".0";

        if (cInfo->entry == currentPet)
            gstring << "*";

        gstring << "[" << count << "] " << GetPetFamily(cInfo->pet_family) << ", " << cInfo->name << ", " << attack_time.str().c_str() << " speed";

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, gstring.str().c_str(), GOSSIP_SENDER_PET_SELECT, entry);

        if (count > max)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show more...", SHOW_PETS, max + 2);
            break;
        }

    } while (result->NextRow());


    if (start > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Go back...", SHOW_PETS, last - 2);
    else
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Go back...", GOSSIP_SENDER_START, 0);

    pPlayer->SEND_GOSSIP_MENU(600003, pCreature->GetObjectGuid());
}

void BuildPetGossipPage(Player* pPlayer, Creature* pCreature, uint32 start)
{
    uint32 count = NULL;
    uint32 max = start + 10;
    uint32 last = start - 10;
    std::list<uint32> displayIDs;
    displayIDs.clear();

    for (uint32 i = 1; i < sCreatureStorage.GetMaxEntry(); ++i)
    {
        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i);
    
        if (!cInfo)
            continue;

        if (cInfo->level_min > pPlayer->GetLevel())
            continue;

        if (!cInfo->isTameable())
            continue;

        if (cInfo->rank == NULL)
            continue;

        if (cInfo->base_attack_time >= 2000)
            continue;

        if (!sCreatureFamilyStore.LookupEntry(cInfo->pet_family) && cInfo->pet_family != CREATURE_FAMILY_HORSE_CUSTOM)
            continue;

        ++count;

        if (count < start)
            continue;

        displayIDs.push_back(cInfo->display_id[0]);

        std::ostringstream ss;
        ss << "[" << count << "] " << GetPetFamily(cInfo->pet_family) << ", " << cInfo->name << ", " << float(cInfo->base_attack_time * 0.0010f) << " attack time";
    
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_PET_SELECT, cInfo->entry);

        if (count > max)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show more...", SHOW_PETS, max + 2);
            break;
        }
    }

    if (start > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Go back...", SHOW_PETS, last - 2);
    else
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Go back...", GOSSIP_SENDER_START, 0);

    pPlayer->SEND_GOSSIP_MENU(600003, pCreature->GetObjectGuid());
}

// Export Talent Spell ID's (not Talent ID's) to Database!
void ExportCharacterTalentsToDB(Player* pPlayer, std::string& gossipTempText)
{
    if (!pPlayer)
        return;

    if (gossipTempText == "")
        return;

    static SqlStatementID insTalents;
    uint32 TempID = GetTemplateID();

    SqlStatement stmtIns = CharacterDatabase.CreateStatement(insTalents, "INSERT INTO template_npc_talents (temp_id, class, talent_id, rank) VALUES (?, ?, ?, ?)");

    if (pPlayer->GetFreeTalentPoints() > 0)
    {
        pPlayer->GetSession()->SendAreaTriggerMessage("You have unspend talent points. Please spend all your talent points.");
        return;
    }

    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((pPlayer->GetClassMask() & talentTabInfo->ClassMask) == 0)
            continue;

        for (int rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            PlayerSpellMap const& uSpells = pPlayer->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = uSpells.begin(); itr != uSpells.end(); ++itr)
            {
                if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled)
                    continue;

                uint32 rank2 = sSpellMgr.GetSpellRank(itr->first);

                if (itr->first == talentInfo->RankID[rank] || sSpellMgr.IsSpellLearnToSpell(talentInfo->RankID[rank], itr->first))
                    stmtIns.PExecute(TempID, GetClassString(pPlayer).c_str(), talentInfo->RankID[rank], rank + 1);
            }
        }
    }
}

bool GossipHello_TemplateNPC(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer)
        return false;

    if (!pCreature)
        return false;

    pPlayer->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_INTERACTING_CANCELS);

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    if (pPlayer->GetHealth() < pPlayer->GetMaxHealth())
        pPlayer->ModifyHealth(pPlayer->GetMaxHealth());

    switch (pPlayer->GetClass())
    {
        case CLASS_PRIEST:
        case CLASS_PALADIN:
        case CLASS_MAGE:
        case CLASS_WARLOCK:
        case CLASS_SHAMAN:
        case CLASS_DRUID:
        case CLASS_HUNTER:
        {
            if (pPlayer->GetPower(POWER_MANA) != pPlayer->GetMaxPower(POWER_MANA))
                pPlayer->ModifyPower(POWER_MANA, pPlayer->GetMaxPower(POWER_MANA));
            break;
        }
        case CLASS_ROGUE:
            pPlayer->ModifyPower(POWER_ENERGY, pPlayer->GetMaxPower(POWER_ENERGY));
        default:
            break;
    }

    if (pPlayer->GetLevel() < 60)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I am weak... Do something!", GOSSIP_SENDER_START, GET_LVL_60);
    }
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "What can I buy from you?", GOSSIP_SENDER_START, GOSSIP_OPTION_VENDOR);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Reset my Talents.", GOSSIP_SENDER_START, RESET_TALENTS);
        if (!pPlayer->HasSpell(33392))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I seek training to ride.", GOSSIP_SENDER_START, TRAIN_RIDE);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remove all Buffs.", GOSSIP_SENDER_START, RESET_BUFFS);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Upgrade all my Talents Ranks.", GOSSIP_SENDER_START, UPGRADE_TALENTS);

        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT item_entry "
            "FROM template_npc_gear WHERE class = '%s' GROUP BY item_entry LIMIT 1;", GetClassString(pPlayer).c_str()));

        if (result)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "GEAR & TALENTS COLLECTION", GOSSIP_SENDER_START, SHOW_SPECS);

        // Only admin can add and delete templates.
        if (pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<ADMIN> Save my current Gear & Talents.", GOSSIP_SENDER_START, SAVE_TEMP);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<ADMIN> Delete a Template.", GOSSIP_SENDER_START, DELETE_TEMP);
        }
    }

    switch (pCreature->GetEntry())
    {
    case PRIEST_NPC:

        if (pPlayer->GetClass() != CLASS_PRIEST)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(600015, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case PALADIN_NPC:

        if (pPlayer->GetClass() != CLASS_PALADIN)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(2999, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case WARRIOR_NPC:

        if (pPlayer->GetClass() != CLASS_WARRIOR)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(5724, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case MAGE_NPC:

        if (pPlayer->GetClass() != CLASS_MAGE)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(561, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case WARLOCK_NPC:
        if (pPlayer->GetClass() != CLASS_WARLOCK)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(5836, pCreature->GetObjectGuid());
        }
        else
        {
            if (Pet* pet = pPlayer->GetPet())
            {
                if (pet && pet->getPetType() == SUMMON_PET)
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teach my Pet all Spells.", GOSSIP_SENDER_START, TEACH_WARLOCK_PET);
                }
            }

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        }
        break;

    case SHAMAN_NPC:

        if (pPlayer->GetClass() != CLASS_SHAMAN)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(600014, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case DRUID_NPC:

        if (pPlayer->GetClass() != CLASS_DRUID)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(4916, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;

    case HUNTER_NPC:

        if (pPlayer->GetClass() == CLASS_HUNTER)
        {
            if (pPlayer->GetLevel() >= 10)
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take me to the Stable.", GOSSIP_SENDER_START, GOSSIP_OPTION_STABLEPET);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I wish to untrain my pet.", GOSSIP_SENDER_START, GOSSIP_OPTION_UNLEARNPETSKILLS);

                if (Pet* pet = pPlayer->GetPet())
                {
                    if (pet && pet->getPetType() == HUNTER_PET)
                    {
                        if (pet->GetLoyaltyLevel() < BEST_FRIEND) {
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Make my Pet Happy again.", GOSSIP_SENDER_START, MAKE_PET_HAPPY);
                        }
                    }
                }

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "RARE PET COLLECTION", GOSSIP_SENDER_START, SHOW_PETS);
            }
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

        }
        else
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(5839, pCreature->GetObjectGuid());
        }
        break;

    case ROGUE_NPC:

        if (pPlayer->GetClass() != CLASS_ROGUE)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me the way to my Trainer please.", GOSSIP_SENDER_START, SHOW_MY_NPC);
            pPlayer->SEND_GOSSIP_MENU(4799, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
        break;
    }

    return true;
}

bool GossipStart_TemplateNPC(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    if (uiAction == GOSSIP_OPTION_STABLEPET)
    {
        pPlayer->GetSession()->SendStablePet(pCreature->GetGUID());
    }
    else if (uiAction == GOSSIP_OPTION_VENDOR)
    {
        pPlayer->GetSession()->SendListInventory(pCreature->GetGUID());
    }
    else if (uiAction == GOSSIP_OPTION_UNLEARNPETSKILLS)
    {
        Pet* pet = pPlayer->GetPet();

        if (pet)
        {
            WorldPacket data(SMSG_PET_UNLEARN_CONFIRM, (8 + 4));
            data << ObjectGuid(pet->GetObjectGuid());
            data << uint32(0);
            pPlayer->GetSession()->SendPacket(&data);
        }
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == GET_LVL_60)
    {
        pPlayer->ModifyMoney(100000000);
        pPlayer->GiveLevel(60);
        LearnQuestSpells(pPlayer);
        LearnAllSpells(pPlayer);
        pPlayer->UpdateSkillsToMaxSkillsForLevel();
        DeleteBagsAndContent(pPlayer);
        AddBags(pPlayer);
        pCreature->CastSpell(pPlayer, 28006, true);

        pPlayer->SaveToDB(true, true);
        
        pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL_2, true);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == MAKE_PET_HAPPY)
    {
        Pet* pPet = pPlayer->GetPet();

        if (pPlayer->GetClass() == CLASS_HUNTER && pPet && pPet->getPetType() == HUNTER_PET)
        {
            pPet->SetPower(POWER_HAPPINESS, pPet->GetMaxPower(POWER_HAPPINESS));
            pPet->SetLoyaltyLevel(BEST_FRIEND);
            pPet->SendSpellGo(pPet, COOL_VISUAL_SPELL_4);
        }
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == TEACH_WARLOCK_PET)
    {
        LearnWarlockPetSpells(pPlayer);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == RESET_TALENTS)
    {
        pPlayer->ResetTalents(true);
        pPlayer->SendSpellGo(pPlayer, COOL_VISUAL_SPELL_2);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == TRAIN_RIDE)
    {
        pPlayer->LearnSpell(33389, false);
        pPlayer->LearnSpell(33392, false);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == RESET_BUFFS)
    {
        if (Pet* pet = pPlayer->GetPet())
            pet->RemoveAllAurasOnDeath();
        pPlayer->RemoveAllAurasOnDeath();
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == UPGRADE_TALENTS)
    {
        LearnAllSpells(pPlayer);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == RESET_COOLDOWNS_AND_CHARGES)
    {
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (Item* item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (const ItemPrototype* pProto = item->GetProto())
                {
                    for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                    {
                        if (pProto->Spells[i].SpellId)
                        {
                            if (pProto->Spells[i].SpellCharges)
                            {
                                int32 charges = pProto->Spells[i].SpellCharges;
                                item->SetSpellCharges(i, charges);
                            }
                        }
                    }
                }
            }
        }
        pPlayer->RemoveAllSpellCooldown();
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == FIX_DB)
    {
        perform_npc_vendor_template();
        //perform_player_factionchange_items();
                pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == DELETE_GEAR)
    {
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
                continue;

            if (Item* haveItemEquipped = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (haveItemEquipped)
                {
                    pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
                }
            }
        }
        pPlayer->SaveToDB(true, true);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == SHOW_SPECS)
    {
       // QueryResult* result = CharacterDatabase.PQuery("SELECT talent_tab_id, patch "
       //     "FROM template_npc_gear WHERE class = '%s' GROUP BY talent_tab_id LIMIT 30", GetClassString(pPlayer));

       std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT talent_tab_id, patch FROM template_npc_gear WHERE class = '%s' GROUP BY talent_tab_id LIMIT 3", GetClassString(pPlayer).c_str()));
       const char* SpecText = "No Specification";

        if (result)
        {
            do
            {
                Field* fields	= result->Fetch();
                uint32 TalentTabID	= fields[0].GetUInt32();
                uint32 Patch	= fields[1].GetUInt32();

                if (sWorld.GetWowPatch() >= Patch)
                {
                    switch (TalentTabID)
                    {
                        case WarriorProtection:
                            SpecText = "Protection";
                            break;
                        case WarriorFury:
                            SpecText = "Fury";
                            break;
                        case WarriorArms:
                            SpecText = "Arms";
                            break;
                        case WarlockDemonology:
                            SpecText = "Demonology";
                            break;
                        case WarlockDestruction:
                            SpecText = "Destruction";
                            break;
                        case WarlockAffliction:
                            SpecText = "Affliction";
                            break;
                        case ShamanRestoration:
                            SpecText = "Restoration";
                            break;
                        case ShamanEnhancement:
                            SpecText = "Enhancement";
                            break;
                        case ShamanElementalCombat:
                            SpecText = "Elemental";
                            break;
                        case RogueSubtlety:
                            SpecText = "Subtlety";
                            break;
                        case RogueCombat:
                            SpecText = "Combat";
                            break;
                        case RogueAssassination:
                            SpecText = "Assassination";
                            break;
                        case PriestShadow:
                            SpecText = "Shadow";
                            break;
                        case PriestHoly:
                            SpecText = "Holy";
                            break;
                        case PriestDiscipline:
                            SpecText = "Discipline";
                            break;
                        case PaladinProtection:
                            SpecText = "Protection";
                            break;
                        case PaladinHoly:
                            SpecText = "Holy";
                            break;
                        case PaladinRetribution:
                            SpecText = "Retribution";
                            break;
                        case MageFrost:
                            SpecText = "Frost";
                            break;
                        case MageFire:
                            SpecText = "Fire";
                            break;
                        case MageArcane:
                            SpecText = "Arcane";
                            break;
                        case HunterSurvival:
                            SpecText = "Survival";
                            break;
                        case HunterMarksmanship:
                            SpecText = "Marksmanship";
                            break;
                        case HunterBeastMastery:
                            SpecText = "Beast Mastery";
                            break;
                        case DruidRestoration:
                            SpecText = "Restoration";
                            break;
                        case DruidFeralCombat:
                            SpecText = "Feral Combat";
                            break;
                        case DruidBalance:
                            SpecText = "Balance";
                            break;
                    }

                    std::ostringstream ss;
                    ss << SpecText;

                    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_TEMP_SPECS, TalentTabID);
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_TEMP_PATCH, TalentTabID);
                }
            } while (result->NextRow());
           // delete result;
        }

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_START, 0);
        pPlayer->SEND_GOSSIP_MENU(600008, pCreature->GetObjectGuid());
    }
    else if (uiAction == DELETE_TEMP)
    {
    // QueryResult* result = CharacterDatabase.PQuery("SELECT talent_tab_id, patch "
    //     "FROM template_npc_gear WHERE class = '%s' GROUP BY talent_tab_id LIMIT 30", GetClassString(pPlayer));

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT talent_tab_id, patch FROM template_npc_gear WHERE class = '%s' GROUP BY talent_tab_id LIMIT 3", GetClassString(pPlayer).c_str()));
    const char* SpecText = "No Specification";

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 TalentTabID = fields[0].GetUInt32();
            uint32 Patch = fields[1].GetUInt32();

            if (sWorld.GetWowPatch() >= Patch)
            {
                switch (TalentTabID)
                {
                    case WarriorProtection:
                        SpecText = "Protection";
                        break;
                    case WarriorFury:
                        SpecText = "Fury";
                        break;
                    case WarriorArms:
                        SpecText = "Arms";
                        break;
                    case WarlockDemonology:
                        SpecText = "Demonology";
                        break;
                    case WarlockDestruction:
                        SpecText = "Destruction";
                        break;
                    case WarlockAffliction:
                        SpecText = "Affliction";
                        break;
                    case ShamanRestoration:
                        SpecText = "Restoration";
                        break;
                    case ShamanEnhancement:
                        SpecText = "Enhancement";
                        break;
                    case ShamanElementalCombat:
                        SpecText = "Elemental";
                        break;
                    case RogueSubtlety:
                        SpecText = "Subtlety";
                        break;
                    case RogueCombat:
                        SpecText = "Combat";
                        break;
                    case RogueAssassination:
                        SpecText = "Assassination";
                        break;
                    case PriestShadow:
                        SpecText = "Shadow";
                        break;
                    case PriestHoly:
                        SpecText = "Holy";
                        break;
                    case PriestDiscipline:
                        SpecText = "Discipline";
                        break;
                    case PaladinProtection:
                        SpecText = "Protection";
                        break;
                    case PaladinHoly:
                        SpecText = "Holy";
                        break;
                    case PaladinRetribution:
                        SpecText = "Retribution";
                        break;
                    case MageFrost:
                        SpecText = "Frost";
                        break;
                    case MageFire:
                        SpecText = "Fire";
                        break;
                    case MageArcane:
                        SpecText = "Arcane";
                        break;
                    case HunterSurvival:
                        SpecText = "Survival";
                        break;
                    case HunterMarksmanship:
                        SpecText = "Marksmanship";
                        break;
                    case HunterBeastMastery:
                        SpecText = "Beast Mastery";
                        break;
                    case DruidRestoration:
                        SpecText = "Restoration";
                        break;
                    case DruidFeralCombat:
                        SpecText = "Feral Combat";
                        break;
                    case DruidBalance:
                        SpecText = "Balance";
                        break;
                }

                std::ostringstream ss;
                ss << SpecText;

                //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_TEMP_SPECS, TalentTabID);
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_TEMP_DELETE_PATCH, TalentTabID);
            }
        } while (result->NextRow());
        // delete result;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_START, 0);
    pPlayer->SEND_GOSSIP_MENU(600008, pCreature->GetObjectGuid());
    }
    else if (uiAction == FIX_QUEST_ITEM_LEVEL)
    {
        std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT item FROM auctionhousebot;"));
        if (result)
        {
            uint32 counter = 0;
            uint32 counter2 = 0;

            do
            {
                Field* fields = result->Fetch();
                uint32 entry = fields[0].GetUInt32();

                ItemPrototype const* pProto = ObjectMgr::GetItemPrototype(entry);
                if (pProto)
                {
                    // Only item with level 0 and BoP / Quest items.
                    if (pProto->RequiredLevel == 0 && (pProto->Bonding == BIND_WHEN_PICKED_UP || pProto->Bonding == BIND_QUEST_ITEM))
                    {
                        bool max_count = false;
                        uint32 required_level = 0;
                        ObjectMgr::QuestMap const& qTemplates = sObjectMgr.GetQuestTemplates();
                        for (const auto& itr : qTemplates)
                        {
                            const auto& qinfo = itr.second;

                            // Go trough every quest and search the item.
                            for (int i = 0; i < QUEST_REWARDS_COUNT; ++i)
                            {
                                if (!qinfo->RewItemId[i])
                                    continue;

                                if (qinfo->RewItemId[i] != pProto->ItemId)
                                    continue;

                                    required_level = qinfo->GetMinLevel();
                            }
                            for (int j = 0; j < QUEST_REWARD_CHOICES_COUNT; ++j)
                            {
                                if (!qinfo->RewChoiceItemId[j])
                                    continue;

                                if (qinfo->RewChoiceItemId[j] != pProto->ItemId)
                                    continue;

                                    required_level = qinfo->GetMinLevel();
                            }

                            // Check if quest is repeatable. If not make item unique.
                            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
                                max_count = true;
                        }

                        // required_level still 0, so no Quest was found for the item.
                        if (required_level == 0)
                            continue;

                        // Update max_count to make Item unique to avoid equipping it twice (for example Blackhand's Breadth).
                        if (max_count && !pProto->MaxCount)
                        {
                            WorldDatabase.PExecute("UPDATE item_template SET max_count = '%u' WHERE entry = '%u'", 1, entry);
                            std::ostringstream ss;
                            ++counter2;
                            ss << counter2 << ". Updating max_count for Quest Item: " << pProto->Name1;
                            ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
                        }

                        // Update the required_level for the item to the min level to start the Quest.
                        WorldDatabase.PExecute("UPDATE item_template SET required_level = '%u' WHERE entry = '%u'", required_level, entry);
                        std::ostringstream ss;
                        ++counter;
                        ss << counter << ". Updating required_level for Quest Item: " << pProto->Name1 << " to " << required_level;
                        ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
                    }

                }

            } while (result->NextRow());

            std::ostringstream ss;
            ss << "Fixed " << counter << " required_level and " << counter2 << " max_count.";
            ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
        }
    }
    else if (uiAction == SAVE_TEMP)
    {

        if (pPlayer->GetFreeTalentPoints() > 0)
        {
            pPlayer->GetSession()->SendAreaTriggerMessage("You have unspend talent points. Please spend all your talent points.");
            return false;
        }

        std::string name = TemplateExportNameString(pPlayer).c_str();

        if (name.length() > 200)
        {
            ChatHandler(pPlayer->GetSession()).PSendSysMessage("Name too long.");
            pPlayer->CLOSE_GOSSIP_MENU();
            return false;
        }
        else
        {
            CharacterDatabase.escape_string(name);
            ExtractGearTemplateToDB(pPlayer, name);
            ExportCharacterTalentsToDB(pPlayer, name);
        }
        pPlayer->CLOSE_GOSSIP_MENU();

        return true;
    }
    else if (uiAction == SHOW_PETS) // List Hunter Pets
    {
    BuildPetGossipPageSQL(pPlayer, pCreature, NULL);
    }
    else if (uiAction == SHOW_MY_NPC) // List Hunter Pets
    {
        switch (pPlayer->GetClass())
        {
            case CLASS_DRUID:
                MoveToNPC(pPlayer, DRUID_NPC);
                break;
            case CLASS_PRIEST:
                MoveToNPC(pPlayer, PRIEST_NPC);
                break;
            case CLASS_SHAMAN:
                MoveToNPC(pPlayer, SHAMAN_NPC);
                break;
            case CLASS_WARLOCK:
                MoveToNPC(pPlayer, WARLOCK_NPC);
                break;
            case CLASS_WARRIOR:
                MoveToNPC(pPlayer, WARRIOR_NPC);
                break;
            case CLASS_PALADIN:
                MoveToNPC(pPlayer, PALADIN_NPC);
                break;
            case CLASS_ROGUE:
                MoveToNPC(pPlayer, ROGUE_NPC);
                break;
            case CLASS_MAGE:
                MoveToNPC(pPlayer, MAGE_NPC);
                break;
            case CLASS_HUNTER:
                MoveToNPC(pPlayer, HUNTER_NPC);
                break;
        }
    }

    else if (uiAction == 0)

        GossipHello_TemplateNPC(pPlayer, pCreature);

    return true;
}

bool GossipSelect_HunterPet(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    Pet* pPet = pPlayer->GetPet();

    if (pPlayer->GetClass() == CLASS_HUNTER && pPet && pPet->getPetType() == HUNTER_PET)
    {            // Permanently abandon pet
        if (pPet->getPetType() == HUNTER_PET)
            pPet->Unsummon(PET_SAVE_AS_DELETED, pPlayer);
        // Simply dismiss
        else
            pPet->Unsummon(PET_SAVE_NOT_IN_SLOT);
    }
    CreateHunterPet(pPlayer, pCreature, uiAction);
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipConfirm_Template(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ok let's do it!", GOSSIP_SENDER_TEMP_EQUIP, uiAction);
    pPlayer->SEND_GOSSIP_MENU(600007, pCreature->GetObjectGuid());

    return true;
}

bool GossipConfirm_Delete_Template(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sure?", GOSSIP_SENDER_TEMP_DELETE, uiAction);
    pPlayer->SEND_GOSSIP_MENU(600007, pCreature->GetObjectGuid());

    return true;
}

bool GossipPatch_Template(Player* pPlayer, Creature* pCreature, uint32 talent_tab_id, bool dDelete)
{
    // Get all Gear Templates by talent_tab_id.
    // Sort each by Item with the highest required Patch.
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM (SELECT * FROM (SELECT temp_id, gossip_text, MAX(patch) AS 'patch' FROM (SELECT * FROM template_npc_gear WHERE talent_tab_id = '%u') p GROUP BY temp_id) p ORDER BY patch ASC) p GROUP BY patch;", talent_tab_id);
    
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 temp_id = fields[0].GetUInt32();
            std::string SpecStr = fields[1].GetString();
            const char* SpecText = fields[1].GetString();
            uint32 Patch = fields[2].GetUInt32();

            std::ostringstream ss;
            ss << pPlayer->GetPatchName(Patch);

            if (sWorld.GetWowPatch() >= Patch)
            {
                if (dDelete)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), Patch + 100, talent_tab_id);
                else
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), Patch, talent_tab_id);
            }

        } while (result->NextRow());
        delete result;
    }

    if (dDelete)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_START, DELETE_TEMP);
    else
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_START, SHOW_SPECS);

    pPlayer->SEND_GOSSIP_MENU(600008, pCreature->GetObjectGuid());

    return true;
}

bool GossipSpecs_Template(Player* pPlayer, Creature* pCreature, uint32 patch, uint32 talent_tab_id)
{
    // Get all Gear Templates by talent_tab_id.
    // Sort each by Item with the highest required Patch.
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM (SELECT temp_id, gossip_text, MAX(patch) AS 'patch' FROM (SELECT * FROM template_npc_gear WHERE talent_tab_id = '%u') p GROUP BY temp_id) p ORDER BY patch ASC;", talent_tab_id);
    
    uint32 sender = GOSSIP_SENDER_START;

    if (result)
    {
        do
        {
            Field* fields			= result->Fetch();
            uint32 temp_id			= fields[0].GetUInt32();
            std::string SpecStr		= fields[1].GetString();
            const char* SpecText	= fields[1].GetString();
            uint32 Patch			= fields[2].GetUInt32();
            sender = Patch;

            std::ostringstream ss;
            ss << SpecText;

            if (sWorld.GetWowPatch() >= Patch && patch == Patch)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_TEMP_CONFIRM, temp_id);

        } while (result->NextRow());
        delete result;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_TEMP_PATCH, talent_tab_id);
    pPlayer->SEND_GOSSIP_MENU(600008, pCreature->GetObjectGuid());

    return true;
}

bool GossipSpecs_Delete_Template(Player* pPlayer, Creature* pCreature, uint32 patch, uint32 talent_tab_id)
{
    patch = patch - 100;
    // Get all Gear Templates by talent_tab_id.
    // Sort each by Item with the highest required Patch.
    QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM (SELECT temp_id, gossip_text, MAX(patch) AS 'patch' FROM (SELECT * FROM template_npc_gear WHERE talent_tab_id = '%u') p GROUP BY temp_id) p ORDER BY patch ASC;", talent_tab_id);

    uint32 sender = GOSSIP_SENDER_START;
    
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 temp_id = fields[0].GetUInt32();
            std::string SpecStr = fields[1].GetString();
            const char* SpecText = fields[1].GetString();
            uint32 Patch = fields[2].GetUInt32();
            sender = Patch;

            std::ostringstream ss;
            ss << "Delete " << SpecText;

            if (sWorld.GetWowPatch() >= Patch && patch == Patch)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_TEMP_DELETE_CONFIRM, temp_id);

        } while (result->NextRow());
        delete result;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_TEMP_DELETE_PATCH, talent_tab_id);
    pPlayer->SEND_GOSSIP_MENU(600008, pCreature->GetObjectGuid());

    return true;
}

bool GossipEquip_Template(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT gossip_text FROM template_npc_gear WHERE temp_id = '%u' GROUP BY temp_id LIMIT 1", uiAction);

    if (result)
    {
        ApplyTemplateToPlayer(pPlayer, pCreature, uiAction);
        pPlayer->CLOSE_GOSSIP_MENU();
        delete result;
    }
    return true;
}

bool GossipDelete_Template(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    CharacterDatabase.PQuery("DELETE FROM template_npc_gear WHERE temp_id = '%u'", uiAction);
    bool res = CharacterDatabase.PQuery("DELETE FROM template_npc_talents WHERE temp_id = '%u'", uiAction);
    if (res)
        pPlayer->GetSession()->SendAreaTriggerMessage("Successfuly deleted");

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipSave_Template(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, const char* code)
{
    std::string name = code;
    static const char* allowedcharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz _-/().,1234567890";
    if (!name.length() || name.find_first_not_of(allowedcharacters) != std::string::npos)
    {
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("invalid template name.");
        pPlayer->CLOSE_GOSSIP_MENU();
        return false;
    }
    else
    {
        pPlayer->SaveToDB(true, true);
        ExtractGearTemplateToDB(pPlayer, name);
        ExportCharacterTalentsToDB(pPlayer, name);
    }
    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipSelect_TemplateNPC(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiSender)
    {
        case GOSSIP_SENDER_PATCH_12:
        case GOSSIP_SENDER_PATCH_13:
        case GOSSIP_SENDER_PATCH_14:
        case GOSSIP_SENDER_PATCH_15:
        case GOSSIP_SENDER_PATCH_16:
        case GOSSIP_SENDER_PATCH_17:
        case GOSSIP_SENDER_PATCH_18:
        case GOSSIP_SENDER_PATCH_19:
        case GOSSIP_SENDER_PATCH_110:
        case GOSSIP_SENDER_PATCH_111:
        case GOSSIP_SENDER_PATCH_112:
            GossipSpecs_Template(pPlayer, pCreature, uiSender, uiAction);
            break;
        case GOSSIP_SENDER_PATCH_12 + 100:
        case GOSSIP_SENDER_PATCH_13 + 100:
        case GOSSIP_SENDER_PATCH_14 + 100:
        case GOSSIP_SENDER_PATCH_15 + 100:
        case GOSSIP_SENDER_PATCH_16 + 100:
        case GOSSIP_SENDER_PATCH_17 + 100:
        case GOSSIP_SENDER_PATCH_18 + 100:
        case GOSSIP_SENDER_PATCH_19 + 100:
        case GOSSIP_SENDER_PATCH_110 + 100:
        case GOSSIP_SENDER_PATCH_111 + 100:
        case GOSSIP_SENDER_PATCH_112 + 100:
            GossipSpecs_Delete_Template(pPlayer, pCreature, uiSender, uiAction);
            break;
        case GOSSIP_SENDER_START:
            GossipStart_TemplateNPC(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_TEMP_DELETE:
            GossipDelete_Template(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_TEMP_CONFIRM:
            GossipConfirm_Template(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_TEMP_DELETE_CONFIRM:
            GossipConfirm_Delete_Template(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_TEMP_EQUIP:
            GossipEquip_Template(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_TEMP_PATCH:
            GossipPatch_Template(pPlayer, pCreature, uiAction, false);
            break;
        case GOSSIP_SENDER_TEMP_DELETE_PATCH:
            GossipPatch_Template(pPlayer, pCreature, uiAction, true);
            break;
        case GOSSIP_SENDER_PET_SELECT:
            GossipSelect_HunterPet(pPlayer, pCreature, uiAction);
            break;
        case SHOW_PETS:
            BuildPetGossipPageSQL(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}


struct TemplateNPCAI : public ScriptedAI
{
    TemplateNPCAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PLAYER);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        me->AddUnitState(UNIT_STAT_NO_COMBAT_MOVEMENT);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }
};

bool GossipHello_gobj_level_o_mat(Player* pPlayer, GameObject* gobj)
{
    if (!pPlayer)
        return false;

    if (!gobj)
        return false;

    if (pPlayer->HasAura(21354)) //i use Snowball Resistant aura to avoid spam
        return false;

    if (pPlayer->GetLevel() < 5)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "You need level 5 or higher to use this Machine. Get level 60 from your Class Trainer first!", GOSSIP_SENDER_START, 3);
    else if (pPlayer->GetLevel() >= 5)
    {
    if (pPlayer->GetLevel() < 60)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "+1 Level.", GOSSIP_SENDER_START, 1);
    if (pPlayer->GetLevel() > 5)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "-1 Level.", GOSSIP_SENDER_START, 2);
    if (pPlayer->GetLevel() != 9)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 9.", GOSSIP_SENDER_START, 9);
    if (pPlayer->GetLevel() != 19)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 19.", GOSSIP_SENDER_START, 19);
    if (pPlayer->GetLevel() != 29)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 29.", GOSSIP_SENDER_START, 29);
    if (pPlayer->GetLevel() != 39)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 39.", GOSSIP_SENDER_START, 39);
    if (pPlayer->GetLevel() != 49)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 49.", GOSSIP_SENDER_START, 49);
    if (pPlayer->GetLevel() != 59)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Level 59.", GOSSIP_SENDER_START, 59);
    }
    pPlayer->SEND_GOSSIP_MENU(6000015, gobj->GetObjectGuid());
    return true;
}

enum ItemSpells
{
    INCREASED_CRITICAL_1 = 7597,
    INCREASED_CRITICAL_2 = 7598,
    INCREASED_CRITICAL_3 = 7599,
    INCREASED_CRITICAL_4 = 7600,
    INCREASED_CRITICAL_1_2 = 9132,
    INCREASED_CRITICAL_5 = 9405,
};

void EquipBestInSlot(Player* pPlayer)
{
    if (!pPlayer)
        return;

    uint8 meleehit = 0;
    uint8 meleecrit = 0;
    uint8 spellhit = 0;
    uint8 spellcrit = 0;
    uint8 stat1 = ITEM_MOD_STAMINA;
    uint8 stat2 = ITEM_MOD_STAMINA;

    switch (pPlayer->GetClass())
    {
        case CLASS_WARRIOR:
            stat1 = ITEM_MOD_STRENGTH;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_PALADIN:
            stat1 = ITEM_MOD_INTELLECT;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_HUNTER:
            stat1 = ITEM_MOD_AGILITY;
            stat1 = ITEM_MOD_INTELLECT;
            break;
        case CLASS_ROGUE:
            stat1 = ITEM_MOD_AGILITY;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_PRIEST:
            stat1 = ITEM_MOD_SPIRIT;
            stat1 = ITEM_MOD_INTELLECT;
            break;
        case CLASS_SHAMAN:
            stat1 = ITEM_MOD_INTELLECT;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_MAGE:
            stat1 = ITEM_MOD_INTELLECT;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_WARLOCK:
            stat1 = ITEM_MOD_INTELLECT;
            stat1 = ITEM_MOD_STAMINA;
            break;
        case CLASS_DRUID:
            stat1 = ITEM_MOD_INTELLECT;
            stat1 = ITEM_MOD_STAMINA;
            break;
        default:
            break;
    }

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        if (Item * nSlot = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (nSlot)
            {
                pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
            }

            ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(nSlot->GetEntry());
            if (item_proto->ItemLevel)

            std::unique_ptr<QueryResult> nItem(WorldDatabase.PQuery("SELECT * FROM item_template WHERE required_level =< '%u' AND allowable_class = '%u' AND inventory_type = '%u';", pPlayer->GetLevel(), pPlayer->GetClassMask()));
        }
    }
}

void LevelUpInit(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        if (Item * haveItemEquipped = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (haveItemEquipped)
            {
                pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
            }
        }
    }

    pPlayer->RemovePet(PET_SAVE_AS_DELETED);
    pPlayer->ResetTalents(true);
    pPlayer->ResetSpells();
    ResetPetFromPlayer(pPlayer);
    LearnQuestSpells(pPlayer);
    LearnAllSpells(pPlayer);
    pPlayer->RemoveAllAurasOnDeath();
    pPlayer->UpdateSkillsToMaxSkillsForLevel();
    pPlayer->InitStatsForLevel(false);
    pPlayer->UpdateAllStats();
    pPlayer->SaveToDB(true, true);
    pPlayer->AddAura(21354);
}

uint32 lastFetchQueueList = 0;

bool GossipSelect_gobj_level_o_mat(Player* pPlayer, GameObject* gobj, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    if (!gobj)
        return false;

    if (pPlayer->HasAura(21354)) //i use Snowball Resistant aura to avoid spam
    {
        pPlayer->GetSession()->SendAreaTriggerMessage("The machine has overheated, please wait.");
        return false;
    }

    switch (uiAction)
    {
        case 1:
        {
            if (pPlayer->GetLevel() < 60)
            {
                pPlayer->GiveLevel(pPlayer->GetLevel() + 1);
                LevelUpInit(pPlayer);
            }
            break;
        }
        case 2:
        {
            if (pPlayer->GetLevel() > 4)
            {
                pPlayer->GiveLevel(pPlayer->GetLevel() - 1);
                LevelUpInit(pPlayer);
            }
            break;
        }
        case 9:
        {
            if (pPlayer->GetLevel() == 9)
                return false;
            pPlayer->GiveLevel(9);
            LevelUpInit(pPlayer);
            break;
        }
        case 19:
        {
            if (pPlayer->GetLevel() == 19)
                return false;
            pPlayer->GiveLevel(19);
            LevelUpInit(pPlayer);
            break;
        }
        case 29:
        {
            if (pPlayer->GetLevel() == 29)
                return false;
            pPlayer->GiveLevel(29);
            LevelUpInit(pPlayer);
            break;
        }
        case 39:
        {
            if (pPlayer->GetLevel() == 39)
                return false;
            pPlayer->GiveLevel(39);
            LevelUpInit(pPlayer);
            break;
        }
        case 49:
        {
            if (pPlayer->GetLevel() == 49)
                return false;
            pPlayer->GiveLevel(49);
            LevelUpInit(pPlayer);
            break;
        }
        case 59:
        {
            if (pPlayer->GetLevel() == 59)
                return false;
            pPlayer->GiveLevel(59);
            LevelUpInit(pPlayer);
            break;
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

CreatureAI* GetAITemplateNPC(Creature* pCreature)
{
    return new TemplateNPCAI(pCreature);
}

void AddSC_TemplateNPC()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "TemplateNPC";
    pNewScript->GetAI = &GetAITemplateNPC;
    pNewScript->pGossipHello = &GossipHello_TemplateNPC;
    pNewScript->pGossipSelect = &GossipSelect_TemplateNPC;
    pNewScript->pGossipSelectWithCode = &GossipSave_Template;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "gobj_level_o_mat";
    pNewScript->pGOGossipHello = &GossipHello_gobj_level_o_mat;
    pNewScript->pGOGossipSelect = &GossipSelect_gobj_level_o_mat;
    pNewScript->RegisterSelf();
}
