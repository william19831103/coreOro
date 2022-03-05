/* Copyright (C) 2009 - 2010 ScriptDevZero <http://github.com/scriptdevzero/scriptdevzero>
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

#include "scriptPCH.h"
#include "custom.h"
#include "ScriptedAI.h"
#include "Bag.h"
#include "Group.h"
#include <ctime>
#include "Arena.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "World.h"
#include "Utilities/EventMap.h"
#include "../world/scourge_invasion.h"
#include "CreatureGroups.h"
#include "WaypointManager.h"

enum ArenaGossip
{
    GOSSIP_ACTION_BATTLEGROUND_WS = 1, 
    GOSSIP_ACTION_BATTLEGROUND_AB = 2,
    GOSSIP_ACTION_BATTLEGROUND_AV = 3,
    GOSSIP_ACTION_1V1_ARENA = 200,
    GOSSIP_ACTION_2V2_ARENA = 201,
    GOSSIP_ACTION_3V3_ARENA = 202,
    GOSSIP_ACTION_5V5_ARENA = 203,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA1v1 = 4,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA2v2 = 5,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA3v3 = 6,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA5v5 = 7,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE1v1 = 8,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE2v2 = 9,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE3v3 = 10,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE5v5 = 11,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL1v1 = 12,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL2v2 = 13,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL3v3 = 14,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL5v5 = 15,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA1v1 = 104,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA2v2 = 105,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA3v3 = 106,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA5v5 = 107,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE1v1 = 108,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE2v2 = 109,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE3v3 = 110,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE5v5 = 111,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL1v1 = 112,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL2v2 = 113,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL3v3 = 114,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL5v5 = 115,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS1v1 = 16,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS2v2 = 17,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS3v3 = 18,
    GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS5v5 = 19,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS1v1 = 116,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS2v2 = 117,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS3v3 = 118,
    GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS5v5 = 119,

    GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMLEVEL = 801,
    GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMPATCH = 802,
    GOSSIP_ACTION_CONFIG_ARENA_STASH = 803,
    GOSSIP_ACTION_CONFIG_ARENA_ALLOW_ITEM_SWAP = 804,
    GOSSIP_ACTION_CONFIG_ARENA_ALLOW_TRINKET_SWAP = 805,

    GOSSIP_SENDER_SPECTATE_MATCH = 999,
    GOSSIP_SENDER_SPECTATE = 900,
    GOSSIP_SENDER_ADMIN = 800
};

enum ArenaNPC
{
    ARENA_ANNOUNCER = 600044
};

enum ArenaGossipText
{
    DEFAULT = 600068
};

enum sSpells
{
    SPAWN_RED_LIGHTNING = 24240, //visual red lightning pillar.
    SPAWN_PINK_LIGHTNING = 28234, //visual pink lightning pillar.
};

int QueueCounter(Player* pPlayer, uint8 bgTypeID)
{
    if (!pPlayer)
        return NULL;

    if (!bgTypeID)
        return NULL;

    uint32 instanceId;
    uint32 removeTime;
    uint8 countQueue, countInArena, countHasinvite, SUM;

    countQueue = 0;
    countInArena = 0;
    countHasinvite = 0;
    SUM = 0;
        
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(bgTypeID));

    if (!bgQueueTypeId)
        return NULL;

    BattleGroundQueue& queue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
    BattleGroundTypeId typeId = BattleGroundMgr::BGTemplateId(BattleGroundQueueTypeId(bgQueueTypeId));

    if (!typeId)
        return NULL;

    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(typeId);

    if (!bg)
        return NULL;

    BattleGroundBracketId bracket_id = pPlayer->GetBattleGroundBracketIdFromLevel(typeId);
    
    if (!bracket_id)
        return NULL;

    // Loop trough all BGs with the bgTypeId.
    for (BattleGroundSet::const_iterator it = sBattleGroundMgr.GetBattleGroundsBegin(BattleGroundTypeId(typeId)); it != sBattleGroundMgr.GetBattleGroundsEnd(BattleGroundTypeId(typeId)); ++it)
    {
        for (BattleGroundQueue::QueuedPlayersMap::const_iterator itr = queue.m_QueuedPlayers.begin(); itr != queue.m_QueuedPlayers.end(); ++itr)
        {
            // Get the instance ID to check how many players are already in.
            instanceId = itr->second.GroupInfo->IsInvitedToBGInstanceGUID;
            removeTime = itr->second.GroupInfo->RemoveInviteTime;
    
            if (it->second->GetInstanceID() == instanceId)
            {
                if (queue.IsPlayerInvited(itr->first, instanceId, removeTime)) // looking for players that are invited to this instanceID.
                {
                    Player* plr = ObjectAccessor::FindPlayerNotInWorld(itr->first);
    
                    if (!plr)
                        continue;
    
                    if (plr->GetBattleGroundBracketIdFromLevel(typeId) != bracket_id)
                        continue;
    
                    if (removeTime) // If there is a time, player has invite.
                        ++countHasinvite;
                    else            // No time, player is just in queue.
                        ++countQueue;
                }
            }
        }
    
        // Skip all running arenas.
        if (it->second->GetStatus() > STATUS_WAIT_JOIN)
            continue;
    
        // Get all players that are in the arena already.
        BattleGround::BattleGroundPlayerMap const& pPlayers = it->second->GetPlayers();
        for (BattleGround::BattleGroundPlayerMap::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
        {
            if (!itr->first)
                continue;
    
            ++countInArena;
        }
    }

    SUM = countQueue + countHasinvite + countInArena;

    return SUM;
}

// TalentTab.dbc -> TalentTabID
const uint32 FORBIDDEN_TALENTS_IN_1V1_ARENA[] =
{
    // Healer
    201, // PriestDiscipline
    202, // PriestHoly
    382, // PaladinHoly
    262, // ShamanRestoration
    282, // DruidRestoration

    // Tanks
    //383, // PaladinProtection
    //163, // WarriorProtection

    0 // End
};

static bool Arena1v1CheckTalents(Player* pPlayer)
{
    if (!pPlayer)
        return false;

    uint32 count = 0;
    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

        if (!talentInfo)
            continue;

        for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] == 0)
                continue;

            if (pPlayer->HasSpell(talentInfo->RankID[rank]))
            {
                for (int8 i = 0; FORBIDDEN_TALENTS_IN_1V1_ARENA[i] != 0; i++)
                    if (FORBIDDEN_TALENTS_IN_1V1_ARENA[i] == talentInfo->TalentTab)
                        count += rank + 1;
            }
        }
    }

    if (count >= 36)
    {
        ChatHandler(pPlayer->GetSession()).SendSysMessage("You can't join, because you have invested to much points in a forbidden talent. Please edit your talents.");
        return false;
    }
    else
        return true;
}

inline std::string GetArenaBracketName(uint8 typeId)
{
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(typeId));

    if (!bgQueueTypeId)
        return "Arena Unknown";

    switch (bgQueueTypeId)
    {
        case BATTLEGROUND_QUEUE_NA1v1:
        case BATTLEGROUND_QUEUE_BE1v1:
        case BATTLEGROUND_QUEUE_RL1v1:
        case BATTLEGROUND_QUEUE_DS1v1:
            return "Arena 1v1"; break;
        case BATTLEGROUND_QUEUE_NA2v2:
        case BATTLEGROUND_QUEUE_BE2v2:
        case BATTLEGROUND_QUEUE_RL2v2:
        case BATTLEGROUND_QUEUE_DS2v2:
            return "Arena 2v2"; break;
        case BATTLEGROUND_QUEUE_NA3v3:
        case BATTLEGROUND_QUEUE_BE3v3:
        case BATTLEGROUND_QUEUE_RL3v3:
        case BATTLEGROUND_QUEUE_DS3v3:
            return "Arena 3v3"; break;
        case BATTLEGROUND_QUEUE_NA5v5:
        case BATTLEGROUND_QUEUE_BE5v5:
        case BATTLEGROUND_QUEUE_RL5v5:
        case BATTLEGROUND_QUEUE_DS5v5:
            return "Arena 5v5"; break;
        default:
            return "Arena Unknown";
    }
}

inline uint8 GetArenaBracketType(uint8 bgtypeId)
{
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(bgtypeId));

    if (!bgQueueTypeId)
        return NULL;

    switch (bgQueueTypeId)
    {
        case BATTLEGROUND_QUEUE_NA1v1:
        case BATTLEGROUND_QUEUE_BE1v1:
        case BATTLEGROUND_QUEUE_RL1v1:
        case BATTLEGROUND_QUEUE_DS1v1:
            return 1; break;
        case BATTLEGROUND_QUEUE_NA2v2:
        case BATTLEGROUND_QUEUE_BE2v2:
        case BATTLEGROUND_QUEUE_RL2v2:
        case BATTLEGROUND_QUEUE_DS2v2:
            return 2; break;
        case BATTLEGROUND_QUEUE_NA3v3:
        case BATTLEGROUND_QUEUE_BE3v3:
        case BATTLEGROUND_QUEUE_RL3v3:
        case BATTLEGROUND_QUEUE_DS3v3:
            return 3; break;
        case BATTLEGROUND_QUEUE_NA5v5:
        case BATTLEGROUND_QUEUE_BE5v5:
        case BATTLEGROUND_QUEUE_RL5v5:
        case BATTLEGROUND_QUEUE_DS5v5:
            return 5; break;
    }
    return NULL;
}

inline uint8 GetArenaBracketQueueChecker(Player* pPlayer, uint8 arenatype)
{
    if (!arenatype)
        return NULL;

    switch (arenatype)
    {
        case 1:
        {
            if (QueueCounter(pPlayer, BATTLEGROUND_NA1v1))
                return BATTLEGROUND_NA1v1;
            if (QueueCounter(pPlayer, BATTLEGROUND_BE1v1))
                return BATTLEGROUND_BE1v1;
            if (QueueCounter(pPlayer, BATTLEGROUND_RL1v1))
                return BATTLEGROUND_RL1v1;
            if (QueueCounter(pPlayer, BATTLEGROUND_DS1v1))
                return BATTLEGROUND_DS1v1;
            break;
        }
        case 2:
        {
            if (QueueCounter(pPlayer, BATTLEGROUND_NA2v2))
                return BATTLEGROUND_NA2v2;
            if (QueueCounter(pPlayer, BATTLEGROUND_BE2v2))
                return BATTLEGROUND_BE2v2;
            if (QueueCounter(pPlayer, BATTLEGROUND_RL2v2))
                return BATTLEGROUND_RL2v2;
            if (QueueCounter(pPlayer, BATTLEGROUND_DS2v2))
                return BATTLEGROUND_DS2v2;
            break;
        }
        case 3:
        {
            if (QueueCounter(pPlayer, BATTLEGROUND_NA3v3))
                return BATTLEGROUND_NA3v3;
            if (QueueCounter(pPlayer, BATTLEGROUND_BE3v3))
                return BATTLEGROUND_BE3v3;
            if (QueueCounter(pPlayer, BATTLEGROUND_RL3v3))
                return BATTLEGROUND_RL3v3;
            if (QueueCounter(pPlayer, BATTLEGROUND_DS3v3))
                return BATTLEGROUND_DS3v3;
            break;
        }
        case 5:
        {
            if (QueueCounter(pPlayer, BATTLEGROUND_NA5v5))
                return BATTLEGROUND_NA5v5;
            if (QueueCounter(pPlayer, BATTLEGROUND_BE5v5))
                return BATTLEGROUND_BE5v5;
            if (QueueCounter(pPlayer, BATTLEGROUND_RL5v5))
                return BATTLEGROUND_RL5v5;
            if (QueueCounter(pPlayer, BATTLEGROUND_DS5v5))
                return BATTLEGROUND_DS5v5;
            break;
        }
    }
    return NULL;
}

inline bool PlayerIsInQueueFor1v1(Player* pPlayer)
{
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_NA1v1))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_BE1v1))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_RL1v1))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_DS1v1))
        return true;

    return false;
}

inline bool PlayerIsInQueueFor2v2(Player* pPlayer)
{
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_NA2v2))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_BE2v2))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_RL2v2))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_DS2v2))
        return true;

    return false;
}

inline bool PlayerIsInQueueFor3v3(Player* pPlayer)
{
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_NA3v3))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_BE3v3))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_RL3v3))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_DS3v3))
        return true;

    return false;
}

inline bool PlayerIsInQueueFor5v5(Player* pPlayer)
{
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_NA5v5))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_BE5v5))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_RL5v5))
        return true;
    if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(BATTLEGROUND_QUEUE_DS5v5))
        return true;

    return false;
}

uint8 GenerateRandomArenaForBracket(uint8 bracket)
{
    if (!bracket)
        return NULL;

    switch (bracket)
    {
        case GOSSIP_ACTION_1V1_ARENA:
        {
            switch (urand(1, 8))
            {
            case 1:
            case 2:
                return BATTLEGROUND_NA1v1; break;
            case 3:
            case 4:
                return BATTLEGROUND_RL1v1; break;
            case 5:
            case 6:
                return BATTLEGROUND_DS1v1; break;
            case 7:
            case 8:
                return BATTLEGROUND_BE1v1; break;
            }
            break;
        }
        case GOSSIP_ACTION_2V2_ARENA:
        {
            switch (urand(1, 4))
            {
            case 1: return BATTLEGROUND_NA2v2; break;
            case 2: return BATTLEGROUND_BE2v2; break;
            case 3: return BATTLEGROUND_RL2v2; break;
            case 4: return BATTLEGROUND_DS2v2; break;
            }
            break;
        }
        case GOSSIP_ACTION_3V3_ARENA:
        {
            switch (urand(1, 4))
            {
            case 1: return BATTLEGROUND_NA3v3; break;
            case 2: return BATTLEGROUND_BE3v3; break;
            case 3: return BATTLEGROUND_RL3v3; break;
            case 4: return BATTLEGROUND_DS3v3; break;
            }
            break;
        }
        case GOSSIP_ACTION_5V5_ARENA:
        {
            switch (urand(1, 4))
            {
            case 1: return BATTLEGROUND_NA5v5; break;
            case 2: return BATTLEGROUND_BE5v5; break;
            case 3: return BATTLEGROUND_RL5v5; break;
            case 4: return BATTLEGROUND_DS5v5; break;
            }
            break;
        }
        default:
            return NULL;
    }
    return NULL;
}

bool JoinQueueArena(Player* pPlayer, GameObject* pGameObject, uint8 bgTypeid)
{
    if (!pPlayer || !pGameObject)
        return false;

    uint64 guid = pPlayer->GetGUID();
    bool joinAsGroup = false;
    bool isPremade = false;
    Group* grp = pPlayer->GetGroup();

    if (grp)
    {
        switch (grp->GetMembersCount())
        {
            case 2:
            case 3:
            case 5:
                joinAsGroup = true;
                break;
            default:
                joinAsGroup = false;
                break;
        }
    }

    // Ignore if we already in BG or BG queue
    if (pPlayer->InBattleGround())
        return false;

    BattleGround* bg = nullptr;

    bg = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(bgTypeid));

    // For example: bgTypeid is BATTLEGROUND_NA1v1 now, BATTLEGROUND_NA1v1 is bracket 1, so lets check if someone else already is in queue in another bracket 1 arena and set bgTypeid to this bg.
    // First get the bracket for the current bgTypeid.
    uint8 bgType_ID = GetArenaBracketType(bgTypeid);
    // Then check all Queues for this bracket if there's some players in queue.
    uint8 arenatype_ID = GetArenaBracketQueueChecker(pPlayer, bgType_ID);

    if (arenatype_ID)
        bg = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(arenatype_ID));

    if (!bg)
        return false;

    BattleGroundTypeId bgTypeID = bg->GetTypeID();

    if (!bgTypeID)
        return false;

    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeID);

    if (!bgQueueTypeId)
        return false;

    if (PlayerIsInQueueFor1v1(pPlayer) && GetArenaBracketType(bgTypeID) == 1 || PlayerIsInQueueFor2v2(pPlayer) && GetArenaBracketType(bgTypeID) == 2 || PlayerIsInQueueFor3v3(pPlayer) && GetArenaBracketType(bgTypeID) == 3 || PlayerIsInQueueFor5v5(pPlayer) && GetArenaBracketType(bgTypeID) == 5)
        return false;

    BattleGroundQueue& queue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
    for (BattleGroundQueue::QueuedPlayersMap::const_iterator it = queue.m_QueuedPlayers.begin(); it != queue.m_QueuedPlayers.end(); ++it)
    {
        if (it->first == pPlayer->GetObjectGuid())
            return false;
    }

    BattleGroundBracketId bracketEntry = pPlayer->GetBattleGroundBracketIdFromLevel(bgTypeID);

    if (bracketEntry == BG_BRACKET_ID_NONE)
        return false;

    // GroupJoinBattleGroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL.
    if (!joinAsGroup)
    {
        // Check if already in queue.
        if (pPlayer->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            return false;

        // Check if has free queue slots.
        if (!pPlayer->HasFreeBattleGroundQueueId())
            return false;

        BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];

        GroupQueueInfo* ginfo = bgQueue.AddGroup(pPlayer, nullptr, bgTypeID, bracketEntry, false);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry);
        uint32 queueSlot = pPlayer->AddBattleGroundQueueId(bgQueueTypeId);
        pPlayer->SetBattleGroundEntryPoint(pPlayer, false);

        WorldPacket data;
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0);
        pPlayer->GetSession()->SendPacket(&data);
    }
    else
    {
        grp = pPlayer->GetGroup();
        // No group found.
        if (!grp)
            return false;

        if (grp->isRaidGroup())
            return false;

        std::vector<uint32> excludedMembers;
        uint32 err = grp->CanJoinBattleGroundQueue(bgTypeID, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), pPlayer, &excludedMembers);
        isPremade = sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH) &&
            (grp->GetMembersCount() >= sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_QUEUE_GROUP_MIN_SIZE));

        if (err == BG_JOIN_ERR_GROUP_DESERTER)
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, BG_GROUPJOIN_DESERTERS);
            pPlayer->GetSession()->SendPacket(&data);
            pPlayer->GetSession()->SendBattleGroundJoinError(err);
            return false;
        }
        else if (err != BG_JOIN_ERR_OK)
        {
            pPlayer->GetSession()->SendBattleGroundJoinError(err);
            return false;
        }

        // if we're here, then the conditions to join a bg are met. We can proceed in joining.

        BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
        DEBUG_LOG("Battleground: the following players are joining as group:");
        GroupQueueInfo* ginfo = bgQueue.AddGroup(pPlayer, grp, bgTypeID, bracketEntry, isPremade, &excludedMembers);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry);
        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member) continue;  // this should never happen

            if (member->IsInWorld()) return false;

            if (std::find(excludedMembers.begin(), excludedMembers.end(), member->GetGUIDLow()) != excludedMembers.end())
            {
                WorldPacket data;
                sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, BG_GROUPJOIN_FAILED);
                member->GetSession()->SendPacket(&data);
                pPlayer->GetSession()->SendBattleGroundJoinError(err);
                continue;
            }

            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);   // Add to queue.
            member->SetBattleGroundEntryPoint(pPlayer, false);                  // Store entry point coords.

            WorldPacket data;
            // Send status packet (in queue).
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bg->GetMapId());
            member->GetSession()->SendPacket(&data);
            DEBUG_LOG("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeID, member->GetGUIDLow(), member->GetName());
        }
        DEBUG_LOG("Battleground: group end");
    }

    sBattleGroundMgr.ScheduleQueueUpdate(bgQueueTypeId, bgTypeID, bracketEntry);

    std::ostringstream ss;

    ss << pPlayer->GetName();

    if (joinAsGroup)
        ss << " and his Group";

    BattleGround* bg_template = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(bgTypeID));
    ASSERT(bg_template);

    ss << " queued up for " << GetArenaBracketName(bgTypeID).c_str();

    if(Creature* Announcer = pGameObject->FindNearestCreature(ARENA_ANNOUNCER, 20.0f))
    {
        Announcer->MonsterYell(ss.str().c_str());
        Announcer->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);
    }
    
    return true;
}

std::string GetClassString2(Player* pPlayer)
{
    switch (pPlayer->GetClass())
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

bool GossipHello_ArenaMaster(Player* pPlayer, GameObject* pGameObject)
{
    if (!pPlayer)
        return false;

    pPlayer->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_INTERACTING_CANCELS);

    if (pPlayer->GetLevel() < 19)
    {
        pGameObject->PlayDirectSound(847, pPlayer);
        pPlayer->GetSession()->SendNotification("You must be Level 19 or higher.");
        return false;
    }

    if (pPlayer->IsInCombat())
    {
        pGameObject->PlayDirectSound(847, pPlayer);
        pPlayer->GetSession()->SendNotification("You are in combat.");
        return false;
    }

    if (pPlayer->IsGameMaster())
    {
        pGameObject->PlayDirectSound(847, pPlayer);
        pPlayer->GetSession()->SendNotification("Please disable GM Mode.");
        return false;
    }

    bool SomeoneInQueue_One_v_One = false;
    bool SomeoneInQueue_Two_v_Two = false;
    bool SomeoneInQueue_Three_v_Three = false;
    bool SomeoneInQueue_Five_v_Five = false;

    const Group* grp = pPlayer->GetGroup();
    if (!grp) // Is not in a group. Can only join solo queue.
    {
        uint8 counter = 0;
        uint8 countQueue;

        // This shows up to 4 Arenas where players are queued up to speed up the way to find games where players queued up actually, just for the lazy pigs.
        for (uint8 bgTypeId = BATTLEGROUND_NA1v1; bgTypeId <= BATTLEGROUND_DS5v5; ++bgTypeId)
        {
            // Don't show more than 4 gossip menus.
            if (counter > 4)
                break;

            countQueue = QueueCounter(pPlayer, bgTypeId);

            if (!countQueue)
                continue;

            std::string playerstr;

            if (countQueue > 1)
                playerstr = " Players";
            else
                playerstr = " Player";

            std::ostringstream gossipname;
            BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(bgTypeId));
            BattleGroundQueue& queue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
            BattleGroundTypeId typeId = BattleGroundMgr::BGTemplateId(BattleGroundQueueTypeId(bgQueueTypeId));
            BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(typeId);

            // Found an arena with not enough Players, show a gossip to join them.
            if (countQueue < bg->GetMaxPlayers())
            {
                switch (GetArenaBracketType(bgTypeId))
                {
                    case 1: SomeoneInQueue_One_v_One = true; break;
                    case 2: SomeoneInQueue_Two_v_Two = true; break;
                    case 3: SomeoneInQueue_Three_v_Three = true; break;
                    case 5: SomeoneInQueue_Five_v_Five = true; break;
                }

                if (!pPlayer->InBattleGroundQueueForBattleGroundQueueType(bgQueueTypeId))
                {
                    gossipname << GetArenaBracketName(bgTypeId).c_str() << "\n(" << std::to_string(countQueue) << playerstr.c_str() << " waiting)\n\n ";
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, gossipname.str().c_str(), 0, bgQueueTypeId);
                }
            }
            // If in queue show a gossip to leave it.
            if (pPlayer->InBattleGroundQueueForBattleGroundQueueType(bgQueueTypeId))
            {
                gossipname << "Leave " << GetArenaBracketName(bgTypeId) << "\n(" << std::to_string(countQueue) << playerstr.c_str() << " waiting)\n\n ";
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, gossipname.str().c_str(), 0, bgQueueTypeId + 100);
            }

            ++counter;
        }

        if (!PlayerIsInQueueFor1v1(pPlayer) && !SomeoneInQueue_One_v_One)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Queue for 1v1 Arena", 0, GOSSIP_ACTION_1V1_ARENA);
        if (!PlayerIsInQueueFor2v2(pPlayer) && !SomeoneInQueue_Two_v_Two)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Queue for 2v2 Arena", 0, GOSSIP_ACTION_2V2_ARENA);
        if (!PlayerIsInQueueFor3v3(pPlayer) && !SomeoneInQueue_Three_v_Three)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Queue for 3v3 Arena", 0, GOSSIP_ACTION_3V3_ARENA);
        if (!PlayerIsInQueueFor5v5(pPlayer) && !SomeoneInQueue_Five_v_Five)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Queue for 5v5 Arena\n\n", 0, GOSSIP_ACTION_5V5_ARENA);
    }
    else if (grp->GetLeaderGuid() != pPlayer->GetObjectGuid() && !pPlayer->InBattleGroundQueue()) // Is in a group and not the leader. Can't do shit.
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You are not the Leader of your group. Get Leader or leave the group to queue.", 0, 300);
        pPlayer->SEND_GOSSIP_MENU(DEFAULT, pGameObject->GetObjectGuid());
        return true;
    }
    else if (grp->GetLeaderGuid() == pPlayer->GetObjectGuid()) // Is in a group and leader. Can only join group queue.
    {
        switch (grp->GetMembersCount())
        {
            case 2:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Group Queue 2v2 Arena", 0, GOSSIP_ACTION_2V2_ARENA);
                break;
            case 3:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Group Queue 3v3 Arena", 0, GOSSIP_ACTION_3V3_ARENA);
                break;
            case 5:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Group Queue 5v5 Arena\n\n", 0, GOSSIP_ACTION_5V5_ARENA);
                break;
            default:
                ChatHandler(pPlayer->GetSession()).SendSysMessage("The number of members in your group will not be accepted to queue up.");
                break;
        }
    }

    // Search for players in arenas and add a spectator menu.
    uint8 players = 0;
    for (uint8 bgTypeId = BATTLEGROUND_NA1v1; bgTypeId <= BATTLEGROUND_DS5v5; ++bgTypeId)
    {
        for (BattleGroundSet::const_iterator it = sBattleGroundMgr.GetBattleGroundsBegin(BattleGroundTypeId(bgTypeId)); it != sBattleGroundMgr.GetBattleGroundsEnd(BattleGroundTypeId(bgTypeId)); ++it)
        {
            if (!it->first)
                continue;

            BattleGround* bg = it->second;

            if (!bg)
                continue;

            // Don't show Spectate menu if no bg is running
            if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                continue;

            BattleGround::BattleGroundPlayerMap const& pPlayers = it->second->GetPlayers();

            for (BattleGround::BattleGroundPlayerMap::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
            {
                if (!itr->first)
                    continue;

                if (Player* plr = ObjectAccessor::FindPlayer(itr->first))
                {
                    if (!plr)
                        continue;

                    // Just need to find 1 Player in an Arena to show the Spectate menu.
                    if (players > 0)
                        break;

                    ++players;

                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Spectate Arena match", GOSSIP_SENDER_SPECTATE, 0);
                }
            }
        }
    }

    // Only admins can config arena.
    if (pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
    {
        uint32 MaxItemLevel =   sWorld.getConfig(CONFIG_UINT32_ARENA_MAX_ITEMLEVEL);
        uint32 MaxItemPatch =   sWorld.getConfig(CONFIG_UINT32_ARENA_MAX_ITEMPATCH);
        bool EnableStash =      sWorld.getConfig(CONFIG_BOOL_ARENA_STASH);
        bool AllowItemSwap =    sWorld.getConfig(CONFIG_BOOL_ARENA_ALLOW_ITEM_SWAP);
        bool AllowTrinketSwap = sWorld.getConfig(CONFIG_BOOL_ARENA_ALLOW_TRINKET_SWAP);

        std::ostringstream MaxItemLevel_ss;
        MaxItemLevel_ss << "Arena.MaxItemLevel = " << MaxItemLevel;
        std::ostringstream MaxItemPatch_ss;
        MaxItemPatch_ss << "Arena.MaxItemPatch = " << MaxItemPatch;
        std::ostringstream EnableStash_ss;
        if (EnableStash)
            EnableStash_ss << "Arena.EnableStash = true";
        else
            EnableStash_ss << "Arena.EnableStash = false";
        std::ostringstream AllowItemSwap_ss;
        if (AllowItemSwap)
            AllowItemSwap_ss << "Arena.AllowItemSwap = true";
        else
            AllowItemSwap_ss << "Arena.AllowItemSwap = false";
        std::ostringstream AllowTrinketSwap_ss;
        if (AllowTrinketSwap)
            AllowTrinketSwap_ss << "Arena.AllowTrinketSwap = true";
        else
            AllowTrinketSwap_ss << "Arena.AllowTrinketSwap = false";

        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1,    MaxItemLevel_ss.str().c_str(),      GOSSIP_SENDER_ADMIN, GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMLEVEL, "Save as...", true);
        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1,    MaxItemPatch_ss.str().c_str(),      GOSSIP_SENDER_ADMIN, GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMPATCH, "Save as...", true);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1,             EnableStash_ss.str().c_str(),       GOSSIP_SENDER_ADMIN, GOSSIP_ACTION_CONFIG_ARENA_STASH);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1,             AllowItemSwap_ss.str().c_str(),     GOSSIP_SENDER_ADMIN, GOSSIP_ACTION_CONFIG_ARENA_ALLOW_ITEM_SWAP);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1,             AllowTrinketSwap_ss.str().c_str(),  GOSSIP_SENDER_ADMIN, GOSSIP_ACTION_CONFIG_ARENA_ALLOW_TRINKET_SWAP);
        //pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "<ADMIN> Save my current Gear & Talents.", GOSSIP_SENDER_START, SAVE_TEMP, "Save as...", true);
        //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<ADMIN> player_factionchange_items.", GOSSIP_SENDER_START, FIX_DB);
    }
    pPlayer->SEND_GOSSIP_MENU(DEFAULT, pGameObject->GetObjectGuid());

    return true;
}

bool GossipSelect_ArenaMaster(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action)
{
    const Group* grp = pPlayer->GetGroup();

    switch (action)
    {
        case GOSSIP_ACTION_BATTLEGROUND_WS:
            pPlayer->GetSession()->SendBattleGroundList(pPlayer->GetGUID(), BATTLEGROUND_WS);
            break;
        case GOSSIP_ACTION_BATTLEGROUND_AB:
            pPlayer->GetSession()->SendBattleGroundList(pPlayer->GetGUID(), BATTLEGROUND_AB);
            break;
        case GOSSIP_ACTION_BATTLEGROUND_AV:
            pPlayer->GetSession()->SendBattleGroundList(pPlayer->GetGUID(), BATTLEGROUND_AV);
            break;
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA1v1:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE1v1:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL1v1:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS1v1:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA2v2:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE2v2:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL2v2:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS2v2:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA3v3:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE3v3:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL3v3:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS3v3:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_NA5v5:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_BE5v5:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_RL5v5:
        case GOSSIP_ACTION_QUEUE_BATTLEGROUND_DS5v5:
        {
            if (pPlayer->HasForbiddenArenaItems(BattleGroundTypeId(action)))
            {
                pGameObject->PlayDirectSound(847, pPlayer);
                pPlayer->GetSession()->SendNotification("You are wearing forbidden items.");
                GossipHello_ArenaMaster(pPlayer, pGameObject);
                return false;
            }
            else
            {
                if (!JoinQueueArena(pPlayer, pGameObject, action))
                    ChatHandler(pPlayer->GetSession()).SendSysMessage("Something went wrong while joining queue. Already in another queue?");
            }
            GossipHello_ArenaMaster(pPlayer, pGameObject);
            break;
        }
        case 300:
        {
            if (pPlayer)
            {
                pPlayer->SetGameMaster(false);
                GossipHello_ArenaMaster(pPlayer, pGameObject);
            }
            break;
        }
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA1v1:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA2v2:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA3v3:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_NA5v5:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE1v1:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE2v2:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE3v3:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_BE5v5:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL1v1:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL2v2:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL3v3:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_RL5v5:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS1v1:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS2v2:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS3v3:
        case GOSSIP_ACTION_LEAVE_BATTLEGROUND_DS5v5:
        {
            BattleGroundQueue &queue = sBattleGroundMgr.m_BattleGroundQueues[action - 100];
            if (&queue)
                queue.LeaveQueue(pPlayer, BattleGroundTypeId(action - 100));
            GossipHello_ArenaMaster(pPlayer, pGameObject);
            break;
        }
        case GOSSIP_ACTION_1V1_ARENA:
        case GOSSIP_ACTION_2V2_ARENA:
        case GOSSIP_ACTION_3V3_ARENA:
        case GOSSIP_ACTION_5V5_ARENA:
        {
            if (pPlayer->HasForbiddenArenaItems(BattleGroundTypeId(action)))
            {
                pGameObject->PlayDirectSound(847, pPlayer);
                pPlayer->GetSession()->SendNotification("You are wearing forbidden items.");
                GossipHello_ArenaMaster(pPlayer, pGameObject);
                return false;
            }
            else
            {
                if (!JoinQueueArena(pPlayer, pGameObject, GenerateRandomArenaForBracket(action)))
                    ChatHandler(pPlayer->GetSession()).SendSysMessage("Something went wrong while joining queue. Already in another queue?");
            }
            GossipHello_ArenaMaster(pPlayer, pGameObject);
            pPlayer->SEND_GOSSIP_MENU(DEFAULT, pGameObject->GetObjectGuid());
            break;
        }
    }

    if (sender == GOSSIP_SENDER_SPECTATE_MATCH)
    {
        // Leave every Arena queue first.
        for (uint8 bgTypeId = BATTLEGROUND_NA1v1; bgTypeId <= BATTLEGROUND_DS5v5; ++bgTypeId)
        {
            BattleGroundQueue& queue = sBattleGroundMgr.m_BattleGroundQueues[bgTypeId];
            if (&queue)
                queue.LeaveQueue(pPlayer, BattleGroundTypeId(bgTypeId));
        }
        if (sObjectMgr.GetPlayer(action))
        {
            Player* target = sObjectMgr.GetPlayer(action);

            if (target)
            {
                //std::string chrNameLink = pPlayer->playerLink(target_name);
                Map* cMap = target->GetMap();
                uint32 instanceId = 0;
                uint32 teleFlags = TELE_TO_GM_MODE;
                InstancePlayerBind* bind = pPlayer->GetBoundInstance(target->GetMapId ());

                if (pPlayer->GetSmartInstanceBindingMode() && bind)
                {
                    instanceId = bind->state->GetInstanceId();
                    pPlayer->UnbindInstance(target->GetMapId());
                }

                if (cMap->IsBattleGround())
                {
                    if (pPlayer->GetBattleGroundId() && pPlayer->GetBattleGroundId() != target->GetBattleGroundId())
                    {
                        //PSendSysMessage(LANG_CANNOT_GO_TO_BG_FROM_BG, chrNameLink.c_str());
                        //SetSentErrorMessage(true);
                        return false;
                    }
                    // all's well, set bg id
                    // when porting out from the bg, it will be reset to 0
                    if (pPlayer->GetBattleGroundId() != target->GetBattleGroundId())
                    {
                        pPlayer->SetBattleGroundId(target->GetBattleGroundId(), target->GetBattleGroundTypeId());
                        teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
                    }

                    // remember current position as entry point for return at bg end teleportation
                    if (!pPlayer->GetMap()->IsBattleGround())
                        pPlayer->SetBattleGroundEntryPoint(pPlayer, true);
                }

                // stop flight if need
                if (pPlayer->IsTaxiFlying())
                {
                    pPlayer->GetMotionMaster()->MovementExpired();
                    pPlayer->GetTaxi().ClearTaxiDestinations();
                }
                // save only in non-flight case
                else
                    pPlayer->SaveRecallPosition();

                uint32 area_id;
                WorldLocation loc;
                pPlayer->GetPosition(loc);
                area_id = pPlayer->GetAreaId();

                pPlayer->SetHomebindToLocation(loc, area_id);

                // To point to see at pTarget with same orientation
                float x, y, z;
                target->GetPosition(x, y, z);

                pPlayer->RemoveAllAurasOnDeath();
                pPlayer->TeleportTo(target->GetMapId(), x, y, z + 5.0f, pPlayer->GetAngle(target), teleFlags);
                pPlayer->SetSpectate(true);
                return true;
            }
        }
    }
    else if (sender == GOSSIP_SENDER_SPECTATE)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        for (uint8 bgTypeId = BATTLEGROUND_NA1v1; bgTypeId <= BATTLEGROUND_DS5v5; ++bgTypeId)
        {
            for (BattleGroundSet::const_iterator it = sBattleGroundMgr.GetBattleGroundsBegin(BattleGroundTypeId(bgTypeId)); it != sBattleGroundMgr.GetBattleGroundsEnd(BattleGroundTypeId(bgTypeId)); ++it)
            {
                if (!it->first)
                    continue;

                BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(bgTypeId));
                BattleGround* bg = it->second;

                if (!bgQueueTypeId)
                    continue;

                if (!bg)
                    continue;

                // Don't show Spectate menu if no bg is running
                if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                    continue;

                BattleGround::BattleGroundPlayerMap const& pPlayers = it->second->GetPlayers();
                std::string playerName = "";
                uint8 i = 0;
                uint32 PlayerGuid = 0;
                std::ostringstream ss;
                ss << bg->GetName();

                for (BattleGround::BattleGroundPlayerMap::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    if (!itr->first)
                        continue;

                    if (sObjectMgr.GetPlayerNameByGUID(itr->first, playerName))
                    {
                        if (Player* plr = ObjectAccessor::FindPlayer(itr->first))
                        {
                            if (!plr)
                                continue;

                            ++i;
                            
                            PlayerGuid = itr->first;

                            if (i > 1)
                                ss << ",";

                                ss << " " << playerName << " " << plr->GetSpecNameByTalentPoints().c_str() << "-" << GetClassString2(plr) << "";
                        }
                    }
                }

                if (PlayerGuid)
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_SPECTATE_MATCH, PlayerGuid);
            }
        }
        pPlayer->SEND_GOSSIP_MENU(DEFAULT, pGameObject->GetObjectGuid());
    }
    else if (sender == GOSSIP_SENDER_ADMIN && pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
    {
        bool EnableStash = sWorld.getConfig(CONFIG_BOOL_ARENA_STASH);
        bool AllowItemSwap = sWorld.getConfig(CONFIG_BOOL_ARENA_ALLOW_ITEM_SWAP);
        bool AllowTrinketSwap = sWorld.getConfig(CONFIG_BOOL_ARENA_ALLOW_TRINKET_SWAP);
        switch (action)
        {
            case GOSSIP_ACTION_CONFIG_ARENA_STASH:
                sWorld.setConfig(CONFIG_BOOL_ARENA_STASH, !EnableStash);
                break;
            case GOSSIP_ACTION_CONFIG_ARENA_ALLOW_ITEM_SWAP:
                sWorld.setConfig(CONFIG_BOOL_ARENA_ALLOW_ITEM_SWAP, !AllowItemSwap);
                break;
            case GOSSIP_ACTION_CONFIG_ARENA_ALLOW_TRINKET_SWAP:
                sWorld.setConfig(CONFIG_BOOL_ARENA_ALLOW_TRINKET_SWAP, !AllowTrinketSwap);
                break;
        }
        GossipHello_ArenaMaster(pPlayer, pGameObject);
    }
    //GossipHello_ArenaMaster(pPlayer, pCreature);
    return true;
}

enum TornadoEvents
{
    EVENT_KNOCK_BACK    = 1,
    EVENT_DESPAWN       = 2,
};

struct npc_nagrand_tornado : public ScriptedAI
{
    npc_nagrand_tornado(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_events.Reset();
        Reset();
        afkplayers.clear();

        m_events.ScheduleEvent(EVENT_DESPAWN, 60000);

        if (SpellAuraHolder* pHolder = m_creature->AddAura(25160))
            pHolder->RemoveAura(EFFECT_INDEX_0); // Remove the triggered dmg spell.
    }

    EventMap m_events;
    std::unordered_map<ObjectGuid, time_t> afkplayers;

    void Reset() override 
    {
        m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), MOVE_PATHFINDING);
        m_events.ScheduleEvent(EVENT_KNOCK_BACK, 2000);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        // Z 4044.42f , Y 2911.51f is the center of the nagrand arena
        // Lets calculate a random waypoint in a 80 yard diameter circle.
        int point = urand(1, 40);
        float orientration = 0.492f;

        float angle = (float(point) * (M_PI / (40 / 2)));
        float x = 4056.0f + float(urand(5, 45)) * cos(angle);
        float y = 2922.0f + float(urand(5, 45)) * sin(angle);
        float z = 13.65f;
        float new_z = m_creature->GetMap()->GetHeight(x, y, z, true, 1.0f);
        if (new_z > INVALID_HEIGHT)
            z = new_z + 0.05f;

        /*
        // Search for afking / drinking players? :p
        std::list<Player*> players;
        m_creature->GetAlivePlayerListInRange(m_creature, players, DEFAULT_VISIBILITY_DISTANCE);

        for (const auto& pTarget : players)
            if (!pTarget->IsMoving())
            {
                x = pTarget->GetPositionX();
                y = pTarget->GetPositionY();
                z = pTarget->GetPositionZ();
                sLog.outBasic("[Arena:npc_nagrand_tornado] Player: %s is not moving since 10 seconds. Tornado is chasing him now.", pTarget->GetName());

                break;
            }
            */
        m_creature->GetMotionMaster()->MovePoint(uiPointId, x, y, z, MOVE_PATHFINDING + MOVE_WALK_MODE);
    }

    void UpdateAI(uint32 const diff) override
    {
        m_events.Update(diff);

        while (uint32 Events = m_events.ExecuteEvent())
        {
            switch (Events)
            {
                case EVENT_KNOCK_BACK:
                {
                    std::list<Player*> players;
                    m_creature->GetAlivePlayerListInRange(m_creature, players, ATTACK_DISTANCE);

                    for (const auto& pTarget : players)
                    {
                        // TBC spell i guess: https://tbc.wowhead.com/spell=43120/cyclone
                        // Clip: https://youtu.be/CJNQwBQh2Ms?t=148
                        // Lowered the damage since its vanilla and players have less health and lower level players can also play arena.
                        pTarget->KnockBackFrom(m_creature, 15, 10);
                        float dmg = urand(((pTarget->GetMaxHealth() * 10.0f) / 100), ((pTarget->GetMaxHealth() * 15.0f) / 100));
                        pTarget->DealDamage(pTarget, dmg, nullptr, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NATURE, nullptr, false);
                    }
                    m_events.ScheduleEvent(EVENT_KNOCK_BACK, 2000);
                    break;
                }
                case EVENT_DESPAWN:
                {
                    m_creature->RemoveAurasDueToSpell(25160);
                    m_creature->DespawnOrUnsummon(4000);
                    break;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_nagrand_tornado(Creature* pCreature)
{
    return new npc_nagrand_tornado(pCreature);
}

bool GossipSelect_ArenaMaster_Ext(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action, const char* code)
{
    std::string name = code;
    static const char* allowedcharacters = "1234567890";
    if (!name.length() || (name.length() > 2) || name.find_first_not_of(allowedcharacters) != std::string::npos)
    {
        ChatHandler(pPlayer->GetSession()).PSendSysMessage("invalid number.");
        pPlayer->CLOSE_GOSSIP_MENU();
        return false;
    }

    uint32 value = atoi(code);

    if (sender == GOSSIP_SENDER_ADMIN && value && pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
    {
        switch (action)
        {
            case GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMLEVEL:
                sWorld.setConfig(CONFIG_UINT32_ARENA_MAX_ITEMLEVEL, value);
                break;
            case GOSSIP_ACTION_CONFIG_ARENA_MAX_ITEMPATCH:
                sWorld.setConfig(CONFIG_UINT32_ARENA_MAX_ITEMPATCH, value);
                break;
        }
        GossipHello_ArenaMaster(pPlayer, pGameObject);
    }
    //GossipHello_ArenaMaster(pPlayer, pCreature);
    return true;
}

bool GossipHello_ArenaAnnouncer(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer)
        return false;

    pPlayer->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_INTERACTING_CANCELS);
    //pPlayer->SetFaction();
    //pPlayer->GetSession()->SendNotification("You are already in a Queue!");
    pPlayer->SEND_GOSSIP_MENU(ARENA_ANNOUNCER, pCreature->GetObjectGuid());
    return true;

}

#define GOSSIP_ACTION_READY 1
#define GOSSIP_ACTION_LEAVE 2
#define GOSSIP_ACTION_CONFIRM_LEAVE 3
#define NPC_TEXT_HELLO 800100
#define NPC_TEXT_SELECT 800101


bool GossipHello_npc_arena_watcher(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer || !pCreature)
        return false;

    if (!pCreature->InArena())
        return false;

    if (!pPlayer->HasAura(SPELL_ALLIANCE_FLAG) && !pPlayer->HasAura(SPELL_HORDE_FLAG))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I'm ready!\n\n", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_READY);


    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Can i leave please?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_LEAVE);

    pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_HELLO, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_arena_watcher(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer || !pCreature)
        return false;

    if (!pCreature->InArena())
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();

    switch (uiAction)
    {
    case GOSSIP_ACTION_READY:
    {
        switch (pPlayer->GetBGTeamId())
        {
            case TEAM_ALLIANCE:
                pPlayer->AddAura(SPELL_ALLIANCE_FLAG);
                break;
            case TEAM_HORDE:
                pPlayer->AddAura(SPELL_HORDE_FLAG);
                break;
        }
        pPlayer->PlayDirectSound(8960, pPlayer);
        
        uint32 maxplayers_ALLIANCE = 0;
        uint32 maxplayers_HORDE = 0;
        
        Map::PlayerList const& PlayerList = pCreature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
        
            if (!pPlayer)
                continue;
        
            if (pPlayer->HasAura(SPELL_ALLIANCE_FLAG))
                ++maxplayers_ALLIANCE;
        
            if (pPlayer->HasAura(SPELL_HORDE_FLAG))
                ++maxplayers_HORDE;
        }
        
        std::string announce;
        
        BattleGround* bg = nullptr;
        bg = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(pPlayer->GetBattleGroundTypeId()));
        
        if (bg)
        {
            if (maxplayers_HORDE + maxplayers_ALLIANCE == bg->GetMaxPlayers())
                announce = "Both Teams are ready to fight!";
            else if (maxplayers_HORDE == bg->GetMaxPlayersPerTeam())
                announce = "The Horde is ready to fight!";
            else if (maxplayers_ALLIANCE == bg->GetMaxPlayersPerTeam())
                announce = "The Alliance is ready to fight!";
        
            if (!announce.empty())
            {
                pCreature->MonsterYell(announce.c_str());
                pCreature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);
            }
        }
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    break;
    case GOSSIP_ACTION_LEAVE:
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes im sure.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CONFIRM_LEAVE);
        pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_SELECT, pCreature->GetObjectGuid());
    }
    break;
    case GOSSIP_ACTION_CONFIRM_LEAVE:
    {
        pPlayer->LeaveBattleground();
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    break;
    }
    return true;
}

