/*
 * sdl2.c
 * sdl2 interfaces -- based on sdl.c
 *
 * (C) 2001 Damian Gryski <dgryski@uwaterloo.ca> (SDL 1 version)
 * (C) 2021 rofl0r (SDL2 port)
 * Joystick code contributed by David Lau
 *
 * Licensed under the GPLv2, or later.
 *
 * TODO: evaluate whether double buffering is desirable
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>


#include "fb.h"
#include "input.h"
#include "rc.h"

struct fb fb;

static int fullscreen = 0;
static int use_altenter = 1;
static int use_joy = 1, sdl_joy_num;
static SDL_Joystick * sdl_joy = NULL;
static const int joy_commit_range = 3276;
static char Xstatus, Ystatus;

static SDL_Window *win;
static SDL_Renderer *renderer;
static SDL_Surface *screen;
static SDL_Texture *texture;

static int vmode[3] = { 0, 0, 32 };

rcvar_t vid_exports[] =
{
	RCV_VECTOR("vmode", &vmode, 3),
	RCV_BOOL("fullscreen", &fullscreen),
	RCV_BOOL("altenter", &use_altenter),
	RCV_END
};

rcvar_t joy_exports[] =
{
	RCV_BOOL("joy", &use_joy),
	RCV_END
};

/* keymap - mappings of the form { scancode, localcode } - from sdl/keymap.c */
extern int keymap[][2];

static int mapscancode(SDL_Keycode sym)
{
	/* this could be faster:  */
	/*  build keymap as int keymap[256], then ``return keymap[sym]'' */

	int i;
	for (i = 0; keymap[i][0]; i++)
		if (keymap[i][0] == sym)
			return keymap[i][1];
	if (sym >= '0' && sym <= '9')
		return sym;
	if (sym >= 'a' && sym <= 'z')
		return sym;
	return 0;
}


static void joy_init()
{
	int i;
	int joy_count;

	/* Initilize the Joystick, and disable all later joystick code if an error occured */
	if (!use_joy) return;

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK))
		return;

	joy_count = SDL_NumJoysticks();

	if (!joy_count)
		return;

	/* now try and open one. If, for some reason it fails, move on to the next one */
	for (i = 0; i < joy_count; i++)
	{
		sdl_joy = SDL_JoystickOpen(i);
		if (sdl_joy)
		{
			sdl_joy_num = i;
			break;
		}
	}

	/* make sure that Joystick event polling is a go */
	SDL_JoystickEventState(SDL_ENABLE);
}

void vid_init()
{
	int flags;
	int scale = rc_getint("scale");

	if (!vmode[0] || !vmode[1])
	{
		if (scale < 1) scale = 1;
		vmode[0] = 160 * scale;
		vmode[1] = 144 * scale;
	}

	flags = SDL_WINDOW_OPENGL;

	if (fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_VIDEO))
		die("SDL: Couldn't initialize SDL: %s\n", SDL_GetError());

	if (!(win = SDL_CreateWindow("gnuboy", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, vmode[0], vmode[1], flags)))
		die("SDL: can't set video mode: %s\n", SDL_GetError());

	/* for SDL2, which uses OpenGL, we internally use scale 1 and
	   render everything into a 32bit high color buffer, and let the
	   hardware do the scaling; thus "fb.delegate_scaling" */

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, scale, scale);
	screen = SDL_CreateRGBSurface(0, 160, 144, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, 160, 144);

	SDL_ShowCursor(0);

	joy_init();

	SDL_LockSurface(screen);

	fb.delegate_scaling = 1;
	fb.w = 160;
	fb.h = 144;
	fb.pelsize = 4;
	fb.pitch = screen->pitch;
	fb.indexed = 0;
	fb.ptr = screen->pixels;
	fb.cc[0].r = screen->format->Rloss;
	fb.cc[0].l = screen->format->Rshift;
	fb.cc[1].r = screen->format->Gloss;
	fb.cc[1].l = screen->format->Gshift;
	fb.cc[2].r = screen->format->Bloss;
	fb.cc[2].l = screen->format->Bshift;

	SDL_UnlockSurface(screen);

	fb.enabled = 1;
	fb.dirty = 0;
}


