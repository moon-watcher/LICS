#include "boingo.h"
#include "vramslots.h"
#include "player.h"
#include "map.h"

void en_init_boingo(en_boingo *e)
{
	e->head.hp = 1;
	e->head.width = 9;
	e->head.height = 15;
	e->head.direction = ENEMY_RIGHT;
	e->head.size[0] = SPRITE_SIZE(3,2);
	e->head.attr[1] = 0;
	e->head.xoff[0] = -12;
	e->head.x += 12;
	e->head.x += 15;

	e->dy = FIX16(0.0);
	e->state = BOINGO_STANDING;
	e->jump_cnt = 0;
	e->anim_cnt = 0;
}

void en_anim_boingo(en_boingo *e)
{
	e->anim_cnt++;


	if (e->state == BOINGO_STANDING)
	{
		e->head.width = BOINGO_GND_W;
		e->head.height = BOINGO_GND_H;
		e->head.xoff[0] = -12;
		e->head.yoff[0] = -15;
		e->head.size[0] = SPRITE_SIZE(3,2);

		e->head.attr[0] = TILE_ATTR_FULL(PLAYER_PALNUM, 0, 0, 0, BOINGO_VRAM_SLOT); 

		if (e->anim_cnt >= BOINGO_ANIM_SPEED_STAND)
		{
			e->anim_cnt = 0;
			e->anim_frame++;
		}
	}
	else
	{
		e->head.width = BOINGO_AIR_W;
		e->head.height = BOINGO_AIR_H;
		e->head.xoff[0] = -8;
		e->head.yoff[0] = -19;
		e->head.size[0] = SPRITE_SIZE(2,3);

		e->head.attr[0] = TILE_ATTR_FULL(PLAYER_PALNUM, 0, 0, 0, BOINGO_VRAM_SLOT + 12);

		if (e->anim_cnt >= BOINGO_ANIM_SPEED_JUMP)
		{
			e->anim_cnt = 0;
			e->anim_frame++;
		}
	}

	if (e->anim_frame == 2)
	{
		e->anim_frame = 0;
	}
	if (e->anim_frame)
	{
		e->head.attr[0] += 6;
	}
}

static void bg_collisions(en_boingo *e)
{
	// Collision with ceiling
	if (e->dy < FIX16(0.0) && map_collision(e->head.x, e->head.y - BOINGO_AIR_H))
	{
		e->dy = FIX16(2.0);
		e->head.y += 5;
	}
	// With ground
	else if (e->dy > FIX16(0.0) && map_collision(e->head.x, e->head.y))
	{
		e->dy = FIX16(0.0);
		// Lock to 8x8 grid
		e->head.y = (e->head.y - 1) & 0xFFF8;
		return;
	}

	// Left side wall
	if (e->head.direction == ENEMY_LEFT && 
	    map_collision(e->head.x - BOINGO_AIR_W, e->head.y - (BOINGO_AIR_H / 2)))
	{
		e->head.direction = ENEMY_RIGHT;
		e->head.x += 4;
	}
	else if (e->head.direction == ENEMY_RIGHT && 
	    map_collision(e->head.x + BOINGO_AIR_W, e->head.y - (BOINGO_AIR_H / 2)))
	{
		e->head.direction = ENEMY_LEFT;
		e->head.x -= 4;
	}
}

static const fix16 str_table[] = 
{
	FIX16(0.00),
	FIX16(-0.70),
	FIX16(-1.41),
	FIX16(-2.20),
	FIX16(-2.81),
	FIX16(-3.52),
	FIX16(-4.22),
	FIX16(-5.0)
};

static void do_jump(en_boingo *e)
{
	if (pl.px < e->head.x)
	{
		e->head.direction = ENEMY_LEFT;
	}
	else
	{
		e->head.direction = ENEMY_RIGHT;
	}
	// Base jump strength
	e->dy = FIX16(-1.0);

	// Additional jump strength from random generator
	e->dy += str_table[GET_HVCOUNTER & 0x07];
	e->state = BOINGO_JUMPING;
	e->jump_cnt = 0;
}

void en_proc_boingo(en_boingo *e)
{
	// Standing state
	if (e->state == BOINGO_STANDING)
	{
		if (e->jump_cnt >= BOINGO_JUMP_TIME)
		{
			do_jump(e);
		}
		else
		{
			e->jump_cnt++;
		}
	}
	else if (e->state == BOINGO_JUMPING)
	{
		e->head.y += fix16ToInt(e->dy);
		e->dy = fix16Add(e->dy, BOINGO_GRAVITY);
		e->head.x += (e->head.direction == ENEMY_RIGHT) ? 1 : -1;
		bg_collisions(e);
	}
}
