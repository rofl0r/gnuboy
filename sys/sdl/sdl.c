/*
 * sdl.c
 * sdl interfaces -- based on svga.c 
 *
 * (C) 2001 Damian Gryski <dgryski@uwaterloo.ca>
 * Joystick code contributed by David Lau
 * Sound code added by Laguna
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

static int hwsurface = 0;
static int fullscreen = 1;
static int use_altenter = 1;
static int use_joy = 1, sdl_joy_num;
static SDL_Joystick * sdl_joy = NULL;
static const int joy_commit_range = 3276;
static int xaxis_max, yaxis_max;
static char Xstatus, Ystatus;

static SDL_Surface *screen;

static int vmode[3] = { 160, 144, 16 };

rcvar_t vid_exports[] =
{
	RCV_VECTOR("vmode", &vmode, 3),
	RCV_BOOL("sdl_hwsurface", &hwsurface),
	RCV_BOOL("sdl_fullscreen", &fullscreen),
	RCV_BOOL("sdl_altenter", &use_altenter),
	RCV_END
};

rcvar_t joy_exports[] =
{
	RCV_BOOL("joy", &use_joy),
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
	if (sym >= '0' && sym <= '9')
		return sym;
	if (sym >= 'a' && sym <= 'z')
		return sym;
	return 0;
}

void vid_init()
{
	int i;
	int joy_count;
	int flags;
	
	flags = SDL_ANYFORMAT | SDL_HWPALETTE;

	if (hwsurface)
		flags |= SDL_HWSURFACE;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

	if (SDL_Init(SDL_INIT_VIDEO))
		die("SDL: Couldn't initialize SDL: %s\n", SDL_GetError());

	if (!(screen = SDL_SetVideoMode(vmode[0], vmode[1], vmode[2], flags)))
		die("SDL: can't set video mode: %s\n", SDL_GetError());

	SDL_ShowCursor(0);

	fb.w = screen->w;
	fb.h = screen->h;
	fb.pelsize = screen->format->BytesPerPixel;
	fb.pitch = screen->pitch;
	fb.indexed = fb.pelsize == 1;
	fb.ptr = screen->pixels;
	fb.cc[0].r = screen->format->Rloss;
	fb.cc[0].l = screen->format->Rshift;
	fb.cc[1].r = screen->format->Gloss;
	fb.cc[1].l = screen->format->Gshift;
	fb.cc[2].r = screen->format->Bloss;
	fb.cc[2].l = screen->format->Bshift;

	fb.enabled = 1;
	fb.dirty = 0;
	
	/* Initilize the Joystick, and disable all later joystick code if an error occured */
	if (!use_joy) return;
	
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK))
	{
		/* No warning output yet, we'll add verbosity levels later */
		/* fprintf(stderr,"SDL: Couldn't initilize Joystick Subsystem: %s\n",SDL_GetError()); */
		return;
	}
	
	joy_count = SDL_NumJoysticks();
	
	if (!joy_count)
	{
		/* fprintf(stderr, "No joysticks available\n"); */
		return;
	}

#if 0
	/* print available joysticks to the console */		
   	for (i = 0; i < joy_count; i++)
		printf("Joystick %u, %s\n", i, SDL_JoystickName(i));
#endif

	/* now try and open one. If, for some reason it fails, move on to the next one */
	for (i = 0; i < joy_count; i++)
	{
		sdl_joy = SDL_JoystickOpen(i);
		if (sdl_joy)
		{
			sdl_joy_num = i;
			/* printf("Opened Joystick %u of %u (%s)", sdl_joy_num+1, joy_count,SDL_JoystickName(sdl_joy_num)); */
			break;
		}	
	}
	
	/* make sure that Joystick event polling is a go */
	SDL_JoystickEventState(SDL_ENABLE);
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
		case SDL_KEYDOWN:
			if ((event.key.keysym.sym == SDLK_RETURN) && (event.key.keysym.mod & KMOD_ALT))
				SDL_WM_ToggleFullScreen(screen);
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
	SDL_Color col;

	col.r = r; col.g = g; col.b = b;

	SDL_SetColors(screen, &col, i, 1);
}

void vid_preinit()
{
}

void vid_close()
{
	SDL_Quit();
	fb.enabled = 0;
}

void vid_settitle(char *title)
{
	SDL_WM_SetCaption(title, title);
}

void vid_begin()
{
	if (hwsurface) SDL_LockSurface(screen);
}

void vid_end()
{
	if (hwsurface) SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void vid_resize()
{
}




#include "pcm.h"


struct pcm pcm;


static int sound = 1;
static int samplerate = 44100;
static int stereo = 1;
static volatile int audio_done;

rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound),
	RCV_INT("stereo", &stereo),
	RCV_INT("samplerate", &samplerate),
	RCV_END
};


static void audio_callback(void *blah, byte *stream, int len)
{
	memcpy(stream, pcm.buf, len);
	audio_done = 1;
}


void pcm_init()
{
	int i;
	SDL_AudioSpec as;

	if (!sound) return;
	
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	as.freq = samplerate;
	as.format = AUDIO_U8;
	as.channels = 1 + stereo;
	as.samples = samplerate / 60;
	for (i = 1; i < as.samples; i<<=1);
	as.samples = i;
	as.callback = audio_callback;
	as.userdata = 0;
	if (SDL_OpenAudio(&as, 0) == -1)
		return;
	
	pcm.hz = as.freq;
	pcm.stereo = as.channels - 1;
	pcm.len = as.size;
	pcm.buf = malloc(pcm.len);
	pcm.pos = 0;
	memset(pcm.buf, 0, pcm.len);
	
	SDL_PauseAudio(0);
}

int pcm_submit()
{
	if (!pcm.buf) return 0;
	if (pcm.pos < pcm.len) return 1;
	while (!audio_done)
	{
		SDL_Delay(4);
	}
	audio_done = 0;
	pcm.pos = 0;
	return 1;
}

void pcm_close()
{
	if (sound) SDL_CloseAudio();
}






