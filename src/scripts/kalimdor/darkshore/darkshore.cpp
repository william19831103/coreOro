/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Darkshore
SD%Complete: 100
SDComment: Quest support: 731, 2078, 5321, 4740
SDCategory: Darkshore
EndScriptData */

/* ContentData
npc_kerlonian
npc_prospector_remtravel
npc_threshwackonator
npc_chasseur_grisebrume
EndContentData */

#include "scriptPCH.h"
#include "WaypointManager.h"

/*####
# npc_kerlonian
####*/

enum KerlonianData
{
    SAY_KER_START     = 6540,
    EMOTE_KER_SLEEP_1 = 6811,
    EMOTE_KER_SLEEP_2 = 6542,
    EMOTE_KER_SLEEP_3 = 6541,
    SAY_KER_SLEEP_1   = 6813,
    SAY_KER_SLEEP_2   = 6543,
    SAY_KER_SLEEP_3   = 6544,
    SAY_KER_SLEEP_4   = 6545,
    EMOTE_KER_AWAKEN  = 6612,
    SAY_KER_ALERT_1   = 6867,
    SAY_KER_ALERT_2   = 6868,
    SAY_KER_END       = 6643,

    SPELL_SLEEP_VISUAL = 25148,
    SPELL_AWAKEN       = 17536,

    QUEST_SLEEPER_AWAKENED = 5321,

    NPC_LILADRIS = 11219 // attackers entries unknown
};

//TODO: make concept similar as "ringo" -escort. Find a way to run the scripted attacks, _if_ player are choosing road.
struct npc_kerlonianAI : public FollowerAI
{
    npc_kerlonianAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiFallAsleepTimer;

