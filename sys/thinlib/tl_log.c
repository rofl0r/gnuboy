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
** tl_log.c
**
** Error logging functions
** $Id: tl_log.c,v 1.2 2000/11/06 02:21:45 matt Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "tl_types.h"
#include "tl_log.h"

static int (*log_func)(const char *format, ... ) = printf;

void thin_printf(const char *format, ... )
{
   /* don't allocate on stack every call */
   static char buffer[1024 + 1];
   va_list arg;

   va_start(arg, format);

   if (NULL != log_func)
   {
      vsprintf(buffer, format, arg);
      log_func(buffer);
   }

   va_end(arg);
}

void thin_setlogfunc(int (*func)(const char *format, ... ))
{
   log_func = func;
}

void thin_assert(int expr, int line, const char *file, char *msg)
{
   if (expr)
      return;

   if (NULL != msg)
      thin_printf("ASSERT: line %d of %s, %s\n", line, file, msg);
   else
      thin_printf("ASSERT: line %d of %s\n", line, file);

   exit(-1);
}

/*
** $Log: tl_log.c,v $
** Revision 1.2  2000/11/06 02:21:45  matt
** logging functions now work
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
