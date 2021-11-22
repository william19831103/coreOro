/*
 * Copyright (C) 2021 MaNGOS <http://uterusone.net/>
 */

#include "BattleGroundWS.h"
#include "Utilities/EventMap.h"
#include "CharacterDatabaseCache.h"

/*######
## npc_attack_dummy_boss
######*/

std::string sDummyClassString(uint8 cclass)
{
    switch (cclass)
    {
        case CLASS_WARRIOR: return "Warrior"; break;
        case CLASS_PALADIN: return "Paladin"; break;
        case CLASS_HUNTER:  return "Hunter";  break;
        case CLASS_ROGUE:   return "Rogue";   break;
        case CLASS_PRIEST:  return "Priest";  break;
        case CLASS_SHAMAN:  return "Shaman";  break;
        case CLASS_MAGE:    return "Mage";    break;
        case CLASS_WARLOCK: return "Warlock"; break;
        case CLASS_DRUID:   return "Druid";   break;
        default: return "Warrior"; break;
    }
}

enum DummySpells
{
    FURIOUS_HOWL			= 24597,	// Party members within 15 yards receive an extra 45 to 58 damage to their next Physical attack. Lasts 10 sec.
    SPELL_BATTLE_SQUAWK		= 23060,	// Attack Speed increased by 5%.
    POWER_INFUSION			= 10060,	// Infuses the target with power, increasing their spell damage and healing by 20%. Lasts 15 sec.
    LEADER_OF_THE_PACK		= 24932,	// 3% crit.
    ESSENCE_OF_THE_RED      = 23513,
    BURNING_ADRENALINE      = 23620,
    ALREADY_IN_FIGHT        = 12898,
    SPAWN_RED_LIGHTNING     = 24240,    // Visual Red lightning pillar.
    SOULSTONE_RESURRECTION  = 20763,
    TRANSFORMATION          = 28234,
    WINDFURY_TOTEM_PASSIVE  = 10612,
    FEIGN_DEATH             = 5384,
    VANISH_RANK1            = 1856,
    VANISH_RANK2            = 27617
};

enum DummyModes
{
    CasterDummyMode     = 1,
    MeleeDummyMode      = 2,
    ModerateDummyMode   = 3,
    FullDummyMode       = 0
};

enum DummyEntries
{
    Vaelastrasz = 700095,
    Maexxna     = 700096
};

enum TimerModes
{
    TwoMinutesMode  = 1498950,
    FiveMinutesMode = 1498951,
    OneMinutesMode  = 1498952,
    VaelMode = 3331000
};

enum ModeAuras
{
    CasterDummyAura     = 18951,
    MeleeDummyAura      = 28126,
    ModerateDummyAura   = 17327,
    FullDummyAura       = 31309
};

uint16 DummyItemLevel(Player* pPlayer)
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

uint32 GetEnchantID(Player* pPlayer, uint8 slot)
{
    Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

    uint32 enchant_id = pItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
    if (enchant_id)
    {
        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        uint32 enchant_id = enchantEntry->spellid[0];

        return enchant_id;
    }
    else
        return 0;
}

bool SaveTheAura(Player* pPlayer, SpellAuraHolder* pHolder, AuraSaveStruct& saveStruct)
{
    if (!pPlayer)
        return false;

    // Double croise : pas de sauvegarde dans la DB (clef unique, peut pas avoir 2x meme aura)
    if (pHolder->GetId() == 20007)
        return false;

    //skip all holders from spells that are passive or channeled
    //do not save single target holders (unless they were cast by the player)
    if (!pHolder->IsPassive() && !pHolder->GetSpellProto()->IsChanneledSpell() && (pHolder->GetCasterGuid() == pPlayer->GetObjectGuid() || !pHolder->IsSingleTarget()))
    {
        saveStruct.effIndexMask = 0;

        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            saveStruct.damage[i] = 0;
            saveStruct.periodicTime[i] = 0;

            if (Aura *aur = pHolder->GetAuraByEffectIndex(SpellEffectIndex(i)))
            {
                // don't save not own area auras
                if (aur->IsAreaAura() && pHolder->GetCasterGuid() != pPlayer->GetObjectGuid())
                    return false;

                saveStruct.damage[i] = aur->GetModifier()->m_amount;
                saveStruct.periodicTime[i] = aur->GetModifier()->periodictime;
                saveStruct.effIndexMask |= (1 << i);
            }
        }

        if (!saveStruct.effIndexMask)
            return false;

        saveStruct.casterGuid = pHolder->GetCasterGuid();
        saveStruct.itemLowGuid = pHolder->GetCastItemGuid().GetCounter();
        saveStruct.spellId = pHolder->GetId();
        saveStruct.stacks = pHolder->GetStackAmount();
        saveStruct.charges = pHolder->GetAuraCharges();
        saveStruct.duration = pHolder->GetAuraDuration();
        saveStruct.maxDuration = pHolder->GetAuraMaxDuration();
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
            saveStruct.charges = pHolder->GetAuraCharges();
        return true;
    }
    return false;
}

bool HasForbiddenItem(Player* pPlayer)
{
    if (!pPlayer)
        return false;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        Item * equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT item FROM auctionhousebot WHERE item = '%u';", equippedItem->GetEntry())); // All allowed items are in this table!
            if (!result)
            {
                ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(equippedItem->GetEntry());
                if (item_proto)
                {
                    std::ostringstream ss;
                    ss << "Item not allowed for Rankings: " << item_proto->Name1;
                    ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
                }
                return true;
            }
        }
    }
    return false;
}

void ResetChargesDummy(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
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
}

void SaveScoreboardDB(Player* pPlayer, uint32 dps, uint32 damage, uint32 time, uint8 vaelmode, uint8 mode)
{
    if (!pPlayer)
        return;

    if (!pPlayer->IsInWorld() || !pPlayer->IsPlayer())
        return;

    if (pPlayer->GetLevel() > 60)
        return;

    if (HasForbiddenItem(pPlayer))
    {
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("You used forbidden Items. This try will not be saved to the Ranking list.");
        return;
    }

    uint64      attack_character_guid   = pPlayer->GetGUID();
    std::string attack_name             = pPlayer->GetName();
    uint8       attack_class            = pPlayer->GetClass();
    uint8       attack_gender           = pPlayer->GetGender();
    uint16      attack_race             = pPlayer->GetRace();
    std::string attack_spec             = pPlayer->GetSpecNameByTalentPoints();
    uint16      attack_item_level       = DummyItemLevel(pPlayer);

    std::ostringstream GearOutputStream;

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            uint16 itemSlot = equippedItem->GetSlot();
            uint32 itemId = equippedItem->GetEntry();
            uint32 item_enchant_Id = GetEnchantID(pPlayer, i);
            //uint32 item_enchant_Id = equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
            ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemId);

            std::string itemName = item_proto->Name1;
            std::string enchantName;
            CharacterDatabase.escape_string(itemName);
            //std::remove(itemName.begin(), itemName.end(), '\'');

            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(item_enchant_Id);

            GearOutputStream << itemId << " " << itemName;

            if (spellInfo)
            {
                std::string enchantName = spellInfo->SpellName[sWorld.GetDefaultDbcLocale()];
                CharacterDatabase.escape_string(enchantName);
                //std::remove(enchantName.begin(), enchantName.end(), '\'');
                GearOutputStream << " with " << enchantName;
            }

            GearOutputStream << ": ";
        }
    }

    std::ostringstream TalentsOutputStream;

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

        uint32 spellid = 0;

        for (int rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            PlayerSpellMap const& uSpells = pPlayer->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = uSpells.begin(); itr != uSpells.end(); ++itr)
            {
                if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled)
                    continue;

                uint32 rank2 = sSpellMgr.GetSpellRank(itr->first);

                if (itr->first == talentInfo->RankID[rank] || sSpellMgr.IsSpellLearnToSpell(talentInfo->RankID[rank], itr->first))
                {
                    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(talentInfo->RankID[rank]);

                    if (spellInfo)
                    {
                        std::string spell_name = spellInfo->SpellName[sWorld.GetDefaultDbcLocale()];
                        CharacterDatabase.escape_string(spell_name);
                        TalentsOutputStream << spell_name;
                        if (rank > 0)
                            TalentsOutputStream << " (Rank" << rank + 1 << ")";

                        TalentsOutputStream << ": ";
                    }

                }
            }
        }
    }


    Player::SpellAuraHolderMap const& auraHolders = pPlayer->GetSpellAuraHolderMap();

    if (auraHolders.empty())
        return;

    std::ostringstream AurasOutputStream;

    AuraSaveStruct s;
    for (Player::SpellAuraHolderMap::const_iterator itr = auraHolders.begin(); itr != auraHolders.end(); ++itr)
    {
        SpellAuraHolder *holder = itr->second;

        if (!SaveTheAura(pPlayer, holder, s))
            continue;

        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(s.spellId);

        if (spellInfo)
        {
            std::string spell_name = spellInfo->SpellName[sWorld.GetDefaultDbcLocale()];
            CharacterDatabase.escape_string(spell_name);

            AurasOutputStream << s.spellId;

            if (!spell_name.empty())
                AurasOutputStream << " (" << spell_name << ")";

            AurasOutputStream << ": ";
        }

    }

    uint8 patch = 0;
    patch = sWorld.GetWowPatch();

    if (!dps)
        sLog.outBasic("[SaveScoreboardDB] no dps");

    if (!damage)
        sLog.outBasic("[SaveScoreboardDB] no damage");

    if (!patch)
        sLog.outBasic("[SaveScoreboardDB] no patch");

    if (!time)
        sLog.outBasic("[SaveScoreboardDB] no time");

    CharacterDatabase.PExecute("INSERT INTO attack_dummy_ranking (`character_guid`, `name`, `class`, `gender`, `race`, `spec`, `item_level`, `dps`, `damage_done`, `equipment_used`, `talents_used`, `auras_used`, `patch`, `vaelmode`, `mode`, `time`, `date`) VALUES ('%u', '%s', '%u', '%u', '%u', '%s', '%u', '%u', '%u', '%s', '%s', '%s', '%u', '%u', '%u', '%u', NOW());", pPlayer->GetGUID(), pPlayer->GetName(), pPlayer->GetClass(), pPlayer->GetGender(), pPlayer->GetRace(), pPlayer->GetSpecNameByTalentPoints().c_str(), DummyItemLevel(pPlayer), dps, damage, (GearOutputStream.str().c_str()), (TalentsOutputStream.str().c_str()), (AurasOutputStream.str().c_str()), patch, vaelmode, mode, time);
}

