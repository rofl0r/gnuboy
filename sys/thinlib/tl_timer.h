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
** tl_timer.h
**
** DOS timer routine defines / prototypes
** $Id: tl_timer.h,v 1.1 2000/11/05 06:29:03 matt Exp $
*/

#ifndef _TL_TIMER_H_
#define _TL_TIMER_H_

typedef void (*timerhandler_t)(void);

extern int thin_timer_init(int hertz, timerhandler_t func_ptr);
extern void thin_timer_shutdown(void);
extern void thin_timer_setrate(int hertz);

#endif /* !_TL_TIMER_H_ */

/*
** $Log: tl_timer.h,v $
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
