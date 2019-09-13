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
** tl_mouse.h
**
** DOS mouse handling defines / prototypes
** $Id: tl_mouse.h,v 1.3 2000/12/13 13:58:20 matt Exp $
*/

#ifndef _TL_MOUSE_H_
#define _TL_MOUSE_H_

#include "tl_types.h"

/* mouse buttons */
#define  THIN_MOUSE_LEFT      0
#define  THIN_MOUSE_MIDDLE    1
#define  THIN_MOUSE_RIGHT     2

#define  THIN_MOUSE_BUTTON_MASK(x)  (1 << (x))

extern int thin_mouse_init(int width, int height, int delta_shift);
extern void thin_mouse_setrange(int width, int height);
extern uint8 thin_mouse_getmotion(int *dx, int *dy);
extern uint8 thin_mouse_getpos(int *x, int *y);

#endif /* !_TL_MOUSE_H_ */

/*
** $Log: tl_mouse.h,v $
** Revision 1.3  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