void ApplyImprovedDebuffsOnDummy(Player* pPlayer, Creature* pCreature, uint32 spellId)
{
    if (!pPlayer || !pCreature || !spellId)
        return;

    if (pCreature->HasAura(spellId))
        return;

    enum Talents
    {
        IMPROVED_HUNTERS_MARK_RANK1 = 19421,
        IMPROVED_HUNTERS_MARK_RANK2 = 19422,
        IMPROVED_HUNTERS_MARK_RANK3 = 19423,
        IMPROVED_HUNTERS_MARK_RANK4 = 19424,
        IMPROVED_HUNTERS_MARK_RANK5 = 19425,
        IMPROVED_SEAL_OF_THE_CRUSADER_RANK1 = 20335,
        IMPROVED_SEAL_OF_THE_CRUSADER_RANK2 = 20336,
        IMPROVED_SEAL_OF_THE_CRUSADER_RANK3 = 20337,
        IMPROVED_EXPOSE_ARMOR_RANK1 = 14168,
        IMPROVED_EXPOSE_ARMOR_RANK2 = 14169,
    };

    enum Talents talentpells[] = {
        IMPROVED_HUNTERS_MARK_RANK5,
        IMPROVED_SEAL_OF_THE_CRUSADER_RANK3,
        IMPROVED_EXPOSE_ARMOR_RANK2,
    };

    pPlayer->RemoveAurasDueToSpell(spellId);

    //learn all high ranks for buffing.
    for (uint32 i = 0; i < 3; ++i) {
        if (!pPlayer->HasSpell(talentpells[i]))
            pPlayer->AddAura(talentpells[i]);
    }

    uint8 Combo_Points = 0;

    if (spellId == 11198)
    {
        Combo_Points = pPlayer->GetComboPoints();
        pPlayer->AddComboPoints(pCreature, 5);
    }

    if (SpellAuraHolder * pHolder = pCreature->AddAura(spellId, NULL, pPlayer)) // this is important .. the aura must be added by the player or Expose Armor is only with 1 combo point instead of 5 from the player.
    {
        pHolder->SetAffectedByDebuffLimit(false);
        pHolder->SetPermanent(true);
        pHolder->UpdateAuraDuration();
    }

    if (spellId == 11198)
    {
        pPlayer->ClearComboPoints();
        pPlayer->AddComboPoints(pCreature, Combo_Points);
    }

    //remove all talent auras again.
    for (uint32 i = 0; i < 3; ++i) {
        if (!pPlayer->HasSpell(talentpells[i]))
            pPlayer->RemoveAurasDueToSpell(talentpells[i]);
    }
}

void ApplyDebuffsOnDummy(Creature* pCreature, uint32 spellId)
{
    if (!pCreature || !spellId)
        return;

    if (pCreature->HasAura(spellId))
        return;

    if (SpellAuraHolder* pHolder = pCreature->AddAura(spellId))
    {
        switch (spellId)
        {
            case 16928: //Armor Shatter
            case 17315: //Puncture Armor
                pHolder->SetStackAmount(3);
                break;
            case 11597: //Sunder Armor
            case 12579: //Winter's Chill
            case 15258: //Shadow Vulnerability Priest
            case 22959: //Improved Scorch
            case 9658:  //Flame Buffet - Arcanite Dragonling
            //case 12654: //Ignite
                pHolder->SetStackAmount(5);
                break;
        }
        pHolder->SetAffectedByDebuffLimit(false);
        pHolder->SetPermanent(true);
        pHolder->UpdateAuraDuration();
        pHolder->SetCasterGuid(NULL);
    }
}

void ApplyBuffs(Player* pPlayer, Creature* pCreature, uint8 DummyDebuffMode)
{
    if (!pPlayer || !pCreature)
        return;

    if (pCreature->IsInCombat())
    {
        switch (DummyDebuffMode)
        {
            case MeleeDummyMode:
            {
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 20303); //Improved Seal of the Crusader
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 14325); //Improved Hunter's Mark
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 11198); //Improved Expose Armor

                ApplyDebuffsOnDummy(pCreature, 9907); //Faerie Fire
                ApplyDebuffsOnDummy(pCreature, 15235); //Crystal Yield
                ApplyDebuffsOnDummy(pCreature, 11374); //Gift of Arthas
                ApplyDebuffsOnDummy(pCreature, 17348); //Hemorrhage
                ApplyDebuffsOnDummy(pCreature, 16928); //Armor Shatter - Annihilator
                ApplyDebuffsOnDummy(pCreature, 23577); //Expose Weakness
                ApplyDebuffsOnDummy(pCreature, 17315); //Puncture Armor
                ApplyDebuffsOnDummy(pCreature, 11717); //Curse of Recklessness
                ApplyDebuffsOnDummy(pCreature, 21992); //Thunderfury
                break;
            }
            case CasterDummyMode:
            {
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 20303); //Improved Seal of the Crusader

                ApplyDebuffsOnDummy(pCreature, 23605); //Spell Vulnerability - Nightfall Axe
                ApplyDebuffsOnDummy(pCreature, 22959); //Improved Scorch
                ApplyDebuffsOnDummy(pCreature, 9658); //Flame Buffet - Arcanite Dragonling
                //ApplyDebuffsOnDummy(me, 12654); //Ignite

                ApplyDebuffsOnDummy(pCreature, 21992); //Thunderfury
                ApplyDebuffsOnDummy(pCreature, 17937); //Curse of Shadow
                ApplyDebuffsOnDummy(pCreature, 12579); //Winter's Chill
                ApplyDebuffsOnDummy(pCreature, 17800); //Shadow Vulnerability Warlock
                ApplyDebuffsOnDummy(pCreature, 15258); //Shadow Vulnerability Priest
                ApplyDebuffsOnDummy(pCreature, 11722); //Curse of the Elements
                break;
            }
            case ModerateDummyMode:
            {
                //remove buffs that almost unabtainable for a raid with all other world buffs.
                pPlayer->RemoveAurasDueToSpell(29534);	//TRACES_OF_SILITHYST
                pPlayer->RemoveAurasDueToSpell(28681);	//SOUL_REVIVAL
                pPlayer->RemoveAurasDueToSpell(1386);	//ECHOES_OF_LORDAERON_ALLIANCE
                pPlayer->RemoveAurasDueToSpell(29520);	//ECHOES_OF_LORDAERON_HORDE
                pPlayer->RemoveAurasDueToSpell(24833); //HOLY_MIGHTSTONE
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 14325); //Improved Hunter's Mark
                ApplyDebuffsOnDummy(pCreature, 21992); //Thunderfury
                ApplyDebuffsOnDummy(pCreature, 9907); //Faerie Fire
                ApplyDebuffsOnDummy(pCreature, 11374); //Gift of Arthas
                ApplyDebuffsOnDummy(pCreature, 11597); //Sunder Armor
                ApplyDebuffsOnDummy(pCreature, 11717); //Curse of Recklessness
                ApplyDebuffsOnDummy(pCreature, 17937); //Curse of Shadow
                ApplyDebuffsOnDummy(pCreature, 12579); //Winter's Chill
                ApplyDebuffsOnDummy(pCreature, 17800); //Shadow Vulnerability Warlock
                ApplyDebuffsOnDummy(pCreature, 15258); //Shadow Vulnerability Priest
                ApplyDebuffsOnDummy(pCreature, 11722); //Curse of the Elements
                break;
            }
            case FullDummyMode:
            {
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 20303); //Improved Seal of the Crusader
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 14325); //Improved Hunter's Mark
                ApplyImprovedDebuffsOnDummy(pPlayer, pCreature, 11198); //Improved Expose Armor

                ApplyDebuffsOnDummy(pCreature, 11374); //Gift of Arthas
                ApplyDebuffsOnDummy(pCreature, 17348); //Hemorrhage
                ApplyDebuffsOnDummy(pCreature, 16928); //Armor Shatter - Annihilator
                ApplyDebuffsOnDummy(pCreature, 23577); //Expose Weakness
                ApplyDebuffsOnDummy(pCreature, 9907);  //Faerie Fire
                ApplyDebuffsOnDummy(pCreature, 15235); //Crystal Yield
                ApplyDebuffsOnDummy(pCreature, 11717); //Curse of Recklessness
                ApplyDebuffsOnDummy(pCreature, 17315); //Puncture Armor

                ApplyDebuffsOnDummy(pCreature, 23605); //Spell Vulnerability - Nightfall Axe
                ApplyDebuffsOnDummy(pCreature, 22959); //Improved Scorch
                ApplyDebuffsOnDummy(pCreature, 9658);  //Flame Buffet - Arcanite Dragonling

                ApplyDebuffsOnDummy(pCreature, 21992); //Thunderfury
                ApplyDebuffsOnDummy(pCreature, 17937); //Curse of Shadow
                ApplyDebuffsOnDummy(pCreature, 12579); //Winter's Chill
                ApplyDebuffsOnDummy(pCreature, 17800); //Shadow Vulnerability Warlock
                ApplyDebuffsOnDummy(pCreature, 15258); //Shadow Vulnerability Priest
                ApplyDebuffsOnDummy(pCreature, 11722); //Curse of the Elements
                break;
            }
        }
    }
}

