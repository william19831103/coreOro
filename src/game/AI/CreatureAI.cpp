/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "CreatureAI.h"
#include "Spell.h"
#include "Creature.h"
#include "DBCStores.h"
#include "Totem.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "Group.h"

CreatureAI::CreatureAI(Creature* creature) : m_creature(creature), m_bUseAiAtControl(false), m_bMeleeAttack(true), m_bCombatMovement(true), m_uiCastingDelay(0), m_uLastAlertTime(0)
{
    SetSpellsList(creature->GetCreatureInfo()->spell_list_id);
}

CreatureAI::~CreatureAI()
{
}

void CreatureAI::JustRespawned()
{
    // Reset spells template to default on respawn.
    SetSpellsList(m_creature->GetCreatureInfo()->spell_list_id);

    // Reset combat movement and melee attack.
    m_bCombatMovement = true;
    m_bMeleeAttack = true;
}

void CreatureAI::AttackedBy(Unit* attacker)
{
    if (!m_creature->GetVictim())
        AttackStart(attacker);
}

void CreatureAI::AttackStart(Unit* pVictim)
{
    if (m_creature->HasReactState(REACT_PASSIVE))
        return;

    if (m_creature->Attack(pVictim, m_bMeleeAttack))
    {
        m_creature->AddThreat(pVictim);
        m_creature->SetInCombatWith(pVictim);
        pVictim->SetInCombatWith(m_creature);

        if (m_bCombatMovement)
            m_creature->GetMotionMaster()->MoveChase(pVictim);
    }
}

SpellCastResult CreatureAI::DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags)
{
    return m_creature->TryToCast(pTarget, uiSpell, uiCastFlags, 100);
}

void CreatureAI::SetSpellsList(uint32 entry)
{
    if (entry == 0)
        m_CreatureSpells.clear();
    else if (CreatureSpellsList const* pSpellsTemplate = sObjectMgr.GetCreatureSpellsList(entry))
        SetSpellsList(pSpellsTemplate);
    else
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "CreatureAI: Attempt to set spells template of creature %u to non-existent entry %u.", m_creature->GetEntry(), entry);
}

void CreatureAI::SetSpellsList(CreatureSpellsList const* pSpellsList)
{
    m_CreatureSpells.clear();
    for (const auto& entry : *pSpellsList)
    {
        m_CreatureSpells.push_back(CreatureAISpellsEntry(entry));
    }
    m_CreatureSpells.shrink_to_fit();
    m_uiCastingDelay = 0;
}

// Creature spell lists should be updated every 1.2 seconds according to research.
// https://www.reddit.com/r/wowservers/comments/834nt5/felmyst_ai_system_research/
#define CREATURE_CASTING_DELAY 1200

void CreatureAI::UpdateSpellsList(uint32 const uiDiff)
{
    if (m_uiCastingDelay <= uiDiff)
    {
        uint32 const uiDesync = (uiDiff - m_uiCastingDelay);
        DoSpellsListCasts(CREATURE_CASTING_DELAY + uiDesync);
        m_uiCastingDelay = uiDesync < CREATURE_CASTING_DELAY ? CREATURE_CASTING_DELAY - uiDesync : 0;
    }
    else
        m_uiCastingDelay -= uiDiff;
}

