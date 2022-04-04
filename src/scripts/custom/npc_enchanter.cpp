/**
Script made by jeremymeile for uterusone.net
*/

#include "scriptPCH.h"
#include "Player.h"
#include "Item.h"

#define SPELL_TRANSMOGRIFY      23359
#define ENCHANTER_MAIN_MENU		99998
#define REMOVE_ENCHANTS			99999
#define GOSSIP_PROPERTY_PAGE1	19999996
#define GOSSIP_PROPERTY_PAGE2	19999997
#define GOSSIP_PROPERTY_PAGE3	19999998
#define GOSSIP_PROPERTY_PAGE4	19999999
#define GOSSIP_PROPERTY_PAGE5	20000000
#define GOSSIP_PROPERTY_PAGE6	20000001
#define GOSSIP_PROPERTY_PAGE7	20000002
#define GOSSIP_SENDER_CHANGEPROPERTY_BACK	9999999

struct EnchanterStruct
{
    uint32 m_SpellEntry;
    int32 m_ClassMask;
};

std::array<EnchanterStruct, 96> const PermEnchantements =
{ {
    { 29483, 1503 }, // Might of the Scourge, Permanently adds to a shoulder slot item increased Attack Power by 26 and also increases your chance to land a critical strike by 1%.
    { 29480, 1503 }, // Fortitude of the Scourge, Permanently adds to a shoulder slot item increased Stamina by 16 and also grants 100 armor. 
    { 29475, 1503 }, // Resilience of the Scourge, Permanently adds to a shoulder slot item increased healing done by magical spells and effects up to 31 and also increases your mana regen by 5 mana per 5 sec.
    { 29467, 1503 }, // Power of the Scourge, Permanently adds to a shoulder slot item increased damage and healing done by magical spells and effects up to 15 and also increases your chance to land a critical strike with spells by 1%.
    { 28165, 32767 }, // Shadow Guard, Permanently adds 10 shadow resistance to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 28163, 32767 }, // Ice Guard, Permanently adds 10 frost resistance to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 28161, 32767 }, // Savage Guard, Permanently adds 10 nature resistance to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 27837, -1 }, // Enchant 2H Weapon - Agility, Permanently enchant a two handed melee weapon to grant +25 Agility.
    { 25086, -1 }, // Enchant Cloak - Dodge, Permanently enchant a cloak to give a 1% chance to dodge.
    { 25084, -1 }, // Enchant Cloak - Subtlety, Permanently enchant a cloak to decrease threat caused by the wearer by 2%.
    { 25083, -1 }, // Enchant Cloak - Stealth, Permanently enchant a cloak to give a increase to stealth.
    { 25082, -1 }, // Enchant Cloak - Greater Nature Resistance, Permanently enchant a cloak to give 15 nature resistance.
    { 25081, -1 }, // Enchant Cloak - Greater Fire Resistance, Permanently enchant a cloak to give 15 fire resistance.
    { 25080, -1 }, // Enchant Gloves - Superior Agility, Permanently enchant gloves to increase agility by 15.
    { 25079, -1 }, // Enchant Gloves - Healing Power, Permanently enchant gloves to increase the caster's healing spells by up to 30.
    { 25078, -1 }, // Enchant Gloves - Fire Power, Permanently enchant gloves to increase fire damage by up to 20.
    { 25074, -1 }, // Enchant Gloves - Frost Power, Permanently enchant gloves to increase frost damage by up to 20.
    { 25073, -1 }, // Enchant Gloves - Shadow Power, Permanently enchant gloves to increase shadow damage by up to 20.
    { 25072, -1 }, // Enchant Gloves - Threat, Permanently enchant gloves to increase threat from all attacks and spells by 2%.
    { 24422, 2047 }, // Zandalar Signet of Might, Permanently adds 30 attack power to a shoulder slot item.
    { 24421, 2047 }, // Zandalar Signet of Mojo, Permanently adds to a shoulder slot item increased damage and healing done by magical spells and effects up to 18.
    { 24420, 2047 }, // Zandalar Signet of Serenity, Permanently adds to a shoulder slot item increased healing done by spells and effects up to 33.
    { 24168, 1024 }, // Animist's Caress, Permanently adds 10 Stamina, 10 Intellect, and increases healing by up to 24 to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24167, 16 }, // Prophetic Aura, Permanently adds 10 Stamina, 4 mana per 5 sec., and increases healing by up to 24 to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24165, 256 }, // Hoodoo Hex, Permanently adds 10 Stamina and increases spell damage and healing by up to 18 to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24164, 128 }, // Presence of Sight, Permanently adds 18 to all healing and damage spells and 1% chance to hit with spells to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24163, 64 }, // Vodouisant's Vigilant Embrace, Permanently adds 15 Intellect and increases all healing and spell damage by up to 13 to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24162, 4 }, // Falcon's Call, Permanently adds 24 Ranged Attack Power, 10 Stamina, and 1% Chance to Hit to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24161, 8 }, // Death's Embrace, Permanently adds 28 Attack Power and 1% Dodge to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24160, 2 }, // Syncretist's Sigil, Permanently adds 10 Stamina, 7 Defense, and increases healing by up to 24 to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 24149, 1 }, // Presence of Might, Permanently adds 10 Stamina, 7 Defense, and 15 Shield Block value to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 23804, -1 }, // Enchant Weapon - Mighty Intellect, Permanently enchant a melee weapon to grant +22 Intellect.
    { 23803, -1 }, // Enchant Weapon - Mighty Spirit, Permanently enchant a melee weapon to grant +20 Spirit.
    { 23802, -1 }, // Enchant Bracer - Healing Power, Permanently enchants bracers to increase the effects of your healing spells by 24.
    { 23801, -1 }, // Enchant Bracer - Mana Regeneration, Permanently enchants bracers to restore 4 mana every 5 seconds.
    { 23800, -1 }, // Enchant Weapon - Agility, Permanently enchant a melee weapon to grant +15 Agility.
    { 23799, -1 }, // Enchant Weapon - Strength, Permanently enchant a melee weapon to grant +15 strength.
    { 22846, -1 }, // Arcanum of Protection, Permanently adds 1% dodge to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 22844, -1 }, // Arcanum of Focus, Permanently adds +8 to your Healing and Damage from spells to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 22840, -1 }, // Arcanum of Rapidity, Permanently adds 1% haste to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 22779, -1 }, // Biznicks 247x128 Accurascope, Attaches a permanent scope to a bow or gun that increases its chance to hit by 3%.
    { 22750, -1 }, // Enchant  Weapon - Healing Power, Permanently enchant a Melee Weapon to add up to 50 points of healing to healing spells.
    { 22749, -1 }, // Enchant  Weapon - Spell Power, Permanently enchant a Melee Weapon to add up to 22 damage to spells.
    { 22599, 2047 }, // Chromatic Mantle of the Dawn, Permanently adds 5 resistance to all magic schools to a shoulder slot item.
    { 21931, -1 }, // Enchant Weapon - Winter's Might, Permanently enchant a weapon to grant up to 7 additional frost damage when casting frost spells.
    { 20036, -1 }, // Enchant 2H Weapon - Major Intellect, Permanently enchant a Two-Handed Melee Weapon to add 9 to intellect.
    { 20034, -1 }, // Enchant Weapon - Crusader, Permanently enchant a melee weapon so that often when attacking in melee it heals for $20007s2 and increases Strength by $20007s1 for $20007d.
    { 20033, -1 }, // Enchant Weapon - Unholy Weapon, Permanently enchant a melee weapon to often inflict a curse on the target reducing their melee damage.
    { 20032, -1 }, // Enchant Weapon - Lifestealing, Permanently enchant a melee weapon to often steal life from the enemy and give it to the wielder.
    { 20031, -1 }, // Enchant Weapon - Superior Striking, Permanently enchant a Melee Weapon to do 5 additional points of damage.
    { 20030, -1 }, // Enchant 2H Weapon - Superior Impact, Permanently enchant a two-handed melee weapon to do +9 damage.
    { 20029, -1 }, // Enchant Weapon - Icy Chill, Permanently enchant a melee weapon to often chill the target reducing their movement and attack speed.
    { 20028, -1 }, // Enchant Chest - Major Mana, Permanently enchant a piece of chest armor to give +100 mana.
    { 20026, -1 }, // Enchant Chest - Major Health, Permanently enchant a piece of chest armor to grant +100 health.
    { 20025, -1 }, // Enchant Chest - Greater Stats, Permanently enchant a piece of chest armor to grant +4 to all stats.
    { 20024, -1 }, // Enchant Boots - Spirit, Permanently enchant boots to give +5 Spirit.
    { 20023, -1 }, // Enchant Boots - Greater Agility, Permanently enchant boots to give +7 Agility.
    { 20020, -1 }, // Enchant Boots - Greater Stamina, Permanently enchant boots to give +7 Stamina.
    { 20017, -1 }, // Enchant Shield - Greater Stamina, Permanently enchant a shield to give +7 Stamina.
    { 20016, -1 }, // Enchant Shield - Superior Spirit, Permanently enchant a shield to give +9 Spirit.
    { 20015, -1 }, // Enchant Cloak - Superior Defense, Permanently enchant a cloak to give 70 additional armor.
    { 20014, -1 }, // Enchant Cloak - Greater Resistance, Permanently enchant a cloak to give 5 to all resistances.
    { 20013, -1 }, // Enchant Gloves - Greater Strength, Permanently enchant gloves to grant +7 Strength.
    { 20012, -1 }, // Enchant Gloves -Greater Agility, Permanently enchant gloves to grant +7 Agility.
    { 20011, -1 }, // Enchant Bracer - Superior Stamina, Permanently enchants bracers to give +9 Stamina.
    { 20010, -1 }, // Enchant Bracer - Superior Strength, Permanently enchants bracers to give +9 Strength.
    { 20009, -1 }, // Enchant Bracer - Superior Spirit, Permanently enchants bracers to give +9 Spirit.
    { 20008, -1 }, // Enchant Bracer - Greater Intellect, Permanently enchants bracers to give +7 Intellect.
    { 16623, -1 }, // Thorium Shield Spike, Attaches a Thorium Spike to your shield that deals damage every time you block with it.
    { 15406, 2047 }, // Lesser Arcane Amalgamation, Permanently adds 8 spirit to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15404, 2047 }, // Lesser Arcane Amalgamation, Permanently adds 8 intellect to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15402, -1 }, // Lesser Arcane Amalgamation, Permanently adds 8 agility to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15400, 2047 }, // Lesser Arcane Amalgamation, Permanently adds 8 stamina to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15397, -1 }, // Lesser Arcane Amalgamation, Permanently adds 8 strength to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15394, -1 }, // Lesser Arcane Amalgamation, Permanently adds 20 fire resistance to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15391, 2047 }, // Lesser Arcane Amalgamation, Permanently adds 125 armor to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15389, -1 }, // Lesser Arcane Amalgamation, Permanently adds 100 hit points to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 15340, -1 }, // Lesser Arcane Amalgamation, Permanently adds 100 mana to a leg or head slot item. Does not stack with other enchantments for the selected equipment slot.
    { 13948, -1 }, // Enchant Gloves - Minor Haste, Permanently enchant gloves to grant a +1% attack speed bonus.
    { 13947, -1 }, // Enchant Gloves - Riding Skill, Permanently enchant gloves to grant a minor movement bonus while mounted. 
    { 13933, -1 }, // Enchant Shield - Frost Resistance, Permanently enchant a shield to give +8 Frost Resistance.
    { 13931, -1 }, // Enchant Bracer - Deflection, Permanently enchants bracers to give +3 Defense.
    { 13898, -1 }, // Enchant Weapon - Fiery Weapon, Permanently enchant a melee weapon to often strike for $13897s1 additional fire damage.
    { 13890, -1 }, // Enchant Boots - Minor Speed, Permanently enchant boots to give a slight movement speed increase.
    { 13882, -1 }, // Enchant Cloak - Lesser Agility, Permanently enchant a cloak to give 3 Agility.
    { 13868, -1 }, // Enchant Gloves - Advanced Herbalism, Permanently enchant gloves to grant +5 herbalism skill.
    { 13841, -1 }, // Enchant Gloves - Advanced Mining, Permanently enchant gloves to grant +5 mining skill.
    { 13698, -1 }, // Enchant Gloves - Skinning, Permanently enchant gloves to grant +5 skinning skill.
    { 13689, -1 }, // Enchant Shield - Lesser Block, Permanently enchant a shield to give +2% chance to block.
    { 13620, -1 }, // Enchant Gloves - Fishing, Permanently enchant gloves to grant +2 fishing skill.
    { 13522, -1 }, // Enchant Cloak - Lesser Shadow Resistance, Permanently enchant a cloak so that it increases resistance to shadow by 10.
    { 12460, -1 }, // Sniper Scope, Attaches a permanent scope to a bow or gun that increases its damage by 7.
    { 9783, -1 }, // Mithril Spurs, Attaches spurs to your boots that increase your mounted movement speed slightly.
    { 7779, -1 }, // Enchant Bracer - Minor Agility, Permanently enchant bracers so they increase the wearer's Agility by 1.
    { 7220, -1 }, // Weapon Chain, Attaches a chain to your weapon, making it impossible to disarm.
    { 7218, -1 }, // Weapon Counterweight, Attaches a counterweight to a two-handed sword, mace, axe or polearm making it 3% faster.
} };