//https://github.com/Oroxzy Arena Patch 1.12.1... end

enum
{
    EVERY_ITEM_VENDOR_NPC = 600018,
    DRUID_NPC = 600016,
    PRIEST_NPC = 600015,
    SHAMAN_NPC = 600014,
    WARLOCK_NPC = 600012,
    WARRIOR_NPC = 600011,
    PALADIN_NPC = 600010,
    ROGUE_NPC = 600006,
    MAGE_NPC = 600003,
    HUNTER_NPC = 600001,
    RANKING_NPC = 600005,
    ARENA_MASTER_NPC = 600044,
    PVP_TOOLS_NPC = 600025,
    CONSUMABLES_NPC = 600002,
    TELEPORT_NPC = 600030,

    STASH_CHEST_GOBJ = 600011,
    ENCHANTER_GOBJ = 600010,
    BUFFER_GOBJ = 900067,

    NPC_GOSSIP = 1,
    GOBJ_GOSSIP = 2,
};

void MoveToNPC2(Player* pPlayer, uint32 npc_id, bool isGameObject = false)
{
    if (!pPlayer)
        return;

    if (!npc_id)
        return;

    if (!isGameObject)
    {
        if (Creature* NPC = pPlayer->FindNearestCreature(npc_id, 200.0f))
        {
            float fX, fY, fZ;
            NPC->GetPosition(fX, fY, fZ);
            pPlayer->CastSpell(pPlayer, 23978, true);
            pPlayer->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_PATHFINDING);
        }
    }
    else
    {
        if (GameObject* Obj = pPlayer->FindNearestGameObject(npc_id, 200.0f))
        {
            float fX, fY, fZ;
            Obj->GetPosition(fX, fY, fZ);
            pPlayer->CastSpell(pPlayer, 23978, true);
            pPlayer->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_PATHFINDING);
        }
    }
}

