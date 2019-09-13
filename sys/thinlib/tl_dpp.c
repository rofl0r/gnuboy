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
** tl_dpp.c
**
** DOS DirectPad Pro scanning code, based on code from
** DirectPad Pro (www.ziplabel.com), written by Earle F. Philhower, III
** $Id: tl_dpp.c,v 1.2 2000/11/05 16:32:36 matt Exp $
*/

#include <pc.h>

#include "tl_types.h"
#include "tl_dpp.h"

#define  NES_PWR     (0x80 + 0x40 + 0x20 + 0x10 + 0x08)
#define  NES_CLK     1
#define  NES_LAT     2
#define  NES_IN      ((inportb(port + 1) & nes_din) ^ xor_val)
#define  NES_OUT(v)  outportb(port, (v))

static const uint8 din_table[] = { 0x40, 0x20, 0x10, 0x08, 0x80 };
static const int xor_table[] = { 1, 1, 1, 1, 0 };

void thin_dpp_read(dpp_t *pad)
{
   int nes_din = din_table[pad->pad_num];
   int xor_val = xor_table[pad->pad_num];
   int port = pad->port;

   ASSERT(pad);

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_LAT + NES_CLK);
   NES_OUT(NES_PWR);
   pad->a = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->b = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->select = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->start = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->up = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->down = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->left = NES_IN;

   NES_OUT(NES_PWR);
   NES_OUT(NES_PWR + NES_CLK);
   NES_OUT(NES_PWR);
   pad->right = NES_IN;

   NES_OUT(0); /* power down */
}

int thin_dpp_init(void)
{
   return 0;
}

int thin_dpp_add(dpp_t *pad, uint16 port, int pad_num)
{
   if (NULL == pad)
      return -1;

   memset(pad, 0, sizeof(dpp_t));

   pad->port = port;
   pad->pad_num = pad_num;

   return 0;
}

void thin_dpp_shutdown(void)
{
}


/*
** $Log: tl_dpp.c,v $
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