std::array<EnchanterStruct, 13> const TempEnchantements =
{ {
    { 28898, -1 }, // Blessed Wizard Oil, Whiled applied to target weapon it increases spell damage against undead by up to 60. Lasts for 1 hour.
    { 28891, -1 }, // Consecrated Weapon, While applied to target weapon it increases attack power against undead. by 100. Lasts for 1 hour.
    { 25123, -1 }, // Brilliant Mana Oil, While applied to target weapon it restores 12 mana to the caster every 5 seconds and increases the effect of healing spells by up to 25.  Lasts for 30 minutes.
    { 25122, -1 }, // Brilliant Wizard Oil, While applied to target weapon it increases spell damage by up to 36 and increases Spell Critical chance by 1% .  Lasts for 30 minutes.
    { 22756, -1 }, // Sharpen Weapon - Critical, Increase critical chance on a melee weapon by 2% for 30 minutes.
    { 16138, -1 }, // Sharpen Blade V, Increase sharp weapon damage by 8 for 30 minutes.
    { 3595, -1 }, // Frost Oil, When applied to a melee weapon it gives a 10% chance of casting Frostbolt at the opponent when it hits.  Lasts 30 minutes.
    { 3594, -1 }, // Shadow Oil, When applied to a melee weapon it gives a 15% chance of casting Shadowbolt III at the opponent when it hits.  Lasts 30 minutes.
    { 25351, 8 }, // Deadly Poison V, Coats a weapon with poison that lasts for 30 minutes. Each strike has a $h% chance of poisoning the enemy for $25349o1 Nature damage over $25349d.  Stacks up to 5 times on a single target.  120 charges.
    { 13227, 8 }, // Wound Poison, Coats a weapon with poison that lasts for 30 minutes. Each strike has a 30% chance of poisoning the enemy, reducing all healing effects used on them by $13224s1 for $13224d.  Stacks up to 5 times on a single target.  105 charges.
    { 11399, 8 }, // Mind-numbing Poison III, Coats a weapon with poison that lasts for 30 minutes. Each strike has a 20% chance of poisoning the enemy, increasing their casting time by $11398s1% for $11398d.  100 charges.
    { 11340, 8 }, // Instant Poison VI, Coats a weapon with poison that lasts for 30 minutes. Each strike has a $h% chance of poisoning the enemy which instantly inflicts $11337s1 Nature damage.  115 charges.
    { 11202, 8 }, // Crippling Poison, Coats a weapon with poison that lasts for 30 minutes. Each strike has a 30% chance of poisoning the enemy, slowing their movement speed to $11201s1% of normal for $11201d.
} };

