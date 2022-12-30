
![vanilla_arena-min(1)](https://user-images.githubusercontent.com/357606/210062280-297a049b-0ece-4873-a943-2be9aec5b72a.png)


## Vanilla Arena (ONLY FOR THE OLD 1.12 CLIENT)
One of my first Projects for VMaNGOS with almost no coding knowledge just copy-paste, endless tries and a lot of beer.
I know it's horrible coded... i don't care. It works and was funny.

## Features:

- All TBC Arenas & Dalaran Arena from Wrath of the Lich King.
- Solo & Group queue for 1v1, 2v2, 3v3 and 5v5.
- Ready Check NPC in the Arena: speak to him to start the arena immediately.
- Spectator Mode.
- Cross Faction support (Horde can team up with Alliance and vice versa).
- Disabled Item switching during the battle (even out of combat) except Weapons of course (Adjustable in mangosd.conf).
- Max patch for allowed gear can be set in mangosd.conf.
- Max item level allowed can be set in mangosd.conf.
- Horde and Alliance players are able to chat with each other if they're in the same team.
- Spell Restriciton: You are able to disable every spell you like, adjustable in the "arena_disabled_spells" table.
- Damage & healing done on Scoreboard added, this feature is new and not available in Classic for BGs. Also it does count Pet damage too and not count over damage (So if you kill a Player with 4k hp with a 6k Shadowbolt it adds only 4k damage)

## How to Play

- Add the Maps etc from source\arena_data into you existing data folder (only the arena maps are contained).
- Add source\sql\arena\world_arena.sql on your world DB.
- Download Patch-3.MPQ and put it into your World of Warcraft\Data Folder: https://github.com/Oroxzy/VMaNGOSArenaPatch.
- Use `.lookup object arena orb` or `.gobj add 187078` to place the Arena Orb and queue up.
![grafik](https://user-images.githubusercontent.com/357606/208283756-fcb72a8e-41e5-4f85-b2b4-9f3f527ab85c.png)

