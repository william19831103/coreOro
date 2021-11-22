
#include "Guild.h"
#include "GuildMgr.h"
#include "Language.h"
#include "Bag.h"
#include "CharacterDatabaseCache.h"

enum PlayerStashGossip
{
    GOSSIP_SENDER_GEAR_DELETE       = 10000001,
    GOSSIP_SENDER_TALENTS_DELETE    = 10000002,
    GOSSIP_SENDER_GEAR_CONFIRM      = 10000003,
    GOSSIP_SENDER_TALENT_CONFIRM    = 10000004,
    GOSSIP_SENDER_GEAR_USE          = 10000007,
    GOSSIP_SENDER_TALENT_USE        = 10000008,
    GOSSIP_SENDER_BUFFS_CONFIRM     = 10000009,
    GOSSIP_SENDER_BUFFS_USE         = 10000010,
    GOSSIP_SENDER_BUFFS_DELETE      = 10000011,
    GOSSIP_SENDER_JOIN_GUILD        = 10000012
};

enum PlayerStashItems
{
    BOTTOMLESS_BAG = 14156
};

enum PlayerStashSounds
{
    ChestOpen          = 1277,
    ChestClose         = 1278,
    SpellFizzleHoly    = 1430,
    Click              = 116
};

enum PlayerStashGeneric
{
    SHOWBANK			= GOSSIP_ACTION_INFO_DEF + 1,
    SHOWMAIL			= GOSSIP_ACTION_INFO_DEF + 2,
    SAVE_TALENTS		= GOSSIP_ACTION_INFO_DEF + 3,
    SAVE_GEAR			= GOSSIP_ACTION_INFO_DEF + 4,
    SHOW_TALENTS		= GOSSIP_ACTION_INFO_DEF + 5,
    SHOW_GEAR			= GOSSIP_ACTION_INFO_DEF + 6,
    DELETE_GEAR			= GOSSIP_ACTION_INFO_DEF + 7,
    DELETE_TALENTS		= GOSSIP_ACTION_INFO_DEF + 8,
    DELETE_GEAR_OK		= GOSSIP_ACTION_INFO_DEF + 9,
    DELETE_TALENTS_OK	= GOSSIP_ACTION_INFO_DEF + 10,
    APPLY_GEAR			= GOSSIP_ACTION_INFO_DEF + 11,
    APPLY_TALENTS		= GOSSIP_ACTION_INFO_DEF + 12,
    BINDHOME			= GOSSIP_ACTION_INFO_DEF + 13,
    SAVE_BUFFS			= GOSSIP_ACTION_INFO_DEF + 14,
    APPLY_BUFFS			= GOSSIP_ACTION_INFO_DEF + 15,
    SHOW_BUFFS			= GOSSIP_ACTION_INFO_DEF + 16,
    DELETE_BUFFS        = GOSSIP_ACTION_INFO_DEF + 17,
    CREATE_GUILD        = GOSSIP_ACTION_INFO_DEF + 18,
    CLEAR_BAGS          = GOSSIP_ACTION_INFO_DEF + 19
};

enum PlayerStashSpells
{
    COOL_VISUAL_SPELL	= 25100
};

std::string StashGetClassString(Player* pPlayer)
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