bool EnchantItem(Player* pPlayer, GameObject* pGameObject, uint32 spellid, uint8 slot, EnchantmentSlot pEnchantmentSlot)
{
    if (!pPlayer)
        return false;

    if (!pGameObject)
        return false;

    if (!spellid)
        return false;

    Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!pItem)
        return false;

    ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(pItem->GetEntry());
    if (!itemProto)
        return false;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellid);
    if (!spellInfo)
        return false;

    uint32 enchantid = spellInfo->EffectMiscValue[0];
    if (!enchantid)
        return false;

    if (!pItem->IsFitToSpellRequirements(spellInfo))
        return false;

    uint32 duration = NULL;

    if (pEnchantmentSlot == TEMP_ENCHANTMENT_SLOT)
        duration = (spellInfo->EffectBasePoints[0] + 1) * IN_MILLISECONDS;
    else if (pEnchantmentSlot == PERM_ENCHANTMENT_SLOT)
        duration = NULL;

    LocaleConstant loc = pPlayer->GetSession()->GetSessionDbcLocale();

    std::ostringstream Hspell;
    Hspell << "|cffffffff|Hspell:" << spellInfo->Id << "|h[" << spellInfo->SpellName[loc] << "]|h|r";

    pPlayer->_ApplyItemMods(pItem, slot, false);
    pItem->ClearEnchantment(pEnchantmentSlot);
    pItem->SetEnchantment(pEnchantmentSlot, enchantid, duration, NULL);
    pPlayer->_ApplyItemMods(pItem, slot, true);
    pPlayer->SendSpellGo(pPlayer, spellid);

    ChatHandler(pPlayer->GetSession()).PSendSysMessage("%s was successfully enchanted with %s.", ChatHandler(pPlayer->GetSession()).GetItemLink(itemProto).c_str(), Hspell.str().c_str());

    return true;
}