    void Reset() override
    {
        m_uiFallAsleepTimer = urand(10000, 45000);
    }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        FollowerAI::JustRespawned();
    }

    void MoveInLineOfSight(Unit *pWho) override
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_LILADRIS)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE * 5))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_SLEEPER_AWAKENED) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_SLEEPER_AWAKENED, m_creature);

                    DoScriptText(SAY_KER_END, m_creature);
                }

                SetFollowComplete();
            }
        }
    }

    void SpellHit(SpellCaster*, SpellEntry const* pSpell) override
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_AWAKEN)
            ClearSleeping();
    }

    void SetSleeping()
    {
        SetFollowPaused(true);

        switch (urand(0, 2))
        {
            case 0:
                DoScriptText(EMOTE_KER_SLEEP_1, m_creature);
                break;
            case 1:
                DoScriptText(EMOTE_KER_SLEEP_2, m_creature);
                break;
            case 2:
                DoScriptText(EMOTE_KER_SLEEP_3, m_creature);
                break;
        }

        switch (urand(0, 3))
        {
            case 0:
                DoScriptText(SAY_KER_SLEEP_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_KER_SLEEP_2, m_creature);
                break;
            case 2:
                DoScriptText(SAY_KER_SLEEP_3, m_creature);
                break;
            case 3:
                DoScriptText(SAY_KER_SLEEP_4, m_creature);
                break;
        }

        m_creature->SetStandState(UNIT_STAND_STATE_SLEEP);
        m_creature->CastSpell(m_creature, SPELL_SLEEP_VISUAL, false);
    }

    void ClearSleeping()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SLEEP_VISUAL);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        DoScriptText(EMOTE_KER_AWAKEN, m_creature);

        SetFollowPaused(false);
    }

    void UpdateFollowerAI(uint32 const uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
        {
            if (!HasFollowState(STATE_FOLLOW_INPROGRESS))
                return;

            if (!HasFollowState(STATE_FOLLOW_PAUSED))
            {
                if (m_uiFallAsleepTimer < uiDiff)
                {
                    SetSleeping();
                    m_uiFallAsleepTimer = urand(25000, 90000);
                }
                else
                    m_uiFallAsleepTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kerlonian(Creature* pCreature)
{
    return new npc_kerlonianAI(pCreature);
}

bool QuestAccept_npc_kerlonian(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SLEEPER_AWAKENED)
    {
        if (npc_kerlonianAI* pKerlonianAI = dynamic_cast<npc_kerlonianAI*>(pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            DoScriptText(SAY_KER_START, pCreature, pPlayer);
            pKerlonianAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
        }
    }

    return true;
}

/*####
# npc_prospector_remtravel
####*/

enum ProspectorRemtravelData
{
    SAY_REM_START         = 925,
    SAY_REM_AGGRO         = 941,
    SAY_REM_RAMP1_1       = 926,
    SAY_REM_RAMP1_2       = 927,
    SAY_REM_BOOK          = 928,
    SAY_REM_TENT1_1       = 929,
    SAY_REM_TENT1_2       = 930,
    SAY_REM_MOSS          = 931,
    EMOTE_REM_MOSS        = 932,
    SAY_REM_MOSS_PROGRESS = 933,
    SAY_REM_PROGRESS      = 935,
    SAY_REM_REMEMBER      = 936,
    EMOTE_REM_END         = 937,

    QUEST_ABSENT_MINDED_PT2 = 731,

    NPC_GRAVEL_SCOUT = 2158,
    NPC_GRAVEL_BONE  = 2159,
    NPC_GRAVEL_GEO   = 2160
};

struct npc_prospector_remtravelAI : public npc_escortAI
{
    npc_prospector_remtravelAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        npc_escortAI::JustRespawned();
    }

    void WaypointReached(uint32 i) override
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 5:
                DoScriptText(SAY_REM_RAMP1_1, m_creature, pPlayer);
                break;
            case 9:
                DoScriptText(SAY_REM_RAMP1_2, m_creature, pPlayer);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                break;
            case 10:
                m_creature->SummonCreature(NPC_GRAVEL_SCOUT, 4639.86f, 631.96f, 7.48f, 4.7f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 13:
                DoScriptText(SAY_REM_BOOK, m_creature, pPlayer);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                break;
            case 18:
                DoScriptText(SAY_REM_TENT1_1, m_creature, pPlayer);
                break;
            case 19:
                DoScriptText(SAY_REM_TENT1_2, m_creature, pPlayer);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                break;
            case 20:
                DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 30:
                DoScriptText(SAY_REM_MOSS, m_creature, pPlayer);
                DoScriptText(EMOTE_REM_MOSS, m_creature, pPlayer);
                break;
            case 31:
                DoScriptText(SAY_REM_MOSS_PROGRESS, m_creature, pPlayer);
                break;
            case 36:
                DoScriptText(SAY_REM_PROGRESS, m_creature, pPlayer);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                break;
            case 37:
                m_creature->SummonCreature(NPC_GRAVEL_BONE, 4564.11f, 553.67f, 5.21f, 2.26f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                m_creature->SummonCreature(NPC_GRAVEL_GEO, 4569.33f, 549.43f, 5.61f, 2.4f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 47:
                DoScriptText(SAY_REM_REMEMBER, m_creature, pPlayer);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                break;
            case 48:
                DoScriptText(EMOTE_REM_END, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_ABSENT_MINDED_PT2, m_creature);
                break;
        }
    }

    void Reset() override { }

    void Aggro(Unit* who) override
    {
        if (urand(0, 1))
            DoScriptText(SAY_REM_AGGRO, m_creature, who);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pSummoned->AI()->AttackStart(pPlayer);
    }
};

CreatureAI* GetAI_npc_prospector_remtravel(Creature* pCreature)
{
    return new npc_prospector_remtravelAI(pCreature);
}

bool QuestAccept_npc_prospector_remtravel(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ABSENT_MINDED_PT2)
    {
        pCreature->SetFactionTemplateId(FACTION_ESCORT_A_NEUTRAL_PASSIVE);

        if (npc_prospector_remtravelAI* pEscortAI = dynamic_cast<npc_prospector_remtravelAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_REM_START, pCreature, pPlayer);
            pCreature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest, true);
        }
    }

    return true;
}

/*####
# npc_threshwackonator
####*/

enum ThreshwackonatorData
{
    EMOTE_START        = 3012,
    SAY_AT_CLOSE       = 2704,
    QUEST_GYROMAST_REV = 2078,
    NPC_GELKAK         = 6667,
    FACTION_HOSTILE    = 14
};

#define GOSSIP_ITEM_INSERT_KEY "[PH] Insert key"

struct npc_threshwackonatorAI : public FollowerAI
{
    npc_threshwackonatorAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        Reset();
    }

    void Reset() override {}

    void MoveInLineOfSight(Unit* pWho) override
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_GELKAK)
        {
            if (m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_AT_CLOSE, pWho);
                DoAtEnd();
            }
        }
    }

    void DoAtEnd()
    {
        m_creature->SetFactionTemplateId(FACTION_HOSTILE);

        if (Player* pHolder = GetLeaderForFollower())
            m_creature->AI()->AttackStart(pHolder);

        SetFollowComplete();
    }
};

CreatureAI* GetAI_npc_threshwackonator(Creature* pCreature)
{
    return new npc_threshwackonatorAI(pCreature);
}

bool GossipHello_npc_threshwackonator(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_GYROMAST_REV) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INSERT_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_threshwackonator(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_threshwackonatorAI* pThreshAI = dynamic_cast<npc_threshwackonatorAI*>(pCreature->AI()))
        {
            DoScriptText(EMOTE_START, pCreature);
            pThreshAI->StartFollow(pPlayer);
        }
    }

    return true;
}

