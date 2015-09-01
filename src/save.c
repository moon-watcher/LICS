#include "save.h"

save_file sram;

void save_write(void)
{
	SRAM_enable();
	u16 *sf = (u16 *)&sram;
	for (u16 i = 0; i < (sizeof(save_file) / sizeof(u16)); i++)
	{
		SRAM_writeWord(i * 2, sf[i]);
	}
	SRAM_disable();
}

void save_load(void)
{
	SRAM_enableRO();
	u16 *sf = (u16 *)&sram;
	for (u16 i = 0; i < (sizeof(save_file) / sizeof(u16)); i++)
	{
		sf[i] = SRAM_readWord(i * 2);
	}
	SRAM_disable();
}

void save_clear(void)
{
	u16 *sf = (u16 *)&sram;
	for (u16 i = 0; i < (sizeof(save_file) / sizeof(u16)); i++)
	{
		sf[i] = 0;
	}	
	sram.magic_0 = SAVE_MAGIC;
	sram.magic_1 = SAVE_MAGIC;
	sram.magic_2 = SAVE_MAGIC;

	save_write();
}
