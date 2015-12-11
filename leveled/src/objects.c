#include "objects.h"

static const char *obj_name[] =
{
	"        ","Room Ptr","Cube    ","Metagrub",
	"Flip    ","Boingo  ","Item    ","Gaxter 1",
	"Gaxter 2","Buggo 1 ","Buggo 2 ","Dncyflwr",
	"Jraff   ","Pilla   ","Hedgedog","Shoot   ",
	"Laser   ","Killzam ","Flargy  ","E. Plant",
	"Tossmuff","Teleprtr","Magibear","Lava Gen",
	"Cow     ","Containr","Hoop    ","Falseblk",
	"CP Pad  ","CP Meter","Blue Dog","Elevator",
	"Ele stop","Fissins ","Boss 1  ","Boss 2  ",
	"Boss F1 ","Boss F2 ","Egg     ","Fissins2",
	"Bounds  ",
	0
};

static const int obj_width[] =
{
	0,16,16,24,
	24,24,16,16,
	16,16,16,24,
	24,16,24,24,
	16,16,16,32,
	16,32,40,16,
	40,16,16,16,
	32,16,48,32,
	32,16,64,64,
	64,64,32,16,
	16,
	0
};

static const int obj_height[] = 
{
	0,32,16,8,
	16,16,16,16,
	16,16,16,48,
	64,16,16,16,
	16,24,32,48,
	24,32,32,16,
	24,16,16,16,
	8,8,32,64,
	8,16,64,64,
	64,64,32,16,
	96,
	0
};

int width_for_obj(int i)
{
	return obj_width[i];
}

int height_for_obj(int i)
{
	return obj_height[i];
}

const char *string_for_obj(int i)
{
	return obj_name[i];
}