void RemoveAllEnchants(Player* pPlayer, GameObject* pGameObject, EnchantmentSlot pEnchantmentSlot)
{
    if (!pPlayer)
        return;

    pPlayer->PlayerTalkClass->ClearMenus();

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            if (equippedItem->GetEnchantmentId(pEnchantmentSlot))
            {
                pPlayer->_ApplyItemMods(equippedItem, i, false);
                equippedItem->ClearEnchantment(pEnchantmentSlot);
                pPlayer->_ApplyItemMods(equippedItem, i, true);
            }
        }
    }
    pPlayer->SendSpellGo(pPlayer, SPELL_TRANSMOGRIFY);
}

bool BuildGossip(Player* pPlayer, int pEquipmentSlots, EnchantmentSlot pEnchantmentSlot, uint32 SpellID, int32 ClassMask)
{
    if (!pPlayer)
        return false;

    if ((ClassMask & pPlayer->GetClassMask()) == 0)
        return false;

    // This should also filter out everything thats not for the actual server patch.
    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(SpellID);
    if (!spellInfo)
        return false;

    if (pPlayer->GetLevel() < spellInfo->spellLevel)
        return false;

    Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, pEquipmentSlots);
    if (!pItem)
        return false;

    if (!pItem->IsFitToSpellRequirements(spellInfo))
        return false;

    uint32 enchantid = spellInfo->EffectMiscValue[0];
    if (!enchantid)
        return false;

    int GOSSIP_ICON = GOSSIP_ICON_INTERACT_1;

    uint32 enchant_id = pItem->GetEnchantmentId(pEnchantmentSlot);

    if (enchant_id == enchantid)
        GOSSIP_ICON = GOSSIP_ICON_TAXI;

    char* description = "Not available.";

    LocaleConstant loc = pPlayer->GetSession()->GetSessionDbcLocale();

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (spellInfo->Effect[i] == NULL)
            continue;

        if (spellInfo->Effect[i] == SPELL_EFFECT_ENCHANT_ITEM ||
            spellInfo->Effect[i] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY ||
            spellInfo->Effect[i] == SPELL_EFFECT_ENCHANT_HELD_ITEM)
        {
            SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(spellInfo->EffectMiscValue[i]);
            if (pEnchant)
            {
                description = pEnchant->description[loc];
                break;
            }
        }
        return false;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON, description, pEquipmentSlots, SpellID);

    return true;
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

            int InventoryType = NULL;
            const char* InventoryName = "[Head]";

            switch (itemProto->InventoryType)
            {
                case INVTYPE_HEAD:
                    ss << "[Head]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_HEAD);
                    break;
                case INVTYPE_SHOULDERS:
                    ss << "[Shoulder]    " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_SHOULDERS);
                    break;
                case INVTYPE_CLOAK:
                    ss << "[Back]           " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_BACK);
                    break;
                case INVTYPE_CHEST:
                    ss << "[Chest]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_CHEST);
                    break;
                case INVTYPE_ROBE:
                    ss << "[Robe]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_CHEST);
                    break;
                case INVTYPE_LEGS:
                    ss << "[Legs]           " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_LEGS);
                    break;
                case INVTYPE_FEET:
                    ss << "[Feet]            " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_FEET);
                    break;
                case INVTYPE_WRISTS:
                    ss << "[Wrist]          " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_WRISTS);
                    break;
                case INVTYPE_HANDS:
                    ss << "[Hands]        " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_HANDS);
                    break;
                case INVTYPE_2HWEAPON:
                case INVTYPE_WEAPON:
                case INVTYPE_WEAPONMAINHAND:
                case INVTYPE_WEAPONOFFHAND:
                {
                    if (i == EQUIPMENT_SLOT_MAINHAND)
                    {
                        ss << "[Main Hand]" << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_MAINHAND);
                    }
                    if (pPlayer->CanDualWield() && i == EQUIPMENT_SLOT_OFFHAND)
                    {
                        ss << "[Offhand]     " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_OFFHAND);
                    }
                    break;
                }
                case INVTYPE_SHIELD:
                    ss << "[Shield]       " << "|" << quality << itemProto->Name1 << "|r";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_OFFHAND);
                    break;
                case INVTYPE_RANGED:
                case INVTYPE_RANGEDRIGHT:
                    if (itemProto->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW || itemProto->SubClass == ITEM_SUBCLASS_WEAPON_GUN || itemProto->SubClass == ITEM_SUBCLASS_WEAPON_BOW)
                    {
                        ss << "[Ranged]      " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_RANGED);
                    }
                    break;

                ss << "[Ranged]      " << "|" << quality << itemProto->Name1 << "|r";
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_RANGED);
            }
        }
    }
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove all my Enchants.", ENCHANTER_MAIN_MENU, REMOVE_ENCHANTS);
    pPlayer->SEND_GOSSIP_MENU(600005, pGameObject->GetObjectGuid());
    return true;
}

