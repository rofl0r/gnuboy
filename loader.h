

#ifndef __LOADER_H__
#define __LOADER_H__


typedef struct loader_s
{
	char *rom;
	char *base;
	char *sram;
	char *state;
	int ramloaded;
} loader_t;


extern loader_t loader;


int rom_load();
int sram_load();
int sram_save();

int loader_init(char *s);
void loader_unload(void);
char *loader_get_error();
void loader_set_error(char *fmt, ...);

void state_save(int n);
void state_load(int n);

#endif