std::string TalentsExportNameString(Player* pPlayer)
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
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorProtection)
                    {
                        spentPoints += j + 1;
                        WarriorProtectionPoints += spentPoints;
                    }
                    break;
                case WarriorFury:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorFury)
                    {
                        spentPoints += j + 1;
                        WarriorFuryPoints += spentPoints;
                    }
                    break;
                case WarriorArms:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarriorArms)
                    {
                        spentPoints += j + 1;
                        WarriorArmsPoints += spentPoints;
                    }
                    break;

                    //  WARLOCK

                case WarlockDemonology:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDemonology)
                    {
                        spentPoints += j + 1;
                        WarlockDemonologyPoints += spentPoints;
                    }
                    break;
                case WarlockDestruction:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockDestruction)
                    {
                        spentPoints += j + 1;
                        WarlockDestructionPoints += spentPoints;
                    }
                    break;
                case WarlockAffliction:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == WarlockAffliction)
                    {
                        spentPoints += j + 1;
                        WarlockAfflictionPoints += spentPoints;
                    }
                    break;

                    //  SHAMAN

                case ShamanElementalCombat:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanElementalCombat)
                    {
                        spentPoints += j + 1;
                        ShamanElementalCombatPoints += spentPoints;
                    }
                    break;
                case ShamanEnhancement:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanEnhancement)
                    {
                        spentPoints += j + 1;
                        ShamanEnhancementPoints += spentPoints;
                    }
                    break;
                case ShamanRestoration:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == ShamanRestoration)
                    {
                        spentPoints += j + 1;
                        ShamanRestorationPoints += spentPoints;
                    }
                    break;

                    //  ROGUE

                case RogueAssassination:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueAssassination)
                    {
                        spentPoints += j + 1;
                        RogueAssassinationPoints += spentPoints;
                    }
                    break;
                case RogueCombat:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueCombat)
                    {
                        spentPoints += j + 1;
                        RogueCombatPoints += spentPoints;
                    }
                    break;
                case RogueSubtlety:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == RogueSubtlety)
                    {
                        spentPoints += j + 1;
                        RogueSubtletyPoints += spentPoints;
                    }
                    break;

                    //  PRIEST

                case PriestDiscipline:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestDiscipline)
                    {
                        spentPoints += j + 1;
                        PriestDisciplinePoints += spentPoints;
                    }
                    break;
                case PriestHoly:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestHoly)
                    {
                        spentPoints += j + 1;
                        PriestHolyPoints += spentPoints;
                    }
                    break;
                case PriestShadow:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PriestShadow)
                    {
                        spentPoints += j + 1;
                        PriestShadowPoints += spentPoints;
                    }
                    break;

                    //  PALADIN

                case PaladinHoly:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinHoly)
                    {
                        spentPoints += j + 1;
                        PaladinHolyPoints += spentPoints;
                    }
                    break;
                case PaladinProtection:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinProtection)
                    {
                        spentPoints += j + 1;
                        PaladinProtectionPoints += spentPoints;
                    }
                    break;
                case PaladinRetribution:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == PaladinRetribution)
                    {
                        spentPoints += j + 1;
                        PaladinRetributionPoints += spentPoints;
                    }
                    break;

                    //  MAGE

                case MageArcane:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageArcane)
                    {
                        spentPoints += j + 1;
                        MageArcanePoints += spentPoints;
                    }
                    break;
                case MageFire:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFire)
                    {
                        spentPoints += j + 1;
                        MageFirePoints += spentPoints;
                    }
                    break;
                case MageFrost:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == MageFrost)
                    {
                        spentPoints += j + 1;
                        MageFrostPoints += spentPoints;
                    }
                    break;

                    //  HUNTER

                case HunterBeastMastery:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterBeastMastery)
                    {
                        spentPoints += j + 1;
                        HunterBeastMasteryPoints += spentPoints;
                    }
                    break;
                case HunterMarksmanship:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterMarksmanship)
                    {
                        spentPoints += j + 1;
                        HunterMarksmanshipPoints += spentPoints;
                    }
                    break;
                case HunterSurvival:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == HunterSurvival)
                    {
                        spentPoints += j + 1;
                        HunterSurvivalPoints += spentPoints;
                    }
                    break;

                    //  DRUID

                case DruidBalance:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidBalance)
                    {
                        spentPoints += j + 1;
                        DruidBalancePoints += spentPoints;
                    }
                    break;
                case DruidFeralCombat:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidFeralCombat)
                    {
                        spentPoints += j + 1;
                        DruidFeralCombatPoints += spentPoints;
                    }
                    break;
                case DruidRestoration:
                    if (pPlayer->HasSpell(talentInfo->RankID[j]) && talentInfo->TalentTab == DruidRestoration)
                    {
                        spentPoints += j + 1;
                        DruidRestorationPoints += spentPoints;
                    }
                    break;
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

    //WARRIOR
    if (WarriorProtectionPoints > WarriorFuryPoints && WarriorProtectionPoints > WarriorArmsPoints)
    {
        PointsStream << "(Protection " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints << ")";
    }
    else if (WarriorFuryPoints > WarriorProtectionPoints && WarriorFuryPoints > WarriorArmsPoints)
    {
        PointsStream << "(Fury " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints << ")";
    }
    else if (WarriorArmsPoints > WarriorFuryPoints && WarriorArmsPoints > WarriorProtectionPoints)
    {
        PointsStream << "(Arms " << WarriorArmsPoints << "/" << WarriorFuryPoints << "/" << WarriorProtectionPoints << ")";
    }

    //WARLOCK
    if (WarlockAfflictionPoints > WarlockDemonologyPoints && WarlockAfflictionPoints > WarlockDestructionPoints)
    {
        PointsStream << "(Affliction " << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints << ")";
    }
    else if (WarlockDemonologyPoints > WarlockAfflictionPoints && WarlockDemonologyPoints > WarlockDestructionPoints)
    {
        PointsStream << "(Demonology" << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints << ")";
    }
    else if (WarlockDestructionPoints > WarlockDemonologyPoints && WarlockDestructionPoints > WarlockAfflictionPoints)
    {
        PointsStream << "(Destruction " << WarlockAfflictionPoints << "/" << WarlockDemonologyPoints << "/" << WarlockDestructionPoints << ")";
    }

    //SHAMAN
    if (ShamanElementalCombatPoints > ShamanEnhancementPoints && ShamanElementalCombatPoints > ShamanRestorationPoints)
    {
        PointsStream << "(Elemental " << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints << ")";
    }
    else if (ShamanEnhancementPoints > ShamanElementalCombatPoints && ShamanEnhancementPoints > ShamanRestorationPoints)
    {
        PointsStream << "(Enhancement " << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints << ")";
    }
    else if (ShamanRestorationPoints > ShamanEnhancementPoints && ShamanRestorationPoints > ShamanElementalCombatPoints)
    {
        PointsStream << "(Restoration " << ShamanElementalCombatPoints << "/" << ShamanEnhancementPoints << "/" << ShamanRestorationPoints << ")";
    }

    //ROGUE
    if (RogueAssassinationPoints > RogueCombatPoints && RogueAssassinationPoints > RogueSubtletyPoints)
    {
        PointsStream << "(Assassination " << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints << ")";
    }
    else if (RogueCombatPoints > RogueAssassinationPoints && RogueCombatPoints > RogueSubtletyPoints)
    {
        PointsStream << "(Combat " << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints << ")";
    }
    else if (RogueSubtletyPoints > RogueCombatPoints && RogueSubtletyPoints > RogueAssassinationPoints)
    {
        PointsStream << "(Subtlety " << RogueAssassinationPoints << "/" << RogueCombatPoints << "/" << RogueSubtletyPoints << ")";
    }

    //PRIEST
    if (PriestDisciplinePoints > PriestHolyPoints && PriestDisciplinePoints > PriestShadowPoints)
    {
        PointsStream << "(Discipline " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints << ")";
    }
    else if (PriestHolyPoints > PriestDisciplinePoints && PriestHolyPoints > PriestShadowPoints)
    {
        PointsStream << "(Holy " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints << ")";
    }
    else if (PriestShadowPoints > PriestDisciplinePoints && PriestShadowPoints > PriestHolyPoints)
    {
        PointsStream << "(Shadow " << PriestDisciplinePoints << "/" << PriestHolyPoints << "/" << PriestShadowPoints << ")";
    }

    //PALADIN
    if (PaladinHolyPoints > PaladinProtectionPoints && PaladinHolyPoints > PaladinRetributionPoints)
    {
        PointsStream << "(Holy " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints << ")";
    }
    else if (PaladinProtectionPoints > PaladinHolyPoints && PaladinProtectionPoints > PaladinRetributionPoints)
    {
        PointsStream << "(Protection " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints << ")";
    }
    else if (PaladinRetributionPoints > PaladinHolyPoints && PaladinRetributionPoints > PaladinProtectionPoints)
    {
        PointsStream << "(Retribution " << PaladinHolyPoints << "/" << PaladinProtectionPoints << "/" << PaladinRetributionPoints << ")";
    }

    //MAGE
    if (MageArcanePoints > MageFirePoints && MageArcanePoints > MageFrostPoints)
    {
        PointsStream << "(Arcane " << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints << ")";
    }
    else if (MageFirePoints > MageArcanePoints && MageFirePoints > MageFrostPoints)
    {
        PointsStream << "(Fire " << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints << ")";
    }
    else if (MageFrostPoints > MageArcanePoints && MageFrostPoints > MageFirePoints)
    {
        PointsStream << "(Frost " << MageArcanePoints << "/" << MageFirePoints << "/" << MageFrostPoints << ")";
    }

    //HUNTER
    if (HunterBeastMasteryPoints > HunterMarksmanshipPoints && HunterBeastMasteryPoints > HunterSurvivalPoints)
    {
        PointsStream << "(Beast Mastery " << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints << ")";
    }
    else if (HunterMarksmanshipPoints > HunterBeastMasteryPoints && HunterMarksmanshipPoints > HunterSurvivalPoints)
    {
        PointsStream << "(Marksmanship " << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints << ")";
    }
    else if (HunterSurvivalPoints > HunterBeastMasteryPoints && HunterSurvivalPoints > HunterMarksmanshipPoints)
    {
        PointsStream << "(Survival " << HunterBeastMasteryPoints << "/" << HunterMarksmanshipPoints << "/" << HunterSurvivalPoints << ")";
    }

    //DRUID
    if (DruidBalancePoints > DruidFeralCombatPoints && DruidBalancePoints > DruidRestorationPoints)
    {
        PointsStream << "(Balance " << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints << ")";
    }
    else if (DruidFeralCombatPoints > DruidBalancePoints && DruidFeralCombatPoints > DruidRestorationPoints)
    {
        PointsStream << "(Feral Combat " << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints << ")";
    }
    else if (DruidRestorationPoints > DruidBalancePoints && DruidRestorationPoints > DruidFeralCombatPoints)
    {
        PointsStream << "(Restoration " << DruidBalancePoints << "/" << DruidFeralCombatPoints << "/" << DruidRestorationPoints << ")";
    }

    return (PointsStream.str().c_str());
}