void CreatureAI::DoSpellsListCasts(uint32 const uiDiff)
{
    bool bDontCast = false;
    for (auto& spell : m_CreatureSpells)
    {
        if (spell.cooldown <= uiDiff)
        {
            // Cooldown has expired.
            spell.cooldown = 0;

            // Prevent casting multiple spells in the same update. Only update timers.
            if (!(spell.castFlags & (CF_TRIGGERED | CF_INTERRUPT_PREVIOUS)))
            {
                if (bDontCast || m_creature->IsNonMeleeSpellCasted(false))
                    continue;
            } 

            // Checked on startup.
            SpellEntry const* pSpellInfo = sSpellMgr.GetSpellEntry(spell.spellId);

            Unit* pTarget = ToUnit(GetTargetByType(m_creature, m_creature, m_creature->GetMap(), spell.castTarget, spell.targetParam1, spell.targetParam2, pSpellInfo));

            SpellCastResult result = m_creature->TryToCast(pTarget, pSpellInfo, spell.castFlags, spell.probability);
            
            switch (result)
            {
                case SPELL_CAST_OK:
                {
                    bDontCast = !(spell.castFlags & CF_TRIGGERED);
                    spell.cooldown = urand(spell.delayRepeatMin, spell.delayRepeatMax);

                    if (spell.castFlags & CF_MAIN_RANGED_SPELL)
                    {
                        if (m_creature->IsMoving())
                            m_creature->StopMoving();

                        SetCombatMovement(false);
                        SetMeleeAttack(false);
                    }

                    // If there is a script for this spell, run it.
                    if (spell.scriptId)
                        m_creature->GetMap()->ScriptsStart(sCreatureSpellScripts, spell.scriptId, m_creature->GetObjectGuid(), pTarget->GetObjectGuid());
                    break;
                }
                case SPELL_FAILED_FLEEING:
                case SPELL_FAILED_SPELL_IN_PROGRESS:
                {
                    // Do nothing so it will try again on next update.
                    break;
                }
                case SPELL_FAILED_TRY_AGAIN:
                {
                    // Chance roll failed, so we reset cooldown.
                    spell.cooldown = urand(spell.delayRepeatMin, spell.delayRepeatMax);
                    if (spell.castFlags & CF_MAIN_RANGED_SPELL)
                    {
                        SetCombatMovement(true);
                        SetMeleeAttack(true);
                    }
                    break;
                }
                default:
                {
                    // other error
                    if (spell.castFlags & CF_MAIN_RANGED_SPELL)
                    {
                        SetCombatMovement(true);
                        SetMeleeAttack(true);
                    }
                    break;
                }
            }
        }
        else
            spell.cooldown -= uiDiff;
    }
}

void CreatureAI::ClearTargetIcon()
// Clears any group/raid icons this creature may have
{
    Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();

    if (players.isEmpty())
        return;

    std::set<Group*> instanceGroups;

    // Clear target icon for every unique group in instance
    for (const auto& player : players)
    {
        if (Group* pGroup = player.getSource()->GetGroup())
        {
            if (instanceGroups.find(pGroup) == instanceGroups.end())
            {
                instanceGroups.insert(pGroup);
                pGroup->ClearTargetIcon(m_creature->GetObjectGuid());
            }
        }
    }
}

void CreatureAI::DoCast(Unit* victim, uint32 spellId, bool triggered)
{
    if (!victim || (m_creature->IsNonMeleeSpellCasted(false) && !triggered))
        return;

    m_creature->CastSpell(victim, spellId, triggered);
}

void CreatureAI::DoCastAOE(uint32 spellId, bool triggered)
{
    if (!triggered && m_creature->IsNonMeleeSpellCasted(false))
        return;

    m_creature->CastSpell((Unit*)nullptr, spellId, triggered);
}

bool CreatureAI::DoMeleeAttackIfReady()
{
    return m_bMeleeAttack ? m_creature->UpdateMeleeAttackingState() : false;
}

void CreatureAI::SetMeleeAttack(bool enabled)
{
    if (m_bMeleeAttack == enabled)
        return;

    m_bMeleeAttack = enabled;

    if (Unit* pVictim = m_creature->GetVictim())
    { 
        if (enabled)
        {
            m_creature->AddUnitState(UNIT_STAT_MELEE_ATTACKING);
            m_creature->SendMeleeAttackStart(pVictim);
        } 
        else
        {
            m_creature->ClearUnitState(UNIT_STAT_MELEE_ATTACKING);
            m_creature->SendMeleeAttackStop(pVictim);
        }
    }
}