bool GossipSelect_EnchanterNPC(Player* pPlayer, GameObject* pGameObject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }
    
    // If uiAction is a valid Enchanting Spell, we should enchant here.
    if (uiSender != ENCHANTER_MAIN_MENU)
    {
        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(uiAction);
        if (spellInfo)
        {
            Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiSender);
            if (pItem)
            {
                uint32 enchantid = spellInfo->EffectMiscValue[0];
                if (enchantid)
                {
                    if (EnchantItem(pPlayer, pGameObject, uiAction, uiSender, PERM_ENCHANTMENT_SLOT))
                    {
                        GossipHello_EnchanterNPC(pPlayer, pGameObject);
                        return true;
                    }
                    else
                    {
                        pPlayer->GetSession()->SendNotification("Something went wrong!");
                        return false;
                    }
                }
            }
        }
    }

    if (uiSender == ENCHANTER_MAIN_MENU && uiAction == REMOVE_ENCHANTS)
    {
        RemoveAllEnchants(pPlayer, pGameObject, PERM_ENCHANTMENT_SLOT);
        GossipHello_EnchanterNPC(pPlayer, pGameObject);
        return true;
    }

    if (uiAction == ENCHANTER_MAIN_MENU) // uiAction 0 means no InventoryType, we use this to go back to the Main Menu.
    {
        GossipHello_EnchanterNPC(pPlayer, pGameObject);
        return true;
    }

    // Loop through all Equipment Slots and stop if uiAction matches the EquipmentSlot.
    for (uint8 equipment_slot = EQUIPMENT_SLOT_START; equipment_slot < EQUIPMENT_SLOT_END; ++equipment_slot)
    {
        if (uiAction == equipment_slot) // EquipmentSlots was the uiAction from GossipHello.
        {
            uint16 gossip_count = NULL;
            uint32 last_count = uiSender;
            uint32 new_count = uiSender;

            if (uiSender == ENCHANTER_MAIN_MENU) // If uiSender = ENCHANTER_MAIN_MENU, set it to NULL.
            { 
                last_count = NULL;
                new_count = NULL;
            }

            // Find all PermEnchantements that matches the EquipmentSlot.
            for (uint16 i = last_count; i < PermEnchantements.size(); ++i)
            {
                new_count = i;

                if (gossip_count == 10) // Do not add more than 10 Gossips per Page or it will visually break.
                    break;
                
                if (BuildGossip(pPlayer, equipment_slot, PERM_ENCHANTMENT_SLOT, PermEnchantements[i].m_SpellEntry, PermEnchantements[i].m_ClassMask))
                    gossip_count++; // Only count successful created Gossips.
            }

            if ((new_count) != (PermEnchantements.size() - 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", new_count, equipment_slot);

            if (last_count <= 0) // If uiSender was NULL we return to GossipHello_EnchanterNPC.
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", NULL, ENCHANTER_MAIN_MENU);
            //else // If not, return to last page (Still need to figure out).
                //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", (last_count - (new_count - last_count)), equipment_slot);

            /*
            std::ostringstream Hspell;
            Hspell << "last_count: " << last_count << " new_count: " << new_count << " PermEnchantements.size: " << PermEnchantements.size() << " last page: " << (last_count - (new_count - last_count));
            ChatHandler(pPlayer->GetSession()).PSendSysMessage(Hspell.str().c_str());
            */
        }
        pPlayer->SEND_GOSSIP_MENU(600008, pGameObject->GetObjectGuid());
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
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_MAINHAND);
                    }
                    if (pPlayer->CanDualWield() && i == EQUIPMENT_SLOT_OFFHAND)
                    {
                        ss << "[Offhand]      " << "|" << quality << itemProto->Name1 << "|r";
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_SET, ss.str().c_str(), ENCHANTER_MAIN_MENU, EQUIPMENT_SLOT_OFFHAND);
                    }
                    break;
                }
            }
        }
        else
            continue;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove all Weapon Enchants.", ENCHANTER_MAIN_MENU, REMOVE_ENCHANTS);
    pPlayer->SEND_GOSSIP_MENU(600005, pGameObject->GetObjectGuid());
    return true;
}

