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
** tl_djgpp.h
**
** some general djgpp stuff
** $Id: tl_djgpp.h,v 1.3 2001/02/01 06:28:26 matt Exp $
*/

#ifndef _TL_DJGPP_H_
#define _TL_DJGPP_H_

/* handle djgpp's quirkiness */
#include <dpmi.h>

#define  THIN_LOCKED_FUNC(x)        void x##_end(void) { }
#define  THIN_LOCKED_STATIC_FUNC(x) static void x##_end(void) { }
#define  THIN_LOCK_DATA(d, s)       _go32_dpmi_lock_data(d, s)
#define  THIN_LOCK_CODE(c, s)       _go32_dpmi_lock_code(c, s)
#define  THIN_LOCK_VAR(x)           THIN_LOCK_DATA((void *) &x, sizeof(x))
#define  THIN_LOCK_FUNC(x)          THIN_LOCK_CODE((void *) x, (long) x##_end - (long) x)

#define  THIN_DISABLE_INTS()        __asm__ __volatile__ ("cli")
#define  THIN_ENABLE_INTS()         __asm__ __volatile__ ("sti")

#include <sys/nearptr.h>
#define  THIN_PHYS_ADDR(x)          ((x) + __djgpp_conventional_base)
extern int thinlib_nearptr;

#endif /* !_TL_DJGPP_H_ */

/*
** $Log: tl_djgpp.h,v $
** Revision 1.3  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.2  2001/01/15 05:25:52  matt
** i hate near pointers
**
** Revision 1.1  2000/12/16 21:17:42  matt
** initial revision
**
*/