void CreatureAI::SetCombatMovement(bool enabled)
{
    if (m_bCombatMovement == enabled)
        return;

    m_bCombatMovement = enabled;

    if (Unit* pVictim = m_creature->GetVictim())
    {
        if (!enabled && (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE))
        {
            m_creature->GetMotionMaster()->MovementExpired(false);
            m_creature->GetMotionMaster()->MoveIdle();
        }
        else if (enabled && (m_creature->GetMotionMaster()->IsUsingIdleOrDefaultMovement()))
        {
            m_creature->GetMotionMaster()->MovementExpired(false);
            m_creature->GetMotionMaster()->MoveChase(pVictim);
        }  
    }
}

struct EnterEvadeModeHelper
{
    explicit EnterEvadeModeHelper(Unit* _source) : source(_source) {}
    void operator()(Unit* unit) const
    {
        if (unit->IsCreature() && unit->ToCreature()->IsTotem())
            ((Totem*)unit)->UnSummon();
        else if (unit->IsAlive())
        {
            unit->GetMotionMaster()->Clear(false);
            // for a controlled unit this will result in a follow move
            unit->GetMotionMaster()->MoveTargetedHome();
            unit->DeleteThreatList();
            unit->CombatStop(true);
        }
    }
    Unit* source;
};

void CreatureAI::OnCombatStop()
{
    // Reset back to default spells template. This also resets timers.
    SetSpellsList(m_creature->GetCreatureInfo()->spell_list_id);

    // Reset combat movement and melee attack.
    m_bCombatMovement = true;
    m_bMeleeAttack = true;
}

void CreatureAI::EnterEvadeMode()
{
    if (!m_creature->IsAlive())
    {
        m_creature->CombatStop(true);
        m_creature->DeleteThreatList();
        return;
    }

    if (!m_creature->IsCharmed())
    {
        m_creature->RemoveAurasAtReset();

        // clear all movement generators except default
        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    m_creature->ClearComboPointHolders();
    m_creature->DeleteThreatList();
    m_creature->CombatStop(true);
    m_creature->SetLootRecipient(nullptr);
    m_creature->CallForAllControlledUnits(EnterEvadeModeHelper(m_creature), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_TOTEMS);
}

// Distract creature, if player gets too close while stealthed/prowling
void CreatureAI::OnMoveInStealth(Unit* who)
{
    if (CanTriggerAlert(who))
        TriggerAlertDirect(who);
}

bool CreatureAI::CanTriggerAlert(Unit const* who)
{
    // If this unit isn't an NPC, is already distracted, is in combat, is confused, stunned or fleeing, do nothing
    if (m_creature->GetTypeId() != TYPEID_UNIT || m_creature->IsInCombat() || m_creature->HasUnitState(UNIT_STAT_NO_FREE_MOVE))
        return false;

    // Only alert for hostiles!
    if (m_creature->IsCivilian() || m_creature->HasReactState(REACT_PASSIVE) || !m_creature->IsValidAttackTarget(who))
        return false;

    // 10 sec cooldown for stealth warning
    if (WorldTimer::getMSTimeDiffToNow(m_uLastAlertTime) < 10000)
        return false;

    // only alert if target is within line of sight
    if (!m_creature->IsWithinLOSInMap(who))
        return false;

    return true;
}

void CreatureAI::TriggerAlertDirect(Unit const* who)
{
    // Send alert sound (if any) for this creature
    m_creature->SendAIReaction(AI_REACTION_ALERT);

    // Face the unit (stealthed player) and set distracted state for 5 seconds
    m_creature->StopMoving();
    m_creature->SetFacingTo(m_creature->GetAngle(who->GetPositionX(), who->GetPositionY()));
    m_creature->GetMotionMaster()->MoveDistract(5 * IN_MILLISECONDS);
    m_uLastAlertTime = WorldTimer::getMSTime();
}