// UterusOne InfoNPC
void SendPOI_NPC(Player* pPlayer, Creature* pCreature, uint32 ID)
{
    if (Creature * target = pCreature->FindNearestCreature(ID, 200.0f))
    {
        pPlayer->PlayerTalkClass->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(6), 99, 0, target->GetName());
    }
}

void SendPOI_GOBJ(Player* pPlayer, Creature* pCreature, uint32 ID)
{
    if (GameObject * target = pCreature->FindNearestGameObject(ID, 200.0f))
    {
        pPlayer->PlayerTalkClass->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(6), 99, 0, target->GetName());
    }
}

bool GossipHello_InfoNPC(Player* pPlayer, Creature* pCreature)
{
    switch (pPlayer->GetClass())
    {
        case CLASS_WARRIOR:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, WARRIOR_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Warrior Trainer", NPC_GOSSIP, WARRIOR_NPC);
            break;
        case CLASS_PALADIN:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, PALADIN_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Paladin Trainer", NPC_GOSSIP, PALADIN_NPC);
            break;
        case CLASS_HUNTER: 
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, HUNTER_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hunter Trainer", NPC_GOSSIP, HUNTER_NPC);
            break;
        case CLASS_ROGUE:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, ROGUE_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Rogue Trainer", NPC_GOSSIP, ROGUE_NPC);
            break;
        case CLASS_PRIEST:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, PRIEST_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Priest Trainer", NPC_GOSSIP, PRIEST_NPC);
            break;
        case CLASS_SHAMAN:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, SHAMAN_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shaman Trainer", NPC_GOSSIP, SHAMAN_NPC);
            break;
        case CLASS_MAGE:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, MAGE_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mage Trainer", NPC_GOSSIP, MAGE_NPC);
            break;
        case CLASS_WARLOCK:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, WARLOCK_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Warlock Trainer", NPC_GOSSIP, WARLOCK_NPC);
            break;
        case CLASS_DRUID:
            if (pPlayer->GetLevel() < 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Level 60", NPC_GOSSIP, DRUID_NPC);
            else if (pPlayer->GetLevel() == 60)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Druid Trainer", NPC_GOSSIP, DRUID_NPC);
            break;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "More Items", NPC_GOSSIP, EVERY_ITEM_VENDOR_NPC);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Enchants", GOBJ_GOSSIP, ENCHANTER_GOBJ);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Buffs", GOBJ_GOSSIP, BUFFER_GOBJ);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bank & Storage", GOBJ_GOSSIP, STASH_CHEST_GOBJ);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Arena Master", NPC_GOSSIP, ARENA_MASTER_NPC);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "PvP Items", NPC_GOSSIP, PVP_TOOLS_NPC);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Consumables", NPC_GOSSIP, CONSUMABLES_NPC);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleporter", NPC_GOSSIP, TELEPORT_NPC);
    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "How can i come back to this Island?", GOBJ_GOSSIP, STASH_CHEST_GOBJ);
    //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "PreTBC Mall"          , GOSSIP_SENDER_MAIN, 74);

    pPlayer->SEND_GOSSIP_MENU(600099, pCreature->GetGUID());

    return true;
}