/*####
# npc_therylune
####*/

enum TheryluneData
{
    SAY_THERYLUNE_START  = 1189,
    SAY_THERYLUNE_FINISH = 1188,

    NPC_THERYSIL = 3585,

    QUEST_ID_THERYLUNE_ESCAPE = 945
};

struct npc_theryluneAI : public npc_escortAI
{
    npc_theryluneAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void Reset() override { }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        npc_escortAI::JustRespawned();
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 17:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ID_THERYLUNE_ESCAPE, m_creature);
                break;
            case 19:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_THERYLUNE_FINISH, m_creature, pPlayer);
                SetRun();
                break;
        }
    }
};

CreatureAI* GetAI_npc_therylune(Creature* pCreature)
{
    return new npc_theryluneAI(pCreature);
}

bool QuestAccept_npc_therylune(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_THERYLUNE_ESCAPE)
    {
        if (npc_theryluneAI* pEscortAI = dynamic_cast<npc_theryluneAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
            DoScriptText(SAY_THERYLUNE_START, pCreature, pPlayer);
            pCreature->SetFactionTemporary(79, TEMPFACTION_RESTORE_RESPAWN);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        }
    }

    return true;
}

/*######
# npc_volcor
######*/

enum VolcorData
{
    SAY_START        = 1237,
    SAY_END          = 1243,
    SAY_FIRST_AMBUSH = 1250,
    SAY_AGGRO_1      = 1251,
    SAY_AGGRO_2      = 1252,
    SAY_AGGRO_3      = 1253,
    SAY_END_2        = 1241,
    SAY_END_3        = 1244,
    SAY_ESCAPE       = 1236,

    NPC_BLACKWOOD_SHAMAN = 2171,
    NPC_BLACKWOOD_URSA   = 2170,
    NPC_GRIMCLAW         = 3695,

    SPELL_MOONSTALKER_FORM = 10849,

    WAYPOINT_ID_QUEST_STEALTH = 16,
    FACTION_FRIENDLY = 35,

    QUEST_ESCAPE_THROUGH_FORCE   = 994,
    QUEST_ESCAPE_THROUGH_STEALTH = 995
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ, m_fO;
};

// Spawn locations
static const SummonLocation aVolcorSpawnLocs[] =
{
    {4630.2f, 22.6f, 70.1f, 2.4f},
    {4603.8f, 53.5f, 70.4f, 5.4f},
    {4627.5f, 100.4f, 62.7f, 5.8f},
    {4692.8f, 75.8f, 56.7f, 3.1f},
    {4747.8f, 152.8f, 54.6f, 2.4f},
    {4711.7f, 109.1f, 53.5f, 2.4f},
};

// Escape Through Stealth
static SummonLocation aVolcorLocations[] =
{
    {4604.54f, -5.17f, 69.51f, 0.0f},
    {4604.26f, -2.02f, 69.42f, 0.0f },
    {4607.75f, 3.79f, 70.13f, 0.0f },
    {4607.75f, 3.79f, 70.13f, 0.0f },
    {4619.77f, 27.47f, 70.40f, 0.0f },
    {4640.33f, 33.74f, 68.22f, 0.0f }
};

