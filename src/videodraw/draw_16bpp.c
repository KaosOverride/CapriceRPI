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



void draw16bpp_border(void)
{
   int colour;
   register dword *mem_ptr;

   colour = GateArray.palette[16];
   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   *mem_ptr = colour; // write one pixel of border colour
   *(mem_ptr+1) = colour;
   *(mem_ptr+2) = colour;
   *(mem_ptr+3) = colour;
   *(mem_ptr+4) = colour;
   *(mem_ptr+5) = colour;
   *(mem_ptr+6) = colour;
   *(mem_ptr+7) = colour;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode0(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *mem_ptr = val; // write one pixels
   *(mem_ptr+1) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+2) = val;
   *(mem_ptr+3) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(mem_ptr+4) = val;
   *(mem_ptr+5) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+6) = val;
   *(mem_ptr+7) = val;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode1(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *mem_ptr = val; // write one pixels
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+1) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+2) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+3) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(mem_ptr+4) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+5) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+6) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+7) = val;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode2(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   dword pen_on, pen_off;
   reg_pair val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x40) ? pen_on : pen_off;
   *mem_ptr = val.d; // write four pixels
   val.w.l = (pat & 0x20) ? pen_on : pen_off;
   val.w.h = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+1) = val.d;
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+2) = val.d;
   val.w.l = (pat & 0x02) ? pen_on : pen_off;
   val.w.h = (pat & 0x01) ? pen_on : pen_off;
   *(mem_ptr+3) = val.d;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+4) = val.d;
   val.w.l = (pat & 0x20) ? pen_on : pen_off;
   val.w.h = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+5) = val.d;
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+6) = val.d;
   val.w.l = (pat & 0x02) ? pen_on : pen_off;
   val.w.h = (pat & 0x01) ? pen_on : pen_off;
   *(mem_ptr+7) = val.d;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_border_double(void)
{
   int colour;
   register dword *mem_ptr;
   register dword next_line;

   colour = GateArray.palette[16];
   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   next_line = CPC.scr_bps;
   *mem_ptr =
   *(mem_ptr+next_line) = colour; // write one pixel of border colour
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = colour;
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = colour;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = colour;
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = colour;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = colour;
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = colour;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = colour;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode0_double(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   register dword next_line;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   next_line = CPC.scr_bps;
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *mem_ptr =
   *(mem_ptr+next_line) = val; // write one pixels
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = val;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = val;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = val;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = val;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode1_double(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   register dword next_line;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   next_line = CPC.scr_bps;
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *mem_ptr =
   *(mem_ptr+next_line) = val; // write one pixels
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = val;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_mode2_double(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   register dword next_line;
   dword pen_on, pen_off;
   reg_pair val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   next_line = CPC.scr_bps;
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x40) ? pen_on : pen_off;
   *mem_ptr =
   *(mem_ptr+next_line) = val.d; // write four pixels
   val.w.l = (pat & 0x20) ? pen_on : pen_off;
   val.w.h = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = val.d;
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = val.d;
   val.w.l = (pat & 0x02) ? pen_on : pen_off;
   val.w.h = (pat & 0x01) ? pen_on : pen_off;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = val.d;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = val.d;
   val.w.l = (pat & 0x20) ? pen_on : pen_off;
   val.w.h = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = val.d;
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = val.d;
   val.w.l = (pat & 0x02) ? pen_on : pen_off;
   val.w.h = (pat & 0x01) ? pen_on : pen_off;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = val.d;
   CPC.scr_offs += 8; // update PC screen buffer address
}



void draw16bpp_border_half(void)
{
   int colour;
   register dword *mem_ptr;

   colour = GateArray.palette[16];
   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   *mem_ptr = colour; // write one pixel of border colour
   *(mem_ptr+1) = colour;
   *(mem_ptr+2) = colour;
   *(mem_ptr+3) = colour;
   CPC.scr_offs += 4; // update PC screen buffer address
}



void draw16bpp_mode0_half(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *mem_ptr = val; // write one pixels
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+1) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(mem_ptr+2) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+3) = val;
   CPC.scr_offs += 4; // update PC screen buffer address
}



void draw16bpp_mode1_half(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   reg_pair val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val.w.l = GateArray.palette[mode1_table[(idx*4)]];
   val.w.h = GateArray.palette[mode1_table[(idx*4)+1]];
   *mem_ptr = val.d; // write one pixels
   val.w.l = GateArray.palette[mode1_table[(idx*4)+2]];
   val.w.h = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+1) = val.d;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val.w.l = GateArray.palette[mode1_table[(idx*4)]];
   val.w.h = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+2) = val.d;
   val.w.l = GateArray.palette[mode1_table[(idx*4)+2]];
   val.w.h = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+3) = val.d;
   CPC.scr_offs += 4; // update PC screen buffer address
}



void draw16bpp_mode2_half(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   dword pen_on, pen_off;
   reg_pair val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x20) ? pen_on : pen_off;
   *mem_ptr = val.d; // write four pixels
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+1) = val.d;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val.w.l = (pat & 0x80) ? pen_on : pen_off;
   val.w.h = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+2) = val.d;
   val.w.l = (pat & 0x08) ? pen_on : pen_off;
   val.w.h = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+3) = val.d;
   CPC.scr_offs += 4; // update PC screen buffer address
}
