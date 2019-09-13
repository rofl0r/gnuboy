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
** tl_vga.c
**
** VGA-related thinlib functions
** $Id: tl_vga.c,v 1.8 2001/02/01 06:28:26 matt Exp $
*/

#include <stdio.h>
#include <string.h>
#include <dpmi.h>
#include <dos.h>
#include "tl_types.h"
#include "tl_bmp.h"
#include "tl_vga.h"
#include "tl_djgpp.h"

#define  DEFAULT_OVERSCAN        0

#define  MODE_TEXT               0x03
#define  MODE_13H                0x13

/* VGA card register addresses */
#define  VGA_ATTR                0x3C0 /* Attribute reg */
#define  VGA_MISC                0x3C2 /* Misc. output register */
#define  VGA_SEQ_ADDR            0x3C4 /* Base port of sequencer */
#define  VGA_SEQ_DATA            0x3C5 /* Data port of sequencer */
#define  VGA_CRTC_ADDR           0x3D4 /* Base port of CRT controller */
#define  VGA_CRTC_DATA           0x3D5 /* Data port of CRT controller */
#define  VGA_STATUS              0x3DA /* Input status #1 register */

#define  VGA_PAL_READ            0x3C7 /* Palette read address */
#define  VGA_PAL_WRITE           0x3C8 /* Palette write address */
#define  VGA_PAL_DATA            0x3C9 /* Palette data register */

/* generic VGA CRTC register indexes */
#define  HZ_DISPLAY_TOTAL        0x00
#define  HZ_DISPLAY_END          0x01
#define  CRTC_OVERFLOW           0x07
#define  VT_DISPLAY_END          0x12
#define  MEM_OFFSET              0x13

/* indices into our register array */
#define  CLOCK_INDEX             0
#define  H_TOTAL_INDEX           1
#define  H_DISPLAY_INDEX         2
#define  H_BLANKING_START_INDEX  3
#define  H_BLANKING_END_INDEX    4
#define  H_RETRACE_START_INDEX   5
#define  H_RETRACE_END_INDEX     6
#define  V_TOTAL_INDEX           7
#define  OVERFLOW_INDEX          8
#define  MAXIMUM_SCANLINE_INDEX  10
#define  V_RETRACE_START_INDEX   11
#define  V_RETRACE_END_INDEX     12
#define  V_END_INDEX             13
#define  MEM_OFFSET_INDEX        14
#define  UNDERLINE_LOC_INDEX     15
#define  V_BLANKING_START_INDEX  16
#define  V_BLANKING_END_INDEX    17
#define  MODE_CONTROL_INDEX      18
#define  MEMORY_MODE_INDEX       20


typedef struct vgareg_s
{
   int port;
   int index;
   uint8 value;
} vgareg_t;

typedef struct vgamode_s
{
   int width;
   int height;
   char *name;
   vgareg_t *regs;
} vgamode_t;

/* 60 Hz */
static vgareg_t mode_256x224[] =
{
   { 0x3C2, 0x00, 0xE3 }, { 0x3D4, 0x00, 0x5F }, { 0x3D4, 0x01, 0x3F },
   { 0x3D4, 0x02, 0x40 }, { 0x3D4, 0x03, 0x82 }, { 0x3D4, 0x04, 0x4A },
   { 0x3D4, 0x05, 0x9A }, { 0x3D4, 0x06, 0x0B }, { 0x3D4, 0x07, 0xB2 },
   { 0x3D4, 0x08, 0x00 }, { 0x3D4, 0x09, 0x61 }, { 0x3d4, 0x10, 0x00 },
   { 0x3D4, 0x11, 0xAC }, { 0x3D4, 0x12, 0xBF }, { 0x3D4, 0x13, 0x20 },
   { 0x3D4, 0x14, 0x40 }, { 0x3D4, 0x15, 0x01 }, { 0x3D4, 0x16, 0x0A },
   { 0x3D4, 0x17, 0xA3 }, { 0x3C4, 0x01, 0x01 }, { 0x3C4, 0x04, 0x0E },
   { 0, 0, 0 }
};

