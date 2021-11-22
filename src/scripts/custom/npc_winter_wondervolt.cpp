
#include "scriptPCH.h"
#include "Player.h"
#include <ctime>

#define SERVER_OWNERS_THRONE 900065

enum sSpells
{
    TRANSFORM_VISUAL = 10389,
    STUN_8S = 15283,
    UNTRANSFORM_HERO = 25100, //Selfbuff Holy glow visual aura.
    KNOCKBACK = 10689, //silence visual knockback.
    SPAWN_RED_LIGHTNING = 24240, //visual red lightning pillar.
    AREA_BURN = 8000, //visual red lightning pillar.
    DMG_SHIELD = 25059, //Thorns sprout from the friendly target causing 10000 Nature damage to attackers when hit. Lasts 0 sec..
    EMBERSEER_TRANSFORM = 16052, //fire visual
    KNOCKBACK_35 = 24199, //(98) Knock Back (350)  
    NEFARIANS_BARRIER = 22663, //(98) Knock Back (350)  
    SPIRIT_PARTICLES_PURPLE = 28126, //(98) Knock Back (350)  
};

enum tTalents
{
    THICK_HIDE = 16933, //Increases your Armor contribution from items by 10%.
};

void ApplyAura(Unit* pUnit, uint32 spellId)
{
    if (!spellId)
        return;

    if (!pUnit)
        return;

    enum Talents
    {
        IMPROVED_BATTLE_SHOUT_RANK5             = 12861,
        RESTORATIVE_TOTEMS_RANK5                = 16208,
        ENHANCING_TOTEMS_RANK2                  = 16295,
        IMPROVED_WEAPON_TOTEMS_RANK2            = 29193,
        IMPROVED_BLESSING_OF_MIGHT_RANK5        = 20048,
        IMPROVED_BLESSING_OF_WISDOM_RANK2       = 20245,
        IMPROVED_MARK_OF_THE_WILD_RANK5         = 17055,
        IMPROVED_DEVOTION_AURA_RANK5            = 20142,
        IMPROVED_IMP_RANK3                      = 18696,
    };

    enum Talents talentpells[] = {
        IMPROVED_BATTLE_SHOUT_RANK5,
        RESTORATIVE_TOTEMS_RANK5,
        ENHANCING_TOTEMS_RANK2,
        IMPROVED_BLESSING_OF_MIGHT_RANK5,
        IMPROVED_BLESSING_OF_WISDOM_RANK2,
        IMPROVED_MARK_OF_THE_WILD_RANK5,
        IMPROVED_DEVOTION_AURA_RANK5,
        IMPROVED_IMP_RANK3,
    };

    pUnit->RemoveAurasDueToSpell(spellId);

    // Learn all high ranks for buffing.
    for (uint32 i = 0; i < 8; ++i) {
        if (!pUnit->HasSpell(talentpells[i]) && !pUnit->HasAura(talentpells[i]))
            pUnit->AddAura(talentpells[i]);
    }

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellId);

    if (!spellInfo)
        return;

    if (spellInfo->spellLevel > pUnit->GetLevel())
        return;

    for (uint32 i = 1; i < sItemStorage.GetMaxEntry(); ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype >(i);

        if (!proto)
            continue;

        for (uint8 j = 0; j < MAX_ITEM_PROTO_SPELLS; ++j)
        {
            if (spellId == proto->Spells[j].SpellId)
            {
                if (pUnit->GetLevel() < proto->RequiredLevel)
                    return;
            }
        }
    }

    SpellSpecific spellId_spec = Spells::GetSpellSpecific(spellId);

    // Buff.
    if (SpellAuraHolder * pHolder = pUnit->AddAura(spellId, NULL, nullptr))
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
        sLog.outBasic("[ApplyAura] spell %d not applied.", spellInfo->SpellName);

    // Remove all talents again.
    for (uint32 i = 0; i < 8; ++i) {
        if (!pUnit->HasSpell(talentpells[i]) && pUnit->HasAura(talentpells[i]))
            pUnit->RemoveAurasDueToSpell(talentpells[i]);
    }
}

