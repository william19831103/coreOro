/*
 * Copyright (C) 2021 MaNGOS <http://uterusone.net/>
 */

#ifndef __Arena_H
#define __Arena_H

#include "BattleGround.h"

enum Arena_Timers
{
    ARENA_TIME_25_MIN = 25 * IN_MILLISECONDS * MINUTE,
    ARENA_TIME_2_MIN = 1 * IN_MILLISECONDS * MINUTE // est
};

enum Arena_Sound
{
    KILL_SOUND                          = 8213,
    LET_THE_GAMES_BEGIN                 = 8280,
    DALARANARENA_WATERINCOMING          = 15355,
    LFG_DENIED                          = 17341,
    LFG_DUNGEONREADY                    = 17318,
    LFG_REWARDS                         = 17316,
    UI_BATTLEGROUNDCOUNTDOWN_TIMER      = 25477,
    UI_BATTLEGROUNDCOUNTDOWN_FINISHED   = 25478
};

enum Arena_MSG
{
    ARENA_PLAYER_JOINED     = 6001,
    ARENA_START_ONE_MINUTE  = 6661,
    ARENA_START_HALF_MINUTE = 6662,
    ARENA_HAS_BEGUN         = 6663,
    ARENA_START_COUNTDOWN   = 6000
};

enum Arena_SpellId
{
    ARENA_PREPARATION       = 32727,
    CALL_PET                = 883,
    HAPPY_PET               = 24716,
    SPELL_HORDE_FLAG	    = 35776,
    SPELL_ALLIANCE_FLAG     = 35777,
    RECENTLY_BANDAGED       = 11196,
    ARENA_DS_SPELL_FLUSH    = 37405,    // Visual water knockback from the pipe.
    FORBIDDEN_GEAR          = 30000,    // unused rn. Gives a debuff with 75% reduced speed, dmg and cast speed?.
    MORTAL_WOUND            = 25646     // Healing effects reduced by 10%, stacks up to 10.
};

enum Arena_State
{
    ARENA_STATE_ALIVE_PLAYERS_RED   = 0,
    ARENA_STATE_ALIVE_PLAYERS_BLUE  = 1
};

enum Arena_WorldStates
{
    ARENA_WORLD_STATE_ALIVE_PLAYERS_RED     = 2575,
    ARENA_WORLD_STATE_ALIVE_PLAYERS_BLUE    = 2576,
    ARENA_WORLD_STATE_TIME_SECONDS          = 8295,
    ARENA_WORLD_STATE_TIME_MINUTES          = 8296
};

enum Arena_DSData
{
    ARENA_DS_WATERFALL_TIMER_MIN            = 30000,
    ARENA_DS_WATERFALL_TIMER_MAX            = 60000,
    ARENA_DS_WATERFALL_WARNING_DURATION     = 7500,
    ARENA_NA_TORNADO_DURATION               = 60000,
    ARENA_NA_TORNADO_KNOCKBACK_TIMER        = 2000,
    ARENA_DS_WATERFALL_DURATION             = 30000,
    ARENA_DS_WATERFALL_KNOCKBACK_TIMER      = 1000,
    ARENA_DS_PIPE_KNOCKBACK_FIRST_DELAY     = 10000,
    ARENA_DS_PIPE_KNOCKBACK_DELAY           = 3000,
    ARENA_DS_PIPE_KNOCKBACK_TOTAL_COUNT     = 2
};

enum Arena_NPCs
{
    NPC_TORNADO                             = 19922,
};

enum Arena_DSGameEvents
{
    // Why i don't use just one Water Spout NPC as kicker and spell caster like TrinityCore?
    // Cause if the kicker is not all the way back in the Pipe you can avoid get kicked out by running against the walls.
    // And if the Kicker all the way back casts the visual Flush it looks wrongly positioned.
    NPC_WATERSPOUT1                                 = 200,
    NPC_WATERSPOUT2                                 = 201,
    NPC_WATERFALL_KICKER                            = 202,
    NPC_PIPE_KICKER_1                               = 203,
    NPC_PIPE_KICKER_2                               = 204,
    DOODAD_DALARANSEWER01                           = 209,
    DOODAD_DALARANSEWER_ARENAWATERFALL              = 210,
    DOODAD_DALARANSEWER_ARENAWATERFALL_COLLISION01  = 211,
    SHADOW_SIGHT                                    = 212,
    STASH                                           = 213,  // The stash chest to switch gear and specc in arena. can be disabled in config.
    NPC_ANNOUNCER_1                                 = 214,  // The readycheck guy.
    NPC_ANNOUNCER_2                                 = 215  // The readycheck guy.
};

class ArenaScore : public BattleGroundScore
{
    public:
        ArenaScore() : DamageDone(0), HealingDone(0) {};
        virtual ~ArenaScore() {};
        uint32 DamageDone;
        uint32 HealingDone;
};

class Arena : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        Arena();
        ~Arena();
        void Update(uint32 diff);
        bool SummonTornado();
        void DoWaterFlush();
        void DoWaterfallKick();
        void KickFromPipe();
        void ApplyMortalStack();
        void TurnOff(uint8 event1);
        void Spawn_Doodad_DalaranSewer01();
        void Spawn_Stash();
        void Despawn_Stash();
        void Spawn_Shadow_Sight();
        void Spawn_Doodad_DalaranSewer_ArenaWaterFall();
        void Spawn_Doodad_DalaranSewer_ArenaWaterFall_Collision01();
        void TurnOn(uint8 event1);

        void AddPlayer(Player* player);
        void StartingEventCloseDoors();
        void StartingEventOpenDoors();
        void UpdatePlayerScore(Player* Source, uint32 type, uint32 value);
        void HandleAreaTrigger(Player* player, uint32 trigger);

        void RemovePlayer(Player * player);
        void HandleKillPlayer(Player *player, Player *killer);
        void EndBattleGround(Team winner);
        void UpdateArenaWorldState();
        void UpdateArenaWorldStateTime(uint32 time);
        virtual void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void CheckWinConditions();

    private:
        uint32 m_counter;
        uint32 m_waterfallwarningTimer;
        uint32 m_waterfallKnockbackTimer;
        uint32 m_tornadoSpawnTimer_1;
        uint32 m_tornadoSpawnTimer_2;
        uint32 m_waterfalloffTimer;
        uint32 m_waterfallonTimer;
        uint32 m_pipeKnockBackTimer;
        uint32 m_despawnDoors;
        uint32 m_arena_timer;
        bool m_waterfall_on;
        bool m_knockback_on;
        bool m_waterfall_off;
        bool m_time_over;
        uint8 m_pipeKnockBackCount;
};
#endif