uint32 GetTalentID()
{
    QueryResult* TalentIdResult = CharacterDatabase.PQuery("SELECT MAX(temp_id) FROM player_stash_talents");

    uint32 TalentId{};

    if (TalentIdResult)
    {
        Field* fields = TalentIdResult->Fetch();
        TalentId = fields[0].GetInt32();
        delete TalentIdResult;
    }
    if (TalentId)
        return TalentId + 1;
    else
        return 50;
}

uint32 GetGearID()
{
    QueryResult* GearIdResult = CharacterDatabase.PQuery("SELECT MAX(temp_id) FROM player_stash_gear");

    uint32 GearId{};

    if (GearIdResult)
    {
        Field* fields = GearIdResult->Fetch();
        GearId = fields[0].GetInt32();
        delete GearIdResult;
    }
    if (GearId)
        return GearId + 1;
    else
        return 50;
}

uint32 GetBuffsID()
{
    QueryResult* BuffsIdResult = CharacterDatabase.PQuery("SELECT MAX(temp_id) FROM player_stash_buffs");

    uint32 BuffsId{};

    if (BuffsIdResult)
    {
        Field* fields = BuffsIdResult->Fetch();
        BuffsId = fields[0].GetInt32();
        delete BuffsIdResult;
    }
    if (BuffsId)
        return BuffsId + 1;
    else
        return 50;
}

