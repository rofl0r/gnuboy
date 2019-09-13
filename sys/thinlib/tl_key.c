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
** tl_key.c
**
** DOS keyboard handler
** $Id: tl_key.c,v 1.3 2000/12/16 21:18:11 matt Exp $
*/

#include <stdlib.h>
#include <go32.h>
#include <dpmi.h>
#include <dos.h>

#include "tl_types.h"
#include "tl_djgpp.h"
#include "tl_key.h"

#define  KEYBOARD_INT      0x09

/* maybe make this globally accesible? */
static int key_status[THIN_NUM_KEYS];

static _go32_dpmi_seginfo old_key_handler;
static _go32_dpmi_seginfo new_key_handler;

/*
** Simple queue routines
*/
#define  KEYQUEUE_MAX      256
#define  KEYQUEUE_MASK     (KEYQUEUE_MAX - 1)
#define  KEYQUEUE_EMPTY()  (q_head == q_tail)

static int q_tail = 0, q_head = 0;
static keydata_t key_queue[KEYQUEUE_MAX];

static void key_enqueue(keydata_t *data)
{
   key_queue[q_head] = *data;
   q_head = (q_head + 1) & KEYQUEUE_MASK;
}
THIN_LOCKED_STATIC_FUNC(key_enqueue)

keydata_t *thin_key_dequeue(void)
{
   int loc;

   if (KEYQUEUE_EMPTY())
      return NULL;

   loc = q_tail;
   q_tail = (q_tail + 1) & KEYQUEUE_MASK;

   return &key_queue[loc];
}

/* keyboard ISR */
static void key_handler(void)
{
   /* read the key */
   keydata_t data;
   uint8 raw_code, ack_code;

   THIN_DISABLE_INTS();

   raw_code = inportb(0x60);
   ack_code = inportb(0x61);
   outportb(0x61, ack_code | 0x80);
   outportb(0x61, ack_code);
   outportb(0x20, 0x20);

   THIN_ENABLE_INTS();

   data.key = raw_code & 0x7F;
   data.signal = (raw_code & 0x80) ? THIN_KEY_BREAK : THIN_KEY_MAKE;

   /* mask out repeat keypresses */
   if (data.signal != key_status[data.key])
      key_enqueue(&data);

   key_status[data.key] = data.signal;
}
THIN_LOCKED_STATIC_FUNC(key_handler)

/* set up variables, lock code/data, set the new handler and save old one */
int thin_key_init(void)
{
   THIN_LOCK_FUNC(key_enqueue);
   THIN_LOCK_FUNC(key_handler);
   THIN_LOCK_VAR(key_queue);
   THIN_LOCK_VAR(q_head);
   THIN_LOCK_VAR(key_status);
   
   memset(key_status, THIN_KEY_BREAK, sizeof(key_status));
   _go32_dpmi_get_protected_mode_interrupt_vector(KEYBOARD_INT, &old_key_handler);
   new_key_handler.pm_offset = (uint32) key_handler;
   new_key_handler.pm_selector = _go32_my_cs();
   _go32_dpmi_allocate_iret_wrapper(&new_key_handler);
   _go32_dpmi_set_protected_mode_interrupt_vector(KEYBOARD_INT, &new_key_handler);

   return 0; /* can't fail */
}

/* restore old keyboard handler */
void thin_key_shutdown(void)
{
   _go32_dpmi_set_protected_mode_interrupt_vector(KEYBOARD_INT, &old_key_handler);
   _go32_dpmi_free_iret_wrapper(&new_key_handler);
}

/*
** $Log: tl_key.c,v $
** Revision 1.3  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
