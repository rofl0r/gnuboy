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
** tl_joy.c
**
** DOS joystick reading routines
** $Id: tl_joy.c,v 1.3 2000/12/16 21:18:11 matt Exp $
*/

#include <dos.h>

#include "tl_types.h"
#include "tl_djgpp.h"
#include "tl_joy.h"

#define  JOY_PORT          0x201
#define  JOY_TIMEOUT       10000

#define  J1_A              0x10
#define  J1_B              0x20
#define  J2_A              0x40
#define  J2_B              0x80

#define  J1_X              0x01
#define  J1_Y              0x02
#define  J2_X              0x04
#define  J2_Y              0x08

#define  JOY_CENTER        0xAA
#define  JOY_MIN_THRESH    0.7
#define  JOY_MAX_THRESH    1.3

static struct
{
   int x_minthresh, x_maxthresh;
   int y_minthresh, y_maxthresh;
   int x_read, y_read;
   uint8 button_state;
} joystate;

/* Read data in from joy port */
static int joy_portread(void)
{
   /* Set timeout to max number of samples */
   int timeout = JOY_TIMEOUT;
   uint8 port_val;

   joystate.x_read = 0;
   joystate.y_read = 0;

   THIN_DISABLE_INTS();

   /* Clear the latch and request a sample */
   port_val = inportb(JOY_PORT);
   outportb(JOY_PORT, port_val);

   do
   {
      port_val = inportb(JOY_PORT);
      if (port_val & J1_X)
         joystate.x_read++;
      if (port_val & J1_Y)
         joystate.y_read++;
   }
   while (--timeout && (port_val & 3));

   joystate.button_state = port_val;

   THIN_ENABLE_INTS();

   if (0 == timeout)
      return -1;
   else
      return 0;
}

int thin_joy_read(joy_t *joy)
{
   if (joy_portread())
      return -1;

   /* Calc X axis */
   joy->left = (joystate.x_read < joystate.x_minthresh) ? true : false;
   joy->right = (joystate.x_read > joystate.x_maxthresh) ? true : false;

   /* Calc Y axis */
   joy->up = (joystate.y_read < joystate.y_minthresh) ? true : false;
   joy->down = (joystate.y_read > joystate.y_maxthresh) ? true : false;

   /* Get button status */
   /* note that buttons returned by hardware are inverted logic */
   joy->button[0] = (joystate.button_state & J1_A) ? false : true;
   joy->button[1] = (joystate.button_state & J2_A) ? false : true;
   joy->button[2] = (joystate.button_state & J1_B) ? false : true;
   joy->button[3] = (joystate.button_state & J2_B) ? false : true;

   return 0;
}

/* Detect presence of joystick */
int thin_joy_init(void)
{
   if (joy_portread())
      return -1;

   /* Set the threshhold */
   joystate.x_minthresh = JOY_MIN_THRESH * joystate.x_read;
   joystate.x_maxthresh = JOY_MAX_THRESH * joystate.x_read;
   joystate.y_minthresh = JOY_MIN_THRESH * joystate.y_read;
   joystate.y_maxthresh = JOY_MAX_THRESH * joystate.y_read;

   return 0;
}

void thin_joy_shutdown(void)
{
}

/*
** $Log: tl_joy.c,v $
** Revision 1.3  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
