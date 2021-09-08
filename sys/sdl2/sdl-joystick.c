#ifndef SDL_H_INCLUDED
#include <SDL2/SDL.h>
#endif

#include "input.h"
#include "rc.h"

enum joyaxis { JA_X=0, JA_Y };
enum joyaxisvalue {
	JAV_LEFT_OR_UP = 0, JAV_CENTERED = 1, JAV_RIGHT_OR_DOWN = 2
};


#define JOY_COMMIT_RANGE 3276

static int use_joy = 1, sdl_joy_num;
static SDL_Joystick * sdl_joy = NULL;
static unsigned char Xstatus, Ystatus;

rcvar_t joy_exports[] =
{
	RCV_BOOL("joy", &use_joy, "enable joystick"),
	RCV_END
};

void joy_init()
{
	int i;
	int joy_count;

	Xstatus = Ystatus = JAV_CENTERED;

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

void joy_close() {}

static void joyaxis_evt(enum joyaxis axis, enum joyaxisvalue newstate)
{
	static const struct {
		unsigned char *const axis;
		int action[3];
	} axis_data[2] = {
		{&Xstatus, {K_JOYLEFT, 0, K_JOYRIGHT}},
		{&Ystatus, {K_JOYUP, 0, K_JOYDOWN}},
	};
	event_t ev;
	if (*axis_data[axis].axis == newstate) return;
	/* release last state */
	ev.type = EV_RELEASE;
	ev.code = axis_data[axis].action[*axis_data[axis].axis];
	ev_postevent(&ev);
	/* store new state */
	*axis_data[axis].axis = newstate;
	/* fire new event, if necessary */
	if (newstate != 1) {
		ev.type = EV_PRESS;
		ev.code = axis_data[axis].action[newstate];
		ev_postevent(&ev);
	}
}

void sdljoy_process_event(SDL_Event *event)
{
	enum joyaxis ja;
	event_t ev;

	switch(event->type) {
	case SDL_JOYHATMOTION:
		switch (event->jhat.value) {
		case SDL_HAT_LEFTUP:
			joyaxis_evt(JA_X, JAV_LEFT_OR_UP);
			joyaxis_evt(JA_Y, JAV_LEFT_OR_UP);
			break;
		case SDL_HAT_UP:
			joyaxis_evt(JA_Y, JAV_LEFT_OR_UP);
			break;
		case SDL_HAT_RIGHTUP:
			joyaxis_evt(JA_X, JAV_RIGHT_OR_DOWN);
			joyaxis_evt(JA_Y, JAV_LEFT_OR_UP);
			break;
		case SDL_HAT_LEFT:
			joyaxis_evt(JA_X, JAV_LEFT_OR_UP);
			break;
		case SDL_HAT_CENTERED:
			joyaxis_evt(JA_X, JAV_CENTERED);
			joyaxis_evt(JA_Y, JAV_CENTERED);
			break;
		case SDL_HAT_RIGHT:
			joyaxis_evt(JA_X, JAV_RIGHT_OR_DOWN);
			break;
		case SDL_HAT_LEFTDOWN:
			joyaxis_evt(JA_X, JAV_LEFT_OR_UP);
			joyaxis_evt(JA_Y, JAV_RIGHT_OR_DOWN);
			break;
		case SDL_HAT_DOWN:
			joyaxis_evt(JA_Y, JAV_RIGHT_OR_DOWN);
			break;
		case SDL_HAT_RIGHTDOWN:
			joyaxis_evt(JA_X, JAV_RIGHT_OR_DOWN);
			joyaxis_evt(JA_Y, JAV_RIGHT_OR_DOWN);
			break;
		}
		break;
	/* case SDL_CONTROLLERAXISMOTION: */
	case SDL_JOYAXISMOTION:
		ja = JA_Y;
		switch (event->jaxis.axis & 1)
		{
		case 0:
			ja = JA_X;
			/* fall-through */
		case 1:
			if (event->jaxis.value > JOY_COMMIT_RANGE)
				joyaxis_evt(ja, JAV_RIGHT_OR_DOWN);
			else if (event->jaxis.value < -JOY_COMMIT_RANGE)
				joyaxis_evt(ja, JAV_LEFT_OR_UP);
			else
				joyaxis_evt(ja, JAV_CENTERED);
			break;
		}
		break;
	case SDL_JOYBUTTONUP:
		if (event->jbutton.button>15) break;
		ev.type = EV_RELEASE;
		ev.code = K_JOY0 + event->jbutton.button;
		ev_postevent(&ev);
		break;
	case SDL_JOYBUTTONDOWN:
		if (event->jbutton.button>15) break;
		ev.type = EV_PRESS;
		ev.code = K_JOY0+event->jbutton.button;
		ev_postevent(&ev);
		break;
	}
}