bool GossipHello_npc_attack_dummy_boss(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer || !pCreature)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    pPlayer->PlayerTalkClass->ClearMenus();

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "FULL MODE:\nStart with every Debuff (Only significant for Retri and Enhancer or Melees with Spellpower equip).\n\n", GOSSIP_SENDER_MAIN + 999, 0);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "MELEE MODE:\nStart with all improved Melee Debuffs.\n\n", GOSSIP_SENDER_MAIN + 999, 1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "CASTER MODE:\nStart with all improved Spell Debuffs.\n\n", GOSSIP_SENDER_MAIN + 999, 2);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "MODERATE MODE:\nStart with default debuffs (As it will probably be in a real Raid).\n\n", GOSSIP_SENDER_MAIN + 999, 3);
    pPlayer->SEND_GOSSIP_MENU(6000002, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_attack_dummy_boss(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer || !pCreature)
        return false;

    switch (uiAction)
    {
        case 0:
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "1 Minute Fight\n\n", FullDummyMode, 6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "2 Minute Fight\n\n", FullDummyMode, 4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "5 Minute Fight\n\n", FullDummyMode, 5);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Vaelastrasz Fight (1 Minute)\n\n", FullDummyMode, VaelMode);
            pPlayer->SEND_GOSSIP_MENU(1, pCreature->GetObjectGuid());
            break;
        }
        case 1:
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "1 Minute Fight\n\n", MeleeDummyMode, 6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "2 Minute Fight\n\n", MeleeDummyMode, 4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "5 Minute Fight\n\n", MeleeDummyMode, 5);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Vaelastrasz Fight (1 Minute)\n\n", MeleeDummyMode, VaelMode);
            pPlayer->SEND_GOSSIP_MENU(1, pCreature->GetObjectGuid());
            break;
        }
        case 2:
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "1 Minute Fight\n\n", CasterDummyMode, 6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "2 Minute Fight\n\n", CasterDummyMode, 4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "5 Minute Fight\n\n", CasterDummyMode, 5);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Vaelastrasz Fight (1 Minute)\n\n", CasterDummyMode, VaelMode);
            pPlayer->SEND_GOSSIP_MENU(1, pCreature->GetObjectGuid());
            break;
        }
        case 3:
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "1 Minute Fight\n\n", ModerateDummyMode, 6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "2 Minute Fight\n\n", ModerateDummyMode, 4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "5 Minute Fight\n\n", ModerateDummyMode, 5);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Vaelastrasz Fight (1 Minute)\n\n", ModerateDummyMode, VaelMode);
            pPlayer->SEND_GOSSIP_MENU(1, pCreature->GetObjectGuid());
            break;
        }
        case 4:
        {
            pCreature->SetMaxHealth(TwoMinutesMode);
            if (pCreature && pCreature->AI())
                pCreature->AI()->JustSummoned(pCreature);
            //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            std::ostringstream ss;
            ss << pPlayer->GetName() << " has chosen a 2 Minute ";
            switch (uiSender)
            {
                case FullDummyMode:
                    ss << "Fully Debuff Dummy";
                    pCreature->AddAura(FullDummyAura);
                    break;
                case MeleeDummyMode:
                    ss << "Melee Debuff Dummy";
                    pCreature->AddAura(MeleeDummyAura);
                    break;
                case CasterDummyMode:
                    ss << "Caster Debuff Dummy";
                    pCreature->AddAura(CasterDummyAura);
                    break;
                case ModerateDummyMode:
                    ss << "Moderate Debuff Dummy";
                    pCreature->AddAura(ModerateDummyAura);
                    break;
            }
            pCreature->MonsterTextEmote(ss.str().c_str());
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case 5:
        {
            pCreature->SetMaxHealth(FiveMinutesMode);
            if (pCreature && pCreature->AI())
                pCreature->AI()->JustSummoned(pCreature);
            //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            std::ostringstream ss;
            ss << pPlayer->GetName() << " has chosen a 5 Minute ";
            switch (uiSender)
            {
                case FullDummyMode:
                    ss << "Fully Debuff Dummy";
                    pCreature->AddAura(FullDummyAura);
                    break;
                case MeleeDummyMode:
                    ss << "Melee Debuff Dummy";
                    pCreature->AddAura(MeleeDummyAura);
                    break;
                case CasterDummyMode:
                    ss << "Caster Debuff Dummy";
                    pCreature->AddAura(CasterDummyAura);
                    break;
                case ModerateDummyMode:
                    ss << "Moderate Debuff Dummy";
                    pCreature->AddAura(ModerateDummyAura);
                    break;
            }
            pCreature->MonsterTextEmote(ss.str().c_str(), NULL);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case 6:
        {
            pCreature->SetMaxHealth(OneMinutesMode);
            if (pCreature && pCreature->AI())
                pCreature->AI()->JustSummoned(pCreature);
            //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            std::ostringstream ss;
            ss << pPlayer->GetName() << " has chosen a 1 Minute ";
            switch (uiSender)
            {
                case FullDummyMode:
                    ss << "Fully Debuff Dummy";
                    pCreature->AddAura(FullDummyAura);
                    break;
                case MeleeDummyMode:
                    ss << "Melee Debuff Dummy";
                    pCreature->AddAura(MeleeDummyAura);
                    break;
                case CasterDummyMode:
                    ss << "Caster Debuff Dummy";
                    pCreature->AddAura(CasterDummyAura);
                    break;
                case ModerateDummyMode:
                    ss << "Moderate Debuff Dummy";
                    pCreature->AddAura(ModerateDummyAura);
                    break;
            }
            pCreature->MonsterTextEmote(ss.str().c_str(), NULL);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
        case VaelMode:
        {
            pCreature->UpdateEntry(700095);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->SetMaxHealth(VaelMode);
            pCreature->SetHealthPercent(30.0f);
            if (pCreature && pCreature->AI())
                pCreature->AI()->JustSummoned(pCreature);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->PlayDirectSound(8284, pPlayer);

            std::ostringstream ss;
            ss << pPlayer->GetName() << " has chosen a Vaelastrasz ";
            switch (uiSender)
            {
                case FullDummyMode:
                    ss << "Fully Debuff Dummy";
                    pCreature->AddAura(FullDummyAura);
                    break;
                case MeleeDummyMode:
                    ss << "Melee Debuff Dummy";
                    pCreature->AddAura(MeleeDummyAura);
                    break;
                case CasterDummyMode:
                    ss << "Caster Debuff Dummy";
                    pCreature->AddAura(CasterDummyAura);
                    break;
                case ModerateDummyMode:
                    ss << "Moderate Debuff Dummy";
                    pCreature->AddAura(ModerateDummyAura);
                    break;
            }
            pCreature->MonsterTextEmote(ss.str().c_str(), NULL);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        }
    }
    return true;
}

bool GossipHello_npc_announcer_dummy(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "One Minute Rankings.", GOSSIP_SENDER_MAIN + 1, 399);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Two Minute Rankings.", GOSSIP_SENDER_MAIN + 1, 400);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Five Minute Rankings.", GOSSIP_SENDER_MAIN + 1, 401);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Vaelastrasz Rankings.", GOSSIP_SENDER_MAIN + 1, 402);
    pPlayer->SEND_GOSSIP_MENU(6000003, pCreature->GetObjectGuid());
    return true;
}

uint32 fighttime = 60;
uint8 vaelmode = 0;

std::string GetClassColor(uint8 cClass)
{
    std::string classcolor = "cFFC79C6E";
    
    if		(cClass == 1)	classcolor = "cFF8F693D";
    else if (cClass == 9)	classcolor = "cFF4E55B4";
    else if (cClass == 8)   classcolor = "cFF0091B3";
    else if (cClass == 11)  classcolor = "cFFBE4900";
    else if (cClass == 5)	classcolor = "cFFFFFFFF";
    else if (cClass == 3)	classcolor = "cFF759D40";
    else if (cClass == 4)   classcolor = "cFFC3BD30";
    else if (cClass == 2)	classcolor = "cFFBB5785";
    else if (cClass == 7)	classcolor = "cFF0042A6";

    return classcolor;
}