static vgareg_t mode_256x240[] =
{
   { 0x3c2, 0x00, 0xe3},{ 0x3d4, 0x00, 0x55},{ 0x3d4, 0x01, 0x3f},
   { 0x3d4, 0x02, 0x80},{ 0x3d4, 0x03, 0x90},{ 0x3d4, 0x04, 0x49},
   { 0x3d4, 0x05, 0x80},{ 0x3D4, 0x06, 0x43},{ 0x3d4, 0x07, 0xb2},
   { 0x3d4, 0x08, 0x00},{ 0x3D4, 0x09, 0x61},{ 0x3d4, 0x10, 0x04},
   { 0x3d4, 0x11, 0xac},{ 0x3D4, 0x12, 0xdf},{ 0x3d4, 0x13, 0x20},
   { 0x3d4, 0x14, 0x40},{ 0x3d4, 0x15, 0x07},{ 0x3D4, 0x16, 0x11},
   { 0x3d4, 0x17, 0xa3},{ 0x3c4, 0x01, 0x01},{ 0x3c4, 0x04, 0x0e},
   { 0, 0, 0 }
};

static vgareg_t mode_256x256[] =
{
   { 0x3C2, 0x00, 0xE3 }, { 0x3D4, 0x00, 0x5F }, { 0x3D4, 0x01, 0x3F },
   { 0x3D4, 0x02, 0x40 }, { 0x3D4, 0x03, 0x82 }, { 0x3D4, 0x04, 0x4A },
   { 0x3D4, 0x05, 0x9A }, { 0x3D4, 0x06, 0x23 }, { 0x3D4, 0x07, 0xB2 },
   { 0x3D4, 0x08, 0x00 }, { 0x3D4, 0x09, 0x61 }, { 0x3D4, 0x10, 0x0A },
   { 0x3D4, 0x11, 0xAC }, { 0x3D4, 0x12, 0xFF }, { 0x3D4, 0x13, 0x20 },
   { 0x3D4, 0x14, 0x40 }, { 0x3D4, 0x15, 0x07 }, { 0x3D4, 0x16, 0x1A },
   { 0x3D4, 0x17, 0xA3 }, { 0x3C4, 0x01, 0x01 }, { 0x3C4, 0x04, 0x0E },
   { 0, 0, 0 }
};

/* 60 Hz */
static vgareg_t mode_256x256wide[] =
{
   { 0x3C2, 0x00, 0xE3 }, { 0x3D4, 0x00, 0x52 }, { 0x3D4, 0x01, 0x3F },
   { 0x3D4, 0x02, 0x80 }, { 0x3D4, 0x03, 0x90 }, { 0x3D4, 0x04, 0x49 },
   { 0x3D4, 0x05, 0x80 }, { 0x3D4, 0x06, 0x55 }, { 0x3D4, 0x07, 0xB2 },
   { 0x3D4, 0x08, 0x00 }, { 0x3D4, 0x09, 0x61 }, { 0x3D4, 0x10, 0x20 },
   { 0x3D4, 0x11, 0xAC }, { 0x3D4, 0x12, 0xFF }, { 0x3D4, 0x13, 0x20 },
   { 0x3D4, 0x14, 0x40 }, { 0x3D4, 0x15, 0x07 }, { 0x3D4, 0x16, 0x1A },
   { 0x3D4, 0x17, 0xA3 }, { 0x3C4, 0x01, 0x01 }, { 0x3C4, 0x04, 0x0E },
   { 0, 0, 0 }
};

/* 60 Hz */
static vgareg_t mode_288x224[] =
{
   { 0x3C2, 0x00, 0xE3 }, { 0x3D4, 0x00, 0x5F }, { 0x3D4, 0x01, 0x47 },
   { 0x3D4, 0x02, 0x50 }, { 0x3D4, 0x03, 0x82 }, { 0x3D4, 0x04, 0x50 },
   { 0x3D4, 0x05, 0x80 }, { 0x3D4, 0x06, 0x08 }, { 0x3D4, 0x07, 0x3E },
   { 0x3D4, 0x08, 0x00 }, { 0x3D4, 0x09, 0x41 }, { 0x3D4, 0x10, 0xDA },
   { 0x3D4, 0x11, 0x9C }, { 0x3D4, 0x12, 0xBF }, { 0x3D4, 0x13, 0x24 },
   { 0x3D4, 0x14, 0x40 }, { 0x3D4, 0x15, 0xC7 }, { 0x3D4, 0x16, 0x04 },
   { 0x3D4, 0x17, 0xA3 }, { 0x3C4, 0x01, 0x01 }, { 0x3C4, 0x04, 0x0E },
   { 0, 0, 0 }
};

