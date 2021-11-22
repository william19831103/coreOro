/**
Script made by jeremymeile for uterusone.net
*/

#include "scriptPCH.h"
#include "Player.h"
#include "Item.h"

// spell defines
#define COOL_VISUAL_SPELL_1		4319
#define REMOVE_ENCHANTS			99
#define GOSSIP_PROPERTY_PAGE1	19999996
#define GOSSIP_PROPERTY_PAGE2	19999997
#define GOSSIP_PROPERTY_PAGE3	19999998
#define GOSSIP_PROPERTY_PAGE4	19999999
#define GOSSIP_PROPERTY_PAGE5	20000000
#define GOSSIP_PROPERTY_PAGE6	20000001
#define GOSSIP_PROPERTY_PAGE7	20000002
#define GOSSIP_SENDER_CHANGEPROPERTY_BACK	9999999

void EnchantItem(Player* pPlayer, GameObject* pGameObject, uint32 spellid, uint8 slot, EnchantmentSlot pEnchantmentSlot)
{
    if (!pPlayer)
        return;

    if (!pGameObject)
        return;

    if (!spellid)
        return;

    Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!pItem)
        return;

    ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(pItem->GetEntry());
    if (!itemProto)
        return;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellid);
    if (!spellInfo)
        return;

    uint32 enchantid = spellInfo->EffectMiscValue[0];
    if (!enchantid)
        return;

    if (!((1 << pItem->GetProto()->SubClass) & spellInfo->EquippedItemSubClassMask) &&
        !((1 << pItem->GetProto()->InventoryType) & spellInfo->EquippedItemInventoryTypeMask))
        return;

    uint32 duration = 0;

    if (pEnchantmentSlot == TEMP_ENCHANTMENT_SLOT)
        duration = (spellInfo->EffectBasePoints[0] + 1) * IN_MILLISECONDS;
    else if (pEnchantmentSlot == PERM_ENCHANTMENT_SLOT)
        duration = 0;

    std::ostringstream Hspell;
    Hspell << "|cffffffff|Hspell:" << spellInfo->Id << "|h[" << spellInfo->SpellName[0] << "]|h|r";

    pPlayer->_ApplyItemMods(pItem, slot, false);
    pItem->ClearEnchantment(pEnchantmentSlot);
    pItem->SetEnchantment(pEnchantmentSlot, enchantid, duration, 0);
    pPlayer->_ApplyItemMods(pItem, slot, true);
    pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL_1, true);
    pPlayer->SaveInventoryAndGoldToDB();
    ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s was successfully enchanted with %s.", ChatHandler(pPlayer->GetSession()).GetItemLink(itemProto).c_str(), Hspell.str().c_str());
}

void RemoveEnchantItem(Player* pPlayer, GameObject* pGameObject, EnchantmentSlot pEnchantmentSlot)
{
    if (!pPlayer || !pEnchantmentSlot)
        return;

    pPlayer->PlayerTalkClass->ClearMenus();

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            pPlayer->_ApplyItemMods(equippedItem, i, false);
            equippedItem->ClearEnchantment(pEnchantmentSlot);
            pPlayer->_ApplyItemMods(equippedItem, i, true);
        }
    }
    pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL_1, true);
}

uint32 CheckEnchantID(Player* pPlayer, EquipmentSlots pEquipmentSlots, EnchantmentSlot pEnchantmentSlot)
{
    if (!pPlayer || !pEquipmentSlots || !pEnchantmentSlot)
        return NULL;

    if (Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, pEquipmentSlots))
    {
        if (!pItem)
            return NULL;

        uint32 enchant_id = pItem->GetEnchantmentId(pEnchantmentSlot);
        if (enchant_id)
        {
            SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
            if (enchantEntry)
            {
                uint32 enchant_id = enchantEntry->ID;
                return enchant_id;
            }
        }
    }
    return NULL;
}

bool GossipHello_EnchanterNPC(Player* pPlayer, GameObject* pGameObject)
{
    if (!pPlayer)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    pPlayer->PlayerTalkClass->ClearMenus();

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

        if (equippedItem)
        {
            uint32 itemId = equippedItem->GetEntry();
            ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(itemId);

            if (equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT))
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;

            std::ostringstream ss;
            std::string quality = "cFF9d9d9d";

            switch (itemProto->Quality)
            {
                case 0: quality = "cFF9d9d9d"; continue; break; //Grey cannot enchant.
                case 1: quality = "cFFffffff"; continue; break; //White cannot enchant.
                case 2: quality = "cFF13a300"; break; //Green
                case 3: quality = "cFF00488e"; break; //Blue
                case 4: quality = "cFF6c0eac"; break; //Epic Purple
                case 5: quality = "cFFa35200"; break; //Legendary Orange
                case 6: quality = "cFF624e13"; break; //Artifact Light Gold
            }

            switch (itemProto->InventoryType)
            {
                case INVTYPE_HEAD:
                    ss << "[Head]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_HEAD);
                    break;
                case INVTYPE_SHOULDERS:
                    ss << "[Shoulder]    " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_SHOULDERS);
                    break;
                case INVTYPE_CLOAK:
                    ss << "[Back]           " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_CLOAK);
                    break;
                case INVTYPE_CHEST:
                    ss << "[Chest]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_CHEST);
                    break;
                case INVTYPE_ROBE:
                    ss << "[Robe]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_CHEST);
                    break;
                case INVTYPE_LEGS:
                    ss << "[Legs]           " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_LEGS);
                    break;
                case INVTYPE_FEET:
                    ss << "[Feet]            " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_FEET);
                    break;
                case INVTYPE_WRISTS:
                    ss << "[Wrist]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_WRISTS);
                    break;
                case INVTYPE_HANDS:
                    ss << "[Hands]        " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_HANDS);
                    break;
                case INVTYPE_2HWEAPON:
                case INVTYPE_WEAPON:
                case INVTYPE_WEAPONMAINHAND:
                case INVTYPE_WEAPONOFFHAND:
                {
                    if (i == EQUIPMENT_SLOT_MAINHAND)
                    {
                        ss << "[Main Hand]" << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_WEAPON);
                    }
                    if (pPlayer->CanDualWield() && i == EQUIPMENT_SLOT_OFFHAND)
                    {
                        ss << "[Offhand]     " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_WEAPON + 200);
                    }
                    break;
                }
                case INVTYPE_SHIELD:
                    ss << "[Shield]       " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_SHIELD);
                    break;
                case INVTYPE_RANGED:
                case INVTYPE_RANGEDRIGHT:
                    if (itemProto->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW || itemProto->SubClass == ITEM_SUBCLASS_WEAPON_GUN || itemProto->SubClass == ITEM_SUBCLASS_WEAPON_BOW)
                    {
                        ss << "[Ranged]      " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_RANGEDRIGHT);
                    }
                    break;
            }
        }
        else
            continue;
    }
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove all my Enchants.", GOSSIP_SENDER_MAIN, REMOVE_ENCHANTS);
    pPlayer->SEND_GOSSIP_MENU(600005, pGameObject->GetObjectGuid());
    return true;
}