bool GossipSelect_npc_announcer_dummy(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiSender)
    {
        case GOSSIP_SENDER_MAIN + 1:
        {
            if (uiAction == 399)
            {
                fighttime = 60;
                vaelmode = 0;
            }
            if (uiAction == 400)
            {
                fighttime = 120;
                vaelmode = 0;
            }
            if (uiAction == 401)
            {
                fighttime = 300;
                vaelmode = 0;
            }
            if (uiAction == 402)
            {
                fighttime = 60;
                vaelmode = 1;
            }

            uint8 getpatch = sWorld.GetWowPatch();

            if (!getpatch)
                return false;

            // Sort the list by DPS then Get only One Class of each and sort again by best DPS.
            QueryResult* result = CharacterDatabase.PQuery("SELECT guid, name, class, spec, MAX(dps) AS dps FROM attack_dummy_ranking WHERE vaelmode = '%u' AND time = '%u' AND patch = '%u' GROUP BY spec ORDER BY `dps` DESC", vaelmode, fighttime, getpatch);

            if (result)
            {
                uint32 counter = 0;
                do
                {
                    Field* fields = result->Fetch();

                    uint32      attack_guid				= fields[0].GetUInt32();
                    std::string attack_name				= fields[1].GetString();
                    uint8       attack_class			= fields[2].GetUInt8();
                    std::string attack_spec				= fields[3].GetString();
                    uint32      attack_dps				= fields[4].GetUInt32();

                    std::ostringstream ss;

                    if (counter < 10)
                    {
                        ss << "[" << counter + 1 << "] " << attack_dps << " DPS, " << "|" << GetClassColor(attack_class) << sDummyClassString(attack_class) << " " << attack_spec << "|r" << "\n" << "     " << attack_name << "\n";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_MAIN, attack_guid);
                    }
                    else
                        continue;

                    counter++;

                } while (result->NextRow());
                delete result;
            }
            if (uiAction != 402)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Class Rankings", GOSSIP_SENDER_MAIN + 2, 200);

            pPlayer->SEND_GOSSIP_MENU(600004, pCreature->GetObjectGuid());
            break;
        }
        case GOSSIP_SENDER_MAIN:
        {
            QueryResult *getresult = CharacterDatabase.PQuery("SELECT * FROM attack_dummy_ranking WHERE guid='%u'", uiAction);

            if (getresult)
            {
                Field* fields = getresult->Fetch();

                uint32      attack_guid				= fields[0].GetUInt32();
                uint64      attack_character_guid	= fields[1].GetUInt64();
                std::string attack_name				= fields[2].GetString();
                uint8 attack_class			= fields[3].GetUInt8();
                uint16      attack_race				= fields[5].GetUInt16();
                std::string attack_spec				= fields[6].GetString();
                uint16      attack_item_level		= fields[7].GetUInt16();
                uint32      attack_dps				= fields[8].GetUInt32();
                std::string attack_equipment_used	= fields[10].GetString();
                std::string attack_talents_used		= fields[11].GetString();

                std::string attack_race_str;

                std::ostringstream ss;

                switch (attack_race)
                {
                case 1:
                    attack_race_str = "Human";
                    break;
                case 2:
                    attack_race_str = "Orc";
                    break;
                case 3:
                    attack_race_str = "Dwarf";
                    break;
                case 4:
                    attack_race_str = "Nightelf";
                    break;
                case 5:
                    attack_race_str = "Undead";
                    break;
                case 6:
                    attack_race_str = "Tauren";
                    break;
                case 7:
                    attack_race_str = "Gnome";
                    break;
                case 8:
                    attack_race_str = "Troll";
                    break;
                }

                std::replace(attack_equipment_used.begin(), attack_equipment_used.end(), ':', '\n');
                std::replace(attack_talents_used.begin(), attack_talents_used.end(), ':', '\n');

                ss << "DPS Information for " << attack_name << "\n" << attack_dps;
                ss << " DPS as " << attack_race_str << " " << attack_spec << " ";
                ss << sDummyClassString(attack_class) << " with Itemlevel " << attack_item_level << "\n" << " ";
                ss << attack_equipment_used;
                ss << attack_talents_used;

                ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());

                delete getresult;
            }
            GossipHello_npc_announcer_dummy(pPlayer, pCreature);
            break;
        }
        case GOSSIP_SENDER_MAIN + 2:
        {
            uint8 getpatch = sWorld.GetWowPatch();

            pPlayer->PlayerTalkClass->ClearMenus();
            QueryResult *result = CharacterDatabase.PQuery("SELECT character_guid, class FROM attack_dummy_ranking WHERE time = '%u' AND patch = '%u' GROUP BY class", fighttime, getpatch);

            if (result)
            {
                do
                {
                    Field* fields = result->Fetch();
                    uint32 GUID = fields[0].GetUInt32();
                    uint8 Class = fields[1].GetUInt8();

                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sDummyClassString(Class).c_str(), GOSSIP_SENDER_MAIN + 4, GUID);

                } while (result->NextRow());
                delete result;
            }

            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN + 3, 200);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SENDER_MAIN + 1, pCreature->GetObjectGuid());
            break;
        }
        case GOSSIP_SENDER_MAIN + 3:
            GossipHello_npc_announcer_dummy(pPlayer, pCreature);
            break;
        case GOSSIP_SENDER_MAIN + 4:
        {
            uint8 getpatch = sWorld.GetWowPatch();

            pPlayer->PlayerTalkClass->ClearMenus();
            QueryResult *GetClass = CharacterDatabase.PQuery("SELECT character_guid, class FROM attack_dummy_ranking WHERE time = '%u' AND character_guid='%u' AND patch = '%u'", fighttime, uiAction, getpatch);

            if (GetClass)
            {
                Field* fields = GetClass->Fetch();
                std::string Class = fields[1].GetString();

                QueryResult *result = CharacterDatabase.PQuery("SELECT * FROM attack_dummy_ranking WHERE time = '%u' AND class='%s' AND patch = '%u' ORDER BY dps DESC", fighttime, Class.c_str(), getpatch);

                if (result)
                {
                    uint32 counter = 0;
                    do
                    {
                        Field* fields = result->Fetch();

                        uint32      attack_guid = fields[0].GetUInt32();
                        uint64      attack_character_guid = fields[1].GetUInt64();
                        std::string attack_name = fields[2].GetString();
                        uint8       attack_class = fields[3].GetUInt8();
                        uint16      attack_race = fields[5].GetUInt16();
                        std::string attack_spec = fields[6].GetString();
                        uint16      attack_item_level = fields[7].GetUInt16();
                        uint32      attack_dps = fields[8].GetUInt32();
                        uint8		attack_patch = fields[13].GetUInt32();
                        uint32      attack_time = fields[14].GetUInt32();

                        std::ostringstream ss;

                        if (counter <= 10)
                        {
                            ss << counter + 1 << ". " << attack_name << ". " << attack_dps << " DPS, " << attack_spec;
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_MAIN, attack_guid);
                        }
                        else
                            continue;

                        counter++;

                    } while (result->NextRow());
                    delete result;
                }
                delete GetClass;
            }

            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN + 2, 200);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SENDER_MAIN + 1, pCreature->GetObjectGuid());
            break;
        }
    }
    return true;
}

enum eDummyEvents
{
    EVENT_EXEUTE = 1,
    EVENT_END = 2,
    EVENT_PULL1 = 3,
    EVENT_PULL2 = 4,
    EVENT_PULL3 = 5,
    EVENT_PULL4 = 6,
    EVENT_PULL5 = 7,
    EVENT_PULL6 = 8,
    EVENT_PULL7 = 9,
    EVENT_PULL8 = 10,
    EVENT_PULL9 = 11,
    EVENT_PULL10 = 12,
    EVENT_PULLGO = 13,
    EVENT_PULL_RESET = 14,
    EVENT_BUFFS = 16,
    EVENT_CHECK_COMBAT = 17,
    EVENT_HATEFULSTRIKE = 18,
    EVENT_HEAL_CHECK = 19,
    EVENT_HITSELF = 20,
    EVENT_SHOW_DPS = 21,
    EVENT_BURNINGADRENALINE = 22
};

struct npc_attack_dummy_bossAI : public ScriptedAI
{
    npc_attack_dummy_bossAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    EventMap    m_events;
    bool        m_bExecute;
    bool        m_bEnd;

    uint32 m_uiCombatTimer;

    std::unordered_map<ObjectGuid, time_t> dpstimer;
    std::unordered_map<ObjectGuid, SpellEntry const*> spelllist;
    std::unordered_map<ObjectGuid, time_t> attackers;
    std::unordered_map<ObjectGuid, uint32> damagedealers;

    enum Talents
    {
        IMPROVED_WEAPON_TOTEMS_RANK1 = 29192,
        IMPROVED_WEAPON_TOTEMS_RANK2 = 29193
    };

    void LearnWeaponTotems(Player* pPlayer)
    {
        if (!pPlayer)
            return;

        if (pPlayer->GetTeam() != HORDE)
            return;

        if (pPlayer->GetClass() == CLASS_SHAMAN && pPlayer->HasSpell(IMPROVED_WEAPON_TOTEMS_RANK2))
            return;

        pPlayer->AddAura(IMPROVED_WEAPON_TOTEMS_RANK2, NULL, m_creature);
    }

    void Reset()
    {
        //ClearAllAttackers();
        m_creature->UpdateEntry(Maexxna);
        m_creature->CombatStop(true);
        m_events.Reset();
        m_creature->RemoveAllAurasOnDeath();
        m_bExecute = false;
        m_bEnd = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        dpstimer.clear();
        spelllist.clear();
        damagedealers.clear();
        attackers.clear();
        m_creature->SetHealthPercent(100.0f);
        m_creature->MonsterTextEmote("%s reseted.");
    }

    void AttackStart(Unit* pWho)
    {
        if (pWho && pWho->IsPlayer())
            AddAttackerToList(pWho->ToPlayer());
    }

    void AddSpellToList(Unit* pWho, SpellEntry const* spellProto)
    {
        if (!pWho)
            return;

        if (!spellProto)
            return;

        auto itr = spelllist.find(pWho->GetObjectGuid());

        if (itr != spelllist.end())
            itr->second = spellProto;
        else
            spelllist.emplace(pWho->GetObjectGuid(), spellProto);
    }