static vgareg_t mode_320x200[] =
{
   { 0, 0, 0 }
};

static vgamode_t vidmodes[] =
{
   { 288, 224, "288x224", mode_288x224 },
   { 256, 224, "256x224", mode_256x224 },
   { 256, 240, "256x240", mode_256x240 },
   { 256, 256, "256x256 (wide)", mode_256x256wide },
   { 256, 256, "256x256", mode_256x256 },
   { 320, 200, "320x200", mode_320x200 },
   { 0, 0, NULL, 0 }
};


static bitmap_t *screen = NULL;
static bitmap_t *hardware = NULL;

/* current VGA mode */
static vgamode_t *vga_mode = NULL;

/* eek */
static int center_x, center_y;

static vgareg_t *tweak_addscanlines(vgareg_t *inreg, int entries)
{
   static vgareg_t outreg[32];
   int maxscan,maxscanout;
   int overflow,overflowout;
   int ytotalin,ytotalout;
   int ydispin,ydispout;
   int vrsin,vrsout,vreout,vblksout,vblkeout;

   /* first - check's it not already a 'non doubled' line mode */
   maxscan = inreg[MAXIMUM_SCANLINE_INDEX].value;
   if ((maxscan & 1) == 0)
   /* it is, so just return the array as is */
      return inreg;
   /* copy across our standard display array */
   memcpy (&outreg, inreg, entries * sizeof(vgareg_t));
   /* keep hold of the overflow register - as we'll need to refer to it a lot */
   overflow = inreg[OVERFLOW_INDEX].value;
   /* set a large line compare value  - as we won't be doing any split window scrolling etc.*/
   maxscanout = 0x40;
   /* half all the y values */
   /* total */
   ytotalin = inreg[V_TOTAL_INDEX].value;
   ytotalin |= ((overflow & 1)<<0x08) | ((overflow & 0x20)<<0x04);
   ytotalout = ytotalin >> 1;
   /* display enable end */
   ydispin = inreg[13].value | ((overflow & 0x02)<< 0x07) | ((overflow & 0x040) << 0x03);
   ydispin ++;
   ydispout = ydispin >> 1;
   ydispout --;
   overflowout = ((ydispout & 0x100) >> 0x07) | ((ydispout && 0x200) >> 0x03);
   outreg[V_END_INDEX].value = (ydispout & 0xff);

   /* avoid top over scan */
   if ((ytotalin - ydispin) < 40 && !center_y)
   {
      vrsout = ydispout;
      /* give ourselves a scanline cushion */
      ytotalout += 2;
   }
   else
   {
      /* vertical retrace start */
      vrsin = inreg[V_RETRACE_START_INDEX].value | ((overflow & 0x04)<<0x06) | ((overflow & 0x80)<<0x02);
      vrsout = vrsin >> 1;
   }
   /* check it's legal */
   if (vrsout < ydispout)
      vrsout = ydispout;
   /* update our output overflow */
   overflowout |= (((vrsout & 0x100) >> 0x06) | ((vrsout & 0x200) >> 0x02));
   outreg[V_RETRACE_START_INDEX].value = (vrsout & 0xff);
   /* vertical retrace end */
   vreout = vrsout + 2;
   /* make sure the retrace fits into our adjusted display size */
   if (vreout > (ytotalout - 9))
      ytotalout = vreout + 9;
   /* write out the vertical retrace end */
   outreg[V_RETRACE_END_INDEX].value &= ~0x0f;
   outreg[V_RETRACE_END_INDEX].value |= (vreout & 0x0f);
   /* vertical blanking start */
   vblksout = ydispout + 1;
   /* check it's legal */
   if(vblksout > vreout)
      vblksout = vreout;
   /* save the overflow value */
   overflowout |= ((vblksout & 0x100) >> 0x05);
   maxscanout |= ((vblksout & 0x200) >> 0x04);
   /* write the v blank value out */
   outreg[V_BLANKING_START_INDEX].value = (vblksout & 0xff);
   /* vertical blanking end */
   vblkeout = vreout + 1;
   /* make sure the blanking fits into our adjusted display size */
   if (vblkeout > (ytotalout - 9))
      ytotalout = vblkeout + 9;
   /* write out the vertical blanking total */
   outreg[V_BLANKING_END_INDEX].value = (vblkeout & 0xff);
   /* update our output overflow */
   overflowout |= ((ytotalout & 0x100) >> 0x08) | ((ytotalout & 0x200) >> 0x04);
   /* write out the new vertical total */
   outreg[V_TOTAL_INDEX].value = (ytotalout & 0xff);

   /* write out our over flows */
   outreg[OVERFLOW_INDEX].value = overflowout;
   /* finally the max scan line */
   outreg[MAXIMUM_SCANLINE_INDEX].value = maxscanout;
   /* and we're done */
   return outreg;
}

