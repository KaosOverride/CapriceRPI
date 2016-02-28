/* Caprice32 - Amstrad CPC Emulator
   (c) Copyright 1997-2004 Ulrich Doewich

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Hitachi HD6845S CRT Controller (CRTC Type 0) emulation
   (c) Copyright 1997-2004 Ulrich Doewich

   Oct 16, 2000 - 23:12 started conversion from assembly to C
   Oct 17, 2000 - 19:25 finished converting main CRTC update loop
   Oct 17, 2000 - 22:04 added framework for mode draw handlers
   Oct 25, 2000 - 22:03 changed all CRTC counters to be only 8 bits wide; fixed address calculation
   Oct 30, 2000 - 19:03 fixed CPC screen address line advancement (test should have been for a _reset_ bit!)
   Mar 20, 2001 - 16:00 added draw_mode2
   Jun 20, 2001 - 23:24 added drawing routines for 32bpp video modes
   Jul 04, 2001 - 22:28 16bpp rendering; updated 8bpp code with VDU visible region limiting
   Sep 26, 2002 - 22:39 moved rendering code to separate files; added line doubling (in software) code
   Oct 07, 2002 - 21:58 removed the CPC.scr_max test in write_video_data; added support for variable line spacing

   May 23, 2004 - 17:38 added some preliminary VDU frame cropping to reduce the amount of data written to the video buffer
   May 24, 2004 - 00:44 moved the write_video_data code into the body of access_video_memory
*/

#include "cap32.h"
#include "crtc.h"
#include "z80.h"

extern t_CPC CPC;
extern t_CRTC CRTC;
extern t_GateArray GateArray;
extern t_VDU VDU;
extern t_z80regs z80;
extern dword colour_table_half[32];

extern byte *pbRAM;
extern byte mode0_table[512], mode1_table[1024];

#ifdef DEBUG_CRTC
extern FILE *pfoDebug;
#endif
extern int CPC_even_frame;


