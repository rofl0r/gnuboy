/*
** thinlib (c) 2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** tl_mouse.c
**
** DOS mouse handling routines
** $Id: tl_mouse.c,v 1.4 2000/12/16 21:18:11 matt Exp $
*/

#include <stdio.h>
#include <dpmi.h>
#include <go32.h>

#include "tl_types.h"
#include "tl_mouse.h"

#define  MOUSE_MAX_BUTTONS 3
#define  MOUSE_FIX         8  // 24.8 fixpoint

#define  MOUSE_INT         0x33
#define  INT_GET_MICKEYS   0x0B
#define  INT_GET_BUTTONS   0x03

static struct
{
   int xpos, ypos;
   int last_x, last_y;
   int maxwidth, maxheight;
   int num_buttons;
   int delta_shift;
   uint8 button;
   bool enabled;
} mouse;

static void _get_mickeys(void)
{
   __dpmi_regs r;

   /* get mickeys */
   r.x.ax = INT_GET_MICKEYS;
   __dpmi_int(MOUSE_INT, &r);
   mouse.xpos += ((int16) r.x.cx << MOUSE_FIX) >> mouse.delta_shift;
   mouse.ypos += ((int16) r.x.dx << MOUSE_FIX) >> mouse.delta_shift;

   if (mouse.xpos < 0)
      mouse.xpos = 0;
   else if (mouse.xpos > mouse.maxwidth)
      mouse.xpos = mouse.maxwidth;

   if (mouse.ypos < 0)
      mouse.ypos = 0;
   else if (mouse.ypos > mouse.maxheight)
      mouse.ypos = mouse.maxheight;
}

static uint8 _get_buttons(void)
{
   __dpmi_regs r;
   uint8 left, middle, right;
   
   r.x.ax = INT_GET_BUTTONS;
   __dpmi_int(MOUSE_INT, &r);

   left = (r.x.bx & 1);
   right = ((r.x.bx >> 1) & 1);
   middle = ((r.x.bx >> 2) & 1);

   mouse.button = right << THIN_MOUSE_RIGHT
                  | middle << THIN_MOUSE_MIDDLE
                  | left << THIN_MOUSE_LEFT;

   return mouse.button;
}

uint8 thin_mouse_getmotion(int *dx, int *dy)
{
   if (false == mouse.enabled)
   {
      *dx = *dy = 0;
      return 0;
   }

   mouse.last_x = mouse.xpos;
   mouse.last_y = mouse.ypos;

   _get_mickeys();

   *dx = (mouse.xpos - mouse.last_x) >> MOUSE_FIX;
   *dy = (mouse.ypos - mouse.last_y) >> MOUSE_FIX;

   return _get_buttons();
}

uint8 thin_mouse_getpos(int *x, int *y)
{
   if (false == mouse.enabled)
   {
      *x = *y = 0;
      return 0;
   }

   _get_mickeys();

   *x = mouse.xpos >> MOUSE_FIX;
   *y = mouse.ypos >> MOUSE_FIX;

   return _get_buttons();
}

void thin_mouse_setrange(int width, int height)
{
   if (false == mouse.enabled)
      return;

   mouse.maxwidth = (width - 1) << MOUSE_FIX;
   mouse.maxheight = (height - 1) << MOUSE_FIX;
}

/* Set up mouse, center pointer */
int thin_mouse_init(int width, int height, int delta_shift)
{
   __dpmi_regs r;

   r.x.ax = 0x00;
   __dpmi_int(MOUSE_INT, &r);

   if (0 == r.x.ax)
   {
      mouse.enabled = false;
      return -1;
   }

   mouse.enabled = true;
   mouse.num_buttons = r.x.bx;
   if (r.x.bx == 0xFFFF)
      mouse.num_buttons = 2;

   mouse.delta_shift = delta_shift;

   if (mouse.num_buttons > 3)
      mouse.num_buttons = 3;

   mouse.button = 0;
   mouse.maxwidth = (width - 1) << MOUSE_FIX;
   mouse.maxheight = (height - 1) << MOUSE_FIX;
   mouse.xpos = (width / 2) << MOUSE_FIX;
   mouse.ypos = (height / 2) << MOUSE_FIX;
   mouse.last_x = mouse.xpos;
   mouse.last_y = mouse.ypos;

   return 0;
}

/*
** $Log: tl_mouse.c,v $
** Revision 1.4  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.3  2000/11/06 02:22:09  matt
** button bug
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