bool GossipSelect_InfoNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ok!", 1, 0);
    switch (sender)
    {
    case 1:
        switch (action)
        {
            case 0:
                GossipHello_InfoNPC(pPlayer, pCreature);
                break;
            case DRUID_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case PRIEST_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case SHAMAN_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case WARLOCK_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case WARRIOR_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case PALADIN_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case ROGUE_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case MAGE_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case HUNTER_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000994, pCreature->GetGUID());
                break;
            case EVERY_ITEM_VENDOR_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000991, pCreature->GetGUID());
                break;
            case ARENA_MASTER_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000995, pCreature->GetGUID());
                break;
            case RANKING_NPC:
                pPlayer->SEND_GOSSIP_MENU(6000996, pCreature->GetGUID());
                break;
            case CONSUMABLES_NPC:
            case TELEPORT_NPC:
            case PVP_TOOLS_NPC:
                pPlayer->CLOSE_GOSSIP_MENU();
                break;
        }
        MoveToNPC2(pPlayer, action, false);
        SendPOI_NPC(pPlayer, pCreature, action);
        break;
    case 2:
        switch (action)
        {
            case ENCHANTER_GOBJ:
                pPlayer->SEND_GOSSIP_MENU(6000992, pCreature->GetGUID());
                break;
            case BUFFER_GOBJ:
                pPlayer->SEND_GOSSIP_MENU(6000997, pCreature->GetGUID());
                break;
            case STASH_CHEST_GOBJ:
                pPlayer->SEND_GOSSIP_MENU(6000993, pCreature->GetGUID());
                break;
        }
        MoveToNPC2(pPlayer, action, true);
        SendPOI_GOBJ(pPlayer, pCreature, action);
        break;
    }
    //GossipHello_InfoNPC(pPlayer, pCreature);
    return true;
}

void CompareCreatureTemplateDBs()
{
    QueryResult* result = WorldDatabase.PQuery("SELECT entry, faction, speed_walk, speed_run, scale, base_attack_time FROM creature_template_sniff");

    if (result)
    {
        do
        {
            auto fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            uint32 faction = fields[1].GetUInt32();
            float speed_walk = fields[2].GetFloat();
            float speed_run = fields[3].GetFloat();
            float scale = fields[4].GetFloat();
            uint32 base_attack_time = fields[5].GetUInt32();

            std::ostringstream ss;

            // Check Existance.
            QueryResult* result_exist = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u'", entry);
            if (!result_exist)
                continue;

            ss << "[CompareCreatureTemplateDBs] UPDATE `creature_template` SET ";

            // Check faction.
            QueryResult* result_faction = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND faction='%u'", entry, faction);
            if (!result_faction)
                ss << "`faction`='" << faction << "', ";

            // Check speed_walk.
            QueryResult* result_speed_walk = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND speed_walk='%f'", entry, speed_walk);
            if (!result_speed_walk)
                ss << "`speed_walk`='" << speed_walk << "', ";

            // Check speed_run.
            QueryResult* result_speed_run = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND speed_run='%f'", entry, speed_run);
            if (!result_speed_run)
                ss << "`speed_run`='" << speed_run << "', ";

            // Check scale.
            if (scale == 1)
            {
                QueryResult* result_scale = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND display_scale1='1' OR display_scale1='0';", entry);
                if (!result_scale)
                    ss << "`display_scale1`='" << scale << "', ";
            }
            else
            {
                QueryResult* result_scale = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND display_scale1='%f'", entry, scale);
                if (!result_scale)
                    ss << "`display_scale1`='" << scale << "', ";
            }

            // Check base_attack_time.
            QueryResult* result_base_attack_time = WorldDatabase.PQuery("SELECT entry FROM creature_template WHERE entry='%u' AND base_attack_time='%u'", entry, base_attack_time);
            if (!result_base_attack_time)
                ss << "`base_attack_time`='" << base_attack_time << "' ";

            ss << "WHERE `entry`='" << entry << "';";

            sLog.outBasic(ss.str().c_str());

        } while (result->NextRow());

        delete result;
    }
}

bool GossipHello_Instance_Reseter(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer)
        return false;

    uint32 counter = 0;

    Player::BoundInstancesMap& binds = pPlayer->GetBoundInstances();
    for (const auto& bind : binds)
    {
        DungeonPersistentState* state = bind.second.state;
        std::string timeleft;
        if (!bind.second.perm)
            timeleft = secsToTimeString(state->GetResetTime() - time(nullptr), true);
        else
            timeleft = secsToTimeString(sMapPersistentStateMgr.GetScheduler().GetResetTimeFor(bind.first) - time(nullptr));

        if (MapEntry const* entry = sMapStorage.LookupEntry<MapEntry>(bind.first))
        {
            std::ostringstream ss;
            ss << "Unbind me from " << entry->name << " please.";

            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ss.str().c_str(), GOSSIP_SENDER_MAIN, bind.first);
        }
        counter++;
    }
    counter = 0;

    if (!pPlayer->GetQuestStatus(9378) == QUEST_STATUS_COMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Allow me to enter Naxxramas permanently.", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF);

    // Only admin can add and delete templates.
    if (pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<ADMIN> Compare creature_templates_sniff with creature_templates.", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->SEND_GOSSIP_MENU(pCreature->GetEntry(), pCreature->GetGUID());

    return true;
}

bool GossipSelect_Instance_Reseter(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (!pPlayer)
        return false;

    if (!action)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();

    uint32 counter = 0;
    uint32 mapid = 0;
    bool got_map = false;

    got_map = true;
    mapid = action;

    if (sender == GOSSIP_SENDER_MAIN)
    {
        Player::BoundInstancesMap& binds = pPlayer->GetBoundInstances();
        for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
        {
            if (got_map && mapid != itr->first)
            {
                ++itr;
                continue;
            }
            if (itr->first != pPlayer->GetMapId())
            {
                DungeonPersistentState* save = itr->second.state;
                std::string timeleft = secsToTimeString(save->GetResetTime() - time(nullptr), true);

                if (MapEntry const* entry = sMapStorage.LookupEntry<MapEntry>(itr->first))
                    pPlayer->CastSpell(pPlayer, SPAWN_PINK_LIGHTNING, true);

                pPlayer->UnbindInstance(itr);
                counter++;
            }
            else
                ++itr;
        }
    }

    if (sender == GOSSIP_SENDER_INFO && action == GOSSIP_ACTION_INFO_DEF)
    {
        pCreature->CastSpell(pPlayer, 28006, true);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    if (sender == GOSSIP_SENDER_INFO && action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        CompareCreatureTemplateDBs();
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    GossipHello_Instance_Reseter(pPlayer, pCreature);
    return true;
}

bool IgnoredNPC(Creature* pCreature)
{
    // not required for Naxx but for MC for example.
    // just add Boss Adds here wich not respawn at boss reset.
    switch (pCreature->GetEntry())
    {
        case 12119: // Lucifron Adds
        case 11661: // Gehennas Adds
        case 12099: // Garr Adds
        case 11997: // Golemagg Adds
        case 11662: // Sulfuron Adds
        case 11663: // Majordomo Adds
        case 11664: // Majordomo Adds
            return true;
    }
    return false;
}

void ScaleMobsInInstance(Player* pPlayer, Creature* pCreature)
{
    uint32 count = 0;
    uint32 entries = 0;

    if (!pCreature->GetMap()->IsDungeon())
        return;

    QueryResult* result = WorldDatabase.PQuery("SELECT guid, id, map FROM creature WHERE map='%u'", pPlayer->GetMapId());

    if (result)
    {
        do
        {
            auto fields = result->Fetch();
            auto guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();

            CreatureData const* data = sObjectMgr.GetCreatureData(guid);

            if (!data)
                continue;

            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(data->creature_id[0]);

            if (!cInfo)
                continue;

            Creature* pTrash = pCreature->GetMap()->GetCreature(ObjectGuid(cInfo->GetHighGuid(), cInfo->entry, guid));

            if (!pTrash)
                continue;

            if (IgnoredNPC(pTrash))
                continue;

            if (pTrash->IsAlive() && !pTrash->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            {
                ++count;
                uint32 PlayersCount = 0;
                Map::PlayerList const& PlayerList = pCreature->GetMap()->GetPlayers();

                for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                {
                    Player* pPlayer = itr->getSource();

                    if (pPlayer)
                        ++PlayersCount;
                }
                uint8 Divison = 6 - PlayersCount;
                uint32 Armor = pTrash->GetArmor() / Divison;
                uint32 Health = pTrash->GetMaxHealth() / Divison;
                uint32 AttackTime = pTrash->GetAttackTime(BASE_ATTACK) * Divison;

                CreatureInfo const* cinfo = pTrash->GetCreatureInfo();
                pTrash->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->dmg_min + cinfo->dmg_min / Divison);
                pTrash->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->dmg_max + cinfo->dmg_max / Divison);
                pTrash->SetArmor(Armor);
                pTrash->SetMaxHealth(Health);
                pTrash->SetAttackTime(BASE_ATTACK, AttackTime);
                pTrash->CastSpell(pTrash, SPAWN_PINK_LIGHTNING, true);
            }

        } while (result->NextRow());

        if (count > 0)
        {
            std::ostringstream ss;
            ss << pCreature->GetName() << " modified " << count << " minions in " << pCreature->GetMap()->GetMapName();
            pPlayer->GetSession()->SendAreaTriggerMessage(ss.str().c_str());
            pCreature->PlayDistanceSound(8436);
            pCreature->CastSpell(pCreature, 10228, true);
        }
        delete result;
    }
}

void ClearRaidTrash(Player* pPlayer, Creature* pCreature)
{
    uint32 count = 0;
    uint32 entries = 0;

    if (!pCreature->GetMap()->IsRaid())
        return;

    QueryResult* result = WorldDatabase.PQuery("SELECT guid, id, map FROM creature WHERE map='%u'", pPlayer->GetMapId());

    if (result)
    {
        do
        {
            auto fields = result->Fetch();
            auto guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();

            CreatureData const* data = sObjectMgr.GetCreatureData(guid);

            if (!data)
                continue;

            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(data->creature_id[0]);

            if (!cInfo)
                continue;

            Creature* pTrash = pCreature->GetMap()->GetCreature(ObjectGuid(cInfo->GetHighGuid(), cInfo->entry, guid));

            if (!pTrash)
                continue;

            if (IgnoredNPC(pTrash))
                continue;

            if (pTrash->IsAlive() && !pTrash->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) && pTrash->IsHostileTo(pPlayer) && !pTrash->IsWorldBoss())
            {
                ++count;

                pTrash->RemoveGuardians();
                pTrash->CastSpell(pTrash, 28265, true);
            }

        } while (result->NextRow());

        if (count > 0)
        {
            std::ostringstream ss;
            ss << pCreature->GetName() << " just slaughtered " << count << " minions in " << pCreature->GetMap()->GetMapName();
            pPlayer->GetSession()->SendAreaTriggerMessage(ss.str().c_str());
            pCreature->PlayDistanceSound(8436);
            pCreature->CastSpell(pCreature, 17680, true);
        }
        delete result;
    }
}

bool GossipHello_naxx_boss_teleporter(Player* pPlayer, Creature* pCreature)
{
    if (!pPlayer)
        return false;

    pCreature->PlayDistanceSound(4041);

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We have no time for this shit!", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pCreature->GetEntry(), pCreature->GetGUID());

    return true;
}

bool GossipSelect_naxx_boss_teleporter(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (!pPlayer)
        return false;

    if (!action)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();

    if (sender == GOSSIP_SENDER_INFO && action == GOSSIP_ACTION_INFO_DEF)
    {
        if (pCreature->GetMap()->IsDungeon() && !pCreature->GetMap()->IsRaid())
            ScaleMobsInInstance(pPlayer, pCreature);
        if (pCreature->GetMap()->IsRaid())
            ClearRaidTrash(pPlayer, pCreature);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

// TELEPORT NPC

bool GossipHello_TeleportNPC(Player* pPlayer, Creature *pCreature)   
{
    // HORDE
    if (pPlayer->GetTeam() == HORDE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Major Cities"               , GOSSIP_SENDER_INFO, 1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Starting Areas"       , GOSSIP_SENDER_INFO, 3);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Instances"            , GOSSIP_SENDER_INFO, 5);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Raids"                 , GOSSIP_SENDER_INFO, 101);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gurubashi Arena" , GOSSIP_SENDER_INFO, 4015);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zones - Kalimdor"     , GOSSIP_SENDER_INFO, 6010);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zones - Eastern Kingdoms"     , GOSSIP_SENDER_INFO, 6020);

        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
    }
    // ALLIANCE
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Major Cities"               , GOSSIP_SENDER_INFO, 2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Starting Areas"       , GOSSIP_SENDER_INFO, 4);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Instances"            , GOSSIP_SENDER_INFO, 5);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Raids"                 , GOSSIP_SENDER_INFO, 101);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gurubashi Arena" , GOSSIP_SENDER_INFO, 4015);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zones - Kalimdor"     , GOSSIP_SENDER_INFO, 6010);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zones - Eastern Kingdoms"     , GOSSIP_SENDER_INFO, 6020);

        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
    }
    return true;
}

