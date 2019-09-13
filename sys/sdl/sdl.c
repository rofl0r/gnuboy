/*
 * sdl.c
 * sdl interfaces -- based on svga.c 
 *
 * (C) 2001 Damian Gryski <dgryski@uwaterloo.ca>
 *
 * Licensed under the GPLv2, or later.
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL/SDL.h>


#include "fb.h"
#include "input.h"
#include "rc.h"


struct fb fb;

SDL_Surface *screen;

rcvar_t vid_exports[] =
{
           RCV_END
};

/* keymap - mappings of the form { scancode, localcode } - from sdl/keymap.c */
extern int keymap[][2];

static int mapscancode(SDLKey sym)
{
	/* this could be faster:  */
	/*  build keymap as int keymap[256], then ``return keymap[sym]'' */

	int i;
	for (i = 0; keymap[i][0]; i++)
		if (keymap[i][0] == sym)
			return keymap[i][1];
	if (sym >= 'a' && sym <= 'z')
		return sym;
	return 0;
}

void vid_init()
{
	int video_flags = SDL_HWSURFACE | SDL_HWPALETTE;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL: Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if ((screen = SDL_SetVideoMode(160, 144, 16, video_flags)) == NULL) {
		fprintf(stderr, "SDL: can't set video mode: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	fb.ptr = screen->pixels;
	fb.w = 160;
	fb.h = 144;
	fb.pelsize = 2;
	fb.pitch = 160*2;
	fb.indexed = 0;
	fb.cc[0].l = 11; fb.cc[0].r = 3;
	fb.cc[1].l = 5;  fb.cc[1].r = 2;
	fb.cc[2].l = 0;  fb.cc[2].r = 3;

	fb.enabled = 1;
	fb.dirty = 0;
}


void ev_poll()
{
	event_t ev;
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_KEYDOWN:
			ev.type = EV_PRESS;
			ev.code = mapscancode(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			ev.type = EV_RELEASE;
			ev.code = mapscancode(event.key.keysym.sym);
			break;
		case SDL_QUIT: exit(1);
		default: break;
		}
		ev_postevent(&ev);
	}
}

void vid_setpal(int i, int r, int g, int b)
{
	SDL_Color col;

	col.r = r; col.g = g; col.b = b;

	SDL_SetColors(screen, &col, i, 1);
}

void vid_preinit()
{
}

void vid_close()
{
	fb.enabled = 0;
}

void vid_begin()
{
}

void vid_end()
{
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void vid_resize()
{
}