    void AddDPSTimerToList(Unit* pWho)
    {
        if (!pWho)
            return;

        auto itr = dpstimer.find(pWho->GetObjectGuid());

        if (itr == dpstimer.end())
        {
            dpstimer.emplace(pWho->GetObjectGuid(), std::time(nullptr));
        }
    }

    void AddAttackerToList(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!pWho)
            return;

        auto itr = attackers.find(pWho->GetObjectGuid());
        if (itr != attackers.end())
        {
            itr->second = std::time(nullptr);
            if (m_creature->GetEntry() != Vaelastrasz)
            {
                pWho->RemoveAurasDueToSpell(ESSENCE_OF_THE_RED);
                pWho->RemoveAurasDueToSpell(BURNING_ADRENALINE);
            }
            if (m_creature->CanReachWithMeleeAutoAttack(pWho->ToPlayer(), 9.0f) && !pWho->HasAura(LEADER_OF_THE_PACK))
                pWho->AddAura(LEADER_OF_THE_PACK);
        }
        else
        {
            if (pWho->IsPlayer() && !pWho->IsInCombat() && pWho->IsWithinCombatDistInMap(m_creature, VISIBILITY_DISTANCE_SMALL))
            {
                pWho->ToPlayer()->DuelComplete(DUEL_INTERRUPTED);	// end duels.
                pWho->ToPlayer()->SetGameMaster(false);				// gm off.
                //check if player has smoke aura to avoid reset cooldowns on other dummies.
                if (!pWho->HasAura(ALREADY_IN_FIGHT) && !pWho->HasAura(FEIGN_DEATH) && !pWho->HasAura(VANISH_RANK1) && !pWho->HasAura(VANISH_RANK2)) //hunter with feign death and rogue with vanish could possibly exploit this.
                {
                    if (m_creature->GetMaxHealth() == VaelMode)
                    {
                        pWho->AddAura(ESSENCE_OF_THE_RED, NULL, m_creature);
                        pWho->ToPlayer()->CastSpell(pWho->ToPlayer(), SPAWN_RED_LIGHTNING, true);
                    }

                    pWho->SetHealthPercent(100.0f);
                    pWho->ToPlayer()->SetPower(POWER_MANA, pWho->ToPlayer()->GetMaxPower(POWER_MANA));
                    pWho->ToPlayer()->SetPower(POWER_ENERGY, pWho->ToPlayer()->GetMaxPower(POWER_ENERGY));
                    pWho->RemoveAllSpellCooldown();
                    pWho->RemoveAllAurasOnDeathByCaster();
                    LearnWeaponTotems(pWho->ToPlayer());
                    ResetChargesDummy(pWho->ToPlayer());
                    pWho->ToPlayer()->SetPower(POWER_RAGE, 0);
                    pWho->AddAura(ALREADY_IN_FIGHT, NULL, m_creature);
                    if (m_creature->CanReachWithMeleeAutoAttack(pWho->ToPlayer(), 9.0f) && pWho->ToPlayer()->GetTeam() == HORDE && !pWho->HasAura(WINDFURY_TOTEM_PASSIVE))
                        pWho->ToPlayer()->AddAura(WINDFURY_TOTEM_PASSIVE, NULL, m_creature);
                    if (m_creature->HasAura(MeleeDummyAura) || m_creature->HasAura(FullDummyAura))
                        pWho->ToPlayer()->AddAura(SPELL_BATTLE_SQUAWK, NULL, m_creature);
                    if (m_creature->HasAura(CasterDummyMode) || m_creature->HasAura(FullDummyAura))
                        pWho->ToPlayer()->AddAura(POWER_INFUSION, NULL, m_creature);
                }

                m_creature->PlayDirectSound(BG_WS_SOUND_HORDE_FLAG_PICKED_UP, pWho->ToPlayer());
            }
            attackers.emplace(pWho->GetObjectGuid(), std::time(nullptr));
        }
    }

    void AddDamageDealerToList(Player* pWho, uint32 uiDamage)
    {
        if (!pWho)
            return;

        auto itr = damagedealers.find(pWho->GetObjectGuid());
        if (itr != damagedealers.end())
        {
            itr->second = itr->second + uiDamage;
            //ChatHandler(pWho->ToPlayer()).PSendSysMessage("AddDamageDealerToList: %u dmg done so far by, %u.", itr->second, pWho->GetName());
        }
        else
        {
            damagedealers.emplace(pWho->GetObjectGuid(), uiDamage);
            //ChatHandler(pWho->ToPlayer()).PSendSysMessage("AddDamageDealerToList: %s added to the list with %u damage.", pWho->GetName(), uiDamage);
        }
    }

    void EraseAttackersDPS(Unit* pWho)
    {
        if (!pWho)
            return;

        auto itr = damagedealers.find(pWho->GetObjectGuid());
        if (itr != damagedealers.end())
        {
            Player* pDamageDealers = ObjectAccessor::FindPlayer(itr->first);

            if (pDamageDealers)
                pDamageDealers->RemoveAllAurasFrom(m_creature->GetObjectGuid());

            itr = damagedealers.erase(itr);
        }
    }

    void ClearAllAttackers()
    {
        for (auto itr = damagedealers.begin(); itr != damagedealers.end();)
        {
            Player* pAttacker = ObjectAccessor::FindPlayer(itr->first);

            if (!pAttacker)
                continue;

            pAttacker->RemoveAllAurasFrom(m_creature->GetObjectGuid());
            itr = damagedealers.erase(itr);

            ++itr;
        }
    }

    void GetDPS(Unit* pWho, uint32& damage)
    {
        if (!pWho)
            return;

        #define COLOR_HORDE		"FF3300"
        #define COLOR_ALLIANCE	"0066B3"
        #define COLOR_SPELL		"ffffff"
        #define COLOR_INFO		"FF9900"
        #define COLOR_HEAL		"00b324"
        #define DO_COLOR(a, b)	"|cff" a "" b "|r"

        uint32 Damage = 0;
        uint32 DPS = 0;
        uint32 TIME = 0;

        auto itr = damagedealers.find(pWho->GetObjectGuid());

        if (itr == damagedealers.end())
            return;

            if (Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first))
            {
                if (!pAttacker || !pAttacker->IsInWorld() || !pAttacker->IsPlayer())
                    return;

                if (itr->second)
                {
                    Damage = (itr->second);
                    auto itr2 = dpstimer.find(pAttacker->GetObjectGuid());
                    if (itr2 != dpstimer.end())
                        if (itr2->second)
                            TIME = std::time(nullptr) - (itr2->second);

                    if (TIME > 0)
                        DPS = Damage / TIME;

                    std::ostringstream spellname;

                    auto itr3 = spelllist.find(pWho->GetObjectGuid());

                    if (itr3 != spelllist.end())
                    {
                        if (itr3->second)
                        {
                            std::string SpellName = itr3->second->SpellName[0];
                            std::string SpellRank = itr3->second->Rank[0];
                            if (SpellRank.empty())
                                spellname << SpellName;
                            else
                                spellname << SpellName << " (" << SpellRank << ")";

                            itr3 = spelllist.erase(itr3);
                        }
                        else
                            spellname << "Attack";
                    }

                    ChatHandler(pWho->ToPlayer()).PSendSysMessage("" DO_COLOR(COLOR_SPELL, "%s ") DO_COLOR(COLOR_HEAL, "%u")" | DPS:" DO_COLOR(COLOR_INFO, " %u")" | Time:" DO_COLOR(COLOR_HORDE, " %u")" | Total:"  DO_COLOR(COLOR_ALLIANCE, " %2u"),
                        spellname.str().c_str(), damage, DPS, TIME, Damage);

                }
            }
    }

    void SpellHit(SpellCaster* pWho, const SpellEntry* spell) override
    {
        AddSpellToList(pWho->ToUnit(), spell);
    }

    void DamageTaken(Unit* pWho, uint32& uiDamage) override
    {
        if (m_creature->GetHealth() <= uiDamage * 2)
            m_creature->SetHealth(uiDamage * 3);

        if (!pWho)
            return;

        Player* pDealer = pWho->GetCharmerOrOwnerPlayerOrPlayerItself();

        if (pDealer)
        {
            AddAttackerToList(pDealer);
            AddDamageDealerToList(pDealer, uiDamage);
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->ForcedDespawn(true); // instant despawn if Dummy dies (can happen)
    }

    void JustSummoned(Creature* pCreature) override
    {
        m_events.ScheduleEvent(EVENT_PULL6, 1000);
        m_creature->CastSpell(me, TRANSFORMATION, true);
    }

    void Aggro(Unit* pWho)
    {

        if (Creature* Announcer = m_creature->FindNearestCreature(600004, 20.0f))
        {
            Announcer->MonsterYell("LET’S GET READY TO RUMBLEEEEEEE!");
            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
        }
        if (Creature * Announcer = m_creature->FindNearestCreature(600005, 20.0f))
        {
            Announcer->MonsterYell("LET’S GET READY TO RUMBLEEEEEEE!");
            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
        }
        switch (m_creature->GetMaxHealth())
        {
            case TwoMinutesMode:
                m_events.ScheduleEvent(EVENT_EXEUTE, 96000); // 1 min 36 sec?
                m_events.ScheduleEvent(EVENT_END, 120000); // 2 minutes.
                break;
            case FiveMinutesMode:
                m_events.ScheduleEvent(EVENT_EXEUTE, 240000); // 4 minutes.
                m_events.ScheduleEvent(EVENT_END, 300000);	// 5 minutes.
                break;
            case OneMinutesMode:
                m_events.ScheduleEvent(EVENT_EXEUTE, 48000);
                m_events.ScheduleEvent(EVENT_END, 60000);	// 5 minutes.
                break;
            case VaelMode:
                m_events.ScheduleEvent(EVENT_EXEUTE, 20000);
                m_events.ScheduleEvent(EVENT_END, 60000);	// 1 minute.
                m_events.ScheduleEvent(EVENT_BURNINGADRENALINE, 40000);
                break;
        }

        if (!pWho)
            return;

        uint8 mode = FullDummyMode;

        if (m_creature->HasAura(CasterDummyAura))
            mode = CasterDummyMode;
        else if (m_creature->HasAura(MeleeDummyAura))
            mode = MeleeDummyMode;
        else if (m_creature->HasAura(ModerateDummyAura))
            mode = ModerateDummyMode;
        else if (m_creature->HasAura(FullDummyAura))
            mode = FullDummyMode;

        if (pWho->IsPet())
            if (pWho->GetOwner()->ToPlayer())
                ApplyBuffs(pWho->GetOwner()->ToPlayer(), me, mode);

        if (pWho->IsPlayer())
            ApplyBuffs(pWho->ToPlayer(), me, mode);

        m_events.ScheduleEvent(EVENT_CHECK_COMBAT, 0);
    }

    void UpdateAI(const uint32 diff)
    {
        m_events.Update(diff);

        if (!me)
            return;

        if (!m_creature->IsInCombat())
        {
            while (uint32 Start_Events = m_events.ExecuteEvent())
            {
                switch (Start_Events)
                {
                    case EVENT_PULL6:
                    {
                        m_creature->MonsterTextEmote("%s is attackable in 5 seconds.", NULL);
                        m_creature->PlayDirectSound(116, 0);
                        m_events.ScheduleEvent(EVENT_PULL7, 1000);
                        break;
                    }
                    case EVENT_PULL7:
                    {
                        m_creature->MonsterTextEmote("%s is attackable in 4 seconds", NULL);
                        m_creature->PlayDirectSound(116, 0);
                        m_events.ScheduleEvent(EVENT_PULL8, 1000);
                        break;
                    }
                    case EVENT_PULL8:
                    {
                        m_creature->MonsterTextEmote("%s is attackable in 3 seconds", NULL);
                        m_creature->PlayDirectSound(116, 0);
                        m_events.ScheduleEvent(EVENT_PULL9, 1000);
                        break;
                    }
                    case EVENT_PULL9:
                    {
                        m_creature->MonsterTextEmote("%s is attackable in 2 seconds", NULL);
                        m_creature->PlayDirectSound(116, 0);
                        m_events.ScheduleEvent(EVENT_PULL10, 1000);
                        break;
                    }
                    case EVENT_PULL10:
                    {
                        m_creature->MonsterTextEmote("%s is attackable in 1 seconds", NULL);
                        m_creature->PlayDirectSound(116, 0);
                        m_events.ScheduleEvent(EVENT_PULLGO, 1000);
                        break;
                    }
                    case EVENT_PULLGO:
                    {
                        m_creature->MonsterTextEmote("%s is attackable and will reset in 10 second if no one attacks!", NULL);
                        m_creature->CastSpell(me, 28136, true);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        //m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_creature->PlayDirectSound(BG_WS_SOUND_FLAGS_RESPAWNED, 0);
                        m_events.ScheduleEvent(EVENT_PULL_RESET, 10000);
                        break;
                    }
                    case EVENT_PULL_RESET:
                    {
                        m_creature->PlayDirectSound(BG_WS_SOUND_FLAG_RETURNED, 0);
                        m_creature->ForcedDespawn();
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        else if (m_creature->IsInCombat())
        {
            while (uint32 Combat_Events = m_events.ExecuteEvent())
            {
                uint64 PlayerCount = 0;
                switch (Combat_Events)
                {
                    case EVENT_CHECK_COMBAT:
                    {
                        for (auto itr = attackers.begin(); itr != attackers.end();)
                        {
                            if (Unit* pAttacker = ObjectAccessor::FindPlayer(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld() || !pAttacker->IsInMap(me) || !pAttacker->IsWithinDistInMap(me, VISIBILITY_DISTANCE_SMALL))
                                {
                                    EraseAttackersDPS(pAttacker);
                                    itr = attackers.erase(itr);
                                    continue;
                                }
                                if (itr->second + 10 < std::time(nullptr))
                                {
                                    m_creature->PlayDirectSound(BG_AV_SOUND_ALLIANCE_GOOD, pAttacker->ToPlayer());
                                    EraseAttackersDPS(pAttacker);
                                    if (Pet * pet = pAttacker->ToPlayer()->GetPet())
                                        m_creature->GetThreatManager().modifyThreatPercent(pet, -101.0f);
                                    if (Unit * charm = pAttacker->ToPlayer()->GetCharm())
                                        m_creature->GetThreatManager().modifyThreatPercent(charm, -101.0f);
                                    m_creature->GetThreatManager().modifyThreatPercent(pAttacker, -101.0f);
                                    itr = attackers.erase(itr);
                                    m_creature->_removeAttacker(pAttacker);
                                    continue;
                                }
                            }
                            else
                            {
                                EraseAttackersDPS(pAttacker);
                                itr = attackers.erase(itr);
                            }

                            ++PlayerCount;

                            ++itr;
                        }
                        if (PlayerCount < 1)
                            EnterEvadeMode();

                        m_events.ScheduleEvent(EVENT_CHECK_COMBAT, 1000);
                        break;
                    }
                    case EVENT_BURNINGADRENALINE:
                    {
                        for (auto itr = attackers.begin(); itr != attackers.end();)
                        {
                            if (Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld())
                                    continue;

                                if (pAttacker && pAttacker->IsPlayer() && pAttacker->IsAlive() && pAttacker->GetPowerType() == POWER_MANA && !pAttacker->HasAura(BURNING_ADRENALINE, EFFECT_INDEX_0))
                                {
                                    pAttacker->CastSpell(pAttacker, BURNING_ADRENALINE, true, nullptr, nullptr, m_creature->GetObjectGuid());
                                    pAttacker->RemoveAura(BURNING_ADRENALINE, EFFECT_INDEX_2); // Remove the Dmg effect.
                                }
                            }

                            ++itr;
                        }
                        break;
                    }
                    case EVENT_BUFFS:
                    {
                        for (auto itr = attackers.begin(); itr != attackers.end();)
                        {
                            if (Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld())
                                    continue;

                                if (m_creature->CanReachWithMeleeAutoAttack(pAttacker, 9.0f))
                                    pAttacker->AddAura(FURIOUS_HOWL, NULL, m_creature);
                            }

                            ++itr;
                        }
                        m_events.ScheduleEvent(EVENT_BUFFS, 10000);
                        break;
                    }
                    case EVENT_EXEUTE:
                    {
                        for (auto itr = attackers.begin(); itr != attackers.end();)
                        {
                            if (Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld() || !pAttacker->IsPlayer())
                                    continue;

                                m_creature->PlayDirectSound(BG_AV_SOUND_ALLIANCE_ASSAULTS, pAttacker->ToPlayer());
                            }
                            ++itr;
                        }
                        if (Creature* Announcer = m_creature->FindNearestCreature(600004, 20.0f))
                        {
                            Announcer->MonsterYell("EXECUTE TIME!");
                            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        }
                        if (Creature * Announcer = m_creature->FindNearestCreature(600005, 20.0f))
                        {
                            Announcer->MonsterYell("EXECUTE TIME!");
                            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        }
                        m_creature->SetHealthPercent(19.0f);
                        m_bExecute = true;
                        break;
                    }
                    case EVENT_END:
                    {
                        m_creature->CombatStop(true);
                        std::ostringstream ss;
                        uint32 old_best_dps = 0;

                        QueryResult *dps_result = CharacterDatabase.PQuery("SELECT MAX(dps) FROM attack_dummy_ranking");

                        if (dps_result)
                        {
                            Field* fields = dps_result->Fetch();
                            uint32      attack_dps = fields[0].GetUInt32();
                            old_best_dps = attack_dps;
                            delete dps_result;
                        }

                        uint8 vaelmode = 0;

                        if (m_creature->GetMaxHealth() == VaelMode)
                            vaelmode = 1;

                        uint8 mode = FullDummyMode;

                        if (m_creature->HasAura(CasterDummyAura))
                            mode = CasterDummyMode;
                        else if (m_creature->HasAura(MeleeDummyAura))
                            mode = MeleeDummyMode;
                        else if (m_creature->HasAura(ModerateDummyAura))
                            mode = ModerateDummyMode;
                        else if (m_creature->HasAura(FullDummyAura))
                            mode = FullDummyMode;

                        m_creature->Kill(me, nullptr);

                        for (auto itr = damagedealers.begin(); itr != damagedealers.end();)
                        {
                            if (Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld() || !pAttacker->IsPlayer())
                                    continue;

                                if (itr->second)
                                {
                                    uint32 Damage = (itr->second);
                                    uint32 DPS = 0;
                                    uint32 TIME = 0;
                                    if (m_creature->GetCombatTime(true) < 65 && m_creature->GetCombatTime(true) > 55)
                                        TIME = 60;
                                    if (m_creature->GetCombatTime(true) < 125 && m_creature->GetCombatTime(true) > 115)
                                        TIME = 120;
                                    if (m_creature->GetCombatTime(true) > 200)
                                        TIME = 300;
                                    DPS = Damage / TIME;
                                    if (DPS > old_best_dps)
                                        ss << "We have a new best overall DPS!\n" << pAttacker->GetName() << " with " << DPS << " DPS and " << Damage << " Damage in " << TIME << " Seconds!\n";

                                    if (DPS < old_best_dps)
                                        ss << pAttacker->GetName() << " did " << DPS << " DPS and " << Damage << " Damage in " << TIME << " Seconds!\n";

                                    SaveScoreboardDB(pAttacker->ToPlayer(), DPS, Damage, TIME, vaelmode, mode);
                                }

                                m_creature->PlayDirectSound(BG_AV_SOUND_NEAR_LOSE, pAttacker->ToPlayer());

                                pAttacker->RemoveAllAurasFrom(m_creature->GetObjectGuid());
                            }

                            ++itr;
                        }
                        if (Creature* Announcer = m_creature->FindNearestCreature(600004, 20.0f))
                        {
                            Announcer->MonsterYell(ss.str().c_str());
                            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        }
                        if (Creature * Announcer = m_creature->FindNearestCreature(600005, 20.0f))
                        {
                            Announcer->MonsterYell(ss.str().c_str());
                            Announcer->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        }
                        m_bEnd = true;
                        break;
                    }
                }
            }
        }
    }
};