void SendDefaultMenu_TeleportNPC(Player* pPlayer, Creature *pCreature, uint32 action)
{
    switch (action)
    {
        case 1: // Cities [HORDE]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Orgrimmar"             , GOSSIP_SENDER_MAIN, 20);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Undercity"             , GOSSIP_SENDER_MAIN, 21);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thunderbluff"          , GOSSIP_SENDER_MAIN, 22);
            //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Goldshire (Stormwind)" , GOSSIP_SENDER_MAIN, 4018);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 2: // Cities [ALLIANCE]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stormwind City"        , GOSSIP_SENDER_MAIN, 23);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ironforge"             , GOSSIP_SENDER_MAIN, 24);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Darnassus"             , GOSSIP_SENDER_MAIN, 25);
            //pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razor Hill(Orgrimmar)" , GOSSIP_SENDER_MAIN, 4017);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 3: // Starting Places [HORDE]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shadow Grave"          , GOSSIP_SENDER_MAIN, 40);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Valley of Trials"      , GOSSIP_SENDER_MAIN, 41);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Camp Narache"          , GOSSIP_SENDER_MAIN, 42);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 4: // Starting Places [ALLIANCE]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Northshire Valley"     , GOSSIP_SENDER_MAIN, 43);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Coldridge Valley"      , GOSSIP_SENDER_MAIN, 44);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shadowglen"            , GOSSIP_SENDER_MAIN, 45);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 5: // Instances [PAGE 1]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Wailing Caverns" , GOSSIP_SENDER_MAIN, 1249);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Deadmines" , GOSSIP_SENDER_MAIN, 1250);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shadowfang Keep" , GOSSIP_SENDER_MAIN, 1251);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blackfathom Deeps" , GOSSIP_SENDER_MAIN, 1252);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razorfen Kraul" , GOSSIP_SENDER_MAIN, 1254);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Razorfen Downs" , GOSSIP_SENDER_MAIN, 1256);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Scarlet Monastery" , GOSSIP_SENDER_MAIN, 1257);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->" , GOSSIP_SENDER_INFO, 5551);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 5551: // Instances [PAGE 2]
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Uldaman" , GOSSIP_SENDER_MAIN, 1258);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zul'Farrak" , GOSSIP_SENDER_MAIN, 1259);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Maraudon" , GOSSIP_SENDER_MAIN, 1260);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Sunken Temple" , GOSSIP_SENDER_MAIN, 1261);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blackrock Depths" , GOSSIP_SENDER_MAIN, 1262);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dire Maul" , GOSSIP_SENDER_MAIN, 1263);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blackrock Spire" , GOSSIP_SENDER_MAIN, 1264);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stratholme" , GOSSIP_SENDER_MAIN, 1265);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Scholomance" , GOSSIP_SENDER_MAIN, 1266);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back" , GOSSIP_SENDER_INFO, 5);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 101: // Raids
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Zul'Gurub" , GOSSIP_SENDER_MAIN, 4000);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Onyxia's Lair" , GOSSIP_SENDER_MAIN, 4001);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Molten Core" , GOSSIP_SENDER_MAIN, 4002);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blackwing Lair" , GOSSIP_SENDER_MAIN, 4003);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ruins of Ahn'Qiraj" , GOSSIP_SENDER_MAIN, 4004);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Temple of Ahn'Qiraj" , GOSSIP_SENDER_MAIN, 4005);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Naxxramas" , GOSSIP_SENDER_MAIN, 4006);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 6010: // Kalimdor
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ashenvale" , GOSSIP_SENDER_MAIN, 601);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Darkshore" , GOSSIP_SENDER_MAIN, 602);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Desolace" , GOSSIP_SENDER_MAIN, 603);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Durotar" , GOSSIP_SENDER_MAIN, 604);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dustwallow Marsh" , GOSSIP_SENDER_MAIN, 605);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Feralas" , GOSSIP_SENDER_MAIN, 606);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Silithus" , GOSSIP_SENDER_MAIN, 607);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stonetalon Mountains" , GOSSIP_SENDER_MAIN, 608);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tanaris" , GOSSIP_SENDER_MAIN, 609);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Barrens" , GOSSIP_SENDER_MAIN, 610);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thousand Needles" , GOSSIP_SENDER_MAIN, 611);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Winterspring" , GOSSIP_SENDER_MAIN, 612);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 6020: // Eastern Kingdoms
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Arathi Highlands" , GOSSIP_SENDER_MAIN, 613);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Badlands" , GOSSIP_SENDER_MAIN, 614);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Dun Morogh" , GOSSIP_SENDER_MAIN, 615);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Duskwood" , GOSSIP_SENDER_MAIN, 616);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Eastern Plaguelands" , GOSSIP_SENDER_MAIN, 617);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Elwynn Forest" , GOSSIP_SENDER_MAIN, 618);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hillsbrad Foothills" , GOSSIP_SENDER_MAIN, 619);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Redridge Mountains" , GOSSIP_SENDER_MAIN, 620);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Silverpine Forest" , GOSSIP_SENDER_MAIN, 621);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stranglethorn Vale" , GOSSIP_SENDER_MAIN, 622);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Next ->", GOSSIP_SENDER_INFO, 6021);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu"                  , GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;
        case 6021: // Eastern Kingdoms Page 2
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Swamp of Sorrows", GOSSIP_SENDER_MAIN, 623);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Hinterlands", GOSSIP_SENDER_MAIN, 624);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tirisfal Glades", GOSSIP_SENDER_MAIN, 625);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Western Plaguelands", GOSSIP_SENDER_MAIN, 626);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Westfall", GOSSIP_SENDER_MAIN, 627);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Wetlands", GOSSIP_SENDER_MAIN, 628);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_INFO, 6029);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_INFO, 100);

            pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
            break;

// ----### CITIES ###----

// ### HORDE ###

        case 20: // Orgrimmar
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 1437.0f, -4421.0f, 25.24f, 1.65f);
            break;
        case 21: // Undercity
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 1822.0999f, 238.638855f, 60.694809f, 0.0f);
            break;
        case 22: // Thunderbluff
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -1272.703735f, 116.886490f, 131.016861f, 0.0f);
            break;

// ### ALLIANCE ###

        case 23: // Stormwind
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -8828.231445f, 627.927490f, 94.055664f, 0.0f);
            break;
        case 24: // Ironforge
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -4917.0f, -955.0f, 502.0f, 0.0f);
            break;
        case 25: // Darnassus
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 9962.712891f, 2280.142822f, 1341.394409f, 0.0f);
            break;

// ----### STARTING PLACES ####----

// ### HORDE ###
        case 40: // Shadow Grave
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 1663.517f, 1678.187744f, 120.5303f, 0.0f);
            break;
        case 41: // Valley of Trials
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -602.1253f, -4262.4208f, 38.956341f, 0.0f);
            break;
        case 42: // Camp Narache
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -2914.16992f, -266.061798f, 53.658211f, 0.0f);
            break;

// ### ALLIANCE ###

        case 43: // Nortshire Valley
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -8943.133789f, -132.934921f, 83.704269f, 0.0f);
            break;
        case 44: // Coldridge Valley
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -6231.106445f, 332.270477f, 383.153931f, 0.0f);
            break;
        case 45: // Shadowglen
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 10329.918945f, 833.500305f, 1326.260620f, 0.0f);
            break;

// ----### INSTANCES ###----

        case 50: // Dire Maul
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -3762.340820f, 1216.537598f, 127.434608f, 0.0f);
            break;
        case 51: // Blackrock Spire
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -7528.554688f, -1222.907227f, 285.732941f, 0.0f);
            break;
        case 52: // Zul'Gurub
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -11916.179688f, -1190.977051f, 85.137901f, 0.0f);
            break;
        case 53: // Onyxia's Lair
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4682.391602f, -3709.857422f, 46.792862f, 0.0f);
            break;
        case 54: // Searing Gorge (Moltencore, Blackwinglair)
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -7342.270020f, -1096.863892f, 277.06930f, 0.0f);
            break;
        case 55: // Naxxramas
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 3121.061768f, -3689.973389f, 133.458786f, 0.0f);
            break;
        case 56: // Stratholme Backdoor Entrance
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 3160.416992f, -4038.750244f, 104.177376f, 0.0f);
            break;
        case 57: // Stratholme Main Entrance
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 3357.214355f, -3379.713135f, 144.780853f, 0.0f);
            break;
        case 58: // Scholomance
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 1252.319824f, -2587.151123f, 92.886772f, 0.0f);
            break;

// ---### ZONES ###---

        case 70: // Silithus
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -7430.070313f, 1002.554565f, 1.249787f, 0.0f);
            break;
        case 71: // Durotar
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 1006.426819f, -4439.258789f, 11.352882f, 0.0f);
            break;
        case 72: // Ashenvale
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 1922.842651f, -2169.429688f, 94.327400f, 0.0f);
            break;
        case 73: // Tanaris
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -7180.401367f, -3773.328369f, 8.728320f, 0.0f);
            break;
        case 4015:// Gurubashi
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -13261.3f, 168.294f, 35.0792f, 1.00688f);
            break;
        case 4017:// Razor Hill
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 315.721f, -4743.4f, 10.4867f, 0.0f);
            break;
        case 4018:// Goldshire
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -9464.0f, 62.0f, 56.0f, 0.0f);
            break;
        case 1249://teleport player to the Wailing Caverns
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -722.53f, -2226.30f, 16.94f, 2.71f);
            break;
        case 1250://teleport player to the Deadmines
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -11212.04f, 1658.58f, 25.67f, 1.45f);
            break;
        case 1251://teleport player to Shadowfang Keep
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -254.47f, 1524.68f, 76.89f, 1.56f);
            break;
        case 1252://teleport player to Blackfathom Deeps
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 4254.58f, 664.74f, -29.04f, 1.97f);
            break;
        case 1254://teleport player to Razorfen Kraul
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4484.04f, -1739.40f, 86.47f, 1.23f);
            break;
        case 1256://teleport player to Razorfen Downs
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4645.08f, -2470.85f, 85.53f, 4.39f);
            break;
        case 1257://teleport player to the Scarlet Monastery
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 2843.89f, -693.74f, 139.32f, 5.11f);
            break;
        case 1258://teleport player to Uldaman
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -6119.70f, -2957.30f, 204.11f, 0.03f);
            break;
        case 1259://teleport player to Zul'Farrak
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -6839.39f, -2911.03f, 8.87f, 0.41f);
            break;
        case 1260://teleport player to Maraudon
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -1433.33f, 2955.34f, 96.21f, 4.82f);
            break;
        case 1261://teleport player to the Sunken Temple
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -10346.92f, -3851.90f, -43.41f, 6.09f);
            break;
        case 1262://teleport player to Blackrock Depths
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -7301.03f, -913.19f, 165.37f, 0.08f);
            break;
        case 1263://teleport player to Dire Maul
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -3982.47f, 1127.79f, 161.02f, 0.05f);
            break;
        case 1264://teleport player to Blackrock Spire
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -7535.43f, -1212.04f, 285.45f, 5.29f);
            break;
        case 1265://teleport player to Stratholme
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 3263.54f, -3379.46f, 143.59f, 0.00f);
            break;
        case 1266://teleport player to Scholomance
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 1219.01f, -2604.66f, 85.61f, 0.50f);
            break;
        case 4000:// Teleport to Zul'Gurub
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -11916.7f, -1212.82f, 92.2868f, 4.6095f);
            break;
        case 4001:// Teleport to Onyxia's Lair
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4707.44f, -3726.82f, 54.6723f, 3.8f);
            break;
        case 4002:// Teleport to Molten Core
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(409, 1096.0f, -467.0f, -104.6f, 3.64f);
            break;
        case 4003:// Teleport to Blackwing Lair
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(469, -7665.55f, -1102.49f, 400.679f, 0.0f);
            break;
        case 4004:// Ruins of Ahn'Qiraj
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -8409.032227f, 1498.830933f, 27.361542f, 2.497567f);
            break;
        case 4005:// Temple of Ahn'Qiraj
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -8245.837891f, 1983.736206f, 129.071686f, 0.936195f);
            break;
        case 4006:// Naxxramas
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(533, 3005.87f, -3435.0f, 293.89f, 0.0f);
            break;
        case 601: // Kalimdor -> Ashenvale
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 3469.43f, 847.62f, 6.36476f, 0.0f);
            break;
        case 602: // Kalimdor -> Darkshore
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 6207.5f, -152.833f, 80.8185f, 0.0f);
            break;
        case 603: // Kalimdor -> Desolace
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -93.1614f, 1691.15f, 90.0649f, 0.0f);
            break;
        case 604: // Kalimdor -> Durotar
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 341.42f, -4684.7f, 31.9493f, 0.0f);
            break;
        case 605: // Kalimdor -> Duswallow Marsh
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -3463.26f, -4123.13f, 18.1043f, 0.0f);
            break;
        case 606: // Kalimdor -> Ferelas
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4458.93f, 243.415f, 65.6136f, 0.0f);
            break;
        case 607: // Kalimdor -> Silithus
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -6824.15f, 821.273f, 50.6675f, 0.0f);
            break;
        case 608: // Kalimdor -> Stonetalon Mountains
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 1145.00f, 85.664f, -6.64f, 0.0f);
            break;
        case 609: // Kalimdor -> Tanaris
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -7373.69f, -2950.2f, 11.7598f, 0.0f);
            break;
        case 610: // Kalimdor -> The Barrens
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -456.263f, -2652.7f, 96.615f, 0.0f);
            break;
        case 611: // Kalimdor -> Thousand Needles
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, -4941.66f, -1595.42f, -33.07f, 0.0f);
            break;
        case 612: // Kalimdor -> Winterspring
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(1, 6107.62f, -4181.6f, 853.322f, 0.0f);
            break;
        case 613: // Eastern Kingdoms -> Arathi Highlands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -1544.93f, -2495.01f, 54.11f, 0.0f);
            break;
        case 614: // Eastern Kingdoms -> Badlands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -6084.95f, -3328.44f, 253.76f, 0.0f);
            break;
        case 615: // Eastern Kingdoms -> Dun Morogh
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -5660.33f, 755.299f, 390.605f, 0.0f);
            break;
        case 616: // Eastern Kingdoms -> Duskwood
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -10879.85f, -327.06f, 37.78f, 0.0f);
            break;
        case 617: // Eastern Kingdoms -> Eastern Plaguelands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 2280.12f, -5313.16f, 87.74f, 0.0f);
            break;
        case 618: // Eastern Kingdoms -> Elwynn Forest
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -9545.78f, -51.81f, 56.72f, 0.0f);
            break;
        case 619: // Eastern Kingdoms -> Hillsbrad Foothills
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -436.03f, -583.27f, 53.58f, 0.0f);
            break;
        case 620: // Eastern Kingdoms -> Reridge Mountains
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -9633.80f, -1875.09f, 68.39f, 0.0f);
            break;
        case 621: // Eastern Kingdoms -> Silverpine Forest
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 641.48f, 1297.84f, 85.45f, 0.0f);
            break;
        case 622: // Eastern Kingdoms -> Stranglethorn Vale
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -11393.77f, -288.94f, 58.99f, 0.0f);
            break;
        case 623: // Eastern Kingdoms -> Swamp of Sorrows
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -10522.55f, -3028.30f, 21.79f, 0.0f);
            break;
        case 624: // Eastern Kingdoms -> The Hinterlands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 155.14f, -2014.95f, 122.55f, 0.0f);
            break;
        case 625: // Eastern Kingdoms -> Tirishfal Glades
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 2255.5f, 288.511f, 35.1138f, 0.0f);
            break;
        case 626: // Eastern Kingdoms -> Western Plaguelands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, 1728.65f, -1602.25f, 63.429f, 1.6558f);
            break;
        case 627: // Eastern Kingdoms -> Westfall
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -10919.09f, 998.47f, 35.47f, 0.0f);
            break;
        case 628: // Eastern Kingdoms -> Wetlands
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TeleportTo(0, -4086.36f, -2610.95f, 47.0143f, 0.0f);
            break;

        case 100: // Main Menu
            GossipHello_TeleportNPC(pPlayer, pCreature);
    }
}
bool GossipSelect_TeleportNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    // Main menu
    SendDefaultMenu_TeleportNPC(pPlayer, pCreature, action);

    if (sender == GOSSIP_SENDER_MAIN)
        pPlayer->CastSpell(pPlayer, 26638, true);

    return true;
}

