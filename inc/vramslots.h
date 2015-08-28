#ifndef VRAMSLOTS_H
#define VRAMSLOTS_H
/*

This is where VRAM slots are determined for everything in the game for some
minimal amount of static memory management.

VRAM_SLOT indeces are specified in tiles. Each tile is 32 bytes, but the VDP
routines disregard the LSB so an increment of 1 is moving across an entire
16-bit word. So, 64 bytes in would be address 32, not address 64. This is why
the length of a 256-tile DMA is only (256 * 16).

*/

#define MAP_FG_VRAM_SLOT 0
#define MAP_FG_VRAM_LEN 256

#define MAP_BG_VRAM_SLOT (MAP_FG_VRAM_SLOT + MAP_FG_VRAM_LEN)
#define MAP_BG_VRAM_LEN 512

#define PLAYER_VRAM_SLOT (MAP_BG_VRAM_SLOT + MAP_BG_VRAM_LEN)
#define PLAYER_VRAM_LEN 12


#define MAP_FG_PALNUM 0
#define MAP_BG_PALNUM 1
#define OBJECTS_PALNUM 2
#define PLAYER_PALNUM 3

#endif