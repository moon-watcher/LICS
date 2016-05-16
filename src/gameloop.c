#include "gameloop.h"
#include "sprites.h"
#include "player.h"
#include "system.h"
#include "gfx.h"
#include "pal.h"
#include "map.h"
#include "mapdata.h"
#include "vramslots.h"
#include "state.h"
#include "save.h"
#include "cubes.h"
#include "particles.h"
#include "hud.h"
#include "music.h"
#include "bg.h"
#include "enemy.h"
#include "powerups.h"
#include "pause.h"
#include "projectiles.h"

#define GAMELOOP_PLANE_W 64
#define GAMELOOP_PLANE_H 32

static u16 first_entrance;

static void puts(const char *s, u16 x, u16 y)
{
	while (*s)
	{
		VDP_setTileMapXY(VDP_getWindowPlanAddress(), TILE_ATTR_FULL(1, 1, 0, 0, 0x500 + *s), x, y);
		x++;
		s++;
	}
}

static void message_screen(const char *s)
{
	u16 i = 60 * 5;
	VDP_doVRamDMAFill(VDP_getWindowAddress(), 64 * 2 * 32, 0);
	VDP_waitDMACompletion();
	puts(s, 8, 8);
//	VDP_drawTextBG(VDP_getWindowPlanAddress(), s, TILE_ATTR(1, 1, 0, 0), 4, 6);
	VDP_setReg(0x12, 0x1E);
	while (i--)
	{
		system_wait_v();
		sprites_dma_simple();
	}
	VDP_setReg(0x12, 0x00);
}

void gameloop_logic(void)
{
	player_run();
	enemy_run();
	cubes_run();
	powerup_run();
	projectiles_run();
	particles_run();
	sfx_counters();
	state_update_progress();

}

void gameloop_gfx(void)
{
	/* BG updates for scrolling */
	map_draw_diffs(state_update_scroll());

	/* Place sprites */
	hud_draw_health(sram.max_hp,pl.hp);
	if (sram.have_phantom)
	{
		hud_draw_cp(pl.cp + 1 + ((pl.cp + 1) >> 1)); // CP scaled 32 --> 48
	}
	particles_draw();
	enemy_draw();
	player_draw();
	cubes_draw();
	projectiles_draw();
	powerup_draw();
	system_debug_cpu_meter();

}

void gameloop_dma(void)
{
	map_dma();
	sprites_dma_simple();
	state_dma_scroll();	
	player_dma();
}

static void gameloop_room_setup(u16 transition)
{	
	// Reset object lists, gameplay variables, etc.
	system_wait_v();
	cubes_init();
	enemy_init();
	particles_init();
	projectiles_init();
	powerup_init();
	player_init_soft();
	pause_init();
	system_wait_v();
	// Load the next room
	state_load_room(state.next_id);

	// Determine whether or not we need to blank for tile changes
	// This is a little hack to make the transition from title screen to game start not flash
	u16 should_blank = !first_entrance;
	first_entrance = 1;

	// Blank the display
	if (should_blank)
	{
		VDP_setEnable(0);
		// Load a bogus backdrop to force it to reload the BG
		bg_load(255);
	}

	// Depending on room entry, the player may need to jump into the frame
	if (transition == STATE_TRANSITION_UP)
	{
		player_do_jump();
	}

	// Locate entry position for player
	player_set_xy_fix32(state_get_entrance_x(), state_get_entrance_y());

	// DMA needed graphics for this room
	map_load_tileset(state.current_room->tileset);

	// Re-load the usuals in case something else is in VRAM there
	particles_dma_tiles();
	projectiles_dma_tiles();
	cube_dma_tiles();
	hud_dma_tiles();
	pause_dma_tiles();
	enemy_dma_tiles();
	powerup_dma_tiles();

	// Set up the far backdrop
	bg_load(state.current_room->background);

	// One frame of logic and graphics is evaluated
	gameloop_logic();
	gameloop_gfx();
	gameloop_dma();

	// Play music pulled from room state
	music_play(state.current_room->music);

	// Wait for vblank, no mid-screen changes wanted
	system_wait_v();

	if (should_blank)
	{
		// Restore the VDP output
		VDP_setEnable(1);
	}

	// Save player's progress for frequent auto-save
	save_write();
}

static inline void gameloop_init(void)
{
	// Debug mode cheats
	if (buttons & BUTTON_X)	
	{
		save_clear();
		sram.have_lift = 1;
		sram.have_jump = 1;
		sram.have_kick = 1;
		sram.have_phantom = 1;
		sram.have_map = 1;
	}

	// The game runs with a 512x256 plane in a 320x240 viewing area.
	VDP_setPlanSize(GAMELOOP_PLANE_W, GAMELOOP_PLANE_H);
	VDP_setScreenWidth320();

	// Set base addresses for tables
	VDP_setBPlanAddress(0xC000);
	VDP_setWindowAddress(0xD000);
	VDP_setAPlanAddress(0xE000);
	VDP_setSpriteListAddress(0xF000);

	// Clear both planes
	VDP_clearPlan(VDP_PLAN_A, 1);
	VDP_waitDMACompletion();
	VDP_clearPlan(VDP_PLAN_B, 1);
	VDP_waitDMACompletion();

	VDP_setHilightShadow(0);
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	// Default to no H split 	
	system_set_h_split(0, 0, NULL);

	// Clear sprites
	sprites_init();

	// Initialize backdrop map system
	map_init();

	first_entrance = 0;

}

void gameloop_main(void)
{
	// Make sure the VDP is all set up for playing the game.
	gameloop_init();

	// Variable to track which side of the room the player enters from
	u16 transition = 0;

	// Initialize a little bit of player and game state
	state.next_id = 1;
	state.next_entrance = 0;
	state.current_id = 64;

	player_init();

	// Main game loop; runs until after a player death anim, quit, or victory.
	while (1)
	{
		// Configure the room we are about to enter
		gameloop_room_setup(transition);

		// Run the gameloop until a room transition is detected
		do
		{
			/* Run one frame of engine logic */
			gameloop_logic();
			gameloop_gfx();

			/* Wait for VBlank. */
			system_wait_v();

			gameloop_dma();
			if ((buttons & BUTTON_START) && (!(buttons_prev & BUTTON_START)))
			{
				pause_screen_loop();	
			}

		}
		while (!(transition = state_watch_transitions()));
	}
}