bool GossipSelect_EnchanterNPC(Player* pPlayer, GameObject* pGameObject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }
    else if (uiAction == INVTYPE_HEAD)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 5)
        {
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2591 && pPlayer->GetClass() == CLASS_DRUID) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect +10, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24168); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2586 && pPlayer->GetClass() == CLASS_HUNTER) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ranged Attack Power +24, Stamina +10, Hit +1%", EQUIPMENT_SLOT_HEAD, 24162); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2588 && pPlayer->GetClass() == CLASS_MAGE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +18, Spell Hit +1%", EQUIPMENT_SLOT_HEAD, 24164); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2584 && pPlayer->GetClass() == CLASS_PALADIN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Defense +7, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24160); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2590 && pPlayer->GetClass() == CLASS_PRIEST) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mana Regen +4, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24167); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2585 && pPlayer->GetClass() == CLASS_ROGUE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Attack Power +28, Dodge +1%", EQUIPMENT_SLOT_HEAD, 24161); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2587 && pPlayer->GetClass() == CLASS_SHAMAN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +13, Intellect +15", EQUIPMENT_SLOT_HEAD, 24163); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2589 && pPlayer->GetClass() == CLASS_WARLOCK) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +18, Stamina +10", EQUIPMENT_SLOT_HEAD, 24165); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) != 2583 && pPlayer->GetClass() == CLASS_WARRIOR) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Defense +7, Stamina + 10, Block value +15", EQUIPMENT_SLOT_HEAD, 24149); }

            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2591 && pPlayer->GetClass() == CLASS_DRUID) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Intellect +10, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24168); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2586 && pPlayer->GetClass() == CLASS_HUNTER) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Ranged Attack Power +24, Stamina +10, Hit +1%", EQUIPMENT_SLOT_HEAD, 24162); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2588 && pPlayer->GetClass() == CLASS_MAGE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +18, Spell Hit +1%", EQUIPMENT_SLOT_HEAD, 24164); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2584 && pPlayer->GetClass() == CLASS_PALADIN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Defense +7, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24160); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2590 && pPlayer->GetClass() == CLASS_PRIEST) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Mana Regen +4, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_HEAD, 24167); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2585 && pPlayer->GetClass() == CLASS_ROGUE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Attack Power +28, Dodge +1%", EQUIPMENT_SLOT_HEAD, 24161); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2587 && pPlayer->GetClass() == CLASS_SHAMAN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +13, Intellect +15", EQUIPMENT_SLOT_HEAD, 24163); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2589 && pPlayer->GetClass() == CLASS_WARLOCK) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +18, Stamina +10", EQUIPMENT_SLOT_HEAD, 24165); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2583 && pPlayer->GetClass() == CLASS_WARRIOR) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Defense +7, Stamina + 10, Block value +15", EQUIPMENT_SLOT_HEAD, 24149); }
        }
        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2544)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing and Spell Damage +8", EQUIPMENT_SLOT_HEAD, 22844);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2545)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Dogde +1%", EQUIPMENT_SLOT_HEAD, 22846);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2543)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Speed +1%", EQUIPMENT_SLOT_HEAD, 22840);
        }
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1503)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "HP +100", EQUIPMENT_SLOT_HEAD, 15389);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1483)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mana +150", EQUIPMENT_SLOT_HEAD, 15340);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1504)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Armor +125", EQUIPMENT_SLOT_HEAD, 15391);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1506)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength +8", EQUIPMENT_SLOT_HEAD, 15397);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1507)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +8", EQUIPMENT_SLOT_HEAD, 15400);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1508)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +8", EQUIPMENT_SLOT_HEAD, 15402);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1509)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +8", EQUIPMENT_SLOT_HEAD, 15404);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1510)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +8", EQUIPMENT_SLOT_HEAD, 15406);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", EQUIPMENT_SLOT_HEAD, INVTYPE_HEAD + 100);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_HEAD, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());

    }
    else if (uiAction == INVTYPE_HEAD + 100)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 1505)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+20 Fire Resistance", EQUIPMENT_SLOT_HEAD, 15394);

        if (sWorld.GetWowPatch() >= 9)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2681)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Nature Resistance", EQUIPMENT_SLOT_HEAD, 28161);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2682)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Frost Resistance", EQUIPMENT_SLOT_HEAD, 28163);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HEAD, PERM_ENCHANTMENT_SLOT) == 2683)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Shadow Resistance", EQUIPMENT_SLOT_HEAD, 28165);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_HEAD, INVTYPE_HEAD);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());

    }
    else if (uiAction == INVTYPE_SHOULDERS)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 9)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2721)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spell Damage +15 and +1% Spell Critical Strike", EQUIPMENT_SLOT_SHOULDERS, 29467);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2715)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing +31 and 5 mana per 5 sec.", EQUIPMENT_SLOT_SHOULDERS, 29475);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2717)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Power +26 and 1% Critical Strike", EQUIPMENT_SLOT_SHOULDERS, 29483);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2716)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +16 and Armor +100", EQUIPMENT_SLOT_SHOULDERS, 29480);
        }
        if (sWorld.GetWowPatch() >= 5)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2606)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Power +30", EQUIPMENT_SLOT_SHOULDERS, 24422);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2605)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing and Spell Damage +18", EQUIPMENT_SLOT_SHOULDERS, 24421);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2604)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing Spells +33", EQUIPMENT_SLOT_SHOULDERS, 24420);
        }
        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_SHOULDERS, PERM_ENCHANTMENT_SLOT) == 2488)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+5 All Resistance", EQUIPMENT_SLOT_SHOULDERS, 22599);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_SHOULDERS, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_CLOAK)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 1889)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Armor +70", EQUIPMENT_SLOT_BACK, 20015);

        if (sWorld.GetWowPatch() >= 7)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 2621)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Threat -2%", EQUIPMENT_SLOT_BACK, 25084);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 2620)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+15 Nature Resistance", EQUIPMENT_SLOT_BACK, 25082);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 2619)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+15 Fire Resistance", EQUIPMENT_SLOT_BACK, 25081);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 910)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Increased Stealth", EQUIPMENT_SLOT_BACK, 25083);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 2622)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Dodge +1%", EQUIPMENT_SLOT_BACK, 25086);
        }
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 804)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Shadow Resistance", EQUIPMENT_SLOT_BACK, 13522);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 1888)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+5 All Resistance", EQUIPMENT_SLOT_BACK, 20014);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_BACK, PERM_ENCHANTMENT_SLOT) == 849)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +3", EQUIPMENT_SLOT_BACK, 13882);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_BACK, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_CHEST)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_CHEST, PERM_ENCHANTMENT_SLOT) == 1893)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mana +100", EQUIPMENT_SLOT_CHEST, 20028);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_CHEST, PERM_ENCHANTMENT_SLOT) == 1891)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "All Stats +4", EQUIPMENT_SLOT_CHEST, 20025);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_CHEST, PERM_ENCHANTMENT_SLOT) == 1892)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Health +100", EQUIPMENT_SLOT_CHEST, 20026);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_CHEST, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WRISTS)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 4)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 2566)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing Spells +24", EQUIPMENT_SLOT_WRISTS, 23802);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 2565)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mana Regen 4 per 5 sec.", EQUIPMENT_SLOT_WRISTS, 23801);
        }
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 1886)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +9", EQUIPMENT_SLOT_WRISTS, 20011);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 1885)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength +9", EQUIPMENT_SLOT_WRISTS, 20010);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 1884)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +9", EQUIPMENT_SLOT_WRISTS, 20009);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 1883)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +7", EQUIPMENT_SLOT_WRISTS, 20008);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 923)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Defense +3", EQUIPMENT_SLOT_WRISTS, 13931);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_WRISTS, PERM_ENCHANTMENT_SLOT) == 247)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +1", EQUIPMENT_SLOT_WRISTS, 7779);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_WRISTS, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_HANDS)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 7)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2564)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +15", EQUIPMENT_SLOT_HANDS, 25080);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2613)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Threat +2%", EQUIPMENT_SLOT_HANDS, 25072);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2617)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing Spells +30", EQUIPMENT_SLOT_HANDS, 25079);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2615)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Frost Damage +20", EQUIPMENT_SLOT_HANDS, 25074);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2616)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Fire Damage +20", EQUIPMENT_SLOT_HANDS, 25078);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 2614)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Shadow Damage +20", EQUIPMENT_SLOT_HANDS, 25073);
        }
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 931)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Speed +1%", EQUIPMENT_SLOT_HANDS, 13948);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 927)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength +7", EQUIPMENT_SLOT_HANDS, 20013);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 1887)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +7", EQUIPMENT_SLOT_HANDS, 20012);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 930)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Minor Mount Speed Increase", EQUIPMENT_SLOT_HANDS, 13947);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 909)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Herbalism skill +5", EQUIPMENT_SLOT_HANDS, 13868);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 865)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Skinning skill +5", EQUIPMENT_SLOT_HANDS, 13698);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 846)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Fishing skill +2", EQUIPMENT_SLOT_HANDS, 13620);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_HANDS, PERM_ENCHANTMENT_SLOT) == 906)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mining skill +5", EQUIPMENT_SLOT_HANDS, 13841);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_HANDS, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_LEGS)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 5)
        {
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2591 && pPlayer->GetClass() == CLASS_DRUID) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect +10, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24168); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2586 && pPlayer->GetClass() == CLASS_HUNTER) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ranged Attack Power +24, Stamina +10, Hit +1%", EQUIPMENT_SLOT_LEGS, 24162); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2588 && pPlayer->GetClass() == CLASS_MAGE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +18, Spell Hit +1%", EQUIPMENT_SLOT_LEGS, 24164); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2584 && pPlayer->GetClass() == CLASS_PALADIN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Defense +7, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24160); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2590 && pPlayer->GetClass() == CLASS_PRIEST) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mana Regen +4, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24167); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2585 && pPlayer->GetClass() == CLASS_ROGUE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Attack Power +28, Dodge +1%", EQUIPMENT_SLOT_LEGS, 24161); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2587 && pPlayer->GetClass() == CLASS_SHAMAN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +13, Intellect +15", EQUIPMENT_SLOT_LEGS, 24163); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2589 && pPlayer->GetClass() == CLASS_WARLOCK) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Healing and Spell Damage +18, Stamina +10", EQUIPMENT_SLOT_LEGS, 24165); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) != 2583 && pPlayer->GetClass() == CLASS_WARRIOR) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Defense +7, Stamina + 10, Block value +15", EQUIPMENT_SLOT_LEGS, 24149); }

            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2591 && pPlayer->GetClass() == CLASS_DRUID) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Intellect +10, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24168); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2586 && pPlayer->GetClass() == CLASS_HUNTER) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Ranged Attack Power +24, Stamina +10, Hit +1%", EQUIPMENT_SLOT_LEGS, 24162); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2588 && pPlayer->GetClass() == CLASS_MAGE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +18, Spell Hit +1%", EQUIPMENT_SLOT_LEGS, 24164); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2584 && pPlayer->GetClass() == CLASS_PALADIN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Defense +7, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24160); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2590 && pPlayer->GetClass() == CLASS_PRIEST) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Mana Regen +4, Stamina +10, Healing Spells +24", EQUIPMENT_SLOT_LEGS, 24167); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2585 && pPlayer->GetClass() == CLASS_ROGUE) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Attack Power +28, Dodge +1%", EQUIPMENT_SLOT_LEGS, 24161); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2587 && pPlayer->GetClass() == CLASS_SHAMAN) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +13, Intellect +15", EQUIPMENT_SLOT_LEGS, 24163); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2589 && pPlayer->GetClass() == CLASS_WARLOCK) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Healing and Spell Damage +18, Stamina +10", EQUIPMENT_SLOT_LEGS, 24165); }
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2583 && pPlayer->GetClass() == CLASS_WARRIOR) { pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Defense +7, Stamina + 10, Block value +15", EQUIPMENT_SLOT_LEGS, 24149); }
        }
        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2544)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing and Spell Damage +8", EQUIPMENT_SLOT_LEGS, 22844);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2545)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Dogde +1%", EQUIPMENT_SLOT_LEGS, 22846);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2543)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Speed +1%", EQUIPMENT_SLOT_LEGS, 22840);
        }
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1503)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "HP +100", EQUIPMENT_SLOT_LEGS, 15389);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1483)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mana +150", EQUIPMENT_SLOT_LEGS, 15340);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1504)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Armor +125", EQUIPMENT_SLOT_LEGS, 15391);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1506)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength +8", EQUIPMENT_SLOT_LEGS, 15397);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1507)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +8", EQUIPMENT_SLOT_LEGS, 15400);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1508)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +8", EQUIPMENT_SLOT_LEGS, 15402);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1509)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +8", EQUIPMENT_SLOT_LEGS, 15404);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1510)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +8", EQUIPMENT_SLOT_LEGS, 15406);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", EQUIPMENT_SLOT_LEGS, INVTYPE_LEGS + 100);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_LEGS, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());

    }
    else if (uiAction == INVTYPE_LEGS + 100)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 1505)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+20 Fire Resistance", EQUIPMENT_SLOT_LEGS, 15394);

        if (sWorld.GetWowPatch() >= 9)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2681)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Nature Resistance", EQUIPMENT_SLOT_LEGS, 28161);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2682)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Frost Resistance", EQUIPMENT_SLOT_LEGS, 28163);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_LEGS, PERM_ENCHANTMENT_SLOT) == 2683)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+10 Shadow Resistance", EQUIPMENT_SLOT_LEGS, 28165);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_LEGS, INVTYPE_LEGS);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());

    }
    else if (uiAction == INVTYPE_FEET)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_FEET, PERM_ENCHANTMENT_SLOT) == 1887)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +7", EQUIPMENT_SLOT_FEET, 20023);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_FEET, PERM_ENCHANTMENT_SLOT) == 851)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +5", EQUIPMENT_SLOT_FEET, 20024);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_FEET, PERM_ENCHANTMENT_SLOT) == 929)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +7", EQUIPMENT_SLOT_FEET, 20020);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_FEET, PERM_ENCHANTMENT_SLOT) == 911)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Minor Speed Increase", EQUIPMENT_SLOT_FEET, 13890);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_FEET, PERM_ENCHANTMENT_SLOT) == 464)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mount Speed +4%", EQUIPMENT_SLOT_FEET, 9783);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_FEET, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WEAPON)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 37)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Immune Disarm", EQUIPMENT_SLOT_MAINHAND, 7220);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1900)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Crusader", EQUIPMENT_SLOT_MAINHAND, 20034);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1898)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Lifestealing", EQUIPMENT_SLOT_MAINHAND, 20032);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1899)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Unholy Weapon", EQUIPMENT_SLOT_MAINHAND, 20033);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1894)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Icy Chill", EQUIPMENT_SLOT_MAINHAND, 20029);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 803)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Fiery Weapon", EQUIPMENT_SLOT_MAINHAND, 13898);

        if (sWorld.GetWowPatch() >= 4)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2567)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +20", EQUIPMENT_SLOT_MAINHAND, 23803);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2568)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +22", EQUIPMENT_SLOT_MAINHAND, 23804);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2563)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength  +15", EQUIPMENT_SLOT_MAINHAND, 23799);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2564)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +15", EQUIPMENT_SLOT_MAINHAND, 23800);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1897)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Weapon Damage +5", EQUIPMENT_SLOT_MAINHAND, 20031);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", EQUIPMENT_SLOT_MAINHAND, INVTYPE_WEAPON + 100);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_MAINHAND, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WEAPON + 100)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2505)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing Power +55", EQUIPMENT_SLOT_MAINHAND, 22750);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2504)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spell Power +30", EQUIPMENT_SLOT_MAINHAND, 22749);
        }
        if (pPlayer->IsTwoHandUsed())
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 34)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Two-handed Attack Speed +3%.", EQUIPMENT_SLOT_MAINHAND, 7218);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1904)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +9", EQUIPMENT_SLOT_MAINHAND, 20036);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 1896)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Weapon Damage +9", EQUIPMENT_SLOT_MAINHAND, 20030);

            if (sWorld.GetWowPatch() >= 8)
            {
                GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
                if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, PERM_ENCHANTMENT_SLOT) == 2646)
                    GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +25", EQUIPMENT_SLOT_MAINHAND, 27837);
            }
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_MAINHAND, INVTYPE_WEAPON);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WEAPON + 200)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 37)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Immune Disarm", EQUIPMENT_SLOT_OFFHAND, 7220);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1900)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Crusader", EQUIPMENT_SLOT_OFFHAND, 20034);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1898)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Lifestealing", EQUIPMENT_SLOT_OFFHAND, 20032);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1899)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Unholy Weapon", EQUIPMENT_SLOT_OFFHAND, 20033);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1894)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Icy Chill", EQUIPMENT_SLOT_OFFHAND, 20029);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 803)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Fiery Weapon", EQUIPMENT_SLOT_OFFHAND, 13898);

        if (sWorld.GetWowPatch() >= 4)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2567)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +20", EQUIPMENT_SLOT_OFFHAND, 23803);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2568)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Intellect +22", EQUIPMENT_SLOT_OFFHAND, 23804);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2563)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Strength  +15", EQUIPMENT_SLOT_OFFHAND, 23799);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2564)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Agility +15", EQUIPMENT_SLOT_OFFHAND, 23800);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1897)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Weapon Damage +5", EQUIPMENT_SLOT_OFFHAND, 20031);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", EQUIPMENT_SLOT_OFFHAND, INVTYPE_WEAPON + 400);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_OFFHAND, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WEAPON + 400)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2505)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing Power +55", EQUIPMENT_SLOT_OFFHAND, 22750);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 2504)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spell Power +30", EQUIPMENT_SLOT_OFFHAND, 22749);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_OFFHAND, INVTYPE_WEAPON + 200);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_RANGEDRIGHT)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        if (sWorld.GetWowPatch() >= 1)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_RANGED, PERM_ENCHANTMENT_SLOT) == 2523)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+3% Ranged Hit", EQUIPMENT_SLOT_RANGED, 22779);
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_RANGED, PERM_ENCHANTMENT_SLOT) == 664)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Ranged Damage +7", EQUIPMENT_SLOT_RANGED, 12460);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_RANGED, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_SHIELD)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_EnchanterNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1704)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Thorium Spike", EQUIPMENT_SLOT_OFFHAND, 16623);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 929)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Stamina +7", EQUIPMENT_SLOT_OFFHAND, 20017);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 1890)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spirit +9", EQUIPMENT_SLOT_OFFHAND, 20016);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 926)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+8 Frost Resistance", EQUIPMENT_SLOT_OFFHAND, 13933);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, PERM_ENCHANTMENT_SLOT) == 863)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Blocking +2%", EQUIPMENT_SLOT_OFFHAND, 13689);

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_OFFHAND, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == REMOVE_ENCHANTS)
    {
        RemoveEnchantItem(pPlayer, pGameObject, PERM_ENCHANTMENT_SLOT);
        pPlayer->SaveInventoryAndGoldToDB();
        GossipHello_EnchanterNPC(pPlayer, pGameObject);
    }
    else if (uiAction == 0)
        GossipHello_EnchanterNPC(pPlayer, pGameObject);
    else
    {
        EnchantItem(pPlayer, pGameObject, uiAction, uiSender, PERM_ENCHANTMENT_SLOT);
        GossipHello_EnchanterNPC(pPlayer, pGameObject);
    }
    return true;
}

