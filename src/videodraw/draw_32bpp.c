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



void draw32bpp_border(void)
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
   *(mem_ptr+8) = colour;
   *(mem_ptr+9) = colour;
   *(mem_ptr+10) = colour;
   *(mem_ptr+11) = colour;
   *(mem_ptr+12) = colour;
   *(mem_ptr+13) = colour;
   *(mem_ptr+14) = colour;
   *(mem_ptr+15) = colour;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode0(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *mem_ptr = val; // write one pixels
   *(mem_ptr+1) = val;
   *(mem_ptr+2) = val;
   *(mem_ptr+3) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+4) = val;
   *(mem_ptr+5) = val;
   *(mem_ptr+6) = val;
   *(mem_ptr+7) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(mem_ptr+8) = val;
   *(mem_ptr+9) = val;
   *(mem_ptr+10) = val;
   *(mem_ptr+11) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+12) = val;
   *(mem_ptr+13) = val;
   *(mem_ptr+14) = val;
   *(mem_ptr+15) = val;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode1(dword addr)
{
   byte idx;
   register dword *mem_ptr;
   dword val;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *mem_ptr = val; // write one pixels
   *(mem_ptr+1) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+2) = val;
   *(mem_ptr+3) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+4) = val;
   *(mem_ptr+5) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+6) = val;
   *(mem_ptr+7) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(mem_ptr+8) = val;
   *(mem_ptr+9) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+10) = val;
   *(mem_ptr+11) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+12) = val;
   *(mem_ptr+13) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+14) = val;
   *(mem_ptr+15) = val;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode2(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   dword pen_on, pen_off;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *mem_ptr = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+1) = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+2) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+3) = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+4) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+5) = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+6) = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+7) = (pat & 0x01) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(mem_ptr+8) = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+9) = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+10) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+11) = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+12) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+13) = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+14) = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+15) = (pat & 0x01) ? pen_on : pen_off;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_border_double(void)
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
   *(mem_ptr+8) =
   *(mem_ptr+next_line+8) = colour;
   *(mem_ptr+9) =
   *(mem_ptr+next_line+9) = colour;
   *(mem_ptr+10) =
   *(mem_ptr+next_line+10) = colour;
   *(mem_ptr+11) =
   *(mem_ptr+next_line+11) = colour;
   *(mem_ptr+12) =
   *(mem_ptr+next_line+12) = colour;
   *(mem_ptr+13) =
   *(mem_ptr+next_line+13) = colour;
   *(mem_ptr+14) =
   *(mem_ptr+next_line+14) = colour;
   *(mem_ptr+15) =
   *(mem_ptr+next_line+15) = colour;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode0_double(dword addr)
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
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = val;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = val;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = val;
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = val;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(mem_ptr+8) =
   *(mem_ptr+next_line+8) = val;
   *(mem_ptr+9) =
   *(mem_ptr+next_line+9) = val;
   *(mem_ptr+10) =
   *(mem_ptr+next_line+10) = val;
   *(mem_ptr+11) =
   *(mem_ptr+next_line+11) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(mem_ptr+12) =
   *(mem_ptr+next_line+12) = val;
   *(mem_ptr+13) =
   *(mem_ptr+next_line+13) = val;
   *(mem_ptr+14) =
   *(mem_ptr+next_line+14) = val;
   *(mem_ptr+15) =
   *(mem_ptr+next_line+15) = val;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode1_double(dword addr)
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
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = val;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = val;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = val;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(mem_ptr+8) =
   *(mem_ptr+next_line+8) = val;
   *(mem_ptr+9) =
   *(mem_ptr+next_line+9) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(mem_ptr+10) =
   *(mem_ptr+next_line+10) = val;
   *(mem_ptr+11) =
   *(mem_ptr+next_line+11) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(mem_ptr+12) =
   *(mem_ptr+next_line+12) = val;
   *(mem_ptr+13) =
   *(mem_ptr+next_line+13) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(mem_ptr+14) =
   *(mem_ptr+next_line+14) = val;
   *(mem_ptr+15) =
   *(mem_ptr+next_line+15) = val;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_mode2_double(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   register dword next_line;
   dword pen_on, pen_off;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   next_line = CPC.scr_bps;
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *mem_ptr =
   *(mem_ptr+next_line) = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+1) =
   *(mem_ptr+next_line+1) = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+2) =
   *(mem_ptr+next_line+2) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+3) =
   *(mem_ptr+next_line+3) = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+4) =
   *(mem_ptr+next_line+4) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+5) =
   *(mem_ptr+next_line+5) = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+6) =
   *(mem_ptr+next_line+6) = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+7) =
   *(mem_ptr+next_line+7) = (pat & 0x01) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(mem_ptr+8) =
   *(mem_ptr+next_line+8) = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+9) =
   *(mem_ptr+next_line+9) = (pat & 0x40) ? pen_on : pen_off;
   *(mem_ptr+10) =
   *(mem_ptr+next_line+10) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+11) =
   *(mem_ptr+next_line+11) = (pat & 0x10) ? pen_on : pen_off;
   *(mem_ptr+12) =
   *(mem_ptr+next_line+12) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+13) =
   *(mem_ptr+next_line+13) = (pat & 0x04) ? pen_on : pen_off;
   *(mem_ptr+14) =
   *(mem_ptr+next_line+14) = (pat & 0x02) ? pen_on : pen_off;
   *(mem_ptr+15) =
   *(mem_ptr+next_line+15) = (pat & 0x01) ? pen_on : pen_off;
   CPC.scr_offs += 16; // update PC screen buffer address
}



void draw32bpp_border_half(void)
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



void draw32bpp_mode0_half(dword addr)
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



void draw32bpp_mode1_half(dword addr)
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



void draw32bpp_mode2_half(dword addr)
{
   byte pat;
   register dword *mem_ptr;
   dword pen_on, pen_off;

   mem_ptr = CPC.scr_base + CPC.scr_offs; // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *mem_ptr = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+1) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+2) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+3) = (pat & 0x02) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(mem_ptr+4) = (pat & 0x80) ? pen_on : pen_off;
   *(mem_ptr+5) = (pat & 0x20) ? pen_on : pen_off;
   *(mem_ptr+6) = (pat & 0x08) ? pen_on : pen_off;
   *(mem_ptr+7) = (pat & 0x02) ? pen_on : pen_off;
   CPC.scr_offs += 8; // update PC screen buffer address
}