struct npc_volcorAI : public npc_escortAI
{
    npc_volcorAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiQuestId;
    bool m_bEscortFinished = false;

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiQuestId = 0;
            StealthDialogueStep = 0;
            StealthDialogueTimer = 0;
        }
    }

    uint16 StealthDialogueStep;
    uint32 StealthDialogueTimer;
    uint16 ForceDialogueStep;
    uint32 ForceDialogueTimer;

    void Aggro(Unit* /*pWho*/) override
    {
        // shouldn't always use text on agro
        switch (urand(0, 4))
        {
            case 0:
                DoScriptText(SAY_AGGRO_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_AGGRO_2, m_creature);
                break;
            case 2:
                DoScriptText(SAY_AGGRO_3, m_creature);
                break;
        }
    }

    void JustRespawned() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetHomePosition(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);

        npc_escortAI::JustRespawned();
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        // No combat for this quest
        if (m_uiQuestId == QUEST_ESCAPE_THROUGH_STEALTH)
            return;

        npc_escortAI::MoveInLineOfSight(pWho);
    }

    void JustSummoned(Creature* pSummoned) override
    {
        pSummoned->AI()->AttackStart(m_creature);
    }
    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        npc_escortAI::MovementInform(uiMoveType, uiPointId);

        if (uiMoveType != POINT_MOTION_TYPE)
            return;
        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (m_uiQuestId == QUEST_ESCAPE_THROUGH_STEALTH && pPlayer->GetQuestStatus(QUEST_ESCAPE_THROUGH_STEALTH) == QUEST_STATUS_INCOMPLETE)
            {
                if (uiPointId < 5)
                    m_creature->GetMotionMaster()->MovePoint(uiPointId + 1, aVolcorLocations[uiPointId + 1].m_fX, aVolcorLocations[uiPointId + 1].m_fY, aVolcorLocations[uiPointId + 1].m_fZ, true);
                else if (uiPointId == 5)
                {
                    if (Player* pPlayer = GetPlayerForEscort())
                        pPlayer->GroupEventHappens(QUEST_ESCAPE_THROUGH_STEALTH, m_creature);
                    m_creature->DisappearAndDie();
                }
            }
        }
    }

    // Wrapper to handle start function for both quests
    void StartEscort(Player* pPlayer, Quest const* pQuest)
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetFacingToObject(pPlayer);
        m_uiQuestId = pQuest->GetQuestId();

        if (pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_STEALTH)
        {
            // Note: faction may not be correct, but only this way works fine
            m_creature->SetFactionTemporary(FACTION_FRIENDLY, TEMPFACTION_RESTORE_RESPAWN);

            Start(true, pPlayer->GetGUID(), pQuest);
            SetEscortPaused(true);
        }
        else
        {
            m_bEscortFinished = false;
            ForceDialogueStep = 0;
            ForceDialogueTimer = 0;
            Start(false, pPlayer->GetGUID(), pQuest);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        }
    }

    void WaypointReached(uint32 uiPointId) override
    {
        if (m_uiQuestId == QUEST_ESCAPE_THROUGH_STEALTH)
            return;

        switch (uiPointId)
        {
            case 2:
                DoScriptText(SAY_START, m_creature);
                break;
            case 5:
                m_creature->SummonCreature(NPC_BLACKWOOD_SHAMAN, aVolcorSpawnLocs[0].m_fX, aVolcorSpawnLocs[0].m_fY, aVolcorSpawnLocs[0].m_fZ, aVolcorSpawnLocs[0].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                m_creature->SummonCreature(NPC_BLACKWOOD_URSA, aVolcorSpawnLocs[1].m_fX, aVolcorSpawnLocs[1].m_fY, aVolcorSpawnLocs[1].m_fZ, aVolcorSpawnLocs[1].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                break;
            case 6:
                DoScriptText(SAY_FIRST_AMBUSH, m_creature);
                break;
            case 11:
                m_creature->SummonCreature(NPC_BLACKWOOD_SHAMAN, aVolcorSpawnLocs[2].m_fX, aVolcorSpawnLocs[2].m_fY, aVolcorSpawnLocs[2].m_fZ, aVolcorSpawnLocs[2].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                m_creature->SummonCreature(NPC_BLACKWOOD_URSA, aVolcorSpawnLocs[3].m_fX, aVolcorSpawnLocs[3].m_fY, aVolcorSpawnLocs[3].m_fZ, aVolcorSpawnLocs[3].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
            // no break
            case 13:
                m_creature->SummonCreature(NPC_BLACKWOOD_URSA, aVolcorSpawnLocs[4].m_fX, aVolcorSpawnLocs[4].m_fY, aVolcorSpawnLocs[4].m_fZ, aVolcorSpawnLocs[4].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                m_creature->SummonCreature(NPC_BLACKWOOD_URSA, aVolcorSpawnLocs[5].m_fX, aVolcorSpawnLocs[5].m_fY, aVolcorSpawnLocs[5].m_fZ, aVolcorSpawnLocs[5].m_fO, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                break;
            case 15:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPE_THROUGH_FORCE, m_creature);
                SetEscortPaused(true);
                ForceDialogueStep = 0;
                ForceDialogueTimer = 3000;
                m_bEscortFinished = true;
                break;
        }
    }

    void UpdateAI(uint32 const uiDiff) override
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (StealthDialogueStep < 4 && m_uiQuestId == QUEST_ESCAPE_THROUGH_STEALTH && pPlayer->GetQuestStatus(QUEST_ESCAPE_THROUGH_STEALTH) == QUEST_STATUS_INCOMPLETE)
            {
                if (StealthDialogueTimer < uiDiff)
                {
                    switch (StealthDialogueStep)
                    {
                        case 0:
                            m_creature->HandleEmote(EMOTE_ONESHOT_BOW);
                            StealthDialogueTimer = 1000;
                            StealthDialogueStep++;
                            break;
                        case 1:
                            if (Player* pPlayer = GetPlayerForEscort())
                                DoScriptText(SAY_ESCAPE, m_creature, pPlayer);
                            StealthDialogueTimer = 4000;
                            StealthDialogueStep++;
                            break;
                        case 2:
                            DoCastSpellIfCan(m_creature, SPELL_MOONSTALKER_FORM);
                            StealthDialogueTimer = 4000;
                            StealthDialogueStep++;
                            break;
                        case 3:
                            m_creature->GetMotionMaster()->MovePoint(0, aVolcorLocations[0].m_fX, aVolcorLocations[0].m_fY, aVolcorLocations[0].m_fZ, true);
                            StealthDialogueTimer = 30000;
                            StealthDialogueStep++;
                            break;
                    }
                }
                else
                    StealthDialogueTimer -= uiDiff;
            }
            else if (m_bEscortFinished && (m_uiQuestId == QUEST_ESCAPE_THROUGH_FORCE))
            {
                if (ForceDialogueTimer < uiDiff)
                {
                    switch (ForceDialogueStep)
                    {
                        case 0:
                            DoScriptText(SAY_END, m_creature);
                            ForceDialogueTimer = 1000;
                            ForceDialogueStep++;
                            break;
                        case 1:
                            if (Creature * pGrimClaw = m_creature->FindNearestCreature(NPC_GRIMCLAW, 40))
                            {
                                pGrimClaw->SetFacingToObject(m_creature);
                                ForceDialogueTimer = 3000;
                                ForceDialogueStep++;
                            }
                            else
                            {
                                m_bEscortFinished = false;
                                m_creature->ForcedDespawn();
                                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                            }
                            break;
                        case 2:
                            if (Creature * pGrimClaw = m_creature->FindNearestCreature(NPC_GRIMCLAW, 40))
                            {
                                pGrimClaw->SetWalk(false);
                                pGrimClaw->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX() + 3.0f, m_creature->GetPositionY() + 2.0f, m_creature->GetPositionZ(), MOVE_PATHFINDING);
                                DoScriptText(SAY_END_2, pGrimClaw);
                                ForceDialogueTimer = 4000;
                                ForceDialogueStep++;
                            }
                            else
                            {
                                m_bEscortFinished = false;
                                m_creature->ForcedDespawn();
                                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                            }
                            break;
                        case 3:
                            DoScriptText(SAY_END_3, m_creature);
                            ForceDialogueTimer = 5000;
                            ForceDialogueStep++;
                            break;
                        case 4:
                            m_bEscortFinished = false;
                            if (Creature * pGrimClaw = m_creature->FindNearestCreature(NPC_GRIMCLAW, 40))
                                pGrimClaw->ForcedDespawn();
                            m_creature->ForcedDespawn();
                            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                            break;
                    }
                }
                else
                    ForceDialogueTimer -= uiDiff;

                return;
            }
        }
        
        npc_escortAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_npc_volcor(Creature* pCreature)
{
    return new npc_volcorAI(pCreature);
}

bool QuestAccept_npc_volcor(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_FORCE || pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_STEALTH)
    {
        if (npc_volcorAI* pEscortAI = dynamic_cast<npc_volcorAI*>(pCreature->AI()))
            pEscortAI->StartEscort(pPlayer, pQuest);
    }

    return true;
}

//Alita
/*####
# npc_captured_rabid_thistle_bear
####*/

enum RabidThistleBearData
{
    SPELL_TRAPPED_BEAR = 9439,
    NPC_CAPTURED_RABID_THISTLE_BEAR = 11836,
};

struct npc_rabid_thistle_bearAI : public FollowerAI
{
    npc_rabid_thistle_bearAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        Reset();
        Captured_Timer = -1;
    }

    int32 Captured_Timer;

    void Reset() override
    {
    }

    void UpdateFollowerAI(uint32 const diff) override
    {
        int32 SignedDiff = diff;
        if (Captured_Timer >= 0)
        {
            if (Captured_Timer < SignedDiff)
            {
                Captured_Timer = -1;
                m_creature->DisappearAndDie();
            }
            else
                Captured_Timer -= SignedDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_CreatureSpells.empty())
            UpdateSpellsList(diff);

        DoMeleeAttackIfReady();
    }

    void StartFollowing(Player* pPlayer)
    {
        Captured_Timer = 180000;
        //Captured_Timer = 10000;
        StartFollow(pPlayer);
    }

    void JustRespawned() override
    {
        FollowerAI::JustRespawned();
        Captured_Timer = -1;
    }
};

CreatureAI* GetAI_npc_rabid_thistle_bear(Creature* pCreature)
{
    return new npc_rabid_thistle_bearAI(pCreature);
}

bool EffectDummyCreature_npc_rabid_thistle_bear(WorldObject* pCaster, uint32 uiSpellId, SpellEffectIndex effIndex, Creature* pCreatureTarget)
{
    //always check spellid and effectindex
    if (uiSpellId == SPELL_TRAPPED_BEAR && effIndex == EFFECT_INDEX_0)
    {
        if (Player* pPlayer = pCaster->ToPlayer())
        {
            pCreatureTarget->UpdateEntry(NPC_CAPTURED_RABID_THISTLE_BEAR);
            pPlayer->KilledMonsterCredit(pCreatureTarget->GetEntry(), pCreatureTarget->GetObjectGuid());
            if (npc_rabid_thistle_bearAI* pBearAI = dynamic_cast<npc_rabid_thistle_bearAI*>(pCreatureTarget->AI()))
            {
                pBearAI->EnterEvadeMode();
                pBearAI->StartFollowing(pPlayer);
            }

            //always return true when we are handling this spell and effect
            return true;
        }
    }
    return false;
}

/*####
# npc_terenthis
####*/

enum TerenthisData
{
    NPC_SENTINEL_SELARIN     = 3694,
    QUEST_HOW_BIG_A_THREAT_2 = 985
};

bool QuestComplete_npc_terenthis(Player* pPlayer, Creature* pQuestGiver, Quest const* pQuest)
{
    if (!pQuestGiver)
        return false;

    if (!pPlayer)
        return false;

    if ((pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_FORCE) || (pQuest->GetQuestId() == QUEST_ESCAPE_THROUGH_STEALTH))
    {
        if (pQuestGiver->FindNearestCreature(NPC_SENTINEL_SELARIN, 40))
            return true; // prevent starting db script if sentinel is already spawned
        else if (Creature* pSentinel = pQuestGiver->SummonCreature(NPC_SENTINEL_SELARIN, 6409.01f, 381.597f, 13.7997f, 1, TEMPSUMMON_TIMED_COMBAT_OR_DEAD_DESPAWN, 120000))
        {
            pSentinel->SetWalk(false);
            return false; // let quest_end_script take over from here
        }
        else
            return true; // prevent starting db script if sentinel was not spawned
    }
    else if (pQuest->GetQuestId() == QUEST_HOW_BIG_A_THREAT_2)
    {
        if (pQuestGiver->FindNearestCreature(NPC_GRIMCLAW, 40))
            return true; // prevent starting db script if grimclaw is already spawned
        else
            return false;
    }

    return false;
}

//would have thought taking it out of quest_template would have been necessary
//but turns out the quest adds the item afterwards with its check(so never adds),
//so it's fine to leave it in quest_template.
bool QuestAcceptGO_beached_sea(Player* player, GameObject* pGo, Quest const* pQuest)
{
    uint32 id = 0;
    uint32 count = 1;
    // Beached Sea turtle (12289)
    if (pQuest->GetQuestId() == 4722 ||  pQuest->GetQuestId() == 4727 ||  pQuest->GetQuestId() == 4732)
        id = 12289;
    //Beached Sea turtle, different item. (12292)
    else if (pQuest->GetQuestId() == 4725 || pQuest->GetQuestId() == 4731)
        id = 12292;
    //Beached Sea Creature (12242)
    else if (pQuest->GetQuestId() == 4723 || pQuest->GetQuestId() == 4728 || pQuest->GetQuestId() == 4730 || pQuest->GetQuestId() == 4733)
        id = 12242;
    if (!id)
        return true;
    //uint32 curItemCount = player->GetItemCount(id, true); //the whole point is to NOT check.
    ItemPosCountVec dest;
    uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count /*- curItemCount*/);
    if (msg == EQUIP_ERR_OK)
    {
        Item* item = player->StoreNewItem(dest, id, true);
        player->SendNewItem(item, count /*- curItemCount*/, true, false);
    }
    return true;
}

 /*###
 #npc_murkdeep
 ###*/