bool GossipHello_PoisoneerNPC(Player* pPlayer, GameObject* pGameObject)
{
    if (!pPlayer)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    pPlayer->PlayerTalkClass->ClearMenus();

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

        if (equippedItem)
        {
            uint32 itemId = equippedItem->GetEntry();
            ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(itemId);

            if (!itemProto)
                continue;

            if (equippedItem->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;

            std::ostringstream ss;
            std::string quality = "cFF9d9d9d";

            switch (itemProto->Quality)
            {
                case 0: quality = "cFF9d9d9d"; continue; break; //Grey cannot enchant.
                case 1: quality = "cFFffffff"; continue; break; //White cannot enchant.
                case 2: quality = "cFF13a300"; break; //Green
                case 3: quality = "cFF00488e"; break; //Blue
                case 4: quality = "cFF6c0eac"; break; //Epic Purple
                case 5: quality = "cFFa35200"; break; //Legendary Orange
                case 6: quality = "cFF624e13"; break; //Artifact Light Gold
            }

            switch (itemProto->InventoryType)
            {
                case INVTYPE_2HWEAPON:
                case INVTYPE_WEAPON:
                case INVTYPE_WEAPONMAINHAND:
                case INVTYPE_WEAPONOFFHAND:
                {
                    if (i == EQUIPMENT_SLOT_MAINHAND)
                    {
                        ss << "[Main Hand] " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_WEAPON);
                    }
                    if (pPlayer->CanDualWield() && i == EQUIPMENT_SLOT_OFFHAND)
                    {
                        ss << "[Offhand]      " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_SENDER_MAIN, INVTYPE_WEAPON + 200);
                    }
                    break;
                }
            }
        }
        else
            continue;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove all Weapon Enchants.", GOSSIP_SENDER_MAIN, REMOVE_ENCHANTS);
    pPlayer->SEND_GOSSIP_MENU(600005, pGameObject->GetObjectGuid());
    return true;
}