bool GossipSelect_PoisoneerNPC(Player* pPlayer, GameObject* pGameObject, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();

    if (pPlayer->IsInCombat())
    {
        pPlayer->GetSession()->SendNotification("You are in combat!");
        return true;
    }

    // If uiAction is a valid Enchanting Spell, we should enchant here.
    if (uiSender != ENCHANTER_MAIN_MENU)
    {
    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(uiAction);
        if (spellInfo)
        {
            Item* pItem = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, uiSender);
            if (pItem)
            {
                uint32 enchantid = spellInfo->EffectMiscValue[0];
                if (enchantid)
                {
                    if (EnchantItem(pPlayer, pGameObject, uiAction, uiSender, TEMP_ENCHANTMENT_SLOT))
                    {
                        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
                        return true;
                    }
                    else
                    {
                        pPlayer->GetSession()->SendNotification("Something went wrong!");
                        return false;
                    }
                }
            }
        }
    }

    if (uiSender == ENCHANTER_MAIN_MENU && uiAction == REMOVE_ENCHANTS)
    {
        RemoveAllEnchants(pPlayer, pGameObject, TEMP_ENCHANTMENT_SLOT);
        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
        return true;
    }

    if (uiAction == ENCHANTER_MAIN_MENU) // uiAction 0 means no InventoryType, we use this to go back to the Main Menu.
    {
        GossipHello_PoisoneerNPC(pPlayer, pGameObject);
        return true;
    }

    // Loop through all Equipment Slots and stop if uiAction matches the EquipmentSlot.
    for (uint8 equipment_slot = EQUIPMENT_SLOT_START; equipment_slot < EQUIPMENT_SLOT_END; ++equipment_slot)
    {
        if (uiAction == equipment_slot) // EquipmentSlots was the uiAction from GossipHello.
        {
            uint16 gossip_count = NULL;
            uint32 last_count = uiSender;
            uint32 new_count = uiSender;

            if (uiSender == ENCHANTER_MAIN_MENU) // If uiSender = ENCHANTER_MAIN_MENU, set it to NULL.
            {
                last_count = NULL;
                new_count = NULL;
            }

            // Find all TempEnchantements that matches the EquipmentSlot.
            for (uint16 i = last_count; i < TempEnchantements.size(); ++i)
            {
                new_count = i;

                if (gossip_count == 10) // Do not add more than 10 Gossips per Page or it will visually break.
                    break;

                if (BuildGossip(pPlayer, equipment_slot, TEMP_ENCHANTMENT_SLOT, TempEnchantements[i].m_SpellEntry, TempEnchantements[i].m_ClassMask))
                    gossip_count++; // Only count successful created Gossips.
            }

            if ((new_count) != (TempEnchantements.size() - 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", new_count, equipment_slot);

            if (last_count <= 0)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", NULL, ENCHANTER_MAIN_MENU);
            //else
                //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", (last_count - (new_count - last_count)), equipment_slot);

            /*
            std::ostringstream Hspell;
            Hspell << "last_count: " << last_count << " new_count: " << new_count << " TempEnchantements.size: " << TempEnchantements.size() << " last page: " << (last_count - (new_count - last_count));
            ChatHandler(pPlayer->GetSession()).PSendSysMessage(Hspell.str().c_str());
            */
        }
        pPlayer->SEND_GOSSIP_MENU(600008, pGameObject->GetObjectGuid());
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
            pPlayer->SendSpellGo(pPlayer, SPELL_TRANSMOGRIFY);
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