enum Murkdeep
{
    NPC_GREYMIST_COASTRUNNER = 2202,
    NPC_GREYMIST_WARRIOR = 2205,
    NPC_GREYMIST_HUNTER = 2206,
    NPC_MURKDEEP = 10323,

    GO_BONFIRE = 61927,

    AREATRIGGER_MURKDEEP = 1966,

    QUEST_WANTED_MURKDEEP = 4740,

    SPELL_SUNDER_ARMOR = 11971,
    SPELL_NET = 6533,
};

struct MurlocStruct
{
    uint32 m_npcEntry;
    int m_wave;
    float m_summon_position[4];
    float m_end_position[3];
    uint32 m_specialWaypoints; // unused rn.
};

std::array<MurlocStruct, 7> const Murlocs =
{ {
    { NPC_GREYMIST_COASTRUNNER, 1, {4992.359863f, 604.164001f, -1.583150f, 4.629250f}, {4985.59f, 548.37f, 5.20154f}, 220201 },
    { NPC_GREYMIST_COASTRUNNER, 1, {4999.430176f, 613.494995f, -2.640280f, 6.000060f}, {4992.22f, 548.325f, 5.15979f}, 220202 },
    { NPC_GREYMIST_COASTRUNNER, 1, {5000.450195f, 611.432007f, -2.141540f, 6.108570f}, {4987.93f, 545.078f, 5.31286f}, 220203 },
    { NPC_GREYMIST_WARRIOR, 2, {4994.259766f, 614.099976f, -3.671960f, 4.425240f}, {4985.59f, 548.37f, 5.20154f}, 220501 },
    { NPC_GREYMIST_WARRIOR, 2, {4990.520020f, 615.265991f, -4.662100f, 6.088590f}, {4992.22f, 548.325f, 5.15979f}, 220502 },
    { NPC_GREYMIST_HUNTER, 3, {4991.589844f, 616.317993f, -5.095080f, 4.696760f}, {4985.59f, 548.37f, 5.20154f}, 220601 },
    { NPC_MURKDEEP, 3, {4993.229980f, 615.023987f, -4.334300f, 4.636230f}, {4987.93f, 545.078f, 5.31286f}, 1032301 },
} };

