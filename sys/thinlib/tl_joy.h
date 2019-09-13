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
** tl_joy.h
**
** DOS joystick reading defines / protos
** $Id: tl_joy.h,v 1.2 2000/11/05 16:32:36 matt Exp $
*/

#ifndef _TL_JOY_H_
#define _TL_JOY_H_

#define  JOY_MAX_BUTTONS   4

typedef struct joy_s
{
   int left, right, up, down;
   int button[JOY_MAX_BUTTONS];
} joy_t;

extern void thin_joy_shutdown(void);
extern int thin_joy_init(void);
extern int thin_joy_read(joy_t *joy);

#endif /* !_TL_JOY_H_ */

/*
** $Log: tl_joy.h,v $
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
