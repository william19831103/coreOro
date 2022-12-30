![vanilla_arena-min](https://user-images.githubusercontent.com/357606/210061106-d7628cdd-3e9a-454e-9998-67477786d041.png)



## VMaNGOS Arena (ONLY FOR THE OLD 1.12 CLIENT!)
One of my first Projects for VMaNGOS with almost no coding knowledge just copy-paste, endless tries and a lot of beer.
I know it's horrible coded... i don't care. It works and was funny.

## Features:

- Arena Watcher NPC in the Arena: speak to him to start the arena immediately (if everyone does it).
- Solo & Group queue for 1v1, 2v2, 3v3 and 5v5
- Cross Faction support (Horde can team up with Alliance and vice versa).
- Disabled Item switching during the battle (even out of combat) except Weapons of course (Adjustable in cfg).
- Max patch for allowed gear can be set in cfg.
- Max item level allowed can be set in cfg.
- Horde and Alliance can chat with each other if they're in the same arena team.
- Spell Restriciton: You are able to disable every spell you like, adjustable in the "arena_disabled_spells" table.
- ![grafik](https://user-images.githubusercontent.com/357606/208445837-caa9f3fe-e2ca-484b-b9a0-edf676f698c2.png)

- Damage & healing done on Scoreboard added, this feature is new and not available in Classic for BGs. Also it does count Pet damage too and not count over damage (So if you kill a Player with 4k hp with a 6k Shadowbolt it adds only 4k damage)

### What Todo

- Add the Maps etc from source\arena_data into you existing data folder (only the arena maps are contained)
- Add source\sql\arena\world_arena.sql on your world DB
- Download Patch-3.MPQ and put it into your World of Warcraft\Data Folder: https://github.com/Oroxzy/VMaNGOSArenaPatch
- Use `.lookup object arena orb` or `.gobj add 187078` to place the Arena Orb and queue up

- ![grafik](https://user-images.githubusercontent.com/357606/208283756-fcb72a8e-41e5-4f85-b2b4-9f3f527ab85c.png)

- Have fun