static void tweak_centermode(vgareg_t *pReg)
{
   int center;
   int hrt_start, hrt_end, hrt, hblnk_start, hblnk_end;
   int vrt_start, vrt_end, vert_total, vert_display, vblnk_start, vrt, vblnk_end;

   if (!pReg)
      return;

   /* vertical retrace width */
   vrt = 2;

   /* check the clock speed, to work out the retrace width */
   if (pReg[CLOCK_INDEX].value == 0xe7)
      hrt = 11;
   else
      hrt = 10;

   /* our center x tweak value */
   center = center_x;

   /* check for double width scanline rather than half clock (15.75kHz modes) */
   if( pReg[H_TOTAL_INDEX].value > 0x96)
   {
      center<<=1;
      hrt<<=1;
   }

   /* set the hz retrace */
   hrt_start = pReg[H_RETRACE_START_INDEX].value;
   hrt_start += center;
   /* make sure it's legal */
   if (hrt_start <= pReg[H_DISPLAY_INDEX].value)
      hrt_start = pReg[H_DISPLAY_INDEX].value + 1;
   pReg[H_RETRACE_START_INDEX].value = hrt_start;

   /* set hz retrace end */
   hrt_end = hrt_start + hrt;
   /* make sure it's legal */
   if( hrt_end > pReg[H_TOTAL_INDEX].value)
      hrt_end = pReg[H_TOTAL_INDEX].value;

   /* set the hz blanking */
   hblnk_start = pReg[H_DISPLAY_INDEX].value + 1;
   /* make sure it's legal */
   if (hblnk_start > hrt_start)
      hblnk_start = pReg[H_RETRACE_START_INDEX].value;
   pReg[H_BLANKING_START_INDEX].value = hblnk_start;

   /* the horizontal blanking end */
   hblnk_end = hrt_end + 2;
   /* make sure it's legal */
   if( hblnk_end > pReg[H_TOTAL_INDEX].value)
      hblnk_end = pReg[H_TOTAL_INDEX].value;
   /* write horizontal blanking - include 7th test bit (always 1) */
   pReg[H_BLANKING_END_INDEX].value = (hblnk_end & 0x1f) | 0x80;
   /* include the 5th bit of the horizontal blanking in the horizontal retrace reg. */
   hrt_end = ((hrt_end & 0x1f) | ((hblnk_end & 0x20) << 2));
   pReg[H_RETRACE_END_INDEX].value = hrt_end;

   /* get the vt retrace */
   vrt_start = pReg[V_RETRACE_START_INDEX].value | ((pReg[OVERFLOW_INDEX].value & 0x04) << 6) |
            ((pReg[OVERFLOW_INDEX].value & 0x80) << 2);

   /* set the new retrace start */
   vrt_start += center_y;
   /* check it's legal, get the display line count */
   vert_display = (pReg[V_END_INDEX].value | ((pReg[OVERFLOW_INDEX].value & 0x02) << 7) |
            ((pReg[OVERFLOW_INDEX].value & 0x40) << 3)) + 1;

   if (vrt_start < vert_display)
      vrt_start = vert_display;

   /* and get the vertical line count */
   vert_total = pReg[V_TOTAL_INDEX].value | ((pReg[OVERFLOW_INDEX].value & 0x01) << 8) |
            ((pReg[OVERFLOW_INDEX].value & 0x20) << 4);

   pReg[V_RETRACE_START_INDEX].value = (vrt_start & 0xff);
   pReg[OVERFLOW_INDEX].value &= ~0x84;
   pReg[OVERFLOW_INDEX].value |= ((vrt_start & 0x100) >> 6);
   pReg[OVERFLOW_INDEX].value |= ((vrt_start & 0x200) >> 2);
   vrt_end = vrt_start + vrt;

   if (vrt_end > vert_total)
      vrt_end = vert_total;

   /* write retrace end, include CRT protection and IRQ2 bits */
   pReg[V_RETRACE_END_INDEX].value = (vrt_end  & 0x0f) | 0x80 | 0x20;

   /* get the start of vt blanking */
   vblnk_start = vert_display + 1;
   /* check it's legal */
   if (vblnk_start > vrt_start)
      vblnk_start = vrt_start;
   /* and the end */
   vblnk_end = vrt_end + 2;
   /* check it's legal */
   if (vblnk_end > vert_total)
      vblnk_end = vert_total;

   /* set vblank start */
   pReg[V_BLANKING_START_INDEX].value = (vblnk_start & 0xff);

   /* write out any overflows */
   pReg[OVERFLOW_INDEX].value &= ~0x08;
   pReg[OVERFLOW_INDEX].value |= ((vblnk_start & 0x100) >> 5);
   pReg[MAXIMUM_SCANLINE_INDEX].value &= ~0x20;
   pReg[MAXIMUM_SCANLINE_INDEX].value |= ((vblnk_start &0x200) >> 4);

   /* set the vblank end */
   pReg[V_BLANKING_END_INDEX].value = (vblnk_end & 0xff);
}