void AddBankBags(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (int i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
    {
        if (Bag* pBag = (Bag*)pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            if (!pBag)
                pPlayer->EquipNewItem(i, BOTTOMLESS_BAG, true);
        }
        else
            pPlayer->EquipNewItem(i, BOTTOMLESS_BAG, true);
            pPlayer->SetBankBagSlotCount(i);
    }
}

void StashApplyBonus(Player* pPlayer, Item* pItem, EnchantmentSlot pEnchantmentSlot, uint32 bonusEntry, uint32 duration, uint32 charges)
{
    if (!pPlayer)
        return;

    if (!pItem)
        return;

    if (!bonusEntry || bonusEntry == 0)
        return;

    pPlayer->_ApplyItemMods(pItem, pEnchantmentSlot, false);
    pItem->SetEnchantment(pEnchantmentSlot, bonusEntry, duration, charges);
    pPlayer->_ApplyItemMods(pItem, pEnchantmentSlot, true);
}

void ExtractGearToDB(Player* pPlayer, std::string& gossipTempText)
{
    if (!pPlayer)
        return;

    uint32 TempID = GetGearID();
    uint32 patch = 0;

    // todo get specid

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
                if (patch < newpatch)
                    patch = newpatch;

                delete getpatch;
            }

            CharacterDatabase.PExecute("INSERT INTO player_stash_gear (`char_guid`, `temp_id`, `gossip_text`, `item_slot`, `item_entry`, `item_enchant`, `patch`) VALUES ('%u', '%u', '%s', '%u', '%u', '%u', '%u');"
                , pPlayer->GetGUID(), TempID, gossipTempText.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT), patch);
        }

        CharacterDatabase.PExecute("UPDATE player_stash_gear SET patch = '%u' WHERE temp_id='%u'", patch, TempID);
    }
    pPlayer->PlayDirectSound(Click, pPlayer);
}

bool ExtractBuff(Player* pPlayer, SpellAuraHolder* pHolder, AuraSaveStruct& saveStruct)
{
    // Double croise : pas de sauvegarde dans la DB (clef unique, peut pas avoir 2x meme aura)
    if (pHolder->GetId() == 20007)
        return false;

    if (pHolder->GetAuraMaxDuration() < 120000)
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

            if (Aura * aur = pHolder->GetAuraByEffectIndex(SpellEffectIndex(i)))
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

void ExtractBuffsToDB(Player* pPlayer, std::string& gossipTempText)
{
    if (!pPlayer)
        return;

    static SqlStatementID insertAuras;
    uint32 TempID = GetBuffsID();

    Player::SpellAuraHolderMap const& auraHolders = pPlayer->GetSpellAuraHolderMap();

    if (auraHolders.empty())
    {
        std::string error = "No valid Buffs.";
        ChatHandler(pPlayer->GetSession()).SendSysMessage(error.c_str());
        return;
    }

    SqlStatement stmt = CharacterDatabase.CreateStatement(insertAuras, "INSERT INTO player_stash_buffs (`char_guid`, `temp_id`, `spell_id`, `gossip_text`) "
        "VALUES (?, ?, ?, ?)");

    AuraSaveStruct s;
    for (Player::SpellAuraHolderMap::const_iterator itr = auraHolders.begin(); itr != auraHolders.end(); ++itr)
    {
        SpellAuraHolder* holder = itr->second;

        if (!ExtractBuff(pPlayer, holder, s))
            continue;

        stmt.addUInt32(pPlayer->GetGUID());
        stmt.addUInt32(TempID);
        stmt.addUInt32(s.spellId);
        stmt.addString(gossipTempText.c_str());
        stmt.Execute();
    }
}

void StashBuffingFromDB(Player* pPlayer, uint32 temp_id)
{
    if (!pPlayer)
        return;

    QueryResult* select = CharacterDatabase.PQuery("SELECT spell_id FROM player_stash_buffs WHERE char_guid = '%u' AND "
        "temp_id = '%u';", pPlayer->GetGUID(), temp_id);

    if (select)
    {
        pPlayer->RemoveAllAurasOnDeath();
        pPlayer->RemoveAllAurasOnDeathByCaster();
        do
        {
            Field* fields = select->Fetch();
            uint32 spellID = fields[0].GetUInt32();

            if (!spellID)
                continue;

            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellID);

            if (!spellInfo)
                continue;

            if (spellInfo->spellLevel > pPlayer->GetLevel())
                continue;

            for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
            {
                ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype >(i);

                if (!proto)
                    continue;

                for (uint8 j = 0; j < MAX_ITEM_PROTO_SPELLS; ++j)
                {
                    if (spellID == proto->Spells[j].SpellId)
                        if (pPlayer->GetLevel() < proto->RequiredLevel)
                            continue;
                }
            }

            SpellSpecific spellId_spec = Spells::GetSpellSpecific(spellID);

            // Buff.
            if (SpellAuraHolder* pHolder = pPlayer->AddAura(spellID, NULL, nullptr))
            {
                pHolder->SetAuraMaxDuration(2 * HOUR * IN_MILLISECONDS);
                pHolder->SetAuraDuration(2 * HOUR * IN_MILLISECONDS);
                pHolder->SetPermanent(false);
                pHolder->UpdateAuraDuration();

                // Allows you to get buffed by multiple Blessings.
                if (spellId_spec == SPELL_BLESSING)
                    pHolder->SetCasterGuid(NULL);
            }
            else
                sLog.outBasic("[ApplyAura] spell %s not applied.", spellInfo->SpellName);

        } while (select->NextRow());
        delete select;
    }
}

