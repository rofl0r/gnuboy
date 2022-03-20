/*
MIT License
Copyright (C) 2022 rofl0r

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* a single-file library to implement a simple menu system in emulators.
   it mainly deals with scrolling/selection in a list of string items.
   the interfacing code supplies a list of strings, optional header and
   footer text, and sends up/down key movement user input. ezmenu then
   takes care of scrolling up and down, so the interface code simply
   has to print the vislines onto the screen using the fontsize
   specified in ezmenu_init(). if a user presses e.g. enter to activate
   a menu item, the interface code has to deal with the input itself
   and read vislines[vissel] to find out which entry was activated. */

#include <string.h>
#include <stdlib.h>

struct ezmenu {
	/* internal list of lines, passed via ezmenu_setlines.
	   if you use malloc()'d strings here you need to free them before
	   repopulating the list with new lines. */
	char **lines;
	/* these are the visible lines you need to render. */
	char **vislines;
	char *header, *footer;
	/* internal bookkeeping for the amount of lines in "lines". */
	unsigned linecount;
	int sel; /* internal bookkeeping of which line of lines is selected */
	int vissel; /* which line out of vislines is selected, for extern use */
	int w, h; /* width and height of visible character grid, in cells */
	int wraparound; /* whether to wrap around if user presses up on first item, or down on last */
};

/* ezmenu only cares about down and up events, to scroll and select.
   any other event is handled by the caller, e.g. if button "a" was
   pressed, you gotta extract the text of vislines[vissel] and act
   upon it. */
enum ezmenu_input {
	EZM_DOWN,
	EZM_UP,
};

static void ezmenu_init(struct ezmenu *m, int hres, int vres,
	int fontw, int fonth) {
	memset(m, 0, sizeof *m);
	m->w = hres/fontw;
	m->h = vres/fonth;
	m->vislines = calloc(sizeof(char*), m->h);
}

static void ezmenu_setlines(struct ezmenu *m, char**lines, unsigned linecount) {
	m->lines = lines;
	m->linecount = linecount;
	m->sel = 0;
}

static void ezmenu_setheader(struct ezmenu *m, char* hdr) {
	m->header = hdr;
	m->vislines[0] = hdr;
	m->vislines[1] = "";
}

static void ezmenu_setfooter(struct ezmenu *m, char* ftr) {
	m->footer = ftr;
	m->vislines[m->h-2] = "";
	m->vislines[m->h-1] = ftr;
}
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
static void ezmenu_update(struct ezmenu *m) {
	int start = (!!m->header)*2, end = m->h - (!!m->footer)*2;
	int vis = end - start;
	int y, ly = MAX(0, m->sel - vis/2);
	m->vissel = (m->sel-ly)+(!!m->header)*2;
	for(y = start; y < end; ++y) {
		m->vislines[y] = ly>=m->linecount?"":m->lines[ly++];
	}
}

static void ezmenu_userinput(struct ezmenu *m, enum ezmenu_input inp) {
	static const int dir[] = {[EZM_DOWN] = 1, [EZM_UP] = -1};
	m->sel += dir[inp];
	if(m->sel < 0) {
		if(m->wraparound)
			m->sel = m->linecount-1;
		else
			m->sel = 0;
	}
	else if(m->sel >= m->linecount) {
		if(m->wraparound)
			m->sel = 0;
		else
			m->sel = m->linecount-1;
	}
	ezmenu_update(m);
}
