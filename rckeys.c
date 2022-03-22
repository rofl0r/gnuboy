#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <string.h>



#include <stdlib.h>

#include "defs.h"
#include "rc.h"
#include "input.h"




char *keybind[MAX_KEYS];




int rc_bindkey(char *keyname, char *cmd)
{
	int key;
	char *a;

	key = k_keycode(keyname);
	if (!key) return -1;

	a = strdup(cmd);
	if (!a) die("out of memory binding key\n");

	if (keybind[key]) free(keybind[key]);
	keybind[key] = a;
	
	return 0;
}



int rc_unbindkey(char *keyname)
{
	int key;

	key = k_keycode(keyname);
	if (!key) return -1;
	
	if (keybind[key]) free(keybind[key]);
	keybind[key] = NULL;
	return 0;
}


void rc_unbindall()
{
	int i;

	for (i = 0; i < MAX_KEYS; i++)
	{
		if (keybind[i])
		{
			free(keybind[i]);
			keybind[i] = NULL;
		}
	}
}

char *rc_getkeybind(int key)
{
	if(key <= 0 || key > MAX_KEYS) return 0;
	return keybind[key];
}

int rc_dokey(int key, int st)
{
	int ret;
	if (!keybind[key]) return 0;
	if (keybind[key][0] != '+' && !st) return 0;
	
	if (st)
		ret = rc_command(keybind[key]);
	else
	{
		keybind[key][0] = '-';
		ret = rc_command(keybind[key]);
		keybind[key][0] = '+';
	}
	return ret;
}




