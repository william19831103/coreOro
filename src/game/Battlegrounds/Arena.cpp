/*
 * Copyright (C) 2021 MaNGOS <http://uterusone.net/>
 */

#include "scriptPCH.h"
#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "Arena.h"
#include "Creature.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "BattleGroundMgr.h"
#include "WorldPacket.h"
#include "Language.h"
#include "MapManager.h"
#include "World.h"
#include <ctime>

Arena::Arena()
{
    m_StartMessageIds[BG_STARTING_EVENT_SECOND]	= ARENA_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]	= ARENA_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH]	= ARENA_HAS_BEGUN;

    m_counter                   = NULL;
    m_pipeKnockBackTimer        = NULL;
    m_waterfallwarningTimer     = NULL;
    m_pipeKnockBackCount        = NULL;
    m_waterfallonTimer          = NULL;
    m_waterfalloffTimer         = NULL;
    m_waterfallKnockbackTimer   = NULL;
    m_tornadoSpawnTimer_1       = NULL;
    m_tornadoSpawnTimer_2       = NULL;
    m_despawnDoors              = NULL;
    m_arena_timer               = NULL;
    m_waterfall_on              = false;
    m_waterfall_off             = true;
    m_knockback_on              = false;
    m_time_over                 = false;
}

Arena::~Arena()
{
}

bool Arena::SummonTornado()
{
    // Only in Nagrand Arena.
    if (!IsNagrandArena())
        return false;

    int point = urand(1, 40);
    float orientration = 0.492f;

    float angle = (float(point) * (M_PI / (40 / 2)));
    float x = 4056.0f + float(urand(5, 45)) * cos(angle);
    float y = 2922.0f + float(urand(5, 45)) * sin(angle);
    float z = 13.65f;
    float new_z = GetBgMap()->GetHeight(x, y, z, true, 1.0f);
    if (new_z > INVALID_HEIGHT)
        z = new_z + 0.05f;

    uint32 remaintime = urand(45000, 60000);

    if (Creature* tornado = GetBgMap()->SummonCreature(NPC_TORNADO, x, y, z, angle - M_PI, TEMPSUMMON_MANUAL_DESPAWN, true))
        sLog.outBasic("[Arena:SummonTornado] summoned tornado 1 on arena map %d for %d", GetBgMap()->GetInstanceId(), remaintime);
    else
    {
        sLog.outBasic("[Arena:SummonTornado] invalid position for tornado 1, retry in 2 seconds", GetBgMap()->GetInstanceId());
        return false;
    }

    return true;
}

void Arena::DoWaterFlush()
{
    for (uint32 i = NPC_WATERSPOUT1; i <= NPC_WATERSPOUT2; ++i)
    {
        if (Creature* waterSpout = GetBgMap()->GetCreature(GetSingleCreatureGuid(i, 0)))
            waterSpout->CastSpell(waterSpout, ARENA_DS_SPELL_FLUSH, true);
    }
}

void Arena::DoWaterfallKick()
{
    if (Creature* waterSpout = GetBgMap()->GetCreature(GetSingleCreatureGuid(NPC_WATERFALL_KICKER, 0)))
    {
        for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* player = sObjectMgr.GetPlayer(itr->first);
            if (player && waterSpout->GetDistance(player) < 5.0)
            {
                player->SetRooted(false);
                player->KnockBackFrom(waterSpout, 5, 20);
            }
        }
    }
}