void StashLearnTalentsFromDB(Player* pPlayer, uint32 temp_id)
{
    if (!pPlayer)
        return;

    QueryResult *select = CharacterDatabase.PQuery("SELECT talent_id FROM player_stash_talents WHERE char_guid = '%u' AND "
        "temp_id = '%u';", pPlayer->GetGUID(), temp_id);

    if (select)
    {
        do
        {
            Field* fields = select->Fetch();
            uint32 talentID = fields[0].GetUInt32();

            if (!talentID)
                continue;

            // known spell
            if (pPlayer->HasSpell(talentID))
                continue;

            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(talentID);

            if (!spellInfo)
                return;

            if (pPlayer->GetLevel() < spellInfo->spellLevel)
                return;

            pPlayer->LearnSpell(talentID, false, true);
        } while (select->NextRow());
        delete select;
    }
}

void StashDeleteEquippedGear(Player* pPlayer)
{
    if (!pPlayer)
        return;

    for (int i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
            continue;

        if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            pPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
        }
    }
}

void StashDeleteBagContent(Player* pPlayer)
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

// Cause of the AQ ClassBooks etc.
void StasUpgradePlayerSpellsToMax(Player* pPlayer)
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

uint32 CountStoredGear(Player* pPlayer)
{
    uint32 inv_count = 0;
    QueryResult *result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_gear WHERE char_guid='%u' GROUP BY temp_id", pPlayer->GetGUID());
    if (result)
    {
        do {
            inv_count++;
        } while (result->NextRow());
        delete result;
    }
    return inv_count;
}

uint32 CountStoredBuffs(Player* pPlayer)
{
    uint32 inv_count = 0;
    QueryResult* result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_buffs WHERE char_guid='%u' GROUP BY temp_id", pPlayer->GetGUID());
    if (result)
    {
        do {
            inv_count++;
        } while (result->NextRow());
        delete result;
    }
    return inv_count;
}

uint32 CountStoredTalents(Player* pPlayer)
{
    uint32 inv_count = 0;
    QueryResult *result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_talents WHERE char_guid='%u' GROUP BY temp_id", pPlayer->GetGUID());
    if (result)
    {
        do {
            inv_count++;
        } while (result->NextRow());
        delete result;
    }
    return inv_count;
}

bool StashEquipItemsFromDB(Player* pPlayer, uint32 temp_id)
{
    if (!temp_id)
    {
        ChatHandler(pPlayer).PSendSysMessage("Invalid ID.");
        return false;
    }

    for (uint8 equipmentSlot = EQUIPMENT_SLOT_START; equipmentSlot < EQUIPMENT_SLOT_END; ++equipmentSlot)
    {
        if (equipmentSlot == EQUIPMENT_SLOT_TABARD || equipmentSlot == EQUIPMENT_SLOT_BODY)
            continue;

        std::unique_ptr<QueryResult> player_stash_gear(CharacterDatabase.PQuery("SELECT item_entry, item_enchant "
            "FROM player_stash_gear WHERE char_guid = '%u' AND temp_id = '%u' AND item_slot = '%u';", pPlayer->GetGUID(), temp_id, equipmentSlot));

        if (!player_stash_gear)
            continue;

        Field* fields = player_stash_gear->Fetch();
        uint32 itemEntry = fields[0].GetUInt32();
        uint32 enchant = fields[1].GetUInt32();

        ItemPrototype const* item_proto = ObjectMgr::GetItemPrototype(itemEntry);

        if (!item_proto)
            continue;

        //Check if we can equip Item.
        if (pPlayer->GetLevel() < item_proto->RequiredLevel)
            return false;

        if (!pPlayer->HasSpell(750) && item_proto->Class == 4 && item_proto->SubClass == 4)
            return false;

        // Set required reputation
        if (item_proto->RequiredReputationFaction && item_proto->RequiredReputationRank)
            if (FactionEntry const* pFaction = sObjectMgr.GetFactionEntry(item_proto->RequiredReputationFaction))
                if (pPlayer->GetReputationMgr().GetRank(pFaction) < item_proto->RequiredReputationRank)
                    pPlayer->GetReputationMgr().SetReputation(pFaction, pPlayer->GetReputationMgr().GetRepPointsToRank(ReputationRank(item_proto->RequiredReputationRank)));

        // Learn required profession
        if (item_proto->RequiredSkill && !pPlayer->HasSkill(item_proto->RequiredSkill))
            pPlayer->SetSkill(item_proto->RequiredSkill, item_proto->RequiredSkillRank, 300);

        //pPlayer->EquipNewItem(equipmentSlot, itemEntry, true);
        pPlayer->StoreNewItemInBestSlots(itemEntry, 1, enchant);

        // Apply Enchants
        //StashApplyBonus(pPlayer, pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, equipmentSlot), PERM_ENCHANTMENT_SLOT, enchant, 0, 0);
    }
    return true;
}

void ExtractTalentsToDB(Player* pPlayer, std::string gossipTempText)
{
    if (!pPlayer)
        return;

    static SqlStatementID insTalents;
    uint32 TempID = GetTalentID();

    SqlStatement stmtIns = CharacterDatabase.CreateStatement(insTalents, "INSERT INTO player_stash_talents (char_guid, temp_id, gossip_text, talent_id, rank) VALUES (?, ?, ?, ?, ?)");

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
                    stmtIns.PExecute(pPlayer->GetGUID(), TempID, gossipTempText.c_str(), talentInfo->RankID[rank], rank + 1);
            }
        }
    }
    pPlayer->PlayDirectSound(Click, pPlayer);
}