struct npc_attack_dummyAI : public ScriptedAI
{
    npc_attack_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    EventMap m_events;
    std::unordered_map<ObjectGuid, time_t> attackers;

    enum Talents
    {
        IMPROVED_WEAPON_TOTEMS_RANK2 = 29193,
    };

    void Reset()
    {
        attackers.clear();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        if (m_creature->GetEntry() != 700097)
            m_creature->SetHealthPercent(100.0f);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (!pVictim)
            return;
    }

    void AttackStart(Unit* pWho)
    {
        if (pWho && pWho->IsPlayer())
            AddAttackerToList(pWho);
    }

    void AddAttackerToList(Unit* pWho)
    {
        if (!pWho)
            return;

        auto itr = attackers.find(pWho->GetObjectGuid());
        if (itr != attackers.end())
        {
            itr->second = std::time(nullptr);
        }
        else
        {
            if (pWho->IsPlayer())
                m_events.ScheduleEvent(EVENT_CHECK_COMBAT, 0);

            attackers.emplace(pWho->GetObjectGuid(), std::time(nullptr));
        }
    }

    void DamageTaken(Unit* pWho, uint32 &uiDamage)
    {
        if (m_creature->GetHealth() <= uiDamage*2)
            m_creature->SetHealth(uiDamage*3);

        if (!pWho)
            return;

        if (pWho->IsPlayer())
            AddAttackerToList(pWho);
    }