void Arena::KickFromPipe()
{
    for (uint32 i = NPC_PIPE_KICKER_1; i <= NPC_PIPE_KICKER_2; ++i)
    {
        if (Creature* kicker = GetBgMap()->GetCreature(GetSingleCreatureGuid(i, 0)))
        {
            for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            {
                Player* player = sObjectMgr.GetPlayer(itr->first);
                if (player && kicker->GetDistance(player) < 53.0)
                {
                    // Why not just use KnockBackFrom? cause it does kick a player away from itself.. if the angle is shit you got kicked into the wall.
                    // This gets the Angle from the kicker to the Waterspout caster wich is straight trough the pipe. So Players will kicked out straight ahead no matter what position they have.
                    float speed = 20.0f + (40.0f - kicker->GetDistance(player));
                    for (uint32 e = NPC_WATERSPOUT1; e <= NPC_WATERSPOUT2; ++e)
                    {
                        if (Creature* target = GetBgMap()->GetCreature(GetSingleCreatureGuid(e, 0)))
                            if (target && target->GetDistance(kicker) < 30.0)
                                player->KnockBackFromTo(kicker, target, speed, 8.0f);
                    }
                }
            }
        }
    }
}

void Arena::ApplyMortalStack()
{
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* player = sObjectMgr.GetPlayer(itr->first);
        if (player)
        {
            if (SpellAuraHolder* pHolder = player->AddAura(MORTAL_WOUND))
            {
                pHolder->SetAuraMaxDuration(180000);
                pHolder->SetAuraDuration(180000);
                pHolder->UpdateAuraDuration();
            }
        }
    }
}

void Arena::Spawn_Stash()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(STASH, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(STASH, 0)].gameobjects.end(); ++itr)
        SpawnBGObject(*itr, RESPAWN_IMMEDIATELY);
}

void Arena::Despawn_Stash()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(STASH, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(STASH, 0)].gameobjects.end(); ++itr)
    {
        Map* map = GetBgMap();
        GameObject* obj = map->GetGameObject(*itr);
        if (!obj)
            return;
        map->Remove(obj, true);
    }
}

void Arena::Spawn_Doodad_DalaranSewer01()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER01, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER01, 0)].gameobjects.end(); ++itr)
        SpawnBGObject(*itr, RESPAWN_IMMEDIATELY);
}

void Arena::Spawn_Shadow_Sight()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(SHADOW_SIGHT, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(SHADOW_SIGHT, 0)].gameobjects.end(); ++itr)
        SpawnBGObject(*itr, BUFF_RESPAWN_TIME);
}

void Arena::Spawn_Doodad_DalaranSewer_ArenaWaterFall()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER_ARENAWATERFALL, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER_ARENAWATERFALL, 0)].gameobjects.end(); ++itr)
        SpawnBGObject(*itr, RESPAWN_IMMEDIATELY);
}

void Arena::Spawn_Doodad_DalaranSewer_ArenaWaterFall_Collision01()
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01, 0)].gameobjects.end(); ++itr)
        SpawnBGObject(*itr, RESPAWN_IMMEDIATELY);
}

void Arena::TurnOff(uint8 event1)
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(event1, 0)].gameobjects.end(); ++itr)
        DoorOpen(*itr);
}

void Arena::TurnOn(uint8 event1)
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, 0)].gameobjects.begin();
    for (; itr != m_EventObjects[MAKE_PAIR32(event1, 0)].gameobjects.end(); ++itr)
        DoorClose(*itr);
}

