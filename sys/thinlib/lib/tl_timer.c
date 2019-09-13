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
** tl_timer.c
**
** DOS timer routines
** $Id: tl_timer.c,v 1.4 2001/03/12 06:06:56 matt Exp $
*/

#include <go32.h>
#include <pc.h>
#include <dpmi.h>

#include "tl_types.h"
#include "tl_djgpp.h"
#include "tl_timer.h"

#define  TIMER_INT         0x08
#define  TIMER_TICKS       1193182L

static _go32_dpmi_seginfo old_handler, new_handler;

static struct
{
   timerhandler_t handler;
   int interval, frac, current;
   bool initialized;
} timer;

/* Reprogram the PIT timer to fire at a specified value */
void thin_timer_setrate(int hertz)
{
   int time;

   if (0 == hertz)
      time = 0;
   else
      time = TIMER_TICKS / (long) hertz;

   timer.interval = time;
   timer.frac = time & 0xFFFF;
   timer.current = 0;

   outportb(0x43, 0x34);
   outportb(0x40, timer.frac & 0xFF);
   outportb(0x40, timer.frac >> 8);
}

static void _timer_int_handler(void)
{
   outportb(0x20, 0x20);
   
   timer.current += timer.frac;
   if (timer.current >= timer.interval)
   {
      timer.current -= timer.interval;
      timer.handler();
   }
}
THIN_LOCKED_STATIC_FUNC(_timer_int_handler);

/* Lock code, data, and chain an interrupt handler */
int thin_timer_init(int hertz, timerhandler_t func_ptr)
{
   THIN_LOCK_FUNC(_timer_int_handler);
   THIN_LOCK_VAR(timer);

   timer.handler = func_ptr;

   /* Save the old vector, stuff the new one in there */
   _go32_dpmi_get_protected_mode_interrupt_vector(TIMER_INT, &old_handler);
   new_handler.pm_offset = (int) _timer_int_handler;
   new_handler.pm_selector = _go32_my_cs();
   _go32_dpmi_chain_protected_mode_interrupt_vector(TIMER_INT, &new_handler);

   /* Set PIC to fire at desired refresh rate */
   thin_timer_setrate(hertz);

   timer.initialized = true;

   return 0;
}

/* Remove the timer handler */
void thin_timer_shutdown(void)
{
   if (false == timer.initialized)
      return;

   /* Restore previous timer setting */
   thin_timer_setrate(0);

   /* Remove the interrupt handler */
   _go32_dpmi_set_protected_mode_interrupt_vector(TIMER_INT, &old_handler);

   timer.initialized = false;
}

/*
** $Log: tl_timer.c,v $
** Revision 1.4  2001/03/12 06:06:56  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.3  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.2  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