    void SpellHit(SpellCaster* pWho, const SpellEntry* pSpell) override
    {
        if (!pWho)
            return;

        if (pWho->IsPlayer())
            AddAttackerToList(pWho->ToUnit());

        if (m_creature->IsWorldBoss() || m_creature->IsElite())
        {
            switch (pSpell->School)
            {
                case SPELL_SCHOOL_NORMAL:
                {
                    ApplyImprovedDebuffsOnDummy(pWho->ToPlayer(), me, 14325); //Improved Hunter's Mark
                    ApplyImprovedDebuffsOnDummy(pWho->ToPlayer(), me, 11198); //Improved Expose Armor
                    ApplyDebuffsOnDummy(me, 11374); //Gift of Arthas
                    ApplyDebuffsOnDummy(me, 17348); //Hemorrhage
                    ApplyDebuffsOnDummy(me, 16928); //Armor Shatter - Annihilator
                    ApplyDebuffsOnDummy(me, 23577); //Expose Weakness
                    ApplyDebuffsOnDummy(me, 9907); //Faerie Fire
                    ApplyDebuffsOnDummy(me, 11717); //Curse of Recklessness
                    break;
                }
                case SPELL_SCHOOL_HOLY:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyImprovedDebuffsOnDummy(pWho->ToPlayer(), me, 20303); //Improved Seal of the Crusader
                    break;
                }
                case SPELL_SCHOOL_FIRE:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyDebuffsOnDummy(me, 11722); //Curse of the Elements
                    ApplyDebuffsOnDummy(me, 22959); //Improved Scorch
                    ApplyDebuffsOnDummy(me, 9658); //Flame Buffet - Arcanite Dragonling
                    break;
                }
                case SPELL_SCHOOL_NATURE:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyDebuffsOnDummy(me, 21992); //Thunderfury
                    break;
                }
                case SPELL_SCHOOL_FROST:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyDebuffsOnDummy(me, 11722); //Curse of the Elements
                    ApplyDebuffsOnDummy(me, 12579); //Winter's Chill
                    break;
                }
                case SPELL_SCHOOL_SHADOW:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyDebuffsOnDummy(me, 17937); //Curse of Shadow
                    ApplyDebuffsOnDummy(me, 17800); //Shadow Vulnerability Warlock
                    ApplyDebuffsOnDummy(me, 15258); //Shadow Vulnerability Priest
                    break;
                }
                case SPELL_SCHOOL_ARCANE:
                {
                    ApplyDebuffsOnDummy(me, 23605); //Spell Vulnerability - Nightfall Axe
                    ApplyDebuffsOnDummy(me, 17937); //Curse of Shadow
                    break;
                }
                default:
                    break;
            }
        }
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->ForcedDespawn(true); // instant despawn if Dummy dies (can happen)
    }

    void UpdateAI(const uint32 diff)
    {
        m_events.Update(diff);

        if (m_creature->IsInCombat())
        {
            while (uint32 Combat_Events = m_events.ExecuteEvent())
            {
                uint64 PlayerCount = 0;
                switch (Combat_Events)
                {
                    case EVENT_CHECK_COMBAT:
                        for (auto itr = attackers.begin(); itr != attackers.end();)
                        {
                            if (Unit* pAttacker = ObjectAccessor::FindPlayer(itr->first))
                            {
                                if (!pAttacker || !pAttacker->IsInWorld() || !pAttacker->IsInMap(me) || !pAttacker->IsWithinDistInMap(me, VISIBILITY_DISTANCE_SMALL))
                                {
                                    itr = attackers.erase(itr);
                                    continue;
                                }
                                if (itr->second + 10 < std::time(nullptr))
                                {
                                    if (Pet * pet = pAttacker->ToPlayer()->GetPet())
                                        m_creature->GetThreatManager().modifyThreatPercent(pet, -101.0f);

                                    m_creature->_removeAttacker(pAttacker);
                                    m_creature->GetThreatManager().modifyThreatPercent(pAttacker, -101.0f);
                                    itr = attackers.erase(itr);
                                    continue;
                                }
                            }
                            else
                            {
                                itr = attackers.erase(itr);
                                continue;
                            }

                            ++PlayerCount;

                            ++itr;
                        }
                        if (PlayerCount < 1)
                            EnterEvadeMode();

                        m_events.ScheduleEvent(EVENT_CHECK_COMBAT, 1000);
                        break;
                    }
            }
        }
    }
};

enum HealDummyEvents
{
    EVENT_KILL = 1,
    EVENT_MELEEHIT = 2,
    EVENT_COMBAT = 3,
};

struct npc_heal_dummyAI : public ScriptedAI
{
    npc_heal_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    EventMap m_events;
    std::unordered_map<ObjectGuid, time_t> m_healer;
    std::unordered_map<ObjectGuid, time_t> m_players;
    std::unordered_map<ObjectGuid, uint64> m_healing;