void Arena::Update(uint32 diff)
{
    // Fixes WorldStates before the arena match has started.
    if (GetStatus() < STATUS_IN_PROGRESS)
    {
        if (m_counter < diff)
        {
            UpdateArenaWorldStateTime(ARENA_TIME_25_MIN);
            UpdateArenaWorldState();
            m_counter = IN_MILLISECONDS; // 1 second.
        }
        else
            m_counter -= diff;
    }

    // Update WorldStates every second.
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // how long this match is already running.
        m_arena_timer = m_arena_timer + diff;

        if (m_counter < diff)
        {
            UpdateArenaWorldStateTime(ARENA_TIME_25_MIN - m_arena_timer);
            CheckWinConditions();
            m_counter = IN_MILLISECONDS; // 1 second.
        }
        else
            m_counter -= diff;

        // 25 minutes are over... let's win the team with the most damge done.
        if ((ARENA_TIME_25_MIN - m_arena_timer) < diff)
            m_time_over = true;

        // Despawn doors 3 seconds after they are opened.
        if (m_despawnDoors < diff)
            StartingEventDespawnDoors();
        else
            m_despawnDoors -= diff;

        if (IsNagrandArena())
        {
            // First Tornado
            if (m_tornadoSpawnTimer_1 < diff)
            {
                if (SummonTornado())
                {
                    m_tornadoSpawnTimer_1 = urand(120000, 180000);
                }
                else
                    m_tornadoSpawnTimer_1 = 2000;

            }
            else
                m_tornadoSpawnTimer_1 -= diff;

            // Second Tornado
            if (m_tornadoSpawnTimer_2 < diff)
            {
                if (SummonTornado())
                    m_tornadoSpawnTimer_2 = urand(120000, 180000);
                else
                    m_tornadoSpawnTimer_2 = 2000;
            }
            else
                m_tornadoSpawnTimer_2 -= diff;
        }

        if (IsDalaranArena())
        {
            if (m_waterfallwarningTimer < diff)
            {
                if (!m_waterfall_on && m_waterfall_off)
                {
                    PlaySoundToAll(DALARANARENA_WATERINCOMING);
                    TurnOn(DOODAD_DALARANSEWER_ARENAWATERFALL);
                    TurnOn(DOODAD_DALARANSEWER01);
                    m_waterfallonTimer   = ARENA_DS_WATERFALL_WARNING_DURATION;
                    m_waterfall_off      = false;
                }
            }
            else
                m_waterfallwarningTimer -= diff;

            if (m_waterfallonTimer < diff)
            {
                if (!m_waterfall_on && !m_waterfall_off)
                {
                    TurnOn(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01);
                    m_waterfall_on = true;
                    m_knockback_on = true;
                    m_waterfalloffTimer = ARENA_DS_WATERFALL_DURATION;
                }
            }
            else
                m_waterfallonTimer -= diff;

            if (m_waterfalloffTimer < diff)
            {
                if (m_waterfall_on && !m_waterfall_off)
                {
                    TurnOff(DOODAD_DALARANSEWER01);
                    TurnOff(DOODAD_DALARANSEWER_ARENAWATERFALL);
                    TurnOff(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01);
                    m_waterfall_on   = false;
                    m_waterfall_off  = true;
                    m_knockback_on   = false;
                    m_waterfallwarningTimer = urand(ARENA_DS_WATERFALL_TIMER_MIN, ARENA_DS_WATERFALL_TIMER_MAX);
                }
            }
            else
                m_waterfalloffTimer -= diff;

            if (m_waterfallKnockbackTimer < diff)
            {
                if (m_waterfall_on && !m_waterfall_off && m_knockback_on)
                {
                    DoWaterfallKick();
                    m_waterfallKnockbackTimer = 1500;
                }
            }
            else
                m_waterfallKnockbackTimer -= diff;

            if (m_pipeKnockBackCount < ARENA_DS_PIPE_KNOCKBACK_TOTAL_COUNT)
            {
                if (m_pipeKnockBackTimer < diff)
                {
                    KickFromPipe();
                    DoWaterFlush();
                    ++m_pipeKnockBackCount;
                    m_pipeKnockBackTimer = ARENA_DS_PIPE_KNOCKBACK_DELAY;
                }
                else
                    m_pipeKnockBackTimer -= diff;
            }
        }
    }

    BattleGround::Update(diff);
}