enum Enchants
{
    WEP2H_SUPERIOR_IMPACT = 20,
    WEP2H_AGILITY,
    WEP_CRUSADER,
    WEP1H_AGILITY,
    WEP_SPELLPOWER,
    WEP_HEAL,
    OFFHAND_SPIRIT,
    OFFHAND_STAM,
    OFFHAND_FROSTRES,
    CHEST_STATS,
    CLOAK_DODGE,
    CLOAK_SUB,
    CLOAK_ARMOR,
    CLOAK_AGILITY,
    BRACER_STAM,
    BRACER_STR,
    BRACER_HEAL,
    BRACER_INT,
    GLOVES_AGI,
    GLOVES_FIRE,
    GLOVES_FROST,
    GLOVES_SHADOW,
    GLOVES_HEALING,
    BOOTS_AGI,
    BOOTS_SPEED,
    BOOTS_STAM,
};

void Enchant(Player* pPlayer, Item* item, uint32 enchantid)
{
    if (!item)
    {
        pPlayer->GetSession()->SendNotification("You must first equip the item you are trying to enchant.");
        return;
    }

    if (!enchantid)
    {
        pPlayer->GetSession()->SendNotification("Something went wrong.");
        return;
    }

    item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
    pPlayer->GetSession()->SendNotification("%s succesfully enchanted", item->GetProto()->Name1);
}

bool GossipHello_EnchantNPC(Player* pPlayer, Creature* pCreature)
{

    pPlayer->ADD_GOSSIP_ITEM(5, "Chest", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_CHEST);
    pPlayer->ADD_GOSSIP_ITEM(5, "Cloak", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_BACK);
    pPlayer->ADD_GOSSIP_ITEM(5, "Bracers", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_WRISTS);
    pPlayer->ADD_GOSSIP_ITEM(5, "Gloves", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_HANDS);
    pPlayer->ADD_GOSSIP_ITEM(5, "Boots", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_FEET);
    pPlayer->ADD_GOSSIP_ITEM(5, "Mainhand", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_MAINHAND);
    pPlayer->ADD_GOSSIP_ITEM(5, "Offhand", GOSSIP_SENDER_MAIN, EQUIPMENT_SLOT_OFFHAND);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}
bool GossipSelect_EnchantNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (sender != GOSSIP_SENDER_MAIN)
        return true;

    if (action < 20)
    {
        switch (action)
        {
            case EQUIPMENT_SLOT_CHEST:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Greater Stats", GOSSIP_SENDER_MAIN, CHEST_STATS);
                break;
            }
            case EQUIPMENT_SLOT_BACK:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Agility", GOSSIP_SENDER_MAIN, CLOAK_AGILITY);
                pPlayer->ADD_GOSSIP_ITEM(5, "Armor", GOSSIP_SENDER_MAIN, CLOAK_ARMOR);
                pPlayer->ADD_GOSSIP_ITEM(5, "Dodge", GOSSIP_SENDER_MAIN, CLOAK_DODGE);
                pPlayer->ADD_GOSSIP_ITEM(5, "Subtlety", GOSSIP_SENDER_MAIN, CLOAK_SUB);
                break;
            }
            case EQUIPMENT_SLOT_WRISTS:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Stamina", GOSSIP_SENDER_MAIN, BRACER_STAM);
                pPlayer->ADD_GOSSIP_ITEM(5, "Strength", GOSSIP_SENDER_MAIN, BRACER_STR);
                pPlayer->ADD_GOSSIP_ITEM(5, "Healing", GOSSIP_SENDER_MAIN, BRACER_HEAL);
                pPlayer->ADD_GOSSIP_ITEM(5, "Intellect", GOSSIP_SENDER_MAIN, BRACER_INT);
                break;
            }
            case EQUIPMENT_SLOT_HANDS:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Agility", GOSSIP_SENDER_MAIN, GLOVES_AGI);
                pPlayer->ADD_GOSSIP_ITEM(5, "Fire Power", GOSSIP_SENDER_MAIN, GLOVES_FIRE);
                pPlayer->ADD_GOSSIP_ITEM(5, "Frost Power", GOSSIP_SENDER_MAIN, GLOVES_FROST);
                pPlayer->ADD_GOSSIP_ITEM(5, "Shadow Power", GOSSIP_SENDER_MAIN, GLOVES_SHADOW);
                pPlayer->ADD_GOSSIP_ITEM(5, "Healing", GOSSIP_SENDER_MAIN, GLOVES_HEALING);
                break;
            }
            case EQUIPMENT_SLOT_FEET:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Stamina", GOSSIP_SENDER_MAIN, BOOTS_STAM);
                pPlayer->ADD_GOSSIP_ITEM(5, "Minor Speed", GOSSIP_SENDER_MAIN, BOOTS_SPEED);
                pPlayer->ADD_GOSSIP_ITEM(5, "Agility", GOSSIP_SENDER_MAIN, BOOTS_AGI);
                break;
            }
            case EQUIPMENT_SLOT_MAINHAND:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Crusader", GOSSIP_SENDER_MAIN, WEP_CRUSADER);
                pPlayer->ADD_GOSSIP_ITEM(5, "1H Agility", GOSSIP_SENDER_MAIN, WEP1H_AGILITY);
                pPlayer->ADD_GOSSIP_ITEM(5, "2H Agility", GOSSIP_SENDER_MAIN, WEP2H_AGILITY);
                pPlayer->ADD_GOSSIP_ITEM(5, "Spellpower", GOSSIP_SENDER_MAIN, WEP_SPELLPOWER);
                pPlayer->ADD_GOSSIP_ITEM(5, "Healing", GOSSIP_SENDER_MAIN, WEP_HEAL);
                break;
            }
            case EQUIPMENT_SLOT_OFFHAND:
            {
                pPlayer->ADD_GOSSIP_ITEM(5, "Spirit", GOSSIP_SENDER_MAIN, OFFHAND_SPIRIT);
                pPlayer->ADD_GOSSIP_ITEM(5, "Stamina", GOSSIP_SENDER_MAIN, OFFHAND_STAM);
                pPlayer->ADD_GOSSIP_ITEM(5, "Frost Resistance", GOSSIP_SENDER_MAIN, OFFHAND_FROSTRES);
                break;
            }
        }
        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
    }
    else
    {
        Item* item = nullptr;
        uint32 id = 0;
        switch (action)
        {
            case WEP2H_SUPERIOR_IMPACT:
            case WEP2H_AGILITY:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                if (item && (action == WEP2H_AGILITY || action == WEP2H_SUPERIOR_IMPACT))
                {
                    if (item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_MACE2
                        && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM
                        && item->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
                    {
                        pPlayer->GetSession()->SendNotification("Requires 2 handed weapon");
                        pPlayer->CLOSE_GOSSIP_MENU();
                        return true;
                    }
                }
                if (action == WEP2H_SUPERIOR_IMPACT)
                    id = 1896;
                else if (action == WEP2H_AGILITY)
                    id = 2646;
                break;
            }
            case WEP_CRUSADER:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                id = 1900;
                break;
            }
            case WEP1H_AGILITY:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                id = 2564;
                break;
            }
            case WEP_SPELLPOWER:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                id = 2504;
                break;
            }
            case WEP_HEAL:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                id = 2505;
                break;
            }
            case OFFHAND_SPIRIT:
            case OFFHAND_STAM:
            case OFFHAND_FROSTRES:
            {
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                if (item && item->GetProto()->SubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
                {
                    pPlayer->GetSession()->SendNotification("Requires Shield");
                    pPlayer->CLOSE_GOSSIP_MENU();
                    return true;
                }
                if (action == OFFHAND_SPIRIT)
                    id = 1890;
                else if (action == OFFHAND_FROSTRES)
                    id = 926;
                else if (action == OFFHAND_STAM)
                    id = 929;
                break;
            }
            case CHEST_STATS:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST);
                id = 1891;
                break;
            case CLOAK_DODGE:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
                id = 2622;
                break;
            case CLOAK_SUB:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
                id = 2621;
                break;
            case CLOAK_ARMOR:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
                id = 1889;
                break;
            case CLOAK_AGILITY:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK);
                id = 849;
                break;
            case BRACER_STAM:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
                id = 1886;
                break;
            case BRACER_STR:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
                id = 1885;
                break;
            case BRACER_HEAL:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
                id = 2566;
                break;
            case BRACER_INT:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS);
                id = 1883;
                break;
            case GLOVES_AGI:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
                id = 2564;
                break;
            case GLOVES_FIRE:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
                id = 2616;
                break;
            case GLOVES_FROST:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
                id = 2615;
                break;
            case GLOVES_SHADOW:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
                id = 2614;
                break;
            case GLOVES_HEALING:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS);
                id = 2617;
                break;
            case BOOTS_AGI:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
                id = 904;
                break;
            case BOOTS_SPEED:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
                id = 911;
                break;
            case BOOTS_STAM:
                item = pPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET);
                id = 929;
                break;
        }
        Enchant(pPlayer, item, id);
        pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}



void LearnSkillRecipesHelper(Player* pPlayer, uint32 skill_id)
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

bool LearnAllRecipesInProfession(Player* pPlayer, SkillType skill)
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
    LearnSkillRecipesHelper(pPlayer, SkillInfo->id);
    pPlayer->GetSession()->SendNotification("All recipes for %s learned", skill_name);
    return true;
}

bool UnLearnAllRecipesInProfession(Player* pPlayer, uint32 skill_id)
{
    for (uint32 j = 0; j < sObjectMgr.GetMaxSkillLineAbilityId(); ++j)
    {
        SkillLineAbilityEntry const* skillLine = sObjectMgr.GetSkillLineAbility(j);
        if (!skillLine)
            continue;

        // wrong skill
        if (skillLine->skillId != skill_id)
            continue;

        pPlayer->RemoveSpell(skillLine->spellId, false, false);
    }
    return true;
}

#define GOSSIP_SENDER_LEARN 1
#define GOSSIP_SENDER_UNLEARN 2