    void Reset()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        m_creature->CombatStop(true);
        m_events.Reset();
        m_creature->SetHealthPercent(100.0f);
        m_players.clear();
        m_healer.clear();
        m_healing.clear();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        m_creature->SetHealth(1);
        m_events.ScheduleEvent(EVENT_MELEEHIT, 2000);
        m_events.ScheduleEvent(EVENT_COMBAT, 6000);
    }

    void AddHealerToList(Unit* pWho)
    {
        if (!pWho)
            return;

        auto itr = m_healer.find(pWho->GetObjectGuid());

        if (itr == m_healer.end())
        {
            m_healer.emplace(pWho->GetObjectGuid(), std::time(nullptr));
        }
    }

    void DamageTaken(Unit* pWho, uint32& uiDamage) override
    {
        if (!pWho)
            return;

        if (!uiDamage)
            return;

        if (me != pWho)
            uiDamage = 0;

        if (m_creature->GetHealth() <= uiDamage)
            uiDamage = 0;
    }

    void AddHealingToList(Player* pWho, uint64 uiHeal)
    {
        if (!pWho)
            return;

        if (!uiHeal)
            return;

        auto itr = m_healing.find(pWho->GetObjectGuid());
        if (itr != m_healing.end())
        {
            itr->second = (itr->second + uiHeal);
        }
        else
        {
            m_healing.emplace(pWho->GetObjectGuid(), uiHeal);
        }
    }

    void AddPlayerToList(Unit* pWho)
    {
        if (!pWho)
            return;

        auto itr = m_players.find(pWho->GetObjectGuid());
        if (itr != m_players.end())
            itr->second = std::time(nullptr);
        else
            m_players.emplace(pWho->GetObjectGuid(), std::time(nullptr));
    }

    uint32 GetAllEHPS()
    {
        std::ostringstream ss;
        uint32 old_best_EHPS = 0;
        uint32 Heal = 0;
        uint32 EHPS = 0;
        uint32 TIME = 0;
        uint32 PlayerCount = 0;

        if (m_healer.empty())
            return 0;

        for (auto itr = m_healer.begin(); itr != m_healer.end();)
        {
            if (itr->second)
            {
                PlayerCount++;
                TIME = TIME + (std::time(nullptr)) - (itr->second + 10);

                auto itr2 = m_healing.find(itr->first);
                if (itr2 != m_healing.end())
                    if (itr2->second)
                        Heal = Heal + (itr2->second);
            }

            ++itr;
        }

        TIME = TIME / PlayerCount;
        if (TIME > 0)
            EHPS = Heal / TIME;

        return EHPS;
    }

    uint32 GetUnitEHPS(Unit* pWho)
    {
        if (!pWho)
            return 0;

        uint32 Heal = 0;
        uint32 EHPS = 0;
        uint32 TIME = 0;

        auto itr = m_healer.find(pWho->GetObjectGuid());

        if (itr == m_healer.end())
            return 0;

        if (Unit* pHealer = m_creature->GetMap()->GetUnit(itr->first))
        {
            if (!pHealer || !pHealer->IsInWorld() || !pHealer->IsPlayer())
                return 0;

            if (itr->second)
            {
                TIME = (std::time(nullptr)) - (itr->second);
                auto itr2 = m_healing.find(pHealer->GetObjectGuid());
                if (itr2 != m_healing.end())
                    if (itr2->second)
                    {
                        Heal = (itr2->second);
                    }

                if (TIME > 0)
                    EHPS = Heal / TIME;
            }
        }

        return EHPS;
    }

    uint32 GetUnitHeal(Unit* pWho)
    {
        if (!pWho)
            return 0;

        uint32 Heal = 0;

        auto itr = m_healer.find(pWho->GetObjectGuid());

        if (itr == m_healer.end())
            return 0;

        if (Unit* pHealer = m_creature->GetMap()->GetUnit(itr->first))
        {
            if (!pHealer || !pHealer->IsInWorld() || !pHealer->IsPlayer())
                return 0;

            if (itr->second)
            {
                auto itr2 = m_healing.find(pHealer->GetObjectGuid());
                if (itr2 != m_healing.end())
                    if (itr2->second)
                    {
                        Heal = (itr2->second);
                    }
            }
        }
        return Heal;
    }

    uint32 GetUnitTime(Unit* pWho)
    {
        if (!pWho)
            return 0;

        uint32 TIME = 0;

        auto itr = m_healer.find(pWho->GetObjectGuid());

        if (itr == m_healer.end())
            return 0;

        if (Unit* pHealer = m_creature->GetMap()->GetUnit(itr->first))
        {
            if (!pHealer || !pHealer->IsInWorld() || !pHealer->IsPlayer())
                return 0;

            if (itr->second)
            {
                TIME = (std::time(nullptr)) - (itr->second);
            }
        }
        return TIME;
    }

    void HealedBy(Unit* pWho, uint32& uiHeal, const SpellEntry* spell) override
    {
        if (!pWho)
            return;

        if (!spell)
            return;

        if (pWho->IsPlayer())
        {
            AddHealerToList(pWho);
            AddPlayerToList(pWho);
        }

        if (uiHeal > (m_creature->GetMaxHealth() - m_creature->GetHealth()))
            uiHeal = (m_creature->GetMaxHealth() - m_creature->GetHealth());

        if (pWho->ToCreature())
        {
            if (pWho->ToCreature()->IsTotem() || pWho->ToCreature()->IsPet() && pWho->GetOwner()->IsPlayer())
                AddHealingToList(pWho->GetOwner()->ToPlayer(), uiHeal);
        }
        if (pWho->IsPlayer())
            AddHealingToList(pWho->ToPlayer(), uiHeal);

        std::string SpellName = spell->SpellName[0];
        std::string SpellRank = spell->Rank[0];

        // colors
        #define COLOR_HORDE		"FF3300"
        #define COLOR_ALLIANCE	"0066B3"
        #define COLOR_SPELL		"ffffff"
        #define COLOR_INFO		"FF9900"
        #define COLOR_HEAL		"00b324"
        #define DO_COLOR(a, b)	"|cff" a "" b "|r"

        std::ostringstream spellname;
        if (SpellRank.empty())
            spellname << SpellName;
        else
            spellname << SpellName << " (" << SpellRank << ")";

        ChatHandler(pWho->ToPlayer()).PSendSysMessage("" DO_COLOR(COLOR_SPELL, "%s ") DO_COLOR(COLOR_HEAL, "%u")" | EHPS:" DO_COLOR(COLOR_INFO, " %u")" | Time:" DO_COLOR(COLOR_HORDE, " %u")" | Total:"  DO_COLOR(COLOR_ALLIANCE, " %2u"),
            spellname.str().c_str(), uiHeal, GetUnitEHPS(pWho), GetUnitTime(pWho), GetUnitHeal(pWho));
    }

    void SpellHit(SpellCaster* pWho, const SpellEntry * pSpell) override
    {
        if (!pWho)
            return;
    }

    void JustDied(Unit* /*pKiller*/) override
    {
        m_creature->ForcedDespawn(true); // instant despawn if Dummy dies (can happen)
    }

    void UpdateAI(const uint32 diff)
    {
        m_events.Update(diff);

        while (uint32 Healing_Events = m_events.ExecuteEvent())
        {
            switch (Healing_Events)
            {
                case EVENT_KILL:
                {
                    if (Creature* Goblin = m_creature->FindNearestCreature(600074, 10.0f))
                    {
                        Goblin->SetTargetGuid(m_creature->GetObjectGuid());
                        Goblin->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                        Goblin->PlayDistanceSound(157); //Axe2H_ArmorFlesh
                        DoScriptText(6214, Goblin);
                        m_creature->PlayDistanceSound(1356); //TaurenMaleDeath
                    }
                    Reset();
                    break;
                }
                case EVENT_MELEEHIT:
                {
                    uint32 damage = 0;

                    if (GetAllEHPS() > 0)
                        damage = (GetAllEHPS() * 2) / 50 * m_creature->GetHealthPercent();
                    else
                        damage = (urand(4000, 7000));

                    if (m_creature->GetHealth() <= damage)
                        damage = (m_creature->GetHealth() - urand(100, 5000));

                    if (m_creature->GetHealthPercent() > 40)
                    {
                        if (Creature* Goblin = m_creature->FindNearestCreature(600074, 10.0f))
                        {
                            Goblin->SetTargetGuid(m_creature->GetObjectGuid());
                            Goblin->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                            Goblin->PlayDistanceSound(157); //Axe2H_ArmorFlesh
                        }
                        m_creature->DealDamage(me, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        m_creature->HandleEmote(EMOTE_ONESHOT_WOUNDCRITICAL);
                        m_creature->PlayDistanceSound(6951); //TaurenMaleEmoteCry
                    }
                    m_events.ScheduleEvent(EVENT_MELEEHIT, 2000);
                    break;
                }
                case EVENT_COMBAT:
                {
                    for (auto itr = m_players.begin(); itr != m_players.end();)
                    {
                        if (Unit* pHealer = m_creature->GetMap()->GetUnit(itr->first))
                        {
                            if (!pHealer || !pHealer->IsInWorld())
                            {
                                itr = m_players.erase(itr);
                                continue;
                            }
                        }
                        else
                        {
                            itr = m_players.erase(itr);
                            continue;
                        }

                        if (itr->second + 5 < std::time(nullptr))
                        {
                            itr = m_players.erase(itr);
                            continue;
                        }

                        ++itr;
                    }
                    m_events.ScheduleEvent(EVENT_COMBAT, 6000);
                    break;
                }
            }
        }

        if (m_creature->IsInCombat())
            m_creature->DeleteThreatList();

        if (m_creature->GetHealth() == 1)
            return;

        if (m_players.empty())
        {
            m_creature->RemoveAllAurasOnDeath();
            if (m_creature->GetHealth() > 1)
            {
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
                m_creature->SetHealth(1);
                m_events.ScheduleEvent(EVENT_KILL, 3000);
            }
            return;
        }
        else
        {
            m_creature->AddAura(30020);
            if (m_creature->GetHealthPercent() > 60.0f)
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->HandleEmote(EMOTE_STATE_STAND);

            if (m_creature->GetHealthPercent() < 60.0f && m_creature->GetHealthPercent() > 20.0f)
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->HandleEmote(EMOTE_STATE_STUN);

            if (m_creature->GetHealthPercent() < 20.0f)
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        }
    }
};

CreatureAI* GetAI_npc_heal_dummy(Creature* pCreature)
{
    return new npc_heal_dummyAI(pCreature);
}

CreatureAI* GetAI_npc_attack_dummy(Creature* pCreature)
{
    return new npc_attack_dummyAI(pCreature);
}

CreatureAI* GetAI_npc_attack_dummy_boss(Creature* pCreature)
{
    return new npc_attack_dummy_bossAI(pCreature);
}

void AddSC_npc_attack_dummy()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "npc_attack_dummy_boss";
    newscript->GetAI = &GetAI_npc_attack_dummy_boss;
    newscript->pGossipHello = &GossipHello_npc_attack_dummy_boss;
    newscript->pGossipSelect = &GossipSelect_npc_attack_dummy_boss;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_attack_dummy";
    newscript->GetAI = &GetAI_npc_attack_dummy;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_heal_dummy";
    newscript->GetAI = &GetAI_npc_heal_dummy;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_announcer_dummy";
    newscript->pGossipHello = &GossipHello_npc_announcer_dummy;
    newscript->pGossipSelect = &GossipSelect_npc_announcer_dummy;
    newscript->RegisterSelf();
}