void Arena::StartingEventCloseDoors()
{
    SetSpawnEventMode(NPC_ANNOUNCER_1, 0, RESPAWN_START);
    SetSpawnEventMode(NPC_ANNOUNCER_2, 0, RESPAWN_START);
    Spawn_Shadow_Sight();

    if (sWorld.getConfig(CONFIG_BOOL_ARENA_STASH))
        Spawn_Stash(); // Spawn Stash on start, so Players can change spec or gear.

    if (IsDalaranArena())
    {
        // if we don't do this, you can not see the water flush visual.
        SetSpawnEventMode(NPC_WATERSPOUT1, 0, RESPAWN_START);
        SetSpawnEventMode(NPC_WATERSPOUT2, 0, RESPAWN_START);
        Spawn_Doodad_DalaranSewer_ArenaWaterFall();
        Spawn_Doodad_DalaranSewer01();
        Spawn_Doodad_DalaranSewer_ArenaWaterFall_Collision01();
        TurnOn(DOODAD_DALARANSEWER_ARENAWATERFALL);
        TurnOn(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01);
        TurnOn(DOODAD_DALARANSEWER01);
    }
}

void Arena::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
    PlaySoundToAll(LET_THE_GAMES_BEGIN);
    Despawn_Stash(); // Remove the Stash if game begins.
    SetSpawnEventMode(NPC_ANNOUNCER_1, 0, DESPAWN_FORCED);
    SetSpawnEventMode(NPC_ANNOUNCER_2, 0, DESPAWN_FORCED);

    if (!GetPlayersCountByTeam(ALLIANCE) || !GetPlayersCountByTeam(HORDE))
        EndBattleGround(TEAM_NONE);

    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* player = sObjectMgr.GetPlayer(itr->first);

        if (!player || !player->IsAlive())
            continue;

        // Reset all Cooldowns, Health and Powers.
        player->RemoveAllAurasWithLessThan30SecondsLeft();
        player->RemoveAllSpellCooldown();
        player->SetHealthPercent(100.0f);
        player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
        player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));
        player->SetPower(POWER_RAGE, 0);
        player->RemoveAurasDueToSpell(RECENTLY_BANDAGED);
        player->RemoveAurasDueToSpell(ARENA_PREPARATION);
        switch (player->GetBGTeamId())
        {
        case TEAM_ALLIANCE:
            player->AddAura(SPELL_ALLIANCE_FLAG);
            break;
        case TEAM_HORDE:
            player->AddAura(SPELL_HORDE_FLAG);
            break;
        }

        if (Pet* pet = player->GetPet())
        {
            if (pet)
            {
                pet->RemoveAllAurasWithLessThan30SecondsLeft();
                pet->RemoveAllSpellCooldown();
            }
        }
    }

    m_despawnDoors = 3000;

    // I don't care if its Blizzlike... i want to stop the waterfall if Doors opens.
    if (IsDalaranArena())
    {
        TurnOff(DOODAD_DALARANSEWER_ARENAWATERFALL);
        TurnOff(DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01);
        TurnOff(DOODAD_DALARANSEWER01);
        m_waterfall_on           = false;
        m_waterfall_off          = true;
        m_knockback_on           = false;
        m_pipeKnockBackCount     = 0;
        m_pipeKnockBackTimer     = ARENA_DS_PIPE_KNOCKBACK_FIRST_DELAY;
        m_waterfallwarningTimer  = 20000;
    }

    // First tornado after 1 minute.
    if (IsNagrandArena())
    {
        m_tornadoSpawnTimer_1 = 60000;
        m_tornadoSpawnTimer_2 = urand(120000, 180000); // Start timer for second tornado.
    }

    CheckWinConditions();
}

void Arena::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetObjectGuid());
    if (itr == m_PlayerScores.end())
        return;
    switch (type)
    {
        case SCORE_DAMAGE_DONE:
            ((ArenaScore*)itr->second)->DamageDone += value;
            break;
        case SCORE_HEALING_DONE:
            ((ArenaScore*)itr->second)->HealingDone += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(Source, type, value);
            break;
    }
    UpdateArenaWorldState();
}