bool List_all_Guilds_from_Player(Player* pPlayer)
{
    if (!pPlayer)
        return false;

    uint32 accid = pPlayer->GetSession()->GetAccountId();

    if (!accid)
        return false;

    if (pPlayer->GetGuildId())
        return false;

    QueryResult* characters = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = '%u'", accid);

    if (characters)
    {
        do
        {
            Field* fields = characters->Fetch();
            uint32 guid = fields[0].GetUInt32();

            if (guid)
            {
                if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                    if (Player* other_char = ObjectAccessor::FindPlayerNotInWorld(guid))
                        if (other_char->GetTeam() == pPlayer->GetTeam())
                            return false;

                QueryResult* guild = CharacterDatabase.PQuery("SELECT guild_id, name FROM guild WHERE leader_guid = '%u'", guid);

                if (guild)
                {
                    Field* fields = guild->Fetch();
                    std::string name = fields[1].GetString();
                    uint8 guild_id = fields[0].GetUInt8();

                    std::ostringstream ss;
                    ss << "Join " << name;


                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_JOIN_GUILD, guild_id);
                }

                delete guild;
            }

        } while (characters->NextRow());

        delete characters;
    }
    return true;
}

bool GossipHello_player_stash(Player* pPlayer, GameObject* gameobject)
{
    if (!pPlayer)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return false;
    }

    pPlayer->PlayerTalkClass->ClearMenus();

    uint32 gearsets				= 0;
    uint32 specs				= 0;
    uint32 buffs				= 0;
    std::string talentstext		= "Save as...";
    std::string gearstext		= "Save as...";
    std::string buffstext       = "Save as...";

    gearsets	= CountStoredGear(pPlayer);
    specs		= CountStoredTalents(pPlayer);
    buffs		= CountStoredBuffs(pPlayer);

    std::ostringstream ss_specs;
    ss_specs << "Specifications (" << specs << ").\n\n";

    std::ostringstream ss_gearsets;
    ss_gearsets << "Equipement Sets (" << gearsets << ").\n\n";

    std::ostringstream ss_buffs;
    ss_buffs << "Buffs (" << buffs << ").\n\n";

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "Bank.\n\n", GOSSIP_SENDER_MAIN, SHOWBANK);
    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Create a new UterusOne Teleporter.", GOSSIP_SENDER_MAIN, BINDHOME);
    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mail.", GOSSIP_SENDER_MAIN, SHOWMAIL);
    if (gearsets > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, ss_gearsets.str().c_str(), GOSSIP_SENDER_MAIN, SHOW_GEAR);
    if (specs > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss_specs.str().c_str(), GOSSIP_SENDER_MAIN, SHOW_TALENTS);

    if (!pPlayer->InArena())
    {
        if (buffs > 0)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, ss_buffs.str().c_str(), GOSSIP_SENDER_MAIN, SHOW_BUFFS);
        if (gearsets < 6)
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Save equipped Items & Enchants.\n\n", GOSSIP_SENDER_MAIN, SAVE_GEAR, gearstext, true);
        if (specs < 6)
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Save current Specification.\n\n", GOSSIP_SENDER_MAIN, SAVE_TALENTS, talentstext, true);
        if (buffs < 6)
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Save current Buffs.\n\n", GOSSIP_SENDER_MAIN, SAVE_BUFFS, buffstext, true);
        if (!pPlayer->GetGuildId())
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Create a Guild.\n\n", GOSSIP_SENDER_MAIN, CREATE_GUILD, buffstext, true);
        List_all_Guilds_from_Player(pPlayer);
    }

    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Clear my Bags.\n\n", GOSSIP_SENDER_MAIN, CLEAR_BAGS);

    pPlayer->SEND_GOSSIP_MENU(600020, gameobject->GetObjectGuid());

    return true;
}