bool GossipHello_ProfessionNPC(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    // Second Prof.
    if (!pPlayer->HasSkill(SKILL_FIRST_AID) || pPlayer->GetSkillValue(SKILL_FIRST_AID) < 300)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me everything of First Aid.", GOSSIP_SENDER_LEARN, SKILL_FIRST_AID);
    if (!pPlayer->HasSkill(SKILL_FISHING) || pPlayer->GetSkillValue(SKILL_FISHING) < 300)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me everything of Fishing.", GOSSIP_SENDER_LEARN, SKILL_FISHING);
    if (!pPlayer->HasSkill(SKILL_COOKING) || pPlayer->GetSkillValue(SKILL_COOKING) < 300)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me everything of Cooking.", GOSSIP_SENDER_LEARN, SKILL_COOKING);

    // Learn and unlearn.
    if (pPlayer->HasSkill(SKILL_ALCHEMY))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Alchemy.", GOSSIP_SENDER_UNLEARN, SKILL_ALCHEMY);
    else if (!pPlayer->HasSkill(SKILL_ALCHEMY) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Alchemy.", GOSSIP_SENDER_LEARN, SKILL_ALCHEMY);
    
    if (pPlayer->HasSkill(SKILL_BLACKSMITHING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Blacksmithing.", GOSSIP_SENDER_UNLEARN, SKILL_BLACKSMITHING);
    else if (!pPlayer->HasSkill(SKILL_BLACKSMITHING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Blacksmithing.", GOSSIP_SENDER_LEARN, SKILL_BLACKSMITHING);

    if (pPlayer->HasSkill(SKILL_LEATHERWORKING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Leatherworking.", GOSSIP_SENDER_UNLEARN, SKILL_LEATHERWORKING);
    else if (!pPlayer->HasSkill(SKILL_LEATHERWORKING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Leatherworking.", GOSSIP_SENDER_LEARN, SKILL_LEATHERWORKING);

    if (pPlayer->HasSkill(SKILL_TAILORING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Tailoring.", GOSSIP_SENDER_UNLEARN, SKILL_TAILORING);
    else if (!pPlayer->HasSkill(SKILL_TAILORING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Tailoring.", GOSSIP_SENDER_LEARN, SKILL_TAILORING);

    if (pPlayer->HasSkill(SKILL_ENGINEERING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Engineering.", GOSSIP_SENDER_UNLEARN, SKILL_ENGINEERING);
    else if (!pPlayer->HasSkill(SKILL_ENGINEERING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Engineering.", GOSSIP_SENDER_LEARN, SKILL_ENGINEERING);

    if (pPlayer->HasSkill(SKILL_ENCHANTING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Enchanting.", GOSSIP_SENDER_UNLEARN, SKILL_ENCHANTING);
    else if (!pPlayer->HasSkill(SKILL_ENCHANTING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Enchanting.", GOSSIP_SENDER_LEARN, SKILL_ENCHANTING);

    if (pPlayer->HasSkill(SKILL_HERBALISM))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Herbalism.", GOSSIP_SENDER_UNLEARN, SKILL_HERBALISM);
    else if (!pPlayer->HasSkill(SKILL_HERBALISM) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Herbalism.", GOSSIP_SENDER_LEARN, SKILL_HERBALISM);

    if (pPlayer->HasSkill(SKILL_SKINNING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Skinning.", GOSSIP_SENDER_UNLEARN, SKILL_SKINNING);
    else if (!pPlayer->HasSkill(SKILL_SKINNING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Skinning.", GOSSIP_SENDER_LEARN, SKILL_SKINNING);

    if (pPlayer->HasSkill(SKILL_MINING))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, "Unlearn Mining.", GOSSIP_SENDER_UNLEARN, SKILL_MINING);
    else if (!pPlayer->HasSkill(SKILL_MINING) && pPlayer->GetFreePrimaryProfessionPoints() > 0)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Mining.", GOSSIP_SENDER_LEARN, SKILL_MINING);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_ProfessionNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    switch (action)
    {
    case SKILL_ALCHEMY:
        if (pPlayer->HasSkill(SKILL_ALCHEMY) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_ALCHEMY);
        else if (!pPlayer->HasSkill(SKILL_ALCHEMY) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_ALCHEMY);
        break;
    case SKILL_BLACKSMITHING:
        if (pPlayer->HasSkill(SKILL_BLACKSMITHING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_BLACKSMITHING);
        else if (!pPlayer->HasSkill(SKILL_BLACKSMITHING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_BLACKSMITHING);
        break;
    case SKILL_LEATHERWORKING:
        if (pPlayer->HasSkill(SKILL_LEATHERWORKING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_LEATHERWORKING);
        else if (!pPlayer->HasSkill(SKILL_LEATHERWORKING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_LEATHERWORKING);
        break;
    case SKILL_TAILORING:
        if (pPlayer->HasSkill(SKILL_TAILORING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_TAILORING);
        else if (!pPlayer->HasSkill(SKILL_TAILORING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_TAILORING);
        break;
    case SKILL_ENGINEERING:
        if (pPlayer->HasSkill(SKILL_ENGINEERING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_ENGINEERING);
        else if (!pPlayer->HasSkill(SKILL_ENGINEERING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_ENGINEERING);
        break;
    case SKILL_ENCHANTING:
        if (pPlayer->HasSkill(SKILL_ENCHANTING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_ENCHANTING);
        else if (!pPlayer->HasSkill(SKILL_ENCHANTING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_ENCHANTING);
        break;
    case SKILL_HERBALISM:
        if (pPlayer->HasSkill(SKILL_HERBALISM) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_HERBALISM);
        else if (!pPlayer->HasSkill(SKILL_HERBALISM) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_HERBALISM);
        break;
    case SKILL_SKINNING:
        if (pPlayer->HasSkill(SKILL_SKINNING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_SKINNING);
        else if (!pPlayer->HasSkill(SKILL_SKINNING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_SKINNING);
        break;
    case SKILL_MINING:
        if (pPlayer->HasSkill(SKILL_MINING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_MINING);
        else if (!pPlayer->HasSkill(SKILL_MINING) && pPlayer->GetFreePrimaryProfessionPoints() > 0 && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_MINING);
        break;
    case SKILL_FIRST_AID:
        if (pPlayer->HasSkill(SKILL_FIRST_AID) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_FIRST_AID);
        else if (!pPlayer->HasSkill(SKILL_FIRST_AID) && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_FIRST_AID);
        break;
    case SKILL_FISHING:
        if (pPlayer->HasSkill(SKILL_FISHING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_FISHING);
        else if (!pPlayer->HasSkill(SKILL_FISHING) && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_FISHING);
        break;
    case SKILL_COOKING:
        if (pPlayer->HasSkill(SKILL_COOKING) && sender == GOSSIP_SENDER_UNLEARN)
            UnLearnAllRecipesInProfession(pPlayer, SKILL_COOKING);
        else if (!pPlayer->HasSkill(SKILL_COOKING) && sender == GOSSIP_SENDER_LEARN)
            LearnAllRecipesInProfession(pPlayer, SKILL_COOKING);
        break;
    }

    if (sender == GOSSIP_SENDER_LEARN)
        pPlayer->CastSpell(pPlayer, 17321, true);
    if (sender == GOSSIP_SENDER_UNLEARN)
        pPlayer->CastSpell(pPlayer, 14867, true);

    GossipHello_ProfessionNPC(pPlayer, pCreature);
    return true;
}

/*
* Custom premade gear and spec scripts
*/

#define SPELL_LIGHTNING_VISUAL 24240

bool GossipHello_PremadeGearNPC(Player* pPlayer, Creature* pCreature)
{
    for (auto itr : sObjectMgr.GetPlayerPremadeGearTemplates())
    {
        if (itr.second.requiredClass == pPlayer->GetClass())
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, itr.second.name.c_str(), GOSSIP_SENDER_MAIN, itr.first);
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_PremadeGearNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->SendSpellGo(pPlayer, SPELL_LIGHTNING_VISUAL);
    sObjectMgr.ApplyPremadeGearTemplateToPlayer(action, pPlayer);
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GossipHello_PremadeSpecNPC(Player* pPlayer, Creature* pCreature)
{
    for (auto itr : sObjectMgr.GetPlayerPremadeSpecTemplates())
    {
        if (itr.second.requiredClass == pPlayer->GetClass())
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_2, itr.second.name.c_str(), GOSSIP_SENDER_MAIN, itr.first);
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_PremadeSpecNPC(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->SendSpellGo(pPlayer, SPELL_LIGHTNING_VISUAL);
    sObjectMgr.ApplyPremadeSpecTemplateToPlayer(action, pPlayer);
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*
* Custom training dummy script
*/

struct npc_training_dummyAI : ScriptedAI
{
    explicit npc_training_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_training_dummyAI::Reset();
    }

    uint32 m_uiCombatTimer;
    std::unordered_map<ObjectGuid, time_t> attackers;

    void Reset() override
    {
        m_uiCombatTimer = 15000;
        attackers.clear();
    }

    void AttackStart(Unit* /*pWho*/) override {}

    void Aggro(Unit* pWho) override
    {
        SetCombatMovement(false);
    }

    void AddAttackerToList(Unit* pWho)
    {
        auto itr = attackers.find(pWho->GetObjectGuid());
        if (itr != attackers.end())
        {
            itr->second = std::time(nullptr);
        }
        else
        {
            attackers.emplace(pWho->GetObjectGuid(), std::time(nullptr));
        }
    }

    void DamageTaken(Unit* pWho, uint32& /*uiDamage*/) override
    {
        if (pWho)
            AddAttackerToList(pWho);
    }

    void SpellHit(SpellCaster* pWho, SpellEntry const* /*pSpell*/) override
    {
        if (Unit* pAttacker = ToUnit(pWho))
            AddAttackerToList(pAttacker);
    }

    void UpdateAI(uint32 const diff) override
    {
        if (m_creature->IsInCombat())
        {
            if (m_uiCombatTimer <= diff)
            {
                for (auto itr = attackers.begin(); itr != attackers.end();)
                {
                    Unit* pAttacker = m_creature->GetMap()->GetUnit(itr->first);

                    if (!pAttacker || !pAttacker->IsInWorld())
                    {
                        itr = attackers.erase(itr);
                        continue;
                    }
                    if (itr->second + 10 < std::time(nullptr))
                    {
                        m_creature->_removeAttacker(pAttacker);
                        m_creature->GetThreatManager().modifyThreatPercent(pAttacker, -101.0f);
                        itr = attackers.erase(itr);
                        continue;
                    }
                    ++itr;
                }

                if (m_creature->GetThreatManager().isThreatListEmpty())
                    EnterEvadeMode();

                m_uiCombatTimer = 15000;
            }
            else
                m_uiCombatTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
    return new npc_training_dummyAI(pCreature);
}

struct npc_summon_debugAI : ScriptedAI
{
    uint32 m_maxSummonCount = 200;
    uint32 m_summonCount = 0;
    Creature *m_summons[200];

    npc_summon_debugAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_summonCount = 0;
        for (uint32 i = 0; i < m_maxSummonCount; ++i)
            m_summons[i] = nullptr;

        Reset();
    }

    void Reset() override
    {
        m_summonCount = 0;
        for (uint32 i = 0; i < m_maxSummonCount; ++i)
        {
            if (m_summons[i])
                ((TemporarySummon*)m_summons[i])->UnSummon();

            m_summons[i] = nullptr;
        }
    }


    void JustDied(Unit* /* killer */) override
    {
        Reset();
    }

    void UpdateAI(uint32 const diff) override
    {
        if (!m_creature->GetVictim())
            return;

        if (m_summonCount >= m_maxSummonCount)
            return;

        m_summons[m_summonCount++] = m_creature->SummonCreature(12458, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0);
    }
};

CreatureAI* GetAI_custom_summon_debug(Creature *creature)
{
    return new npc_summon_debugAI(creature);
}

bool CheckQuestItemLevels(Player* pPlayer)
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
                if (pProto->RequiredLevel == 0 && pProto->Stackable == 1 && (pProto->Bonding == BIND_WHEN_PICKED_UP || pProto->Bonding == BIND_QUEST_ITEM))
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
                        ++counter2;

                    ++counter;
                }
            }

        } while (result->NextRow());

        if (!counter && !counter2)
            return false;
        else
            return true;

    }
    return false;
}

void FixQuestItemLevels(Player* pPlayer)
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
                if (pProto->RequiredLevel == 0 && pProto->Stackable == 1 && (pProto->Bonding == BIND_WHEN_PICKED_UP || pProto->Bonding == BIND_QUEST_ITEM))
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
    
        sObjectMgr.LoadItemPrototypes();

        std::ostringstream ss;
        ss << "Fixed " << counter << " required_level and " << counter2 << " max_count.";
        ChatHandler(pPlayer->GetSession()).PSendSysMessage(ss.str().c_str());
    }
}

bool GossipHello_AuctionhouseBot(Player* pPlayer, Creature* pCreature)
{
    // remove fake death
    if (pPlayer->HasUnitState(UNIT_STAT_FEIGN_DEATH))
        pPlayer->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);
    // Only admin can proceed this.
    if (pPlayer->GetSession()->GetSecurity() >= SEC_ADMINISTRATOR && CheckQuestItemLevels(pPlayer))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fix quest item levels for Auctionhousebot items.", 1, 1);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    else
    {
        pPlayer->SetAuctionAccessMode(1);
        pPlayer->GetSession()->SendAuctionHello(pPlayer);
    }

    return true;
}

bool GossipSelect_AuctionhouseBot(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->SendSpellGo(pPlayer, SPELL_LIGHTNING_VISUAL);
    FixQuestItemLevels(pPlayer);
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

enum
{
    EVENT_EITRIGG_SWITCH_TARGET = 1,
    EVENT_EITRIGG_START         = 2,
    SPELL_WHIRLWIND             = 26083,
    SPELL_EITRIGG_ENRAGE        = 24318,
    SPELL_SLEEP                 = 19231,
    SPELL_WARSONGFLAG           = 23333,
    SPELL_BREAK_BIG_STUFF       = 7962,
    SPELL_SUMMON_FLAMES         = 19629,
    BROADCASTTEXT1              = 6171,
    BROADCASTTEXT2              = 6172,
    BROADCASTTEXT3              = 1345,
    VAR_EITRIGGKILLS            = 600071,
    GOBJ_BIG_BARRACKS_FLAME     = 176746,
    GOBJ_SMALL_BARRACKS_FLAME   = 176747
};

Unit* SelectRandomEnemy(Creature* pUnit, Unit* except, float radius)
{
    std::list<Unit*> targets;

    MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(pUnit, pUnit, radius);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(pUnit, searcher, radius);

    // remove current target
    if (except)
        targets.remove(except);

    for (std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!(*tIter)->IsCreature() || !pUnit->IsValidAttackTarget((*tIter)) || (*tIter)->GetEntry() == NPC_NECROTIC_SHARD || (*tIter)->GetEntry() == NPC_DAMAGED_NECROTIC_SHARD)
        {
            std::list<Unit*>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return nullptr;

    // select random
    uint32 rIdx = urand(0, targets.size() - 1);
    std::list<Unit*>::const_iterator tcIter = targets.begin();
    for (uint32 i = 0; i < rIdx; ++i)
        ++tcIter;

    return *tcIter;
}

struct BossEitrigg : public ScriptedAI
{
    BossEitrigg(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_events.Reset();
        Reset();
    }

    EventMap m_events;
    uint32 kills = NULL;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        DoScriptText(BROADCASTTEXT2, m_creature, m_creature, CHAT_TYPE_YELL);
        m_events.Reset();
        m_events.ScheduleEvent(EVENT_EITRIGG_START, urand(160000, 180000));
        m_creature->AddAura(SPELL_WARSONGFLAG);
        m_creature->SetStandState(UNIT_STAND_STATE_SIT);
        sObjectMgr.SetSavedVariable(VAR_EITRIGGKILLS, (kills + sObjectMgr.GetSavedVariable(VAR_EITRIGGKILLS)), true);
        kills = NULL;
    }

    void EnterEvadeMode() override
    {
        // Prevents Eitrigg from reseting if he still has enrage.
        if (m_creature->HasAura(SPELL_EITRIGG_ENRAGE))
        {
            switchTarget(true);
            return;
        }

        ScriptedAI::EnterEvadeMode();
    }

    void KilledUnit(Unit* pWho) override
    {
        if (!pWho)
            return;

        pWho->SendSpellGo(pWho, 7962);
        std::list<GameObject*> FireList;
        GetGameObjectListWithEntryInGrid(FireList, m_creature, { GOBJ_SMALL_BARRACKS_FLAME, GOBJ_BIG_BARRACKS_FLAME }, ATTACK_DISTANCE);
        if (FireList.empty())
        {
            m_creature->CastSpell(pWho, SPELL_SUMMON_FLAMES, true);
        }

        kills++;

        if (m_creature->HasAura(SPELL_EITRIGG_ENRAGE))
            switchTarget(true);
    }

    bool switchTarget(bool attack)
    {
        if (m_creature->GetVictim() && m_creature->GetVictim()->IsAlive() && m_creature->IsInRange(m_creature->GetVictim(), 0.0f, ATTACK_DISTANCE, false))
            return false;

        if (Unit* pTarget = SelectRandomEnemy(m_creature, m_creature, VISIBILITY_DISTANCE_LARGE))
        {
            if (attack && pTarget->IsValidAttackTarget(m_creature))
            {
                m_creature->UpdateSpeed(MOVE_RUN, true, 2.4f);
                m_creature->AI()->AttackStart(pTarget);
            }
            return true;
        }
        else
            m_creature->RemoveAurasDueToSpell(SPELL_EITRIGG_ENRAGE);

        return false;
    }

    void UpdateAI(uint32 const diff) override
    {
        m_events.Update(diff);

        while (uint32 Events = m_events.ExecuteEvent())
        {
            switch (Events)
            {
                case EVENT_EITRIGG_SWITCH_TARGET:
                {
                    switchTarget(true);
                    m_events.ScheduleEvent(EVENT_EITRIGG_SWITCH_TARGET, urand(2000,5000));
                    break;
                }
                case EVENT_EITRIGG_START:
                {
                    // Only start the event if there are some enemies around him.
                    if (switchTarget(false))
                    {
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(PickRandomValue(BROADCASTTEXT3, BROADCASTTEXT1), m_creature, m_creature, CHAT_TYPE_YELL);
                        m_creature->CastSpell(m_creature, SPELL_EITRIGG_ENRAGE, true);
                        m_events.ScheduleEvent(EVENT_EITRIGG_SWITCH_TARGET, 3000);
                    }
                    else
                        m_events.ScheduleEvent(EVENT_EITRIGG_START, urand(60000, 80000));

                    break;
                }
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (!m_creature->HasAura(SPELL_WHIRLWIND))
            m_creature->CastSpell(m_creature, SPELL_WHIRLWIND, true);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_BossEitrigg(Creature* pCreature)
{
    return new BossEitrigg(pCreature);
}

bool GossipHello_BossEitrigg(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "How many monsters have you already killed?", 1, 1);
    pPlayer->SEND_GOSSIP_MENU(6993, pCreature->GetGUID());

    return true;
}

bool GossipSelect_BossEitrigg(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    int numb = sObjectMgr.GetSavedVariable(VAR_EITRIGGKILLS);
    std::ostringstream ss;
    ss << "I've already killed " << numb << " of these ugly motherfuckers!";
    pCreature->MonsterSay(ss.str().c_str());
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

enum GuardDidier
{
    SPELL_MARK_OF_DIDIER = 28114,
    EVENT_GUARDDIDIER_RESPAWN = 1,
    EVENT_GUARDDIDIER_MULE_DIED = 2,
    EVENT_GUARDDIDIER_MARK_OF_DIDIER = 3,
    EVENT_GUARDDIDIER_SUMMON_MULES = 4,
    EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_1 = 5,
    EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_2 = 6,
    EVENT_GUARDDIDIER_START_WAYPOINTS = 7,
    EVENT_GUARDDIDIER_EMOTE_ORIENTATION = 9,
    EVENT_GUARDDIDIER_EMOTE_ONESHOTPOINT = 10,
    EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_3 = 11,
    EVENT_GUARDDIDIER_DESPAWN = 12,
    EVENT_GUARDDIDIER_CHECK_MULE = 13,
};

struct MuleStruct
{
    uint32 m_entry;
    float m_position[4];
    float m_distance;
    float m_angle;
};

std::array<MuleStruct, 5> const Mules =
{ {
    { 16232, {2305.88f, -5290.34f, 82.3359f, 1.8326f}, 5.253697f, 1.606888f },
    { 16232, {2308.86f, -5290.45f, 82.1197f, 1.79769f}, 5.512784f, 2.234314f },
    { 16232, {2306.39f, -5294.27f, 82.0796f, 1.81514f}, 5.432998f, 2.864145f },
    { 16232, {2309.44f, -5294.44f, 82.0796f, 1.72788f}, 5.208000f, 3.490531f },
    { 16232, {2308.2f, -5297.16f, 82.0796f, 1.78024f}, 5.019997f, 4.118827f },
} };

void SwitchReactState(Creature* pCreature, bool passiv)
{
    if (!pCreature)
        return;

    if (passiv)
    {
        if (pCreature->GetReactState() == REACT_PASSIVE)
            return;

        pCreature->SetReactState(REACT_PASSIVE);
        pCreature->RemoveAurasDueToSpell(31309);
        pCreature->AddAura(31748);
    }
    else
    {
        if (pCreature->GetReactState() == REACT_DEFENSIVE)
            return;

        pCreature->SetReactState(REACT_DEFENSIVE);
        pCreature->RemoveAurasDueToSpell(31748);
        pCreature->AddAura(31309);
    }

    pCreature->AddAura(SPELL_MARK_OF_DIDIER);
};

struct GuardDidierAI : public ScriptedAI
{
    EventMap m_events;

    ObjectGuidSet m_caravan_mule;
    bool escort_failed = false;

    GuardDidierAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_events.Reset();
        SwitchReactState(m_creature, true);
        m_creature->SetDeathState(JUST_ALIVED);
        m_creature->SetCorpseDelay(18); // Corpse despawns 18 seconds after death.
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_events.ScheduleEvent(EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_1, 7000);
        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DIDIER);
    }

    void Reset() override
    {
        SwitchReactState(m_creature, true);
        // Mark of Didier resets, so we need to apply it again on reset.
        // Apply only after waypoint 5, or it can be triggered if a player set Argent faction at war.
        if (m_creature->GetMotionMaster()->getLastReachedWaypoint() >= 5)
            m_creature->AddAura(SPELL_MARK_OF_DIDIER);
    }

    void DamageTaken(Unit* pDealer, uint32& /*uiDamage*/) override
    {
        m_creature->AI()->OnScriptEventHappened(NULL, NULL, pDealer);
    }

    void OnScriptEventHappened(uint32 /*uiEvent*/, uint32 /*uiData*/, WorldObject* pInvoker) override
    {
        if (m_caravan_mule.empty())
            return;

        for (const auto& guid : m_caravan_mule)
        {
            if (Creature* pMule = m_creature->GetMap()->GetCreature(guid))
            {
                SwitchReactState(pMule, false);
                if (!pMule->SelectHostileTarget() || !pMule->GetVictim())
                    pMule->AI()->AttackStart(pInvoker->ToCreature());
            }
        }

        SwitchReactState(m_creature, false);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        m_creature->ResetHomePosition();
    }

    void GroupMemberJustDied(Creature* pCreature, bool /*isLeader*/) override
    {
        // Remove dead Caravan Mule.
        if (m_caravan_mule.find(pCreature->GetObjectGuid()) != m_caravan_mule.end())
            m_caravan_mule.erase(pCreature->GetObjectGuid());
    }

    void CorpseRemoved(uint32& /*uiRespawnDelay*/) override
    {
        if (sGameEventMgr.IsActiveEvent(9))
            sGameEventMgr.StopEvent(9);
    }

    void UpdateAI(uint32 const diff) override
    {
        m_events.Update(diff);

        while (uint32 Events = m_events.ExecuteEvent())
        {
            switch (Events)
            {
                case EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_1:
                {
                    DoScriptText(12105, m_creature, nullptr, CHAT_TYPE_SAY);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_2, 5000);
                    break;
                }
                case EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_2:
                {
                    DoScriptText(12113, m_creature, nullptr, CHAT_TYPE_SAY);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_EMOTE_ORIENTATION, 4000);
                    break;
                }
                case EVENT_GUARDDIDIER_EMOTE_ORIENTATION:
                {
                    m_creature->SetOrientation((float)1.2210f);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_EMOTE_ONESHOTPOINT, 1000);
                    break;
                }
                case EVENT_GUARDDIDIER_EMOTE_ONESHOTPOINT:
                {
                    m_creature->HandleEmote(EMOTE_ONESHOT_POINT);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_START_WAYPOINTS, 3000);
                    break;
                }
                case EVENT_GUARDDIDIER_START_WAYPOINTS:
                {
                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveWaypoint(0, PATH_FROM_SPECIAL, 0, 0, 1622601, false);
                    m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_CHECK_MULE, 1000);
                    std::list<Creature*> muleList;
                    GetCreatureListWithEntryInGrid(muleList, m_creature, 16232, 50.0f);
                    for (const auto& pMule : muleList)
                    {
                        //pMule->JoinCreatureGroup(m_creature, Mules[count].m_distance, Mules[count].m_angle, OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER);
                        m_caravan_mule.insert(pMule->GetObjectGuid());
                    }
                    break;
                }
                case EVENT_GUARDDIDIER_CHECK_MULE:
                {
                    if (m_caravan_mule.size() < 5 && !escort_failed)
                    {
                        escort_failed = true; // Some Mule died, Escort failed.
                        m_events.ScheduleEvent(EVENT_GUARDDIDIER_MULE_DIED, 4000);
                        m_creature->GetMotionMaster()->Initialize();
                        m_creature->GetMotionMaster()->MoveIdle();
                        m_creature->StopMoving(true);
                    }
                    else
                        m_events.ScheduleEvent(EVENT_GUARDDIDIER_CHECK_MULE, 1000);
                    break;
                }
                case EVENT_GUARDDIDIER_MULE_DIED:
                {
                    m_creature->MonsterSay("EVENT_GUARDDIDIER_MULE_DIED");
                    if (!m_creature->IsInCombat() && m_creature->IsStopped()) // Waiting until not moving anymore.
                    {
                        m_creature->MonsterSay("EMOTE_ONESHOT_BEG");
                        m_creature->HandleEmote(EMOTE_ONESHOT_BEG);
                        m_events.ScheduleEvent(EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_3, 3000);
                    }
                    else
                    {
                        m_events.ScheduleEvent(EVENT_GUARDDIDIER_MULE_DIED, 1000);
                    }
                    break;
                }
                case EVENT_GUARDDIDIER_CHAT_MSG_MONSTER_SAY_3:
                {
                    m_creature->SetDefaultGossipMenuId(7168);
                    DoScriptText(12118, m_creature, nullptr, CHAT_TYPE_SAY);
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_events.ScheduleEvent(EVENT_GUARDDIDIER_DESPAWN, 90000);
                    break;
                }
                case EVENT_GUARDDIDIER_DESPAWN:
                {
                    if (sGameEventMgr.IsActiveEvent(9))
                        sGameEventMgr.StopEvent(9);
                    break;
                }
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_GuardDidierAI(Creature* pCreature)
{
    return new GuardDidierAI(pCreature);
}

struct CaravanMuleAI : public ScriptedAI
{
    EventMap m_events;

    std::set<ObjectGuid> m_caravan_mule;

    CaravanMuleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_events.Reset();
        SwitchReactState(m_creature, true);
        m_creature->SetDeathState(JUST_ALIVED);
        m_creature->SetCorpseDelay(5); // Corpse despawns 18 seconds after death.
        m_creature->RemoveAurasDueToSpell(SPELL_MARK_OF_DIDIER);
    }

    void Reset() override
    {
        SwitchReactState(m_creature, true);
        // Mark of Didier resets, so we need to apply it again on reset.
        // Apply only after waypoint 5, or it can be triggered if a player set Argent faction at war.
        if (Creature* pDidier = m_creature->GetMap()->GetCreature(m_creature->GetCreatureGroup()->GetLeaderGuid()))
            if (pDidier->GetMotionMaster()->getLastReachedWaypoint() >= 5)
                m_creature->AddAura(SPELL_MARK_OF_DIDIER);
    }

    void DamageTaken(Unit* pDealer, uint32& /*uiDamage*/) override
    {
        if (Creature* pDidier = m_creature->GetMap()->GetCreature(m_creature->GetCreatureGroup()->GetLeaderGuid()))
            pDidier->AI()->OnScriptEventHappened(NULL, NULL, pDealer);
    }

    /*
    void MoveInLineOfSight(Unit* pWho) override
    {
        if (pWho->IsWithinCombatDistInMap(m_creature, ATTACK_DISTANCE) && pWho->GetThreatManager().getThreat(m_creature))
            m_creature->SetReactState(REACT_DEFENSIVE);

        ScriptedAI::MoveInLineOfSight(pWho);
    }
    */

    void UpdateAI(uint32 const diff) override
    {
        m_events.Update(diff);

        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_CaravanMuleAI(Creature* pCreature)
{
    return new CaravanMuleAI(pCreature);
}

void AddSC_custom_creatures()
{
    Script *newscript;

    //https://github.com/Oroxzy Arena Patch 1.12.1... begin
    newscript = new Script;
    newscript->Name = "custom_ArenaMaster";
    newscript->pGOGossipHello = &GossipHello_ArenaMaster;
    newscript->pGOGossipSelect = &GossipSelect_ArenaMaster;
    newscript->pGOGossipSelectWithCode = &GossipSelect_ArenaMaster_Ext;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_ArenaAnnouncer";
    newscript->pGossipHello = &GossipHello_ArenaAnnouncer;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_arena_watcher";
    newscript->pGossipHello = &GossipHello_npc_arena_watcher;
    newscript->pGossipSelect = &GossipSelect_npc_arena_watcher;
    newscript->RegisterSelf(false);

    //https://github.com/Oroxzy Arena Patch 1.12.1... end

    newscript = new Script;
    newscript->Name = "custom_teleport_npc";
    newscript->pGossipHello = &GossipHello_TeleportNPC;
    newscript->pGossipSelect = &GossipSelect_TeleportNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_enchant_npc";
    newscript->pGossipHello = &GossipHello_EnchantNPC;
    newscript->pGossipSelect = &GossipSelect_EnchantNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_professions_npc";
    newscript->pGossipHello = &GossipHello_ProfessionNPC;
    newscript->pGossipSelect = &GossipSelect_ProfessionNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_premade_gear_npc";
    newscript->pGossipHello = &GossipHello_PremadeGearNPC;
    newscript->pGossipSelect = &GossipSelect_PremadeGearNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_premade_spec_npc";
    newscript->pGossipHello = &GossipHello_PremadeSpecNPC;
    newscript->pGossipSelect = &GossipSelect_PremadeSpecNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_npc_training_dummy";
    newscript->GetAI = &GetAI_npc_training_dummy;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_npc_summon_debugAI";
    newscript->GetAI = &GetAI_custom_summon_debug;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_npc_auctionhousebot";
    newscript->pGossipHello = &GossipHello_AuctionhouseBot;
    newscript->pGossipSelect = &GossipSelect_AuctionhouseBot;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_info_npc";
    newscript->pGossipHello = &GossipHello_InfoNPC;
    newscript->pGossipSelect = &GossipSelect_InfoNPC;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_instance_reseter";
    newscript->pGossipHello = &GossipHello_Instance_Reseter;
    newscript->pGossipSelect = &GossipSelect_Instance_Reseter;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "custom_naxx_boss_teleporter";
    newscript->pGossipHello = &GossipHello_naxx_boss_teleporter;
    newscript->pGossipSelect = &GossipSelect_naxx_boss_teleporter;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_nagrand_tornado";
    newscript->GetAI = &GetAI_npc_nagrand_tornado;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_guard_didier";
    newscript->GetAI = &GetAI_GuardDidierAI;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_caravan_mule";
    newscript->GetAI = &GetAI_CaravanMuleAI;
    newscript->RegisterSelf(false);

    newscript = new Script;
    newscript->Name = "npc_boss_eitrigg";
    newscript->pGossipHello = &GossipHello_BossEitrigg;
    newscript->pGossipSelect = &GossipSelect_BossEitrigg;
    newscript->GetAI = &GetAI_BossEitrigg;
    newscript->RegisterSelf(false);
}
