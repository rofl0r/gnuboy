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
** tl_log.h
**
** Error logging header file
** $Id: tl_log.h,v 1.2 2000/11/06 02:21:45 matt Exp $
*/

#ifndef _TL_LOG_H_
#define _TL_LOG_H_

#include <stdio.h>

extern void thin_printf(const char *format, ... );
extern void thin_setlogfunc(int (*logfunc)(const char *string, ... ));
extern void thin_assert(int expr, int line, const char *file, char *msg);

#endif /* !_TL_LOG_H_ */

/*
** $Log: tl_log.h,v $
** Revision 1.2  2000/11/06 02:21:45  matt
** logging functions now work
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