void FullBuffed(Player* pPlayer)
{
    enum WorldBuffs
    {
        ECHOES_OF_LORDAERON_ALLIANCE                = 1386, //Increases melee, ranged and spell damage dealt to Undead by 5%.
        ECHOES_OF_LORDAERON_HORDE                   = 29520, //Increases melee, ranged and spell damage dealt to Undead by 5%.
        WARCHIEFS_BLESSING                          = 16609, //Increases hitpoints by 300. 15% haste to melee attacks. 10 mana regen every 5 seconds.
        RALLYING_CRY_OF_THE_DRAGONSLAYER            = 22888, //Increases critical chance of spells by 10%, melee and ranged by 5% and grants 140 attack power. 120 minute duration.
        SPIRIT_OF_ZANDALAR                          = 24425, //Increases movement speed by 10 % and all stats by 15 % for 2 hours.
        SONGFLOWER_SERENADE                         = 15366, //Increases chance for a melee, ranged, or spell critical by 5% and all attributes by 15 for 1 hr.
        SLIPKIKS_SAVVY_DIRE_MAUL                    = 22820, //Chance for a critical hit with a spell increased by 3%.
        FENGUS_FEROCITY_DIRE_MAUL                   = 22817, //Attack power increased by 200.
        MOLDARS_MOXIE_DIRE_MAUL                     = 22818, //Overall Stamina increased by 15%.
        SAYGES_DARK_FORTUNE_OF_DAMAGE               = 23768, //Increases damage by 10%.
        SAYGES_DARK_FORTUNE_OF_STRENGTH             = 23735, //Increases Strength by 10%.
        SAYGES_DARK_FORTUNE_OF_AGILITY              = 23736, //Increases Agility by 10%.
        SAYGES_DARK_FORTUNE_OF_SPIRIT               = 23738, //Increases damage by 10%.
        SAYGES_DARK_FORTUNE_OF_STAMINA              = 23737, //Increases stamina by 10%.
        SAYGES_DARK_FORTUNE_OF_INTELLIGENCE         = 23766, //Increases intellect by 10%.
        TRACES_OF_SILITHYST                         = 29534, //Melee, ranged and spell damage dealt increased by 5%.
        SOUL_REVIVAL                                = 28681, //During the invasion you need to kill regular undead to turn the Necrotic Crystal into a Damaged Necrotic Crystal. This takes roughly 200 kills of regular undead spawns to do. When the crystal is destroyed a localised buff is cast on all players within 65 yards of the crystal. This Heals you to full, Restores your mana to full, and gives you this + 10 % damage buff for 30 minutes.
        ELUNES_BLESSING                             = 26393, //Increasing total stats by 10% for 60 min.
    };

    enum ClassBuffs
    {
        ARCANE_BRILLIANCE                           = 23028, //Infuses the target's party with brilliance, increasing their Intellect by 31 for 1 hr.
        PRAYER_OF_FORTITUDE                         = 21564, //Power infuses the target's party, increasing their Stamina by 54 for 1 hr.
        PRAYER_OF_SPIRIT                            = 27681, //Power infuses the target's party, increasing their Spirit by 40 for 1 hr.
        MOONKIN_AURA                                = 24907, //Increases spell critical chance by 3%.
        TRUESHOT_AURA                               = 20906, //Increases the attack power of party members within 45 yards by 100. Lasts 30 min.
        LEADER_OF_THE_PACK                          = 24932, //While in Cat, Bear or Dire Bear Form, the Leader of the Pack increases ranged and melee critical chance of all party members within 45 yards by 3%.
        GREATER_BLESSING_OF_KINGS                   = 25898, //increasing total stats by 10% for 15 min.
        GREATER_BLESSING_OF_MIGHT                   = 25916, //increasing melee attack power by 185 for 15 min
        GREATER_BLESSING_OF_WISDOM                  = 25918, //Gives all members of the raid or group that share the same class with the target the Greater Blessing of Kings, increasing total stats by 10% for 15 min. Players may only have one Blessing on them per Paladin at any one time.
        MARK_OF_THE_WILD                            = 9885, //Increases the friendly target's armor by 285, all attributes by 12 and all resistances by 20 for 30 min.
        BATTLE_SHOUT                                = 25289, //The warrior shouts, increasing the melee attack power of all party members within 20 yards by 232. Lasts 2 min.
        GRACE_OF_AIR                                = 25360,
        STRENGTH_OF_EARTH                           = 25362,
        MANA_SPRING                                 = 10494,
        BLOOD_PACT                                  = 11767,
        DEVOTION_AURA                               = 10293, //Gives 735 additional armor to party members within 30 yards. Players may only have one Aura on them per Paladin at any one time.
    };

    enum WeapenBuffs
    {
        CONSECRATED_WEAPON                          = 2684, //While applied to target weapon it increases attack power against undead by 100. Lasts for 1 hour.
        ELEMENTAL_SHARPENING_STONE                  = 2506, //Increase critical chance on a melee weapon by 2% for 30 minutes.
        IRON_COUNTERWEIGHT                          = 34, //Attaches a counterweight to a two-handed sword, mace, axe or polearm making it 3% faster.
        DENSE_WEIGHTSTONE                           = 1703, //Increase the damage of a blunt weapon by 8 for 30 minutes.
        BRILLIANT_MANA_OIL                          = 2629, //While applied to target weapon it restores 12 mana to the caster every 5 seconds and increases the effect of healing spells by up to 25. Lasts for 30 minutes.
        BRILLIANT_WIZARD_OIL                        = 2628, //While applied to target weapon it restores 12 mana to the caster every 5 seconds and increases the effect of healing spells by up to 25. Lasts for 30 minutes.
    };

    enum Flask
    {
        HOLY_MIGHTSTONE                             = 24833, //Shatters the Holy Mightstone, granting 300 Attack Power and increasing Holy damage from spells and effects by up to 400 when fighting undead. Lasts 10 min.
        BUTTERMILK_DELIGHT                          = 27720, //Increases Defense by 13 for 1 hr.
        SWEET_SURPRISE                              = 27722, //Increases healing done by magical spells by up to 44 for 1 hr.
        VERY_BERRY_CREAM                            = 27721, //Increases damage done from spells by up to 23 for 1 hr.
        DARK_DESIRE                                 = 27723, //Improves your chance to hit by 2% for 1 hr.
        CEREBRAL_CORTEX_COMPOUND                    = 10690, //Increases Intellect by 25 when consumed.
        HEADMASTERS_CHARGE                          = 18264, //Gives 20 additional intellect to party members within 30 yards.
        BLESSING_OF_BLACKFATHOM                     = 8733, //5 Intellect, 5 Spirit, 15 Frost Damage.
        GROUND_SCORPOK_ASSAY                        = 10669, //Increases Agility by 25 when consumed. Effect lasts for 60 minutes.
        FURY_OF_THE_BOGLING                         = 5665, //Increases physical damage by 1 for 10 min.
        MANA_RUBY                                   = 10058, //Restores 1000 to 1201 mana. (cooldown 2 min)
        GREATER_DREAMLESS_SLEEP_POTION              = 24360, //Puts the imbiber in a dreamless sleep for 12 sec. During that time the imbiber heals 2100 health and 2100 mana. (cooldown 2 min)
        MAJOR_TROLLS_BLOOD_POTION                   = 24361, //Regenerate 20 health every 5 sec for 1 hr.
        FLASK_OF_CHROMATIC_RESISTANCE               = 17629, //Increases your resistance to all schools of magic by 25 for 2 hr. You can only have the effect of one flask at a time. This effect persists through death and stacks with all other resistance spells and items.
        FLASK_OF_PETRIFICATION                      = 17624, //You turn to stone, protecting you from all physical attacks and spells for 1 min, but during that time you cannot attack, move or cast spells. You can only have the effect of one flask at a time.
        FLASK_OF_THE_TITANS                         = 17626, //Increases the player's maximum health by 1200 for 2 hr. You can only have the effect of one flask at a time. This effect persists through death.
        FLASK_OF_DISTILLED_WISDOM                   = 17627, //Increases the player's maximum mana by 2000 for 2 hr. You can only have the effect of one flask at a time. This effect persists through death.
        FLASK_OF_SUPREME_POWER                      = 17628, //Increases damage done by magical spells and effects by up to 150 for 2 hr. You can only have the effect of one flask at a time. This effect persists through death.
        MAJOR_MANA_POTION                           = 17531, //Restores 1350 to 2251 mana. (cooldown 2 min)
        GREATER_SHADOW_PROTECTION_POTION            = 17548, //Absorbs 1950 to 3251 shadow damage. Lasts 1 hr. (cooldown 2 min)
        GREATER_FIRE_PROTECTION_POTION              = 17543, //Absorbs 1950 to 3251 fire damage. Lasts 1 hr. (cooldown 2 min)
        GREATER_ARCANE_PROTECTION_POTION            = 17549, //Absorbs 1950 to 3251 arcane damage. Lasts 1 hr. (cooldown 2 min)
        GREATER_FROST_PROTECTION_POTION             = 17544, //Absorbs 1950 to 3251 frost damage. Lasts 1 hr. (cooldown 2 min)
        GREATER_HOLY_PROTECTION_POTION              = 17545,
        GREATER_ARCANE_ELIXIR                       = 17539, //Increases spell damage by up to 35 for 1 hr.
        MANA_CITRINE                                = 10057, //Restores 775 to 926 mana. (cooldown 2 min)
        GREATER_NATURE_PROTECTION_POTION            = 17546, //Absorbs 1950 to 3251 nature damage. Lasts 1 hr. (cooldown 2 min)
        LIVING_ACTION_POTION                        = 24364, //Makes you immune to Stun and Movement Impairing effects for the next 5 sec. Also removes existing Stun and Movement Impairing effects. (cooldown 2 min)
        PURIFICATION_POTION                         = 17550, //Attempts to remove one Curse, one Disease and one Poison from the Imbiber. (cooldown 2 min)
        GREATER_STONESHIELD_POTION                  = 17540, //Increases armor by 2000 for 2 min. (cooldown 2 min)
        ELIXIR_OF_THE_MONGOOSE                      = 17538, //Increases Agility by 25 and chance to get a critical hit by 2% for 1 hr.
        MIGHTY_RAGE_POTION                          = 17528, //Increases Rage by 45 to 346 and increases Strength by 60 for 20 sec. (cooldown 2 min)
        RAW_WHITESCALE_SALMON                       = 1129, //Restores 1392 health over 30 sec. Must remain seated while eating.
        // does not stack with juju - ELIXIR_OF_BRUTE_FORCE = 17537, //Increases Strength and Stamina by 18 for 1 hr.
        LIMITED_INVULNERABILITY_POTION              = 3169, //Imbiber is immune to physical attacks for the next 6 sec. (cooldown 2 min)
        ELIXIR_OF_THE_SAGES                         = 17535, //Increases Intellect and Spirit by 18 for 1 hr.
        ELIXIR_OF_SUPERIOR_DEFENSE                  = 11348, //Increases armor by 450 for 1 hr.
        ELIXIR_OF_SHADOW_POWER                      = 11474, //Increases spell shadow damage by up to 40 for 30 min.
        MAGEBLOOD_POTION                            = 24363, //Regenerate 12 mana per 5 sec for 1 hr. (cooldown 10 sec)
        ELIXIR_OF_DEMONSLAYING                      = 11406, //Increases attack power by 265 against demons. Lasts 5 min. 
        ELIXIR_OF_GREATER_FIREPOWER                 = 26276, //Increases spell fire damage by up to 40 for 30 min.
        ELIXIR_OF_DETECT_DEMON                      = 11407, //Shows the location of all nearby demons on the minimap for 1 hr.
        // does not stack with mongoose - ELIXIR_OF_GREATER_AGILITY = 11334, //Increases Agility by 25 for 1 hr.
        // does not stack with juju - ELIXIR_OF_GIANTS = 11405, //Increases your Strength by 25 for 1 hr.
        ARCANE_ELIXIR                               = 11390, //Increases spell damage by up to 20 for 30 min.
        ELIXIR_OF_GREATER_INTELLECT                 = 11396, //Increases Intellect by 25 for 1 hr.
        ELIXIR_OF_DETECT_UNDEAD                     = 11389, //Shows the location of all nearby undead on the minimap for 1 hr.
        NOGGENFOGGER_ELIXIR                         = 16589, //Drink Me.
        POACHED_SUNSCALE_SALMON                     = 18232, //Restores 874.8 health over 27 sec. Must remain seated while eating. Also restores 6 health every 5 seconds for 10 min.
        ELIXIR_OF_GREATER_WATER_BREATHING           = 22807, //Allows the Imbiber to breathe water for 1 hr.
        MAGIC_RESISTANCE_POTION                     = 11364, //Increases your resistance to all schools of magic by 50 for 3 min. (cooldown 2 min)
        CATSEYE_ELIXIR                              = 12608, //Slightly increases your stealth detection for 10 min.
        ELIXIR_OF_GREATER_DEFENSE                   = 11349, //Increases armor by 250 for 1 hr.
        ELIXIR_OF_DETECT_LESSER_INVISIBILITY        = 6512, //Grants detect lesser invisibility for 10 min.
        FROST_PROTECTION_POTION                     = 7239, //Absorbs 1350 to 2251 frost damage. Lasts 1 hr. (cooldown 2 min)
        NATURE_PROTECTION_POTION                    = 7254, //Absorbs 1350 to 2251 nature damage. Lasts 1 hr. (cooldown 2 min)
        ELIXIR_OF_FROST_POWER                       = 21920, //Increases spell frost damage by up to 15 for 30 min.
        ELIXIR_OF_AGILITY                           = 11328, //Increases Agility by 15 for 1 hr.
        MIGHTY_TROLLS_BLOOD_POTION                  = 3223, //Regenerate 12 health every 5 sec for 1 hr.
        ELIXIR_OF_FORTITUDE                         = 3593, //Increases the player's maximum health by 120 for 1 hr.
        FIRE_PROTECTION_POTION                      = 7233, //Absorbs 975 to 1626 fire damage. Lasts 1 hr. (cooldown 2 min)
        STRONG_TROLLS_BLOOD_POTION                  = 3222, //Regenerate 6 health every 5 sec for 1 hr.
        HOLY_PROTECTION_POTION                      = 7245, //Absorbs 300 to 501 holy damage. Lasts 1 hr. (cooldown 2 min)
        BLOODKELP_ELIXIR_OF_DODGING                 = 27653, //Increases chance to dodge by 3% for 30 min.
        BLOODKELP_ELIXIR_OF_RESISTANCE              = 27652, //Increases all magical resistances by 15 for 30 min.
        RUMSEY_RUM_BLACK_LABEL                      = 25804, //Increases Stamina by 15 for 15 min and gets you drunk to boot!
        ELIXIR_OF_WATER_WALKING                     = 8827, //Lets you walk on water for 30 min.
        NIGHTFIN_SOUP                               = 18194, //Restores 874.8 health over 27 sec. Must remain seated while eating. Also restores 162 Mana every 5 seconds for 10 min.
        WINTERFALL_FIREWATER                        = 17038, //Increases your melee attack power by 35 and size for 20 min.
        JUJU_MIGHT                                  = 16329, //Increases attack power by 40 for 10 min.
        JUJU_POWER                                  = 16323, //Increases the target's Strength by 30 for 30 min.
        SMOKED_DESERT_DUMPLINGS                     = 24799, //Restores 2148 health over 30 sec. Must remain seated while eating. If you spend at least 10 seconds eating you will become well fed and gain 20 Strength for 15 min.
        BLESSED_SUNFRUIT                            = 18125, //Increases Strength by 10 for 10 min.
        GRILLED_SQUID                               = 18192, //Increases Agility by 10 for 10 min.
        BOBBING_APPLE                               = 24870, //Restores 2% of your health per second for 24 sec. Must remain seated while eating. If you spend at least 10 seconds eating you will become well fed and gain Stamina and Spirit for 15 min.
        ROIDS                                       = 10667, //Increases Strength by 25 when consumed. Effect lasts for 60 minutes.
        RUNN_TUM_TUBER_SURPRISE                     = 22730, //Increases Intellect by 10 for 10 min.
        SWIFTNESS_OF_ZANZA                          = 24383, //Increases the player's run speed by 20% for 2 hr. You can only have the effect of one Zanza potion at a time.
    };

    enum Crystals
    {
        CRYSTAL_WARD                                = 15233, //Increases the target's Armor by 200 for 30 min.
        CRYSTAL_FORCE                               = 15231, //Increases the target's Spirit by 30 for 30 min.
        CRYSTAL_SPIRE                               = 15279, //A crystal shield surrounds the friendly target, doing 12 damage to anyone pWho hits him. Lasts 10 min.
    };

    enum Misc
    {
        HAPPY_PET                                   = 24716 // 	(30) Energize (4) Value: 1000000
    };

    pPlayer->RemoveAllAurasOnDeath();
    pPlayer->RemoveAurasDueToSpell(15007);

    if (pPlayer->GetTeam() == HORDE)
        ApplyAura(pPlayer, ECHOES_OF_LORDAERON_HORDE);
    if (pPlayer->GetTeam() == ALLIANCE)
        ApplyAura(pPlayer, ECHOES_OF_LORDAERON_ALLIANCE);
    ApplyAura(pPlayer, HOLY_MIGHTSTONE);

    ApplyAura(pPlayer, WARCHIEFS_BLESSING); // alliance can have this too with mind control!
    ApplyAura(pPlayer, RALLYING_CRY_OF_THE_DRAGONSLAYER);
    ApplyAura(pPlayer, SPIRIT_OF_ZANDALAR);
    ApplyAura(pPlayer, SONGFLOWER_SERENADE);

    switch (pPlayer->GetClass())
    {
    case CLASS_WARRIOR:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, GRACE_OF_AIR);
            ApplyAura(pPlayer, STRENGTH_OF_EARTH);
        }
        ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, LEADER_OF_THE_PACK);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        ApplyAura(pPlayer, TRUESHOT_AURA);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
        }
        ApplyAura(pPlayer, BATTLE_SHOUT);
        break;
    case CLASS_PRIEST:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, MANA_SPRING);
        }
        ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
        ApplyAura(pPlayer, MOONKIN_AURA);
        ApplyAura(pPlayer, ARCANE_BRILLIANCE);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
        }
        break;
    case CLASS_PALADIN:
        if (pPlayer->GetTalentTabID() == PaladinProtection)
        {
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, LEADER_OF_THE_PACK);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            ApplyAura(pPlayer, TRUESHOT_AURA);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
            ApplyAura(pPlayer, BATTLE_SHOUT);
        }
        if (pPlayer->GetTalentTabID() == PaladinRetribution)
        {
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, LEADER_OF_THE_PACK);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            ApplyAura(pPlayer, TRUESHOT_AURA);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
            ApplyAura(pPlayer, BATTLE_SHOUT);
        }
        if (pPlayer->GetTalentTabID() == PaladinHoly)
        {
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
        }
        break;
    case CLASS_ROGUE:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, GRACE_OF_AIR);
            ApplyAura(pPlayer, STRENGTH_OF_EARTH);
        }
        ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, LEADER_OF_THE_PACK);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        ApplyAura(pPlayer, TRUESHOT_AURA);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
        }
        ApplyAura(pPlayer, BATTLE_SHOUT);
        break;
    case CLASS_MAGE:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, MANA_SPRING);
        }
        ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
        ApplyAura(pPlayer, MOONKIN_AURA);
        ApplyAura(pPlayer, ARCANE_BRILLIANCE);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
        }
        break;
    case CLASS_SHAMAN:
        if (pPlayer->GetTalentTabID() == ShamanElementalCombat)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
            }
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
        }
        if (pPlayer->GetTalentTabID() == ShamanEnhancement)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
                ApplyAura(pPlayer, STRENGTH_OF_EARTH);
                ApplyAura(pPlayer, GRACE_OF_AIR);
            }
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, LEADER_OF_THE_PACK);
            //ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            //ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            //ApplyAura(pPlayer, MARK_OF_THE_WILD);
            ApplyAura(pPlayer, TRUESHOT_AURA);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
            ApplyAura(pPlayer, BATTLE_SHOUT);
        }
        if (pPlayer->GetTalentTabID() == ShamanRestoration)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
            }
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
        }
        break;
    case CLASS_HUNTER:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, STRENGTH_OF_EARTH);
            ApplyAura(pPlayer, GRACE_OF_AIR);
        }
        ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, LEADER_OF_THE_PACK);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        ApplyAura(pPlayer, TRUESHOT_AURA);
        ApplyAura(pPlayer, BATTLE_SHOUT);

        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
        }
        if (Pet * pet = pPlayer->GetPet())
        {
            if (!pet)
                return;

            if (!pet->isControlled())
                return;

            if (pet->getPetType() != HUNTER_PET)
                return;

            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pet, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pet, GREATER_BLESSING_OF_MIGHT);
            }
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pet, STRENGTH_OF_EARTH);
                ApplyAura(pet, GRACE_OF_AIR);
            }
            ApplyAura(pet, WARCHIEFS_BLESSING);
            ApplyAura(pet, RALLYING_CRY_OF_THE_DRAGONSLAYER);
            ApplyAura(pet, SPIRIT_OF_ZANDALAR);
            ApplyAura(pet, FENGUS_FEROCITY_DIRE_MAUL);
            ApplyAura(pet, SOUL_REVIVAL);
            ApplyAura(pet, LEADER_OF_THE_PACK);
            ApplyAura(pet, MARK_OF_THE_WILD);
            ApplyAura(pet, TRUESHOT_AURA);
            ApplyAura(pet, BATTLE_SHOUT);
            ApplyAura(pPlayer, HAPPY_PET);
        }
        break;
    case CLASS_DRUID:
        if (pPlayer->GetTalentTabID() == DruidBalance)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
            }
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
        }
        if (pPlayer->GetTalentTabID() == DruidFeralCombat)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
                ApplyAura(pPlayer, STRENGTH_OF_EARTH);
                ApplyAura(pPlayer, GRACE_OF_AIR);
            }
            ApplyAura(pPlayer, FENGUS_FEROCITY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, LEADER_OF_THE_PACK);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            ApplyAura(pPlayer, TRUESHOT_AURA);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_MIGHT);
            }
            ApplyAura(pPlayer, BATTLE_SHOUT);
        }
        if (pPlayer->GetTalentTabID() == DruidRestoration)
        {
            if (pPlayer->GetTeam() == HORDE)
            {
                ApplyAura(pPlayer, MANA_SPRING);
            }
            ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
            ApplyAura(pPlayer, TRACES_OF_SILITHYST);
            ApplyAura(pPlayer, SOUL_REVIVAL);
            ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
            ApplyAura(pPlayer, MOONKIN_AURA);
            ApplyAura(pPlayer, ARCANE_BRILLIANCE);
            ApplyAura(pPlayer, MARK_OF_THE_WILD);
            if (pPlayer->GetTeam() == ALLIANCE)
            {
                ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
                ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
            }
        }
        break;
    case CLASS_WARLOCK:
        if (pPlayer->GetTeam() == HORDE)
        {
            ApplyAura(pPlayer, MANA_SPRING);
        }
        ApplyAura(pPlayer, SLIPKIKS_SAVVY_DIRE_MAUL);
        ApplyAura(pPlayer, TRACES_OF_SILITHYST);
        ApplyAura(pPlayer, SOUL_REVIVAL);
        ApplyAura(pPlayer, PRAYER_OF_SPIRIT);
        ApplyAura(pPlayer, MOONKIN_AURA);
        ApplyAura(pPlayer, ARCANE_BRILLIANCE);
        ApplyAura(pPlayer, MARK_OF_THE_WILD);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            ApplyAura(pPlayer, GREATER_BLESSING_OF_KINGS);
            ApplyAura(pPlayer, GREATER_BLESSING_OF_WISDOM);
        }
        break;
    }

    switch (pPlayer->GetClass())
    {
    case CLASS_WARRIOR:
        if (pPlayer->GetTalentTabID() == WarriorProtection)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_STAMINA);
            ApplyAura(pPlayer, MOLDARS_MOXIE_DIRE_MAUL);
            ApplyAura(pPlayer, PRAYER_OF_FORTITUDE);
            ApplyAura(pPlayer, FLASK_OF_THE_TITANS);
            ApplyAura(pPlayer, ELIXIR_OF_FORTITUDE);
            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            ApplyAura(pPlayer, ELIXIR_OF_SUPERIOR_DEFENSE);
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            ApplyAura(pPlayer, BLOOD_PACT);
            ApplyAura(pPlayer, GREATER_STONESHIELD_POTION);
            ApplyAura(pPlayer, CRYSTAL_WARD);
            ApplyAura(pPlayer, ROIDS);
            ApplyAura(pPlayer, BLOODKELP_ELIXIR_OF_DODGING);
            ApplyAura(pPlayer, RUMSEY_RUM_BLACK_LABEL);
            if (pPlayer->GetTeam() == ALLIANCE)
                ApplyAura(pPlayer, DEVOTION_AURA);
        }
        else if (pPlayer->GetTalentTabID() == WarriorArms)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FURY_OF_THE_BOGLING);
            //Agility
            //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            //Attack Power
            ApplyAura(pPlayer, WINTERFALL_FIREWATER);
            ApplyAura(pPlayer, JUJU_MIGHT);
            //Strength
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
            ApplyAura(pPlayer, JUJU_POWER);
            //Blasted Lands buffs
            ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
            ApplyAura(pPlayer, ROIDS);
            //Food
            ApplyAura(pPlayer, GRILLED_SQUID);
            ApplyAura(pPlayer, BLESSED_SUNFRUIT);
            ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
            ApplyAura(pPlayer, DARK_DESIRE);
        }
        else if (pPlayer->GetTalentTabID() == WarriorFury)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FURY_OF_THE_BOGLING);
            //Agility
            //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            //Attack Power
            ApplyAura(pPlayer, WINTERFALL_FIREWATER);
            ApplyAura(pPlayer, JUJU_MIGHT);
            //Strength
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
            ApplyAura(pPlayer, JUJU_POWER);
            //Blasted Lands buffs
            ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
            ApplyAura(pPlayer, ROIDS);
            //Food
            ApplyAura(pPlayer, GRILLED_SQUID);
            ApplyAura(pPlayer, BLESSED_SUNFRUIT);
            ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
            ApplyAura(pPlayer, DARK_DESIRE);
        }
        break;
    case CLASS_PRIEST:
        if (pPlayer->GetTalentTabID() == PriestShadow)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FLASK_OF_SUPREME_POWER);
            ApplyAura(pPlayer, GREATER_ARCANE_ELIXIR);
            ApplyAura(pPlayer, ELIXIR_OF_SHADOW_POWER);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
        }
        else if (pPlayer->GetTalentTabID() == PriestDiscipline)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, ELIXIR_OF_THE_SAGES);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, SWEET_SURPRISE);
        }
        else if (pPlayer->GetTalentTabID() == PriestHoly)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, ELIXIR_OF_THE_SAGES);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, SWEET_SURPRISE);
        }
        break;
    case CLASS_PALADIN:
        if (pPlayer->GetTalentTabID() == PaladinProtection)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_STAMINA);
            ApplyAura(pPlayer, MOLDARS_MOXIE_DIRE_MAUL);
            ApplyAura(pPlayer, PRAYER_OF_FORTITUDE);
            ApplyAura(pPlayer, FLASK_OF_THE_TITANS);
            ApplyAura(pPlayer, ELIXIR_OF_FORTITUDE);
            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            ApplyAura(pPlayer, ELIXIR_OF_SUPERIOR_DEFENSE);
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            ApplyAura(pPlayer, BLOOD_PACT);
            ApplyAura(pPlayer, GREATER_STONESHIELD_POTION);
            ApplyAura(pPlayer, CRYSTAL_WARD);
            ApplyAura(pPlayer, ROIDS);
            ApplyAura(pPlayer, BLOODKELP_ELIXIR_OF_DODGING);
            ApplyAura(pPlayer, RUMSEY_RUM_BLACK_LABEL);
            ApplyAura(pPlayer, DEVOTION_AURA);
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
        }
        if (pPlayer->GetTalentTabID() == PaladinRetribution)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);

            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            ApplyAura(pPlayer, JUJU_MIGHT);
            ApplyAura(pPlayer, ROIDS);
            ApplyAura(pPlayer, WINTERFALL_FIREWATER);
            ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
        }
        if (pPlayer->GetTalentTabID() == PaladinHoly)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, ELIXIR_OF_THE_SAGES);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, SWEET_SURPRISE);
        }
        break;
    case CLASS_ROGUE:
        ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
        ApplyAura(pPlayer, FURY_OF_THE_BOGLING);

        //Agility 
        //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
        ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
        //Attack Power 
        ApplyAura(pPlayer, WINTERFALL_FIREWATER);
        ApplyAura(pPlayer, JUJU_MIGHT);
        //Strength
        //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
        //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
        ApplyAura(pPlayer, JUJU_POWER);
        //Blasted Lands buffs
        ApplyAura(pPlayer, ROIDS);
        ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
        //Food
        ApplyAura(pPlayer, BLESSED_SUNFRUIT);
        ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
        ApplyAura(pPlayer, GRILLED_SQUID);
        ApplyAura(pPlayer, DARK_DESIRE);
        break;
    case CLASS_MAGE:
        ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
        ApplyAura(pPlayer, FLASK_OF_SUPREME_POWER);
        ApplyAura(pPlayer, GREATER_ARCANE_ELIXIR);
        ApplyAura(pPlayer, MAGEBLOOD_POTION);
        ApplyAura(pPlayer, CRYSTAL_FORCE);
        ApplyAura(pPlayer, HEADMASTERS_CHARGE);
        ApplyAura(pPlayer, BLESSING_OF_BLACKFATHOM);
        ApplyAura(pPlayer, CEREBRAL_CORTEX_COMPOUND);
        ApplyAura(pPlayer, RUNN_TUM_TUBER_SURPRISE);
        ApplyAura(pPlayer, VERY_BERRY_CREAM);
        if (pPlayer->GetTalentTabID() == MageArcane)
            ApplyAura(pPlayer, ELIXIR_OF_FROST_POWER); //there is no arcane spell power+ pot, so we use frost...
        if (pPlayer->GetTalentTabID() == MageFrost)
            ApplyAura(pPlayer, ELIXIR_OF_FROST_POWER);
        if (pPlayer->GetTalentTabID() == MageFire)
            ApplyAura(pPlayer, ELIXIR_OF_GREATER_FIREPOWER);
        break;
    case CLASS_SHAMAN:
        if (pPlayer->GetTalentTabID() == ShamanElementalCombat)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FLASK_OF_SUPREME_POWER);
            ApplyAura(pPlayer, GREATER_ARCANE_ELIXIR);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, HEADMASTERS_CHARGE);
            ApplyAura(pPlayer, BLESSING_OF_BLACKFATHOM);
            ApplyAura(pPlayer, CEREBRAL_CORTEX_COMPOUND);
            ApplyAura(pPlayer, RUNN_TUM_TUBER_SURPRISE);
            ApplyAura(pPlayer, ELIXIR_OF_GREATER_FIREPOWER);
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
        }
        if (pPlayer->GetTalentTabID() == ShamanEnhancement)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, FURY_OF_THE_BOGLING);
            //ApplyAura(pPlayer, MAGEBLOOD_POTION);
            //Agility 
            //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
            ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
            //Attack Power 
            ApplyAura(pPlayer, WINTERFALL_FIREWATER);
            ApplyAura(pPlayer, JUJU_MIGHT);
            //Blasted Lands buffs
            ApplyAura(pPlayer, ROIDS);
            ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
            //Strength
            //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
            //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
            ApplyAura(pPlayer, JUJU_POWER);
            // Spellpower
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
            //Food
            ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
            break;
        }
        if (pPlayer->GetTalentTabID() == ShamanRestoration)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, ELIXIR_OF_THE_SAGES);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, SWEET_SURPRISE);
        }
        break;
    case CLASS_HUNTER:
        ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
        ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
        //ApplyAura(pPlayer, FURY_OF_THE_BOGLING);
        //Agility 
        //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
        ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
        //Attack Power 
        ApplyAura(pPlayer, WINTERFALL_FIREWATER);
        ApplyAura(pPlayer, JUJU_MIGHT);
        //Blasted Lands buffs
        ApplyAura(pPlayer, ROIDS);
        ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
        //Strength
        //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
        //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
        ApplyAura(pPlayer, JUJU_POWER);
        //Food
        ApplyAura(pPlayer, BLESSED_SUNFRUIT);
        ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
        ApplyAura(pPlayer, GRILLED_SQUID);
        ApplyAura(pPlayer, DARK_DESIRE);
        ApplyAura(pPlayer, SWIFTNESS_OF_ZANZA);
        break;
    case CLASS_DRUID:
        if (pPlayer->GetTalentTabID() == DruidBalance)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
            ApplyAura(pPlayer, FLASK_OF_SUPREME_POWER);
            ApplyAura(pPlayer, GREATER_ARCANE_ELIXIR);
            ApplyAura(pPlayer, ELIXIR_OF_SHADOW_POWER);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, VERY_BERRY_CREAM);
        }
        if (pPlayer->GetTalentTabID() == DruidFeralCombat)
        {
            if (pPlayer->HasSpell(THICK_HIDE))
            {
                ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_STAMINA);
                ApplyAura(pPlayer, MOLDARS_MOXIE_DIRE_MAUL);
                ApplyAura(pPlayer, PRAYER_OF_FORTITUDE);
                ApplyAura(pPlayer, FLASK_OF_THE_TITANS);
                ApplyAura(pPlayer, ELIXIR_OF_FORTITUDE);
                ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
                ApplyAura(pPlayer, ELIXIR_OF_SUPERIOR_DEFENSE);
                //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
                ApplyAura(pPlayer, BLOOD_PACT);
                ApplyAura(pPlayer, GREATER_STONESHIELD_POTION);
                ApplyAura(pPlayer, CRYSTAL_WARD);
                ApplyAura(pPlayer, ROIDS);
                ApplyAura(pPlayer, BLOODKELP_ELIXIR_OF_DODGING);
                ApplyAura(pPlayer, RUMSEY_RUM_BLACK_LABEL);
                ApplyAura(pPlayer, DARK_DESIRE);
                if (pPlayer->GetTeam() == ALLIANCE)
                    ApplyAura(pPlayer, DEVOTION_AURA);
            }
            else
            {
                ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
                ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
                ApplyAura(pPlayer, FURY_OF_THE_BOGLING);
                ApplyAura(pPlayer, MAGEBLOOD_POTION);
                //Agility 
                //ApplyAura(pPlayer, ELIXIR_OF_GREATER_AGILITY);
                ApplyAura(pPlayer, ELIXIR_OF_THE_MONGOOSE);
                //Attack Power 
                ApplyAura(pPlayer, WINTERFALL_FIREWATER);
                ApplyAura(pPlayer, JUJU_MIGHT);
                //Blasted Lands buffs
                ApplyAura(pPlayer, ROIDS);
                ApplyAura(pPlayer, GROUND_SCORPOK_ASSAY);
                //Strength
                //ApplyAura(pPlayer, ELIXIR_OF_BRUTE_FORCE);
                //ApplyAura(pPlayer, ELIXIR_OF_GIANTS);
                ApplyAura(pPlayer, JUJU_POWER);
                //Food
                ApplyAura(pPlayer, BLESSED_SUNFRUIT);
                ApplyAura(pPlayer, SMOKED_DESERT_DUMPLINGS);
                ApplyAura(pPlayer, GRILLED_SQUID);
                ApplyAura(pPlayer, DARK_DESIRE);
                if (pPlayer->GetTeam() == ALLIANCE)
                {
                    ApplyAura(pPlayer, DEVOTION_AURA);
                }
            }
        }
        if (pPlayer->GetTalentTabID() == DruidRestoration)
        {
            ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            ApplyAura(pPlayer, FLASK_OF_DISTILLED_WISDOM);
            ApplyAura(pPlayer, ELIXIR_OF_THE_SAGES);
            ApplyAura(pPlayer, MAGEBLOOD_POTION);
            ApplyAura(pPlayer, NIGHTFIN_SOUP);
            ApplyAura(pPlayer, CRYSTAL_FORCE);
            ApplyAura(pPlayer, SWEET_SURPRISE);
        }
        break;
    case CLASS_WARLOCK:
        ApplyAura(pPlayer, SAYGES_DARK_FORTUNE_OF_DAMAGE);
        ApplyAura(pPlayer, FLASK_OF_SUPREME_POWER);
        ApplyAura(pPlayer, GREATER_ARCANE_ELIXIR);
        ApplyAura(pPlayer, ELIXIR_OF_SHADOW_POWER);
        ApplyAura(pPlayer, MAGEBLOOD_POTION);
        ApplyAura(pPlayer, CRYSTAL_FORCE);
        ApplyAura(pPlayer, HEADMASTERS_CHARGE);
        ApplyAura(pPlayer, BLESSING_OF_BLACKFATHOM);
        ApplyAura(pPlayer, RUNN_TUM_TUBER_SURPRISE);
        ApplyAura(pPlayer, VERY_BERRY_CREAM);
        break;
    default:
        break;
    }
}