struct go_murkdeep_bonfire : public GameObjectAI
{
    // This is the only Bonfire with this entry.
    explicit go_murkdeep_bonfire(GameObject* gobj) : GameObjectAI(gobj)
    {
    }

    std::set<ObjectGuid> m_greymist_murlocs;

    int wave = 1;
    bool b_wave_active = false;
    bool b_murkdeep_active = false;
    bool b_event_done = false; // If Murkdeep is a corpse.

    void SummonedCreatureJustDied(Creature* pSummoned) override
    {
        // Only if Murkdeep is not spawned, the next wave spawns instantly after this one is cleaned.
        if (!b_murkdeep_active && !b_event_done)
        {
            if (m_greymist_murlocs.find(pSummoned->GetObjectGuid()) != m_greymist_murlocs.end())
            {
                m_greymist_murlocs.erase(pSummoned->GetObjectGuid());
                sLog.outBasic("[SummonedCreatureJustDied] erase %d.", pSummoned->GetEntry());
            }
        }

        switch (pSummoned->GetEntry())
        {
            case NPC_GREYMIST_COASTRUNNER:
                wave = 2;
                break;
            case NPC_GREYMIST_WARRIOR:
                wave = 3;
                break;
            case NPC_GREYMIST_HUNTER: // If the Hunter has been killed from Murkdeeps wave, set back to wave 1, because they can respawn.
            case NPC_MURKDEEP:
                wave = 1;
                break;
        }

        if (SummoneNextWave())
            sLog.outBasic("[SummoneNextWave] successful.");
    }