void ev_poll()
{
	event_t ev;
	SDL_Event event;
	int axisval;

	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_WINDOWEVENT:
			/* https://wiki.libsdl.org/SDL_WindowEvent */
			switch(event.window.event) {
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_HIDDEN:
				fb.enabled = 0; break;
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_RESTORED:
				fb.enabled = 1; break;
			}
			break;
		case SDL_KEYDOWN:
			if ((event.key.keysym.sym == SDLK_RETURN) && (event.key.keysym.mod & KMOD_ALT)) {
				SDL_SetWindowFullscreen(win, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
				fullscreen = !fullscreen;
			}
			ev.type = EV_PRESS;
			ev.code = mapscancode(event.key.keysym.sym);
			ev_postevent(&ev);
			break;
		case SDL_KEYUP:
			ev.type = EV_RELEASE;
			ev.code = mapscancode(event.key.keysym.sym);
			ev_postevent(&ev);
			break;
		case SDL_JOYAXISMOTION:
			switch (event.jaxis.axis)
			{
			case 0: /* X axis */
				axisval = event.jaxis.value;
				if (axisval > joy_commit_range)
				{
					if (Xstatus==2) break;

					if (Xstatus==0)
					{
						ev.type = EV_RELEASE;
						ev.code = K_JOYLEFT;
						ev_postevent(&ev);
					}

					ev.type = EV_PRESS;
					ev.code = K_JOYRIGHT;
					ev_postevent(&ev);
					Xstatus=2;
					break;
				}

				if (axisval < -(joy_commit_range))
				{
					if (Xstatus==0) break;

					if (Xstatus==2)
					{
						ev.type = EV_RELEASE;
						ev.code = K_JOYRIGHT;
						ev_postevent(&ev);
					}

					ev.type = EV_PRESS;
					ev.code = K_JOYLEFT;
					ev_postevent(&ev);
					Xstatus=0;
					break;
				}

				/* if control reaches here, the axis is centered,
				 * so just send a release signal if necisary */

				if (Xstatus==2)
				{
					ev.type = EV_RELEASE;
					ev.code = K_JOYRIGHT;
					ev_postevent(&ev);
				}

				if (Xstatus==0)
				{
					ev.type = EV_RELEASE;
					ev.code = K_JOYLEFT;
					ev_postevent(&ev);
				}
				Xstatus=1;
				break;

			case 1: /* Y axis*/
				axisval = event.jaxis.value;
				if (axisval > joy_commit_range)
				{
					if (Ystatus==2) break;

					if (Ystatus==0)
					{
						ev.type = EV_RELEASE;
						ev.code = K_JOYUP;
						ev_postevent(&ev);
					}

					ev.type = EV_PRESS;
					ev.code = K_JOYDOWN;
					ev_postevent(&ev);
					Ystatus=2;
					break;
				}

				if (axisval < -joy_commit_range)
				{
					if (Ystatus==0) break;

					if (Ystatus==2)
					{
						ev.type = EV_RELEASE;
						ev.code = K_JOYDOWN;
						ev_postevent(&ev);
					}

					ev.type = EV_PRESS;
					ev.code = K_JOYUP;
					ev_postevent(&ev);
					Ystatus=0;
					break;
				}

				/* if control reaches here, the axis is centered,
				 * so just send a release signal if necisary */

				if (Ystatus==2)
				{
					ev.type = EV_RELEASE;
					ev.code = K_JOYDOWN;
					ev_postevent(&ev);
				}

				if (Ystatus==0)
				{
					ev.type = EV_RELEASE;
					ev.code = K_JOYUP;
					ev_postevent(&ev);
				}
				Ystatus=1;
				break;
			}
			break;
		case SDL_JOYBUTTONUP:
			if (event.jbutton.button>15) break;
			ev.type = EV_RELEASE;
			ev.code = K_JOY0 + event.jbutton.button;
			ev_postevent(&ev);
			break;
		case SDL_JOYBUTTONDOWN:
			if (event.jbutton.button>15) break;
			ev.type = EV_PRESS;
			ev.code = K_JOY0+event.jbutton.button;
			ev_postevent(&ev);
			break;
		case SDL_QUIT:
			exit(1);
			break;
		default:
			break;
		}
	}
}

void vid_setpal(int i, int r, int g, int b)
{
	/* not supposed to be called */
}

void vid_preinit()
{
}

void vid_close()
{
	SDL_UnlockSurface(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	fb.enabled = 0;
}

void vid_settitle(char *title)
{
	SDL_SetWindowTitle(win, title);
}

void vid_begin()
{
	SDL_LockSurface(screen);
	fb.ptr = screen->pixels;
}

void vid_end()
{
	SDL_UnlockSurface(screen);
	if (fb.enabled) {
		SDL_UpdateTexture(texture, 0, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}