/* Set a VGA mode */
static void vga_setvgamode(uint8 mode)
{
   __dpmi_regs r;
   r.x.ax = mode;
   __dpmi_int(0x10, &r);
}

static void vga_set_overscan(int index)
{
   outportb(VGA_ATTR, 0x31);
   outportb(VGA_ATTR, index);
}

static void vga_outregs(vgareg_t *reg)
{
   uint8 crtc_val;

   /* Disable interrupts, wait for vertical retrace */
//   thin_vga_waitvsync();
   THIN_DISABLE_INTS();

   /* Sequencer reset */
   outportb(VGA_SEQ_ADDR, 0x00);
   outportb(VGA_SEQ_DATA, 0x01);
   crtc_val = inportb(VGA_CRTC_DATA) & 0x7F;
   /* Unprotect registers 0-7 */
   outportb(VGA_CRTC_ADDR, 0x11);
   outportb(VGA_CRTC_DATA, crtc_val);

   /* Reset read/write flip-flop */
   inportb(VGA_STATUS);

   /* Do the icky register stuff */
   while (reg->port)
   {
      switch(reg->port)
      {
      case VGA_ATTR:
         /* Reset read/write flip-flop */
         inportb(VGA_STATUS);
         /* Ensure VGA output is enabled - bit 5 */
         outportb(VGA_ATTR, reg->index | 0x20);
         outportb(VGA_ATTR, reg->value);
         break;
      case VGA_MISC:
         /* Write directly to port */
         outportb(reg->port, reg->value);
         break;
      case VGA_SEQ_ADDR:
      case VGA_CRTC_ADDR:
      default:
         /* Index to port, value to port + 1 */
         outportb(reg->port, reg->index);
         outportb(reg->port + 1, reg->value);
         break;
      }

      reg++;
   }

   /* Set overscan color */
   vga_set_overscan(DEFAULT_OVERSCAN);

   /* Clear sequencer reset */
   outportb(VGA_SEQ_ADDR, 0x00);
   outportb(VGA_SEQ_DATA, 0x03);

   THIN_ENABLE_INTS();
}

static int vga_getregsize(vgareg_t *regs)
{
   int reg_size = 0;

   while (regs->port)
   {
      reg_size++;
      regs++;
   }

   return reg_size;
}