struct npc_winter_wondervoltAI : public ScriptedAI
{
    npc_winter_wondervoltAI(Creature* pCreature) : ScriptedAI(pCreature) {
        leaved_duel_arena_timer = 0;
        Reset();
    }

    enum Spells
    {
        HOLY_WRATH = 23979,
        HOLY_FIRE = 18165,
        EXPLODING_HEART = 27676,
        SHADOW_WRATH = 27286,
        SPELL_BANISHEMENT_OF_SCALE = 16404,
        SPELL_NEFARIUS_CORRUPTION = 23642,
        SPIRIT_CHANNELING = 22011,
        WARLOCK_CHANNELING = 8675,
        CORRUPTION = 23642,
        FLAMESHOCKER_IMMOLATE_VISUAL = 28330,
    };

    enum NPC
    {
        TIRION_FORDRING = 600070,
        EITRIGG = 600071,
        NEFARIAN = 600075,
    };

    uint64 killedUnits = 0;
    uint64 killedUnitsStep = 0;
    uint32 m_throwTimer;
    uint32 m_windfuryTimer;
    uint32 m_WonderVoltTimer;
    uint32 m_KnockTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiCombatTimer;
    uint32 m_HolySoundTimer;

    bool ShotReady;
    bool windfuryReady;
    bool WonderVoltReady;