bool GossipSelect_PoisoneerNPC(Player* pPlayer, GameObject* pGameObject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return false;
    }

    else if (uiAction == INVTYPE_WEAPON)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_PoisoneerNPC(pPlayer, pGameObject);
            return false;
        }

        ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(pItem->GetEntry());

        if (!itemProto)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_PoisoneerNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2685)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+60 Spell damage against Undead.", EQUIPMENT_SLOT_MAINHAND, 28898);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2629)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing +25 and 12 mana per 5 sec.", EQUIPMENT_SLOT_MAINHAND, 25123);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2628)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spell Damage +36 and +1% Spell Critical Strike", EQUIPMENT_SLOT_MAINHAND, 25122);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 26)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "10% chance of casting Frostbolt on hit.", EQUIPMENT_SLOT_MAINHAND, 3595);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 25)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "15% chance of casting Shadowbolt III on hit.", EQUIPMENT_SLOT_MAINHAND, 3594);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2684)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Power +100 against Undead.", EQUIPMENT_SLOT_MAINHAND, 28891);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2506)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+2% Melee Critical Strike.", EQUIPMENT_SLOT_MAINHAND, 22756);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 1643)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Weapon Damage +8.", EQUIPMENT_SLOT_MAINHAND, 16138);

        if (pPlayer->GetClass() == CLASS_ROGUE)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 603)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Crippling Poison II", EQUIPMENT_SLOT_MAINHAND, 11202);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 2630)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Deadly Poison V", EQUIPMENT_SLOT_MAINHAND, 25351);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 625)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Instant Poison VI", EQUIPMENT_SLOT_MAINHAND, 11340);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 706)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Wound Poison IV", EQUIPMENT_SLOT_MAINHAND, 13227);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_MAINHAND, TEMP_ENCHANTMENT_SLOT) == 643)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mind-numbing Poison III", EQUIPMENT_SLOT_MAINHAND, 11399);
        }
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_MAINHAND, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == INVTYPE_WEAPON + 200)
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        if (!pItem)
        {
            pPlayer->GetSession()->SendNotification("There is no item equipped in the specifified slot.");
            GossipHello_PoisoneerNPC(pPlayer, pGameObject);
            return false;
        }

        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2685)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+60 Spell damage against Undead.", EQUIPMENT_SLOT_OFFHAND, 28898);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2629)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Healing +25 and 12 mana per 5 sec.", EQUIPMENT_SLOT_OFFHAND, 25123);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2628)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Spell Damage +36 and +1% Spell Critical Strike", EQUIPMENT_SLOT_OFFHAND, 25122);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 26)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "10% chance of casting Frostbolt on hit.", EQUIPMENT_SLOT_OFFHAND, 3595);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 25)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "15% chance of casting Shadowbolt III on hit.", EQUIPMENT_SLOT_OFFHAND, 3594);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2684)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Attack Power +100 against Undead.", EQUIPMENT_SLOT_OFFHAND, 28891);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2506)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "+2% Melee Critical Strike.", EQUIPMENT_SLOT_OFFHAND, 22756);
        GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
        if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 1643)
            GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Weapon Damage +8.", EQUIPMENT_SLOT_OFFHAND, 16138);

        if (pPlayer->GetClass() == CLASS_ROGUE)
        {
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 603)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Crippling Poison II", EQUIPMENT_SLOT_OFFHAND, 11202);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 2630)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Deadly Poison V", EQUIPMENT_SLOT_OFFHAND, 25351);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 625)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Instant Poison VI", EQUIPMENT_SLOT_OFFHAND, 11340);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 706)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Wound Poison IV", EQUIPMENT_SLOT_OFFHAND, 13227);
            GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;
            if (CheckEnchantID(pPlayer, EQUIPMENT_SLOT_OFFHAND, TEMP_ENCHANTMENT_SLOT) == 643)
                GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, "Mind-numbing Poison III", EQUIPMENT_SLOT_OFFHAND, 11399);
        }

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", EQUIPMENT_SLOT_OFFHAND, 0);
        pPlayer->SEND_GOSSIP_MENU(600006, pGameObject->GetObjectGuid());
    }
    else if (uiAction == REMOVE_ENCHANTS)
    {
        RemoveEnchantItem(pPlayer, pGameObject, TEMP_ENCHANTMENT_SLOT);
        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
    }
    else if (uiAction == 0)
        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
    else
    {
        EnchantItem(pPlayer, pGameObject, uiAction, uiSender, TEMP_ENCHANTMENT_SLOT);
        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
    }
    return true;
}

