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
** thinlib.h
**
** main library header
** $Id: thinlib.h,v 1.6 2000/12/16 21:18:11 matt Exp $
*/

#ifndef _THINLIB_H_
#define _THINLIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* types */
#include "tl_types.h"
#include "tl_log.h"

/* system */
#include "tl_djgpp.h"
#include "tl_timer.h"

/* input */
#include "tl_key.h"
#include "tl_mouse.h"
#include "tl_joy.h"
#include "tl_dpp.h"

/* video */
#include "tl_bmp.h"
#include "tl_video.h"

/* audio */
#include "tl_sound.h"

#define  THIN_KEY       0x0001
#define  THIN_MOUSE     0x0002
#define  THIN_JOY       0x0004
#define  THIN_DPP       0x0008
#define  THIN_TIMER     0x0010
#define  THIN_VIDEO     0x0020
#define  THIN_SOUND     0x0040

extern int thin_init(int devices);
extern void thin_shutdown(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_THINLIB_H */

/*
** $Log: thinlib.h,v $
** Revision 1.6  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.5  2000/12/16 17:39:57  matt
** tl_sound
**
** Revision 1.4  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.3  2000/11/06 02:22:33  matt
** generalized video driver (tl_video.c)
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
*/
