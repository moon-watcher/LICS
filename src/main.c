#include <genesis.h>
#include "mpad.h"
#include "sprites.h"
#include "player.h"
#include "system.h"
#include "gfx.h"
#include "pal.h"
#include "map.h"
#include "mapdata.h"
#include "vramslots.h"
#include "state.h"

void player_test(void)
{
	player pl;
	state_load_room(1);
	map_draw_full(0,0);
	/*
	for (int i = 0; i < 32; i++)
	{

		VDP_doVRamDMA((state.current_map + (80 * state.current_room->w) * (i)),VDP_getAPlanAddress() + 128 * i,40);
	}
	*/

	player_init(&pl);	
	pl.y = intToFix32(160);
	pl.x = intToFix32(224 - 32);
	for (;;)
	{
		player_eval_grounded(&pl);
		player_input(&pl);
		player_cp(&pl);
		player_accel(&pl);
		player_jump(&pl);
		player_move(&pl);

		player_eval_grounded(&pl);
		player_calc_anim(&pl);

		state_update_scroll(fix32ToInt(pl.x),fix32ToInt(pl.y));
		player_draw(&pl);
		
		system_wait_v();
		if (pl.input & KEY_A)
		{
			map_draw_full(state.cam_x,state.cam_y);
		}
		player_dma(&pl);
		sprites_dma_simple();

	}
}

int main(void)
{
	system_init();

	player_test();
	return 0;	
}