bool GossipHello_PropertyNPC(Player* pPlayer, GameObject* pGameObject)
{
    if (!pPlayer)
        return false;

    if (!pGameObject)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return false;
    }

    pPlayer->PlayerTalkClass->ClearMenus();

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

        if (!equippedItem)
            continue;

        uint32 itemId = equippedItem->GetEntry();
        ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(itemId);

        if (!itemProto)
            continue;

        if (!itemProto->RandomProperty)
            continue;

        uint32 randomPropId = equippedItem->GetItemRandomPropertyId();
        ItemRandomPropertiesEntry const* randomProp = sItemRandomPropertiesStore.LookupEntry(randomPropId);

        if (!randomProp)
            continue;

        std::ostringstream ss;
        std::string quality = "cFF9d9d9d";

        switch (itemProto->Quality)
        {
            case 0: quality = "cFF9d9d9d"; break; // Grey
            case 1: quality = "cFFffffff"; break; // White
            case 2: quality = "cFF13a300"; break; // Green
            case 3: quality = "cFF00488e"; break; // Blue
            case 4: quality = "cFF6c0eac"; break; // Epic Purple
            case 5: quality = "cFFa35200"; break; // Legendary Orange
            case 6: quality = "cFF624e13"; break; // Artifact Light Gold
        }

        switch (itemProto->InventoryType)
        {
            case INVTYPE_HEAD:
                ss << "[Head]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_NECK:
                ss << "[Neck]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_SHOULDERS:
                ss << "[Shoulder]    " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_CLOAK:
                ss << "[Back]           " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_CHEST:
                ss << "[Chest]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_ROBE:
                ss << "[Robe]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_WAIST:
                ss << "[Waist]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_LEGS:
                ss << "[Legs]           " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_FEET:
                ss << "[Feet]            " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_WRISTS:
                ss << "[Wrist]          " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_HANDS:
                ss << "[Hands]        " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_FINGER:
                ss << "[Finger]        " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_2HWEAPON:
            case INVTYPE_WEAPON:
            case INVTYPE_WEAPONMAINHAND:
            case INVTYPE_WEAPONOFFHAND:
            {
                if (i == EQUIPMENT_SLOT_MAINHAND)
                    ss << "[Main Hand]" << "|" << quality << itemProto->Name1;
                if (pPlayer->CanDualWield() && i == EQUIPMENT_SLOT_OFFHAND)
                    ss << "[Offhand]     " << "|" << quality << itemProto->Name1;
                break;
            }
            case INVTYPE_SHIELD:
                ss << "[Shield]       " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_RANGED:
            case INVTYPE_RANGEDRIGHT:
                ss << "[Ranged]      " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_HOLDABLE:
                ss << "[Offhand]     " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_THROWN:
                ss << "[Shield]       " << "|" << quality << itemProto->Name1;
                break;
            case INVTYPE_RELIC:
                ss << "[Relic]        " << "|" << quality << itemProto->Name1;
                break;
        }

        std::string nameSuffix(randomProp->internalName);
        if (pPlayer->GetSession()->GetSessionDbcLocale() < MAX_DBC_LOCALE)
        {
            std::string localeSuffix = randomProp->nameSuffix[pPlayer->GetSession()->GetSessionDbcLocale()];
            if (localeSuffix.length() > 0)
                nameSuffix = localeSuffix;
        }

        if (!nameSuffix.empty())
            ss << " " << nameSuffix; // Get the suffix name "of the fiery Wrath" for example.
        else
            ss << " " << randomProp->internalName;

        ss << "|r";

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), GOSSIP_PROPERTY_PAGE1, i);
    }

    pPlayer->SEND_GOSSIP_MENU(600016, pGameObject->GetObjectGuid());
    return true;
}