    void Reset()
    {
        //m_creature->HasInvisibilityAura();
        m_throwTimer = 0;
        ShotReady = true;
        m_windfuryTimer = 0;
        m_uiConflagrationTimer = 5000;
        m_uiCombatTimer = 5000;
        windfuryReady = true;
        m_creature->IsInRoots();
        SetCombatMovement(false);
        m_creature->CombatStop(true);

        switch (m_creature->GetEntry())
        {
            case TIRION_FORDRING:
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetUInt32Value(UNIT_CHANNEL_SPELL, SPIRIT_CHANNELING);
                m_creature->SetChannelObjectGuid(m_creature->GetObjectGuid());
                return;
            }
            case NEFARIAN:
            {
                //m_creature->AddAura(FLAMESHOCKER_IMMOLATE_VISUAL);
                m_creature->SetUInt32Value(UNIT_CHANNEL_SPELL, WARLOCK_CHANNELING);
                m_creature->SetChannelObjectGuid(m_creature->GetObjectGuid());
                return;
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        switch (m_creature->GetEntry())
        {
        case TIRION_FORDRING:
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            break;
        }
    }

    uint32 leaved_duel_arena_timer;

    void UpdateAI(const uint32 uiDiff) override
    {
        switch (m_creature->GetEntry())
        {
        case TIRION_FORDRING:
            // Conflagration
            if (m_uiConflagrationTimer < uiDiff)
            {
                if (m_creature->GetVictim())
                {
                    switch (urand(0, 20))
                    {
                    case 0:
                        DoScriptText(6194, m_creature);
                        //m_creature->PlayDistanceSound(5830);
                        break;
                    case 1:
                        DoScriptText(6195, m_creature);
                        //m_creature->PlayDistanceSound(5833);
                        break;
                    case 2:
                        DoScriptText(6196, m_creature);
                        //m_creature->PlayDistanceSound(5831);
                        break;
                    case 3:
                        DoScriptText(6534, m_creature);
                        //m_creature->PlayDistanceSound(5832);
                        break;
                    default:
                        break;
                    }
                    m_uiConflagrationTimer = urand(30000, 50000);
                }
            }
            else
                m_uiConflagrationTimer -= uiDiff;

            // combat reseter
            if (m_uiCombatTimer < uiDiff)
            {
                if (!m_creature->GetVictim())
                {
                    Reset();
                    m_uiCombatTimer = 5000;
                }
            }
            else
                m_uiCombatTimer -= uiDiff;

            return;
        case 90007:
            if (leaved_duel_arena_timer < uiDiff)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap)
                    return;

                Map::PlayerList const& PlayerList = pMap->GetPlayers();

                for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                {
                    Player* pPlayer = itr->getSource();

                    if (!pPlayer || m_creature->GetDistance(pPlayer) < 49.5f || m_creature->GetDistance(pPlayer) > 200)
                        continue;

                        std::ostringstream ss;
                        ss << "Don't run away!  " << pPlayer->GetName() << "!";

                        m_creature->MonsterYell(ss.str().c_str());
                        pPlayer->ToPlayer()->TeleportTo(0, 1903.0f, 2548.0f, 131.252f, 4.68586f);
                        pPlayer->DuelComplete(DUEL_FLED);;
                        pPlayer->CastSpell(pPlayer, SPAWN_RED_LIGHTNING, true);
                        //pPlayer->AddAura(STUN_8S, ADD_AURA_POSITIVE, pPlayer);
                        pPlayer->CastSpell(pPlayer, STUN_8S, true);
                }

                leaved_duel_arena_timer = 1000;
            }
            else
                leaved_duel_arena_timer -= uiDiff;
            return;
        }
    }

    void DamageTaken(Unit* pUnit, uint32& damage) override
    {
        damage = 0;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (!pWho)
            return;

        if (m_creature->IsInEvadeMode())
            Reset();

        if (pWho->IsCreature())
            if (pWho->ToCreature()->IsInEvadeMode())
                Reset();

        switch (m_creature->GetEntry())
        {
        case TIRION_FORDRING:
            if (m_creature->IsValidAttackTarget(pWho) && m_creature->IsWithinDistInMap(pWho, 300.0f))
            {
                if (m_creature->IsWithinDistInMap(pWho, 65.0f))
                    m_creature->CastSpell(pWho, HOLY_WRATH, true);

                if (m_creature->IsWithinDistInMap(pWho, 25.0f))
                {
                    m_creature->CastSpell(pWho, HOLY_FIRE, true);
                    m_creature->SetPowerPercent(POWER_MANA, 100.0f);
                }
                if (m_creature->IsWithinDistInMap(pWho, 10.0f))
                    pWho->CastSpell(pWho, EXPLODING_HEART, true);
                break;
            }
        case NEFARIAN:
            if (m_creature->IsValidAttackTarget(pWho) && m_creature->IsWithinDistInMap(pWho, 300.0f))
            {
                if (m_creature->IsWithinDistInMap(pWho, 65.0f))
                    m_creature->CastSpell(pWho, SHADOW_WRATH, true);

                if (m_creature->IsWithinDistInMap(pWho, 25.0f))
                {
                    m_creature->CastSpell(pWho, HOLY_FIRE, true);
                    m_creature->SetPowerPercent(POWER_MANA, 100.0f);
                }
                if (m_creature->IsWithinDistInMap(pWho, 10.0f))
                    pWho->CastSpell(pWho, EXPLODING_HEART, true);
                break;
            }
        }

        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        switch (m_creature->GetEntry())
        {
        case 80000:
            if (m_creature->IsWithinDistInMap(pWho, 0.2f))
            {
                if(pWho->IsInCombat())
                {
                    pWho->CastSpell(pWho, KNOCKBACK, true);
                    m_creature->MonsterWhisper("ERROR CODE 404 (YOU ARE IN COMBAT).", pWho->ToPlayer());
                }
                else
                {
                    if (pWho->HasAura(AREA_BURN))
                        return;

                    pWho->ToPlayer()->DuelComplete(DUEL_INTERRUPTED);
                    pWho->ToPlayer()->RemoveAllSpellCooldown();
                    pWho->ToPlayer()->ResetCharges();
                    FullBuffed(pWho->ToPlayer());
                    pWho->CastSpell(pWho, SPAWN_RED_LIGHTNING, true);
                    pWho->ModifyHealth(pWho->GetMaxHealth());
                    if (pWho->GetPower(POWER_MANA) != pWho->GetMaxPower(POWER_MANA))
                        pWho->ModifyPower(POWER_MANA, pWho->GetMaxPower(POWER_MANA));
                    pWho->AddAura(AREA_BURN);
                }
            }
            return;
        case 80001:
            if (m_creature->IsWithinDistInMap(pWho, 1.2f))
            {
                pWho->CastSpell(pWho, 16807, true);
                pWho->ToPlayer()->TeleportTo(1, -11857.2f, -4747.97f, 6.61282f, 5.83314f);
            }
            return;
        case 80005:
            if (m_creature->IsWithinDistInMap(pWho, 3.0f))
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                {
                    //pWho->ToPlayer()->SetStandState(UNIT_STAND_STATE_STAND);
                    m_creature->CastSpell(pWho, KNOCKBACK, true);
                    pWho->CastSpell(pWho, EMBERSEER_TRANSFORM, true);
                }
            }
            if (m_creature->IsWithinDistInMap(pWho, 0.001f))
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                {
                    //float x, y, z;
                    //float add_z = 2.0f;
                    //pWho->ToPlayer()->GetRelativePositions(0.0f, 0.0f, add_z, x, y, z);
                    //pWho->ToPlayer()->NearLandTo(x, y, z, pWho->ToPlayer()->GetOrientation());
                    pWho->CastSpell(pWho, KNOCKBACK_35, true);
                    pWho->CastSpell(pWho, EMBERSEER_TRANSFORM, true);
                    m_creature->MonsterWhisper("Your are not allowed to sit down here.", pWho->ToPlayer());
                }
                else
                {
                    GameObject* go = m_creature->FindNearestGameObject(185578, 1.0f);
                    if (!go)
                    {
                        float x, y, z;
                        m_creature->GetRelativePositions(0.0f, 0.0f, 0.0f, x, y, z);
                        m_creature->SummonGameObject(185578, x, y, z, 0.0f);
                    }
                }
            }
            else
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
                {
                    GameObject* go = m_creature->FindNearestGameObject(185578, 1.0f);
                    if (go)
                    {
                        go->Delete();
                    }
                }
            }
            return;
        case 80006:
            if (m_creature->IsWithinDistInMap(pWho, 3.0f))
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() < SEC_GAMEMASTER && pWho->ToPlayer()->GetGuildId() == 1)
                {
                    //pWho->ToPlayer()->SetStandState(UNIT_STAND_STATE_STAND);
                    m_creature->CastSpell(pWho, KNOCKBACK, true);
                    pWho->CastSpell(pWho, EMBERSEER_TRANSFORM, true);
                }
            }
            if (m_creature->IsWithinDistInMap(pWho, 0.001f))
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() < SEC_GAMEMASTER && pWho->ToPlayer()->GetGuildId() == 1)
                {
                    pWho->CastSpell(pWho, KNOCKBACK_35, true);
                    pWho->CastSpell(pWho, EMBERSEER_TRANSFORM, true);
                    m_creature->MonsterWhisper("Your are not allowed to sit down here.", pWho->ToPlayer());
                }
                else
                {
                    GameObject* go = m_creature->FindNearestGameObject(185578, 1.0f);
                    if (!go)
                    {
                        float x, y, z;
                        m_creature->GetRelativePositions(0.0f, 0.0f, 0.0f, x, y, z);
                        m_creature->SummonGameObject(185578, x, y, z, 0.0f);
                    }
                }
            }
            else
            {
                if (pWho->ToPlayer()->GetSession()->GetSecurity() >= SEC_GAMEMASTER && pWho->ToPlayer()->GetGuildId() == 1)
                {
                    GameObject* go = m_creature->FindNearestGameObject(185578, 1.0f);
                    if (go)
                    {
                        go->Delete();
                    }
                }
            }
            return;
        }
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    uint32 RandNmbr = urand(20, 45);

    void KilledUnit(Unit* pWho)
    {
        if (!pWho)
            return;

        switch (m_creature->GetEntry())
        {
            case 600070:
            {
                killedUnits++;
                killedUnitsStep++;
                m_creature->ClearInCombat();
                if (killedUnitsStep == RandNmbr)
                    killedUnitsStep = 0;

                if (Creature* Eitrigg = m_creature->FindNearestCreature(600071, 30.0f))
                {
                    if (killedUnitsStep == RandNmbr - 1)
                    {
                        std::ostringstream ss;
                        ss << killedUnits << "!";
                        Eitrigg->MonsterSay(ss.str().c_str(), 0, 0);
                        Eitrigg->HandleEmote(EMOTE_ONESHOT_APPLAUD);

                    }
                    if (killedUnits > 100000)
                    {
                        killedUnits = 0;
                        killedUnitsStep = 0;
                        Eitrigg->MonsterSay("Hmmm... How far were we? I just start again", 0, 0);
                    }
                }
                return;
            }
        }
    }

};

