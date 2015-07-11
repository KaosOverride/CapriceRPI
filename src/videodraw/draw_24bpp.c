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



void draw24bpp_border(void)
{
   int colour;
   register byte *mem_ptr;

   colour = GateArray.palette[16];
   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   *(dword *)mem_ptr = colour; // write one pixel of border colour
   *(dword *)(mem_ptr+3) = colour;
   *(dword *)(mem_ptr+6) = colour;
   *(dword *)(mem_ptr+9) = colour;
   *(dword *)(mem_ptr+12) = colour;
   *(dword *)(mem_ptr+15) = colour;
   *(dword *)(mem_ptr+18) = colour;
   *(dword *)(mem_ptr+21) = colour;
   *(dword *)(mem_ptr+24) = colour;
   *(dword *)(mem_ptr+27) = colour;
   *(dword *)(mem_ptr+30) = colour;
   *(dword *)(mem_ptr+33) = colour;
   *(dword *)(mem_ptr+36) = colour;
   *(dword *)(mem_ptr+39) = colour;
   *(dword *)(mem_ptr+42) = colour;
   *(dword *)(mem_ptr+45) = colour;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode0(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)mem_ptr = val; // write one pixels
   *(dword *)(mem_ptr+3) = val;
   *(dword *)(mem_ptr+6) = val;
   *(dword *)(mem_ptr+9) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+12) = val;
   *(dword *)(mem_ptr+15) = val;
   *(dword *)(mem_ptr+18) = val;
   *(dword *)(mem_ptr+21) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)(mem_ptr+24) = val;
   *(dword *)(mem_ptr+27) = val;
   *(dword *)(mem_ptr+30) = val;
   *(dword *)(mem_ptr+33) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+36) = val;
   *(dword *)(mem_ptr+39) = val;
   *(dword *)(mem_ptr+42) = val;
   *(dword *)(mem_ptr+45) = val;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode1(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)mem_ptr = val; // write one pixels
   *(dword *)(mem_ptr+3) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+6) = val;
   *(dword *)(mem_ptr+9) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+12) = val;
   *(dword *)(mem_ptr+15) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+18) = val;
   *(dword *)(mem_ptr+21) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)(mem_ptr+24) = val;
   *(dword *)(mem_ptr+27) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+30) = val;
   *(dword *)(mem_ptr+33) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+36) = val;
   *(dword *)(mem_ptr+39) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+42) = val;
   *(dword *)(mem_ptr+45) = val;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode2(dword addr)
{
   byte pat;
   register byte *mem_ptr;
   dword pen_on, pen_off;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *(dword *)mem_ptr = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+3) = (pat & 0x40) ? pen_on : pen_off;
   *(dword *)(mem_ptr+6) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+9) = (pat & 0x10) ? pen_on : pen_off;
   *(dword *)(mem_ptr+12) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+15) = (pat & 0x04) ? pen_on : pen_off;
   *(dword *)(mem_ptr+18) = (pat & 0x02) ? pen_on : pen_off;
   *(dword *)(mem_ptr+21) = (pat & 0x01) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(dword *)(mem_ptr+24) = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+27) = (pat & 0x40) ? pen_on : pen_off;
   *(dword *)(mem_ptr+30) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+33) = (pat & 0x10) ? pen_on : pen_off;
   *(dword *)(mem_ptr+36) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+39) = (pat & 0x04) ? pen_on : pen_off;
   *(dword *)(mem_ptr+42) = (pat & 0x02) ? pen_on : pen_off;
   *(dword *)(mem_ptr+45) = (pat & 0x01) ? pen_on : pen_off;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_border_double(void)
{
   int colour;
   register byte *mem_ptr;
   register dword next_line;

   colour = GateArray.palette[16];
   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   next_line = CPC.scr_bps << 2;
   *(dword *)mem_ptr =
   *(dword *)(mem_ptr+next_line) = colour; // write one pixel of border colour
   *(dword *)(mem_ptr+3) =
   *(dword *)(mem_ptr+next_line+3) = colour;
   *(dword *)(mem_ptr+6) =
   *(dword *)(mem_ptr+next_line+6) = colour;
   *(dword *)(mem_ptr+9) =
   *(dword *)(mem_ptr+next_line+9) = colour;
   *(dword *)(mem_ptr+12) =
   *(dword *)(mem_ptr+next_line+12) = colour;
   *(dword *)(mem_ptr+15) =
   *(dword *)(mem_ptr+next_line+15) = colour;
   *(dword *)(mem_ptr+18) =
   *(dword *)(mem_ptr+next_line+18) = colour;
   *(dword *)(mem_ptr+21) =
   *(dword *)(mem_ptr+next_line+21) = colour;
   *(dword *)(mem_ptr+24) =
   *(dword *)(mem_ptr+next_line+24) = colour;
   *(dword *)(mem_ptr+27) =
   *(dword *)(mem_ptr+next_line+27) = colour;
   *(dword *)(mem_ptr+30) =
   *(dword *)(mem_ptr+next_line+30) = colour;
   *(dword *)(mem_ptr+33) =
   *(dword *)(mem_ptr+next_line+33) = colour;
   *(dword *)(mem_ptr+36) =
   *(dword *)(mem_ptr+next_line+36) = colour;
   *(dword *)(mem_ptr+39) =
   *(dword *)(mem_ptr+next_line+39) = colour;
   *(dword *)(mem_ptr+42) =
   *(dword *)(mem_ptr+next_line+42) = colour;
   *(dword *)(mem_ptr+45) =
   *(dword *)(mem_ptr+next_line+45) = colour;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode0_double(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   register dword next_line;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   next_line = CPC.scr_bps << 2;
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)mem_ptr =
   *(dword *)(mem_ptr+next_line) = val; // write one pixels
   *(dword *)(mem_ptr+3) =
   *(dword *)(mem_ptr+next_line+3) = val;
   *(dword *)(mem_ptr+6) =
   *(dword *)(mem_ptr+next_line+6) = val;
   *(dword *)(mem_ptr+9) =
   *(dword *)(mem_ptr+next_line+9) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+12) =
   *(dword *)(mem_ptr+next_line+12) = val;
   *(dword *)(mem_ptr+15) =
   *(dword *)(mem_ptr+next_line+15) = val;
   *(dword *)(mem_ptr+18) =
   *(dword *)(mem_ptr+next_line+18) = val;
   *(dword *)(mem_ptr+21) =
   *(dword *)(mem_ptr+next_line+21) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)(mem_ptr+24) =
   *(dword *)(mem_ptr+next_line+24) = val;
   *(dword *)(mem_ptr+27) =
   *(dword *)(mem_ptr+next_line+27) = val;
   *(dword *)(mem_ptr+30) =
   *(dword *)(mem_ptr+next_line+30) = val;
   *(dword *)(mem_ptr+33) =
   *(dword *)(mem_ptr+next_line+33) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+36) =
   *(dword *)(mem_ptr+next_line+36) = val;
   *(dword *)(mem_ptr+39) =
   *(dword *)(mem_ptr+next_line+39) = val;
   *(dword *)(mem_ptr+42) =
   *(dword *)(mem_ptr+next_line+42) = val;
   *(dword *)(mem_ptr+45) =
   *(dword *)(mem_ptr+next_line+45) = val;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode1_double(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   register dword next_line;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   next_line = CPC.scr_bps << 2;
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)mem_ptr =
   *(dword *)(mem_ptr+next_line) = val; // write one pixels
   *(dword *)(mem_ptr+3) =
   *(dword *)(mem_ptr+next_line+3) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+6) =
   *(dword *)(mem_ptr+next_line+6) = val;
   *(dword *)(mem_ptr+9) =
   *(dword *)(mem_ptr+next_line+9) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+12) =
   *(dword *)(mem_ptr+next_line+12) = val;
   *(dword *)(mem_ptr+15) =
   *(dword *)(mem_ptr+next_line+15) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+18) =
   *(dword *)(mem_ptr+next_line+18) = val;
   *(dword *)(mem_ptr+21) =
   *(dword *)(mem_ptr+next_line+21) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)(mem_ptr+24) =
   *(dword *)(mem_ptr+next_line+24) = val;
   *(dword *)(mem_ptr+27) =
   *(dword *)(mem_ptr+next_line+27) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+30) =
   *(dword *)(mem_ptr+next_line+30) = val;
   *(dword *)(mem_ptr+33) =
   *(dword *)(mem_ptr+next_line+33) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+36) =
   *(dword *)(mem_ptr+next_line+36) = val;
   *(dword *)(mem_ptr+39) =
   *(dword *)(mem_ptr+next_line+39) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+42) =
   *(dword *)(mem_ptr+next_line+42) = val;
   *(dword *)(mem_ptr+45) =
   *(dword *)(mem_ptr+next_line+45) = val;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_mode2_double(dword addr)
{
   byte pat;
   register byte *mem_ptr;
   register dword next_line;
   dword pen_on, pen_off;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   next_line = CPC.scr_bps << 2;
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *(dword *)mem_ptr =
   *(dword *)(mem_ptr+next_line) = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+3) =
   *(dword *)(mem_ptr+next_line+3) = (pat & 0x40) ? pen_on : pen_off;
   *(dword *)(mem_ptr+6) =
   *(dword *)(mem_ptr+next_line+6) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+9) =
   *(dword *)(mem_ptr+next_line+9) = (pat & 0x10) ? pen_on : pen_off;
   *(dword *)(mem_ptr+12) =
   *(dword *)(mem_ptr+next_line+12) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+15) =
   *(dword *)(mem_ptr+next_line+15) = (pat & 0x04) ? pen_on : pen_off;
   *(dword *)(mem_ptr+18) =
   *(dword *)(mem_ptr+next_line+18) = (pat & 0x02) ? pen_on : pen_off;
   *(dword *)(mem_ptr+21) =
   *(dword *)(mem_ptr+next_line+21) = (pat & 0x01) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(dword *)(mem_ptr+24) =
   *(dword *)(mem_ptr+next_line+24) = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+27) =
   *(dword *)(mem_ptr+next_line+27) = (pat & 0x40) ? pen_on : pen_off;
   *(dword *)(mem_ptr+30) =
   *(dword *)(mem_ptr+next_line+30) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+33) =
   *(dword *)(mem_ptr+next_line+33) = (pat & 0x10) ? pen_on : pen_off;
   *(dword *)(mem_ptr+36) =
   *(dword *)(mem_ptr+next_line+36) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+39) =
   *(dword *)(mem_ptr+next_line+39) = (pat & 0x04) ? pen_on : pen_off;
   *(dword *)(mem_ptr+42) =
   *(dword *)(mem_ptr+next_line+42) = (pat & 0x02) ? pen_on : pen_off;
   *(dword *)(mem_ptr+45) =
   *(dword *)(mem_ptr+next_line+45) = (pat & 0x01) ? pen_on : pen_off;
   CPC.scr_offs += 12; // update PC screen buffer address
}