bool Gossipstart_player_stash(Player* pPlayer, GameObject* gameobject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    if (uiAction == SHOWBANK)
    {
        pPlayer->GetSession()->SendShowBank(pPlayer->GetGUID());
        AddBankBags(pPlayer);
    }
    if (uiAction == BINDHOME)
    {
        pPlayer->AddItem(90678, 1);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    else if (uiAction == SHOW_GEAR)
    {
        QueryResult *result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id, patch "
            "FROM player_stash_gear WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                const char* SpecText = fields[0].GetString();
                uint32 temp_id = fields[1].GetUInt32();
                uint32 patch = fields[2].GetUInt32();
                std::ostringstream ss;
                ss << "Equip " << SpecText << ".\n\n";

                if (sWorld.GetWowPatch() >= patch)
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, ss.str().c_str(), GOSSIP_SENDER_GEAR_CONFIRM, temp_id);
                }
            } while (result->NextRow());
        }

        if (!pPlayer->InArena())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Delete an Equipement Set.", GOSSIP_SENDER_MAIN, DELETE_GEAR);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
        pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == SHOW_TALENTS)
    {
        QueryResult *result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id "
            "FROM player_stash_talents WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                const char* SpecText = fields[0].GetString();
                uint32 temp_id = fields[1].GetUInt32();
                std::ostringstream ss;
                ss << "Learn " << SpecText << ".\n\n";

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_TALENT_CONFIRM, temp_id);

            } while (result->NextRow());
            delete result;
        }

        if (!pPlayer->InArena())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Delete a Specification.", GOSSIP_SENDER_MAIN, DELETE_TALENTS);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
        pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == SHOW_BUFFS)
    {
        QueryResult* result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id "
            "FROM player_stash_buffs WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                const char* SpecText = fields[0].GetString();
                uint32 temp_id = fields[1].GetUInt32();
                std::ostringstream ss;
                ss << "Buff " << SpecText << ".\n\n";

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, ss.str().c_str(), GOSSIP_SENDER_BUFFS_CONFIRM, temp_id);

            } while (result->NextRow());
            delete result;
        }

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Delete Buffs.", GOSSIP_SENDER_MAIN, DELETE_BUFFS);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
        pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == DELETE_TALENTS)
    {
        QueryResult *result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id "
            "FROM player_stash_talents WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                const char* SpecText = fields[0].GetString();
                uint32 temp_id = fields[1].GetUInt32();
                std::ostringstream ss;
                ss << "Delete " << SpecText << ".";

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_TALENTS_DELETE, temp_id);

            } while (result->NextRow());
            delete result;
        }

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
        pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == DELETE_GEAR)
    {
        QueryResult *result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id, patch "
            "FROM player_stash_gear WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                const char* SpecText = fields[0].GetString();
                uint32 temp_id = fields[1].GetUInt32();
                std::ostringstream ss;
                ss << "Delete " << SpecText << ".";

                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_GEAR_DELETE, temp_id);

            } while (result->NextRow());
            delete result;
        }

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
        pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == DELETE_BUFFS)
    {
    QueryResult* result = CharacterDatabase.PQuery("SELECT gossip_text, temp_id "
        "FROM player_stash_buffs WHERE char_guid = '%u' GROUP BY temp_id", pPlayer->GetGUID());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            const char* SpecText = fields[0].GetString();
            uint32 temp_id = fields[1].GetUInt32();
            std::ostringstream ss;
            ss << "Delete " << SpecText << ".";

            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, ss.str().c_str(), GOSSIP_SENDER_BUFFS_DELETE, temp_id);

        } while (result->NextRow());
        delete result;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 0);
    pPlayer->SEND_GOSSIP_MENU(1, gameobject->GetObjectGuid());
    }
    else if (uiAction == 0)
        GossipHello_player_stash(pPlayer, gameobject);

    return true;
}

void Create_Guild(Player* pPlayer, const char* gname)
{
    if (!gname)
        return;

    std::string name = gname;
    static const char* allowedcharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz _-/().,1234567890";
    if (!name.length() || name.find_first_not_of(allowedcharacters) != std::string::npos)
    {
        pPlayer->GetSession()->SendNotification("invalid guild name");
        pPlayer->CLOSE_GOSSIP_MENU();
        return;
    }
    else
    {
        pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL, true);

        std::string guildname = name;

        if (pPlayer->GetGuildId())
        {
            ChatHandler(pPlayer->GetSession()).SendSysMessage(LANG_PLAYER_IN_GUILD);
        }

        Guild* guild = new Guild;
        if (!guild->Create(pPlayer, guildname))
        {
            delete guild;
            ChatHandler(pPlayer->GetSession()).SendSysMessage(LANG_GUILD_NOT_CREATED);
        }

        sGuildMgr.AddGuild(guild);
    }
}

bool GossipDelete_Talents(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    bool res = CharacterDatabase.PQuery("DELETE FROM player_stash_talents WHERE temp_id = '%u' AND char_guid = '%u'", uiAction, pPlayer->GetGUID());
    if (res)
        pPlayer->GetSession()->SendAreaTriggerMessage("Successfuly deleted");

    Gossipstart_player_stash(pPlayer, gameobject, GOSSIP_SENDER_MAIN, DELETE_TALENTS);
    pPlayer->PlayDirectSound(Click, pPlayer);
    return true;
}

bool GossipDelete_Gear(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    bool res = CharacterDatabase.PQuery("DELETE FROM player_stash_gear WHERE temp_id = '%u' AND char_guid = '%u'", uiAction, pPlayer->GetGUID());
    if (res)
        pPlayer->GetSession()->SendAreaTriggerMessage("Successfuly deleted");

    Gossipstart_player_stash(pPlayer, gameobject, GOSSIP_SENDER_MAIN, DELETE_GEAR);
    pPlayer->PlayDirectSound(Click, pPlayer);
    return true;
}

bool GossipDelete_Buffs(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    bool res = CharacterDatabase.PQuery("DELETE FROM player_stash_buffs WHERE temp_id = '%u' AND char_guid = '%u'", uiAction, pPlayer->GetGUID());
    if (res)
        pPlayer->GetSession()->SendAreaTriggerMessage("Successfuly deleted");

    Gossipstart_player_stash(pPlayer, gameobject, GOSSIP_SENDER_MAIN, DELETE_BUFFS);
    pPlayer->PlayDirectSound(Click, pPlayer);
    return true;
}

bool GossipJoin_Guild(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    if (!uiAction)
        return false;

    Guild* targetGuild = sGuildMgr.GetGuildById(uiAction);

    if (!targetGuild)
    {
        ChatHandler(pPlayer->GetSession()).SendSysMessage(LANG_GUILD_NOT_FOUND);
        return false;
    }

    // player's guild membership checked in AddMember before add
    auto status = targetGuild->AddMember(pPlayer->GetGUID(), targetGuild->GetLowestRank());

    if (status != GuildAddStatus::OK)
    {
        std::string error;

        switch (status) // bad
        {
        case GuildAddStatus::ALREADY_IN_GUILD:
            error = "Player is already in a guild.";
            break;
        case GuildAddStatus::GUILD_FULL:
            error = "The target guild is full.";
            break;
        case GuildAddStatus::PLAYER_DATA_ERROR:
            error = "Player data appears to be corrupt - tell an administrator.";
            break;
        case GuildAddStatus::UNKNOWN_PLAYER:
            error = "Unable to find target player.";
            break;
        default:
            error = "Unhandled guild invite error.";
        }

        ChatHandler(pPlayer->GetSession()).SendSysMessage(error.c_str());
        return false;
    }

    pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL, true);
    pPlayer->PlayDirectSound(SpellFizzleHoly, 0);
    GossipHello_player_stash(pPlayer, gameobject);

    return true;
}