bool GOOpen_server_owners_throne(Player* pUser, GameObject* pGo)
{
    if (!pUser || !pGo)
        return false;

    switch (pGo->GetEntry())
    {
    case 900065:
        if (pUser->ToPlayer()->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
        {
            pUser->CastSpell(pUser, KNOCKBACK, true);
            pUser->CastSpell(pUser, EMBERSEER_TRANSFORM, true);
            return true;
        }
        else
        {
            return false;
        }
        break;
    case 900066:
        if (pUser->ToPlayer()->GetSession()->GetSecurity() < SEC_GAMEMASTER && !pUser->ToPlayer()->GetGuildId() == 1)
        {
            pUser->CastSpell(pUser, KNOCKBACK, true);
            pUser->CastSpell(pUser, EMBERSEER_TRANSFORM, true);
            return true;
        }
        else
        {
            return false;
        }
        break;
    }
    return true;
}

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL	= 22011,	// Spirit Heal Channel
    RESURRECTION_IMPACT_VISUAL	= 24171,	// Spirit Heal
    SPELL_WAITING_TO_RESURRECT	= 2584,		// players pWho cancel this aura don't want a resurrection
    TEST_NPC_RESURRECTION		= 21074
};

struct npc_AoE_RezzAI : public ScriptedAI
{
    npc_AoE_RezzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        uiTimerRez = 0;