    // Not sure if this working.
    void SummonedCreatureDespawn(Creature* pSummoned) override
    {
        if (m_greymist_murlocs.find(pSummoned->GetObjectGuid()) != m_greymist_murlocs.end())
        {
            m_greymist_murlocs.erase(pSummoned->GetObjectGuid());
            sLog.outBasic("[SummonedCreatureDespawn] erase %d.", pSummoned->GetEntry());
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId) override
    {
        if (uiMotionType != POINT_MOTION_TYPE || !pSummoned)
            return;

        pSummoned->GetMotionMaster()->MoveRandom(true, 5.0f);

        sLog.outBasic("[SummonedMovementInform] %d reached point %d.", pSummoned->GetEntry(), uiPointId);
    }

    bool SummoneNextWave()
    {
        b_wave_active = false;
        b_murkdeep_active = false;
        b_event_done = false;

        sLog.outBasic("[SummoneNextWave] m_greymist_murlocs size: %d.", m_greymist_murlocs.size());

        std::list<Creature*> murlocsInCamp;
        GetCreatureListWithEntryInGrid(murlocsInCamp, me, { NPC_GREYMIST_COASTRUNNER, NPC_GREYMIST_WARRIOR, NPC_GREYMIST_HUNTER, NPC_MURKDEEP }, DEFAULT_VISIBILITY_DISTANCE);
        for (const auto pMurloc : murlocsInCamp)
        {
            if (!pMurloc)
                continue;

            sLog.outBasic("[SummoneNextWave] GetCreatureListWithEntryInGrid: %d.", pMurloc->GetEntry());

            if (m_greymist_murlocs.find(pMurloc->GetObjectGuid()) != m_greymist_murlocs.end()) // Found in our list.
            {
                sLog.outBasic("[SummoneNextWave] in m_greymist_murlocs: %d.", pMurloc->GetEntry());

                switch (pMurloc->GetEntry())
                {
                    case NPC_GREYMIST_COASTRUNNER:
                    case NPC_GREYMIST_WARRIOR:
                    case NPC_GREYMIST_HUNTER:
                        b_wave_active = true;
                        break;
                    case NPC_MURKDEEP:
                        b_murkdeep_active = true;
                        if (pMurloc->IsDead())
                            b_event_done = true;
                        break;
                }
            }
            else if (wave == 1 && pMurloc && pMurloc->IsAlive()) // No Wave summons until all Murlocs in the camp are dead.
            {
                sLog.outBasic("[SummoneNextWave] Not all Murlocs dead.");
                return false; // Not all Murlocs dead.
            }
        }

        // If all summoned NPCs are dead while Murkdeep is alive or not summoned.
        if (!b_wave_active && !b_event_done)
        {
            for (uint8 i = 0; i < Murlocs.size(); ++i)
            {
                if (Murlocs[i].m_wave == wave)
                {
                    // Don't allow multiple Murkdeep summons.
                    if (b_murkdeep_active && Murlocs[i].m_npcEntry == NPC_MURKDEEP)
                        continue;

                    if (Creature* pMurloc = me->SummonCreature(Murlocs[i].m_npcEntry,
                        Murlocs[i].m_summon_position[0], Murlocs[i].m_summon_position[1], Murlocs[i].m_summon_position[2], Murlocs[i].m_summon_position[3],
                        TEMPSUMMON_DEAD_DESPAWN, 0, true))
                    {
                        m_greymist_murlocs.insert(pMurloc->GetObjectGuid());
                        pMurloc->SetWalk(false);
                        pMurloc->SetHomePosition(Murlocs[i].m_end_position[0], Murlocs[i].m_end_position[1], Murlocs[i].m_end_position[2], Murlocs[i].m_summon_position[3]); // why is this not working??
                        pMurloc->GetMotionMaster()->MovePoint(1, Murlocs[i].m_end_position[0], Murlocs[i].m_end_position[1], Murlocs[i].m_end_position[2], MOVE_PATHFINDING);
                        sLog.outBasic("[SummoneNextWave] SummonCreature %d.", Murlocs[i].m_npcEntry);
                    }
                }
            }
        }
        else
        {
            sLog.outBasic("[SummoneNextWave] m_greymist_murlocs not cleared.");
            return false;
        }

        return true;
    }
};

GameObjectAI* GetAI_go_murkdeep_bonfire(GameObject* gameobject)
{
    return new go_murkdeep_bonfire(gameobject);
}

bool at_murloc_camp(Player* pPlayer, AreaTriggerEntry const *pAt)
{
    if (pAt->id == AREATRIGGER_MURKDEEP)
    {
        if (pPlayer->IsGameMaster() || pPlayer->IsDead())
            return false;

        // Waves will be summoned even if the Quest is completed but not turned in.
        if (pPlayer && (pPlayer->GetQuestStatus(QUEST_WANTED_MURKDEEP) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_WANTED_MURKDEEP) == QUEST_STATUS_COMPLETE))
        {
            GameObject* pBonfire = GetClosestGameObjectWithEntry(pPlayer, GO_BONFIRE, DEFAULT_VISIBILITY_DISTANCE);
            if (pBonfire)
            {
                if (auto pBonfireAI = dynamic_cast<go_murkdeep_bonfire*>(pBonfire->AI()))
                    pBonfireAI->SummoneNextWave();
            }
        }
    }

