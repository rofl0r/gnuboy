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
** thinlib.c
**
** main library init / shutdown code
** $Id: thinlib.c,v 1.7 2001/02/01 06:28:26 matt Exp $
*/

#include "tl_types.h"
#include "thinlib.h"

#include "tl_key.h"
#include "tl_timer.h"
#include "tl_joy.h"
#include "tl_dpp.h"

#include "tl_sound.h"
#include "tl_video.h"
#include "tl_djgpp.h"

#include <crt0.h>

int _crt0_startup_flags = _CRT0_FLAG_NONMOVE_SBRK;

/* our global "near pointer" flag. */
int thinlib_nearptr = 0;

/* Reduce size of djgpp executable */
char **__crt0_glob_function(char *_argument) 
{ 
   UNUSED(_argument);
   return (char **) 0;
}

void __crt0_load_environment_file(char *_app_name)
{
   UNUSED(_app_name);
}

static int initialized_flags = 0;

int thin_init(int devices)
{
   int success = 0;

   /* Try to enable near pointers through djgpp's default mechanism.
   ** This allows us to manipulate memory-mapped devices (sound cards,
   ** video cards, etc.) as if they were regular memory addresses, at
   ** the cost of disabling memory protection.  Win NT and 2000 strictly
   ** disallow near pointers, so we need to flag this.
   */

   if (0 == __djgpp_nearptr_enable())
      thinlib_nearptr = 0;
   else
      thinlib_nearptr = 1;

   if (devices & THIN_KEY)
   {
      if (thin_key_init())
         return -1;
      success |= THIN_KEY;
   }

   if (devices & THIN_JOY)
   {
      if (0 == thin_joy_init())
         success |= THIN_JOY;
   }

   if (devices & THIN_DPP)
   {
      if (0 == thin_dpp_init())
         success |= THIN_DPP;
   }

   /* THIN_SOUND, THIN_VIDEO, THIN_TIMER implicitly successful.. */

   initialized_flags = success;

   return success;
}

void thin_shutdown(void)
{
   /* not started from thin_init */
   thin_sound_shutdown();
   thin_timer_shutdown();
   thin_vid_shutdown();

   /* started from thin_init... */
   if (initialized_flags & THIN_KEY)
      thin_key_shutdown();

   if (initialized_flags & THIN_JOY)
      thin_joy_shutdown();

   if (initialized_flags & THIN_DPP)
      thin_dpp_shutdown();

   /* back to memory protection, if need be */
   if (thinlib_nearptr)
      __djgpp_nearptr_disable();
}

/*
** $Log: thinlib.c,v $
** Revision 1.7  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.6  2001/01/15 05:25:52  matt
** i hate near pointers
**
** Revision 1.5  2000/12/16 17:39:57  matt
** tl_sound
**
** Revision 1.4  2000/12/13 14:14:27  matt
** DJGPP_USE_NEARPTR -> THINLIB_NEARPTR
**
** Revision 1.3  2000/11/06 02:22:33  matt
** generalized video driver (tl_video.c)
**
** Revision 1.2  2000/11/05 22:22:00  matt
** djgpp specifics rolled into thinlib.c from osd.c
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