bool GossipSelect_PropertyNPC(Player* pPlayer, GameObject* pGameObject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    if (uiSender < 0)
        return false;

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return false;
    }

    pPlayer->PlayerTalkClass->ClearMenus();
    uint8 GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

    if (uiSender == GOSSIP_PROPERTY_PAGE1 || uiSender == GOSSIP_PROPERTY_PAGE2 || uiSender == GOSSIP_PROPERTY_PAGE3 || uiSender == GOSSIP_PROPERTY_PAGE4 || uiSender == GOSSIP_PROPERTY_PAGE5 || uiSender == GOSSIP_PROPERTY_PAGE6 || uiSender == GOSSIP_PROPERTY_PAGE7)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiAction);

        if (!equippedItem)
            return false;

        uint32 itemId = equippedItem->GetEntry();

        ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(itemId);

        if (!itemProto)
            return false;

        if (!itemProto->RandomProperty)
            return false;

        uint32 randomPropId = equippedItem->GetItemRandomPropertyId();
        ItemRandomPropertiesEntry const* PropID_of_Equipped_Item = sItemRandomPropertiesStore.LookupEntry(randomPropId);

        uint32 ench;
        uint32 counter = 0;

        QueryResult* result = WorldDatabase.PQuery("SELECT entry, ench FROM item_enchantment_template WHERE ((%u >= patch_min) && (%u <= patch_max)) AND entry = '%u'", sWorld.GetWowPatch(), sWorld.GetWowPatch(), itemProto->RandomProperty);

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();

                ench = fields[1].GetUInt32(); // We need only enchant entry here.

                ItemRandomPropertiesEntry const* random_id = sItemRandomPropertiesStore.LookupEntry(ench);

                std::ostringstream ss;
                std::string nameSuffix(random_id->internalName);
                if (pPlayer->GetSession()->GetSessionDbcLocale() < MAX_DBC_LOCALE)
                {
                    std::string localeSuffix = random_id->nameSuffix[pPlayer->GetSession()->GetSessionDbcLocale()];
                    if (localeSuffix.length() > 0)
                        nameSuffix = localeSuffix;
                }

                if (!nameSuffix.empty())
                    ss << nameSuffix << " | "; // Get the suffix name "of the fiery Wrath" for example.
                else
                    ss << random_id->internalName << " | ";


                for (uint32 i = 0; i < 3; ++i)
                {
                    SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(random_id->enchant_id[i]);

                    if (!enchantEntry)
                        continue;

                    std::string enchantDescription(enchantEntry->description[0]);
                    if (pPlayer->GetSession()->GetSessionDbcLocale() < MAX_DBC_LOCALE)
                    {
                        std::string localeSuffix = enchantEntry->description[pPlayer->GetSession()->GetSessionDbcLocale()];
                        if (localeSuffix.length() > 0)
                            enchantDescription = localeSuffix;
                    }

                    if (!enchantDescription.empty())
                        ss << enchantDescription << " "; // Get the suffix name "of the fiery Wrath" for example.
                    else
                        ss << enchantEntry->description[0] << " "; // Get the description "+13 Fire Spell Damage" for example.
                }

                // Why 12, 24, 36 etc? If you are displaying more than 15 (counter > 12 = 13 & back and next menu are 15) gossip items it will be messed up.
                if (counter > 12 && uiSender == GOSSIP_PROPERTY_PAGE1) // Show first 12 gossips
                    break;

                if (counter > 24 && uiSender == GOSSIP_PROPERTY_PAGE2) // Show next 12 gossips
                    break;

                if (counter > 36 && uiSender == GOSSIP_PROPERTY_PAGE3) // Show next 12 gossips
                    break;

                if (counter > 48 && uiSender == GOSSIP_PROPERTY_PAGE4) // Show next 12 gossips
                    break;

                if (counter > 60 && uiSender == GOSSIP_PROPERTY_PAGE5) // Show next 12 gossips
                    break;

                if (counter > 72 && uiSender == GOSSIP_PROPERTY_PAGE6) // Show next 12 gossips
                    break;

                if (counter > 84 && uiSender == GOSSIP_PROPERTY_PAGE7) // Last page more are no required until now.
                    break;

                // Show Taxi flight icon for the current used property on gossip item.
                if (random_id->ID == PropID_of_Equipped_Item->ID)
                    GOSSIP_ICON_SET = GOSSIP_ICON_TAXI;
                else
                    GOSSIP_ICON_SET = GOSSIP_ICON_INTERACT_1;

                if (uiSender == GOSSIP_PROPERTY_PAGE1 ||
                    uiSender == GOSSIP_PROPERTY_PAGE2 && counter > 12 ||
                    uiSender == GOSSIP_PROPERTY_PAGE3 && counter > 24 ||
                    uiSender == GOSSIP_PROPERTY_PAGE4 && counter > 36 ||
                    uiSender == GOSSIP_PROPERTY_PAGE5 && counter > 48 ||
                    uiSender == GOSSIP_PROPERTY_PAGE6 && counter > 60 ||
                    uiSender == GOSSIP_PROPERTY_PAGE7 && counter > 72)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), uiAction, (random_id->ID));

                ++counter;

            } while (result->NextRow());

            if (counter > ((uiSender - (GOSSIP_PROPERTY_PAGE1 - 1)) * 12)) // idk my own weird way to not display the "next" gossip item on the last page.
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", uiSender + 1, uiAction);

            if (uiSender == GOSSIP_PROPERTY_PAGE1)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_CHANGEPROPERTY_BACK, uiAction);
            else
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", uiSender - 1, uiAction);

            delete result;
        }
    }
    else if (uiSender == GOSSIP_SENDER_CHANGEPROPERTY_BACK) // Go back from page2 is gossiphello.
        GossipHello_PropertyNPC(pPlayer, pGameObject);
    else
    {
        Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiSender);
        if (pItem)
        {
            pPlayer->_ApplyItemMods(pItem, uiSender, false);
            pItem->SetItemRandomProperties(uiAction);
            pPlayer->_ApplyItemMods(pItem, uiSender, true);
            GossipHello_PropertyNPC(pPlayer, pGameObject);
            pPlayer->CastSpell(pPlayer, COOL_VISUAL_SPELL_1, true);
            pPlayer->SaveInventoryAndGoldToDB();
        }
    }
    pPlayer->SEND_GOSSIP_MENU(600016, pGameObject->GetObjectGuid());

    return true;
}

void AddSC_npc_enchanter()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "npc_enchanter";
    newscript->pGOGossipHello = &GossipHello_EnchanterNPC;
    newscript->pGOGossipSelect = &GossipSelect_EnchanterNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_poisoneer";
    newscript->pGOGossipHello = &GossipHello_PoisoneerNPC;
    newscript->pGOGossipSelect = &GossipSelect_PoisoneerNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_property";
    newscript->pGOGossipHello = &GossipHello_PropertyNPC;
    newscript->pGOGossipSelect = &GossipSelect_PropertyNPC;
    newscript->RegisterSelf(false);
}