    return false;
}

void AddSC_darkshore()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_kerlonian";
    newscript->GetAI = &GetAI_npc_kerlonian;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_kerlonian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_prospector_remtravel";
    newscript->GetAI = &GetAI_npc_prospector_remtravel;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_prospector_remtravel;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_threshwackonator";
    newscript->GetAI = &GetAI_npc_threshwackonator;
    newscript->pGossipHello = &GossipHello_npc_threshwackonator;
    newscript->pGossipSelect = &GossipSelect_npc_threshwackonator;
    newscript->RegisterSelf();

    //import SD2
    newscript = new Script;
    newscript->Name = "npc_therylune";
    newscript->GetAI = &GetAI_npc_therylune;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_therylune;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_volcor";
    newscript->GetAI = &GetAI_npc_volcor;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_volcor;
    newscript->RegisterSelf();

    //Alita
    newscript = new Script;
    newscript->Name = "npc_rabid_thistle_bear";
    newscript->GetAI = &GetAI_npc_rabid_thistle_bear;
    newscript->pEffectDummyCreature = &EffectDummyCreature_npc_rabid_thistle_bear;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_terenthis";
    newscript->pQuestRewardedNPC = &QuestComplete_npc_terenthis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_beached_quest";
    newscript->pGOQuestAccept = &QuestAcceptGO_beached_sea;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_murkdeep_bonfire";
    newscript->GOGetAI = &GetAI_go_murkdeep_bonfire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_murloc_camp";
    newscript->pAreaTrigger = &at_murloc_camp;
    newscript->RegisterSelf();
}