        npc_AoE_RezzAI::Reset();
    }

    void Reset() override
    {
    }

    uint32 uiTimerRez;

    void UpdateAI(const uint32 uiDiff) override
    {
        if (uiTimerRez < uiDiff)
        {
            Map* pMap = m_creature->GetMap();

            if (!pMap)
                return;

            Map::PlayerList const& PlayerList = pMap->GetPlayers();

            for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            {
                Player* pPlayer = itr->getSource();

                if (!pPlayer || !pPlayer->IsWithinDistInMap(m_creature, 20.0f) || pPlayer->IsAlive())
                    continue;

                // repop player again - now this node won't be counted and another node is searched
                //pPlayer->ResurrectPlayer(1.0, false);
                m_creature->CastSpell(pPlayer, TEST_NPC_RESURRECTION, true);
            }

            m_creature->InterruptNonMeleeSpells(true);
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, false);
            uiTimerRez = 30000;
        }
        else
            uiTimerRez -= uiDiff;
    }

    void DamageTaken(Unit* /*pFrom*/, uint32& damage) override
    {
        damage = 0;
    }
};

CreatureAI* GetAI_npc_winter_wondervolt(Creature* pCreature)
{
    return new npc_winter_wondervoltAI(pCreature);
}

CreatureAI* GetAI_npc_AoE_RezzAI(Creature* pCreature)
{
    return new npc_AoE_RezzAI(pCreature);
}

bool GossipHello_npc_AoE_RezzAI(Player* pPlayer, Creature* pCreature)
{
    pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);
    return true;
}

void AddSC_npc_winter_wondervolt()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "npc_winter_wondervolt";
    pNewScript->GetAI = &GetAI_npc_winter_wondervolt;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_server_owners_throne";
    pNewScript->pGOHello = &GOOpen_server_owners_throne;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_AoE_RezzAI";
    pNewScript->GetAI = &GetAI_npc_AoE_RezzAI;
    pNewScript->RegisterSelf();
}