void Arena::HandleAreaTrigger(Player* player, uint32 trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!IsDalaranArena())
        return;

    switch (trigger)
    {
    case 5347:
    case 5348:
        // Someone has get back into the pipes and the knockback has already been performed,
        // so we reset the knockback count for kicking the player again into the arena.
        KickFromPipe();
        DoWaterFlush();
        break;
        // OUTSIDE OF ARENA, TELEPORT!
    case 5328:
        player->NearTeleportTo(1290.44f, 744.96f, 3.16f, 1.6f);
        break;
    case 5329:
        player->NearTeleportTo(1292.6f, 837.07f, 3.161f, 4.7f);
        break;
    case 5330:
        player->NearTeleportTo(1250.68f, 790.86f, 3.16f, 0.0f);
        break;
    case 5331:
        player->NearTeleportTo(1332.50f, 790.9f, 3.16f, 3.14f);
        break;
    case 5326: // -10
    case 5343: // -40
    case 5344: // -60
        player->NearTeleportTo(1330.0f, 800.0f, 3.16f, player->GetOrientation());
        break;
    }
}

void Arena::AddPlayer(Player* player)
{
    if (!player)
        return;

    BattleGround::AddPlayer(player);

    // Leave all other Arena queues.
    for (uint8 bgTypeId = BATTLEGROUND_NA1v1; bgTypeId <= BATTLEGROUND_DS5v5; ++bgTypeId)
    {
        BattleGroundQueue &queue = sBattleGroundMgr.m_BattleGroundQueues[bgTypeId];
        if (&queue)
            queue.LeaveQueue(player, BattleGroundTypeId(bgTypeId));
    }

    // Repair all items and remove all Buffs.
    player->UnequipForbiddenArenaItems(player->GetBattleGroundTypeId());
    player->Unmount();
    player->DurabilityRepairAll(false, 0);
    player->RemoveAllAurasOnDeath();
    player->SetGameMaster(false);
    player->SetGMVisible(true);
    player->SetCheatGod(false);
    player->RemoveAllAurasWithLessThan30SecondsLeft();
    player->RemoveAllSpellCooldown();
    player->AddAura(ARENA_PREPARATION);

    std::string BGTeamName = "Alliance";

    switch (player->GetBGTeamId())
    {
    case TEAM_ALLIANCE:
        BGTeamName = "Alliance";
        break;
    case TEAM_HORDE:
        BGTeamName = "Horde";
        break;
    }

    if (player->HasForbiddenArenaItems(player->GetBattleGroundTypeId()))
        player->AddAura(FORBIDDEN_GEAR);
    else
        player->RemoveAurasDueToSpell(FORBIDDEN_GEAR);

    // this port shows a message to all if a player enters the arena.
    PSendMessageToAll(ARENA_PLAYER_JOINED, CHAT_MSG_BG_SYSTEM_NEUTRAL, nullptr, (std::string)(player->GetName()), (std::string)(BGTeamName));

    // this is necessary to remove all auras.
    if (player->GetClass() == CLASS_HUNTER)
        player->CastSpell(player, CALL_PET, true);

    if (Pet* pet = player->GetPet())
    {
        if (pet)
        {
            pet->RemoveAllAurasOnDeath();
            pet->RemoveAllAurasWithLessThan30SecondsLeft();
            pet->SetHealth(pet->GetMaxHealth());
            if (pet->getPetType() == HUNTER_PET)
            {
                player->AddAura(HAPPY_PET);
                pet->SetLoyaltyLevel(BEST_FRIEND);
            }
        }
    }

    ArenaScore* sc = new ArenaScore;

    m_PlayerScores[player->GetObjectGuid()] = sc;

    UpdateArenaWorldState();
}