void draw24bpp_border_half(void)
{
   int colour;
   register byte *mem_ptr;

   colour = GateArray.palette[16];
   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   *(dword *)mem_ptr = colour; // write one pixel of border colour
   *(dword *)(mem_ptr+3) = colour;
   *(dword *)(mem_ptr+6) = colour;
   *(dword *)(mem_ptr+9) = colour;
   *(dword *)(mem_ptr+12) = colour;
   *(dword *)(mem_ptr+15) = colour;
   *(dword *)(mem_ptr+18) = colour;
   *(dword *)(mem_ptr+21) = colour;
   CPC.scr_offs += 6; // update PC screen buffer address
}



void draw24bpp_mode0_half(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)mem_ptr = val; // write one pixels
   *(dword *)(mem_ptr+3) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+6) = val;
   *(dword *)(mem_ptr+9) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode0_table[(idx*2)]];
   *(dword *)(mem_ptr+12) = val;
   *(dword *)(mem_ptr+15) = val;
   val = GateArray.palette[mode0_table[(idx*2)+1]];
   *(dword *)(mem_ptr+18) = val;
   *(dword *)(mem_ptr+21) = val;
   CPC.scr_offs += 6; // update PC screen buffer address
}



void draw24bpp_mode1_half(dword addr)
{
   byte idx;
   register byte *mem_ptr;
   dword val;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   idx = *(pbRAM + addr); // grab first CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)mem_ptr = val; // write one pixels
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+3) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+6) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+9) = val;

   idx = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   val = GateArray.palette[mode1_table[(idx*4)]];
   *(dword *)(mem_ptr+12) = val;
   val = GateArray.palette[mode1_table[(idx*4)+1]];
   *(dword *)(mem_ptr+15) = val;
   val = GateArray.palette[mode1_table[(idx*4)+2]];
   *(dword *)(mem_ptr+18) = val;
   val = GateArray.palette[mode1_table[(idx*4)+3]];
   *(dword *)(mem_ptr+21) = val;
   CPC.scr_offs += 6; // update PC screen buffer address
}



void draw24bpp_mode2_half(dword addr)
{
   byte pat;
   register byte *mem_ptr;
   dword pen_on, pen_off;

   mem_ptr = (byte *)(CPC.scr_base + CPC.scr_offs); // PC screen buffer address
   pen_on = GateArray.palette[1];
   pen_off = GateArray.palette[0];

   pat = *(pbRAM + addr); // grab first CPC screen memory byte
   *(dword *)mem_ptr = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+3) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+6) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+9) = (pat & 0x02) ? pen_on : pen_off;

   pat = *(pbRAM + ((addr+1)&0xffff)); // grab second CPC screen memory byte
   *(dword *)(mem_ptr+12) = (pat & 0x80) ? pen_on : pen_off;
   *(dword *)(mem_ptr+15) = (pat & 0x20) ? pen_on : pen_off;
   *(dword *)(mem_ptr+18) = (pat & 0x08) ? pen_on : pen_off;
   *(dword *)(mem_ptr+21) = (pat & 0x02) ? pen_on : pen_off;
   CPC.scr_offs += 6; // update PC screen buffer address
}