void access_video_memory(int repeat_count)
{
   register byte char_count, line_count, raster_count;

   do {
      char_count = CRTC.char_count;
      line_count = CRTC.line_count;
      raster_count = CRTC.raster_count;
      char_count++; // update character count (cc)
      VDU.char_count++;
      if (CRTC.flags & HT_flag) { // reached horizontal total on last cc?
         CRTC.flags &= ~HT_flag;
         CRTC.hsw_active = CRTC.hsw;
         VDU.hsw_active = VDU.hsw;
         char_count = 0; // reset cc
// next raster ----------------------------------------------------------------
         raster_count += 8; // advance rc by one
         if (CRTC.flags & VS_flag) { // in VSYNC?
            CRTC.vsw_count++; // update width counter
            if (CRTC.vsw_count >= CRTC.vsw) { // reached end of VSYNC?
               CRTC.flags = (CRTC.flags & ~VS_flag) | VSf_flag; // reset VSYNC, set 'just finished'
            }
         }
         if (CRTC.flags & MR_flag) { // reached maximum raster address on last rc?
            CRTC.flags &= ~MR_flag;
            raster_count = 0; // reset rc
            if (!(CRTC.flags & HDT_flag)) { // HDISPTMG still on (i.e. R01 > R00)?
               CRTC.addr += CRTC.last_hdisp * 2; // advance CPC screen address to next line
            }
            line_count++; // update line count
            line_count &= 127; // limit to 7 bits
         }
         if (CRTC.vt_adjust_count) { // vertical total adjust active?
            if (--CRTC.vt_adjust_count == 0) { // done with vta?
               CRTC.flags = (CRTC.flags & ~VSf_flag) | VDT_flag; // enable VDISPTMG
               raster_count = 0; // reset rc
               line_count = 0; // reset lc
               CRTC.addr = CRTC.requested_addr; // update start of CPC screen address
            }
         }
         if (CRTC.flags & VT_flag) { // reached vertical total on last lc?
            CRTC.flags &= ~VT_flag;
            if (CRTC.vt_adjust) { // do a vertical total adjust?
               CRTC.vt_adjust_count = CRTC.vt_adjust; // init vta counter
            } else {
               CRTC.flags = (CRTC.flags & ~VSf_flag) | VDT_flag; // enable VDISPTMG
               raster_count = 0; // reset rc
               line_count = 0; // reset lc
               CRTC.addr = CRTC.requested_addr; // update start of CPC screen address
            }
         }
         if (raster_count == CRTC.max_raster) { // rc = maximum raster address?
            CRTC.flags |= MR_flag; // process max raster address on next rc
            if (!CRTC.vt_adjust_count) { // no vertical total adjust?
               if (line_count == CRTC.registers[4]) { // lc = vertical total?
                  CRTC.flags |= VT_flag; // takes effect on next lc
               }
            }
         }
         if (line_count == CRTC.registers[6]) { // lc = vertical displayed?
            CRTC.flags &= ~VDT_flag; // disable VDISPTMG
         }
         if (line_count == CRTC.registers[7]) { // lc = vertical sync position?
            if (!(CRTC.flags & (VSf_flag | VS_flag))) { // not in VSYNC?
               CRTC.flags |= VS_flag;
               CRTC.vsw_count = 0; // clear vsw counter
               VDU.vdelay = 2; // GA delays vsync by 2 scanlines
               VDU.vsw_count = 4; // GA vsync is always 4 scanlines long
               GateArray.int_delay = 2; // arm GA two scanlines interrupt delay
            }
         }
// ----------------------------------------------------------------------------
         CRTC.flags |= HDT_flag; // enable horizontal display
      }
      if (char_count == CRTC.registers[0]) { // cc = horizontal total?
         CRTC.flags |= HT_flag; // takes effect on next cc
      }
      if (char_count == CRTC.registers[1]) { // cc = horizontal displayed?
         CRTC.flags &= ~HDT_flag; // disable horizontal display
         CRTC.last_hdisp = CRTC.registers[1]; // save width for line advancement
      }
      if (CRTC.flags & HS_flag) { // in horizontal sync?
// check hsw ------------------------------------------------------------------
         if (VDU.hdelay == 2) { // ready to trigger VDU HSYNC?
            if (--VDU.hsw_count == 0) {
               if (CPC.scr_line++ < CPC_scr_height) {
                  if (VDU.vcount) { // in the visible portion of the screen?
                     CPC.scr_base += CPC.scr_line_offs; // advance to next line
                  }
               }
               CPC.scr_offs = 0;
               VDU.char_count = 0;

/*             if (VDU.vcount) { // in the visible portion of the screen?
                  VDU.vcount--;
               } else if (CPC.scr_line == VDU.vtestoffset) {
                  VDU.vcount = VDU.vcountinit;
               }
*/             VDU.hdelay++; // prevent reentry
            }
         } else {
            VDU.hdelay++; // update delay counter
         }
         if (--CRTC.hsw_count == 0) { // end of HSYNC?
// hsw end --------------------------------------------------------------------
            CRTC.flags &= ~HS_flag; // reset HSYNC
            GateArray.scr_mode = GateArray.requested_scr_mode; // execute mode change
            VDU.scanline++;
            if (VDU.vdelay) { // monitor delaying VSYNC?
               VDU.vdelay--;
            }
            if (VDU.vdelay == 0) { // enter monitor VSYNC?
               if (VDU.vsw_count) { // still in monitor VSYNC?
                  if (--VDU.vsw_count == 0) { // done with monitor VSYNC?
                     if (VDU.scanline > VDU.scanline_min) { // above minimum scanline count?
                        CPC.scr_offs = 0;
                        CPC.scr_line = 0;
                        VDU.frame_completed = 1; // force exit of emulation loop
                     }
                  }
               }
            }
// GA interrupt trigger -------------------------------------------------------
            GateArray.sl_count++; // update GA scanline counter
            if (GateArray.int_delay) { // delay on VSYNC?
               if (--GateArray.int_delay == 0) { // delay expired?
                  if (GateArray.sl_count >= 32) { // counter above save margin?
                     z80.int_pending = 1; // queue interrupt
                     GateArray.sl_count = 0; // clear counter
                  } else {
                     GateArray.sl_count = 0; // clear counter
                  }
               }
            }
            if (GateArray.sl_count == 52) { // trigger interrupt?
               z80.int_pending = 1; // queue interrupt
               GateArray.sl_count = 0; // clear counter
            }
         }
      }
// ----------------------------------------------------------------------------
      if (char_count == CRTC.registers[2]) { // cc = horizontal sync position?
         if (CRTC.hsw_active) { // allow HSYNCs?
            CRTC.flags |= HS_flag; // set HSYNC
            CRTC.hsw_count = CRTC.hsw_active; // load hsw counter
            VDU.hdelay = 0; // clear VDU 2 chars HSYNC delay
            VDU.hsw_count = VDU.hsw_active; // load VDU hsw counter
         }
      }
      CRTC.char_count = char_count; // store cc
      CRTC.line_count = line_count; // store lc
      CRTC.raster_count = raster_count; // store rc

      {
         reg_pair addr;
         addr.d = char_count;
         addr.d = (addr.d * 2) + CRTC.addr; // cc x2 + CPC screen memory address
         if (addr.b.h & 0x20) { // 32K screen?
            addr.b.h += 0x40; // advance to next 16K segment
         }
         addr.b.h &= 0xc7; // apply 11000111 mask
         addr.b.h |= (raster_count & 0x38); // insert rc, masked with 00111000

         #ifndef DEBUG_NO_VIDEO
         if (VDU.hcount) {
            if ((CRTC.flags & (HDT_flag | VDT_flag)) == (HDT_flag | VDT_flag)) { // DISPTMG enabled?
               if (CRTC.skew) {
                  CRTC.skew--;
                  GateArray.scr_border();
               } else {
                  GateArray.scr_mode(addr.w.l); // call apropriate mode handler
               }
            } else {
               GateArray.scr_border();
            }
            VDU.hcount--;
         } else {
            if (VDU.char_count == VDU.hstart) {
               if (VDU.vcount) {
                  VDU.hcount = VDU.hwidth;
                  VDU.vcount--;
               } else {
                  if (CPC.scr_line == VDU.vstart) {
                     VDU.vcount = VDU.vheight;
                  }
               }
            }
         }
         #endif

         #ifdef DEBUG_CRTC
         fprintf(pfoDebug, "%04x | ", addr.w.l);
         #endif
      }

      #ifdef DEBUG_CRTC
      {
         int n, f;
         char str[16];

         strcpy(str, "vhddHVMa");
         for (n = 0, f = CRTC.flags; n < 8; n++, f >>= 1) {
            if (!(f & 1)) {
               str[n] = '.';
            }
         }
         fprintf(pfoDebug, "CC:%3d RC:%2d LC:%2d - HSWC:%2d VSWC:%2d - %s | %d\r\n", CRTC.char_count,
            CRTC.raster_count>>3, CRTC.line_count, CRTC.hsw_count, CRTC.vsw_count, str, (int)(CPC.scr_base+CPC.scr_offs));
      }
      #endif

   } while (--repeat_count);
}



#include "videodraw/draw_8bpp.c"

#include "videodraw/draw_16bpp.c"

#include "videodraw/draw_24bpp.c"

#include "videodraw/draw_32bpp.c"