void thin_vga_scanlines(bool scanlines_on)
{
   vgareg_t *reg;
   
   if (NULL == vga_mode)
      return;

   /* modify (or not) for scanlines */
   if (true == scanlines_on)
      reg = tweak_addscanlines(vga_mode->regs, vga_getregsize(vga_mode->regs));
   else
      reg = vga_mode->regs;

   /* center the screen */
   if (vga_getregsize(vga_mode->regs) > 1)
      tweak_centermode(reg);

   /* send them to the VGA controller */
   vga_outregs(reg);
}

/* Set up VGA mode 13h, then tweak it appropriately */
int thin_vga_setmode(int width, int height)
{
   vga_mode = vidmodes;

   /* Search for the video mode */
   while ((vga_mode->width != width) || (vga_mode->height != height))
   {
      if (NULL == vga_mode->regs)
      {
         vga_mode = NULL;
         return -1;
      }
      vga_mode++;
   }

   /* Set up our standard mode 13h */
   vga_setvgamode(MODE_13H);

   /* modify the register values, and send 'em out */
   thin_vga_scanlines(false);

   return 0;
}

/* Destroy VGA */
void thin_vga_shutdown(void)
{
   /* set textmode */
   vga_setvgamode(MODE_TEXT);
   thin_bmp_destroy(&screen);

   if (0 == thinlib_nearptr)
      thin_bmp_destroy(&hardware);
}

/* Initialize VGA */
int thin_vga_init(int width, int height)
{
   if (thinlib_nearptr)
   {
      screen = thin_bmp_createhw((uint8 *) THIN_PHYS_ADDR(0xA0000),
                                 width, height, width);
      if (NULL == screen)
         return -1;
   }
   else
   {
      hardware = thin_bmp_createhw((uint8 *) 0xA0000,
                                   width, height, width);
      if (NULL == hardware)
         return -1;

      screen = thin_bmp_create(width, height, 0);
      if (NULL == screen)
         return -1;
   }

   /* Set the initial video mode, no scanlines */
   if (thin_vga_setmode(width, height))
   {
      thin_vga_shutdown();
      return -1;
   }

   return 0;
}

/* cram an 8-bit, 256 entry rgb palette into 6-bit vga */
void thin_vga_setpalette(rgb_t *palette)
{
   int i;
   
   /* we also want to find the closest color index to black,
   ** and set that as our overscan color
   */
   int overscan_index = 0;
   rgb_t overscan = { 255, 255, 255 };

   outportb(VGA_PAL_WRITE, 0);

   for (i = 0; i < 256; i++)
   {
      if (palette[i].r <= overscan.r
          && palette[i].g <= overscan.g
          && palette[i].b <= overscan.b)
      {
         overscan = palette[i];
         overscan_index = i;
      }

      outportb(VGA_PAL_DATA, palette[i].r >> 2);
      outportb(VGA_PAL_DATA, palette[i].g >> 2);
      outportb(VGA_PAL_DATA, palette[i].b >> 2);
   }

   vga_set_overscan(overscan_index);
}

void thin_vga_waitvsync(void)
{
   while (0 == (inportb(VGA_STATUS) & 0x08));
   //while (inportb(VGA_STATUS) & 0x08);
}

bitmap_t *thin_vga_lockwrite(void)
{
   /* always return screen */
   return screen;
}

void thin_vga_freewrite(int num_dirties, rect_t *dirty_rects)
{
   UNUSED(num_dirties);
   UNUSED(dirty_rects);

   if (0 == thinlib_nearptr)
   {
      int line;

      for (line = 0; line < hardware->height; line++)
      {
         dosmemput(screen->line[line], hardware->pitch, 
                   (int) hardware->line[line]);
      }
   }
}

/*
** $Log: tl_vga.c,v $
** Revision 1.8  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.7  2001/01/15 05:25:52  matt
** i hate near pointers
**
** Revision 1.6  2000/12/16 21:18:11  matt
** thinlib cleanups
**
** Revision 1.5  2000/12/16 17:30:15  matt
** handle overscan more elegantly
**
** Revision 1.4  2000/11/25 20:28:34  matt
** moved verboseness into correct places
**
** Revision 1.3  2000/11/06 02:22:33  matt
** generalized video driver (tl_video.c)
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
