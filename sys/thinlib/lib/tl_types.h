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
** tl_types.h
**
** type definitions for thinlib
** $Id: tl_types.h,v 1.2 2001/03/12 06:06:56 matt Exp $
*/

#ifndef _TL_TYPES_
#define _TL_TYPES_

#ifdef THINLIB_DEBUG

#include "tl_log.h"

#define  THIN_ASSERT(expr)    thin_assert((int) (expr), __LINE__, __FILE__, NULL)
#define  THIN_ASSERT_MSG(msg) thin_assert(false, __LINE__, __FILE__, (msg))

#else /* !THINLIB_DEBUG */

#define  THIN_ASSERT(expr)
#define  THIN_ASSERT_MSG(msg)

#endif /* !THINLIB_DEBUG */

/* quell stupid compiler warnings */
#define  UNUSED(x)   ((x) = (x))

typedef  signed char    int8;
typedef  signed short   int16;
typedef  signed int     int32;
typedef  unsigned char  uint8;
typedef  unsigned short uint16;
typedef  unsigned int   uint32;

#ifndef __cplusplus
#undef   false
#undef   true
#undef   NULL

typedef enum
{
   false = 0,
   true = 1
} bool;

#ifndef  NULL
#define  NULL     ((void *) 0)
#endif

#endif /* !__cplusplus */

#endif /* !_TL_TYPES_ */

/*
** $Log: tl_types.h,v $
** Revision 1.2  2001/03/12 06:06:56  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
