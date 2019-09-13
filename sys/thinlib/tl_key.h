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
** tl_key.h
**
** DOS keyboard handling defines / protos
** Thanks to Shawn Hargreaves and Allegro for most of this.
** $Id: tl_key.h,v 1.2 2000/12/13 13:58:20 matt Exp $
*/

#ifndef _TL_KEY_H_
#define _TL_KEY_H_

/* Keyboard scancodes */
#define  THIN_KEY_ESC            1
#define  THIN_KEY_1              2
#define  THIN_KEY_2              3
#define  THIN_KEY_3              4
#define  THIN_KEY_4              5
#define  THIN_KEY_5              6
#define  THIN_KEY_6              7
#define  THIN_KEY_7              8
#define  THIN_KEY_8              9
#define  THIN_KEY_9              10
#define  THIN_KEY_0              11
#define  THIN_KEY_MINUS          12
#define  THIN_KEY_EQUALS         13
#define  THIN_KEY_BACKSPACE      14
#define  THIN_KEY_TAB            15
#define  THIN_KEY_Q              16
#define  THIN_KEY_W              17
#define  THIN_KEY_E              18
#define  THIN_KEY_R              19
#define  THIN_KEY_T              20
#define  THIN_KEY_Y              21
#define  THIN_KEY_U              22
#define  THIN_KEY_I              23
#define  THIN_KEY_O              24
#define  THIN_KEY_P              25
#define  THIN_KEY_OPEN_BRACE     26
#define  THIN_KEY_CLOSE_BRACE    27
#define  THIN_KEY_ENTER          28
#define  THIN_KEY_LEFT_CONTROL   29
#define  THIN_KEY_A              30
#define  THIN_KEY_S              31
#define  THIN_KEY_D              32
#define  THIN_KEY_F              33
#define  THIN_KEY_G              34
#define  THIN_KEY_H              35
#define  THIN_KEY_J              36
#define  THIN_KEY_K              37
#define  THIN_KEY_L              38
#define  THIN_KEY_COLON          39
#define  THIN_KEY_QUOTE          40
#define  THIN_KEY_TILDE          41
#define  THIN_KEY_LEFT_SHIFT     42
#define  THIN_KEY_BACKSLASH      43
#define  THIN_KEY_Z              44
#define  THIN_KEY_X              45
#define  THIN_KEY_C              46
#define  THIN_KEY_V              47
#define  THIN_KEY_B              48
#define  THIN_KEY_N              49
#define  THIN_KEY_M              50
#define  THIN_KEY_COMMA          51
#define  THIN_KEY_PERIOD         52
#define  THIN_KEY_SLASH          53
#define  THIN_KEY_RIGHT_SHIFT    54
#define  THIN_KEY_ASTERISK       55
#define  THIN_KEY_ALT            56
#define  THIN_KEY_SPACE          57
#define  THIN_KEY_CAPS_LOCK      58
#define  THIN_KEY_F1             59
#define  THIN_KEY_F2             60
#define  THIN_KEY_F3             61
#define  THIN_KEY_F4             62
#define  THIN_KEY_F5             63
#define  THIN_KEY_F6             64
#define  THIN_KEY_F7             65
#define  THIN_KEY_F8             66
#define  THIN_KEY_F9             67
#define  THIN_KEY_F10            68
#define  THIN_KEY_NUM_LOCK       69
#define  THIN_KEY_SCROLL_LOCK    70
#define  THIN_KEY_HOME           71
#define  THIN_KEY_UP             72
#define  THIN_KEY_PAGE_UP        73
#define  THIN_KEY_KEYPAD_MINUS   74
#define  THIN_KEY_LEFT           75
#define  THIN_KEY_CENTER         76
#define  THIN_KEY_RIGHT          77
#define  THIN_KEY_KEYPAD_PLUS    78
#define  THIN_KEY_END            79
#define  THIN_KEY_DOWN           80
#define  THIN_KEY_PAGE_DOWN      81
#define  THIN_KEY_INSERT         82
#define  THIN_KEY_DELETE         83
#define  THIN_KEY_PRINT_SCREEN   84
#define  THIN_KEY_F11            87
#define  THIN_KEY_F12            88
#define  THIN_KEY_LEFT_WINDOWS   91
#define  THIN_KEY_RIGHT_WINDOWS  92
#define  THIN_KEY_MENU           93
#define  THIN_KEY_PAD            100
#define  THIN_KEY_RIGHT_CONTROL  120
#define  THIN_KEY_ALTGR          121

#define  THIN_NUM_KEYS           128

enum
{
   THIN_KEY_BREAK = 0,
   THIN_KEY_MAKE
};

typedef struct keydata_s
{
   int key, signal;
} keydata_t;

extern int thin_key_init(void);
extern void thin_key_shutdown(void);
extern keydata_t *thin_key_dequeue(void);

#endif /* !_TL_KEY_H_ */

/*
** $Log: tl_key.h,v $
** Revision 1.2  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