bool GossipEquip_Gear(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_gear WHERE temp_id = '%u'", uiAction);

    if (result)
    {
        StashDeleteEquippedGear(pPlayer);
        if (StashEquipItemsFromDB(pPlayer, uiAction))
        {
            pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL, true);
            pPlayer->PlayDirectSound(SpellFizzleHoly, 0);
        }
        GossipHello_player_stash(pPlayer, gameobject);
        delete result;
    }
    return true;
}

bool GossipUse_Talents(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_talents WHERE temp_id = '%u'", uiAction);

    if (result)
    {
        pPlayer->ResetTalents(true);
        StashLearnTalentsFromDB(pPlayer, uiAction);
        pPlayer->SaveToDB(true, true);
        StasUpgradePlayerSpellsToMax(pPlayer);
        GossipHello_player_stash(pPlayer, gameobject);
        pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL, true);
        pPlayer->PlayDirectSound(SpellFizzleHoly, 0);
        delete result;
    }
    return true;
}

bool GossipUse_Buffs(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    QueryResult* result = CharacterDatabase.PQuery("SELECT char_guid FROM player_stash_buffs WHERE temp_id = '%u'", uiAction);

    if (result)
    {
        StashBuffingFromDB(pPlayer, uiAction);
        GossipHello_player_stash(pPlayer, gameobject);
        pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL, true);
        pPlayer->PlayDirectSound(SpellFizzleHoly, 0);
        delete result;
    }
    return true;
}

bool GossipConfirm_Gear(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes.", GOSSIP_SENDER_GEAR_USE, uiAction);
    pPlayer->SEND_GOSSIP_MENU(600007, gameobject->GetObjectGuid());

    return true;
}

bool GossipConfirm_Talents(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes.", GOSSIP_SENDER_TALENT_USE, uiAction);
    pPlayer->SEND_GOSSIP_MENU(600021, gameobject->GetObjectGuid());

    return true;
}

bool GossipConfirm_Buffs(Player* pPlayer, GameObject* gameobject, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes.", GOSSIP_SENDER_BUFFS_USE, uiAction);
    pPlayer->SEND_GOSSIP_MENU(600021, gameobject->GetObjectGuid());

    return true;
}

bool GossipSelect_player_stash(Player* pPlayer, GameObject* gameobject, uint32 uiSender, uint32 uiAction)
{
    switch (uiSender)
    {
    case GOSSIP_SENDER_MAIN:
        Gossipstart_player_stash(pPlayer, gameobject, uiSender, uiAction);
        break;
    case GOSSIP_SENDER_GEAR_DELETE:
        GossipDelete_Gear(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_TALENTS_DELETE:
        GossipDelete_Talents(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_GEAR_CONFIRM:
        GossipConfirm_Gear(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_TALENT_CONFIRM:
        GossipConfirm_Talents(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_BUFFS_CONFIRM:
        GossipConfirm_Buffs(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_GEAR_USE:
        GossipEquip_Gear(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_TALENT_USE:
        GossipUse_Talents(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_BUFFS_USE:
        GossipUse_Buffs(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_BUFFS_DELETE:
        GossipDelete_Buffs(pPlayer, gameobject, uiAction);
        break;
    case GOSSIP_SENDER_JOIN_GUILD:
        GossipJoin_Guild(pPlayer, gameobject, uiAction);
        break;
    case CLEAR_BAGS:
        StashDeleteBagContent(pPlayer);
        break;
    }
    return true;
}

bool GossipSelectCode_player_stash(Player* pPlayer, GameObject* gameobject, uint32 sender, uint32 action, const char* code)
{
    std::string name = code;

    if (name.length() > 30)
    {
        pPlayer->GetSession()->SendNotification("Name is too long. Max Name length: 30");
        pPlayer->CLOSE_GOSSIP_MENU();
        return false;
    }
    else
    {
        CharacterDatabase.escape_string(name);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case SAVE_BUFFS:
                ExtractBuffsToDB(pPlayer, name);
                break;
            case SAVE_GEAR:
                ExtractGearToDB(pPlayer, name);
                break;
            case CREATE_GUILD:
                Create_Guild(pPlayer, code);
                break;
            case SAVE_TALENTS:
            {
                if (pPlayer->GetFreeTalentPoints() > 0)
                {
                    pPlayer->GetSession()->SendAreaTriggerMessage("You have unspend talent points. Please spend all your talent points.");
                    return false;
                }
                std::ostringstream ss;
                ss << name << " " << TalentsExportNameString(pPlayer);
                ExtractTalentsToDB(pPlayer, ss.str().c_str());
            }
                break;
            }
            pPlayer->CLOSE_GOSSIP_MENU();
        }
        return true;
    }
}

void AddSC_player_stash()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "player_stash";
    newscript->pGOGossipHello = &GossipHello_player_stash;
    newscript->pGOGossipSelect = &GossipSelect_player_stash;
    newscript->pGOGossipSelectWithCode = &GossipSelectCode_player_stash;
    newscript->RegisterSelf();
}