void Arena::RemovePlayer(Player * player)
{
    if (GetStatus() < STATUS_WAIT_LEAVE)
        PlaySoundToAll(LFG_DENIED);

    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    if (Pet* pet = player->GetPet())
    {
        if (pet && pet->isControlled())
        {
            pet->RemoveAllAurasOnDeath();
            pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
            pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
            pet->SetDeathState(ALIVE);
            pet->ClearUnitState(UNIT_STAT_ALL_DYN_STATES);
            pet->SetHealth(pet->GetMaxHealth());
        }
    }

    player->RemoveAllAurasOnDeath();
    player->SetSpectate(false);
    CheckWinConditions();
}

void Arena::EndBattleGround(Team winner)
{
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player* player = sObjectMgr.GetPlayer(itr->first);

        if (!player)
            continue;

        if (Pet * pet = player->GetPet())
        {
            if (pet && pet->isControlled())
            {
                pet->RemoveAllAurasOnDeath();
                pet->SetDeathState(ALIVE);
                pet->SetHealth(pet->GetMaxHealth());
                pet->SetHealth(pet->GetMaxHealth());
                pet->Unsummon(PET_SAVE_AS_CURRENT);
            }
        }

        player->SetSpectate(false);
        player->Unmount();
        player->DurabilityRepairAll(false, 0);
        player->RemoveAllAurasOnDeath();
        player->CombatStopWithPets(true);

        if (winner == TEAM_NONE)
            player->LeaveBattleground(true);
    }
    BattleGround::EndBattleGround(winner);
}

void Arena::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);
    PlaySoundToAll(KILL_SOUND);
    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    CheckWinConditions();
}

void Arena::CheckWinConditions()
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (m_time_over)
        {
            uint32 alliance_damage = 0;
            uint32 horde_damage = 0;

            for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            {
                Player* player = sObjectMgr.GetPlayer(itr->first);

                if (!player)
                    continue;

                BattleGroundScoreMap::iterator score = m_PlayerScores.find(player->GetGUID());

                if (score != m_PlayerScores.end())
                {
                    switch (player->GetBGTeamId())
                    {
                    case TEAM_ALLIANCE:
                        alliance_damage = alliance_damage + ((ArenaScore*)score->second)->DamageDone;
                        break;
                    case TEAM_HORDE:
                        horde_damage = horde_damage + ((ArenaScore*)score->second)->DamageDone;
                        break;
                    }
                }
            }

            if (alliance_damage == horde_damage)
                EndBattleGround(TEAM_NONE);
            else if (alliance_damage > horde_damage)
                EndBattleGround(ALLIANCE);
            else if (alliance_damage < horde_damage)
                EndBattleGround(HORDE);
        }

        if (!GetAlivePlayersCountByTeam(ALLIANCE) && GetPlayersCountByTeam(HORDE))
            EndBattleGround(HORDE);
        else if (GetPlayersCountByTeam(ALLIANCE) && !GetAlivePlayersCountByTeam(HORDE))
            EndBattleGround(ALLIANCE);
    }
    UpdateArenaWorldState();
}

void Arena::UpdateArenaWorldStateTime(uint32 time)
{
    int milli = time;
    int min = (milli / (1000 * 60)) % 60;
    int sec = (milli / 1000) % 60;

    UpdateWorldState(ARENA_WORLD_STATE_TIME_MINUTES, min);
    UpdateWorldState(ARENA_WORLD_STATE_TIME_SECONDS, sec);
}

void Arena::UpdateArenaWorldState()
{
    UpdateWorldState(ARENA_WORLD_STATE_ALIVE_PLAYERS_RED, int32(GetAlivePlayersCountByTeam(HORDE)));
    UpdateWorldState(ARENA_WORLD_STATE_ALIVE_PLAYERS_BLUE, int32(GetAlivePlayersCountByTeam(ALLIANCE)));
}

void Arena::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, ARENA_WORLD_STATE_ALIVE_PLAYERS_RED, int32(GetAlivePlayersCountByTeam(HORDE)));
    FillInitialWorldState(data, count, ARENA_WORLD_STATE_ALIVE_PLAYERS_BLUE, int32(GetAlivePlayersCountByTeam(ALLIANCE)));
}
