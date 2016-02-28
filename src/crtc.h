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

#ifndef CRTC_H
#define CRTC_H

#include "z80.h"

void access_video_memory(int repeat_count);

void draw8bpp_border(void);
void draw8bpp_mode0(dword addr);
void draw8bpp_mode1(dword addr);
void draw8bpp_mode2(dword addr);
void draw8bpp_border_double(void);
void draw8bpp_mode0_double(dword addr);
void draw8bpp_mode1_double(dword addr);
void draw8bpp_mode2_double(dword addr);
void draw8bpp_border_half(void);
void draw8bpp_mode0_half(dword addr);
void draw8bpp_mode1_half(dword addr);
void draw8bpp_mode2_half(dword addr);

void draw8bpp_border_scanplus(void);
void draw8bpp_mode0_scanplus(dword addr);
void draw8bpp_mode1_scanplus(dword addr);
void draw8bpp_mode2_scanplus(dword addr);


void draw16bpp_border(void);
void draw16bpp_mode0(dword addr);
void draw16bpp_mode1(dword addr);
void draw16bpp_mode2(dword addr);
void draw16bpp_border_double(void);
void draw16bpp_mode0_double(dword addr);
void draw16bpp_mode1_double(dword addr);
void draw16bpp_mode2_double(dword addr);
void draw16bpp_border_half(void);
void draw16bpp_mode0_half(dword addr);
void draw16bpp_mode1_half(dword addr);
void draw16bpp_mode2_half(dword addr);

void draw16bpp_border_scanplus(void);
void draw16bpp_mode0_scanplus(dword addr);
void draw16bpp_mode1_scanplus(dword addr);
void draw16bpp_mode2_scanplus(dword addr);

void draw16bpp_border_CRT(void);
void draw16bpp_mode0_CRT(dword addr);
void draw16bpp_mode1_CRT(dword addr);
void draw16bpp_mode2_CRT(dword addr);

void draw24bpp_border(void);
void draw24bpp_mode0(dword addr);
void draw24bpp_mode1(dword addr);
void draw24bpp_mode2(dword addr);
void draw24bpp_border_double(void);
void draw24bpp_mode0_double(dword addr);
void draw24bpp_mode1_double(dword addr);
void draw24bpp_mode2_double(dword addr);
void draw24bpp_border_half(void);
void draw24bpp_mode0_half(dword addr);
void draw24bpp_mode1_half(dword addr);
void draw24bpp_mode2_half(dword addr);

void draw24bpp_border_scanplus(void);
void draw24bpp_mode0_scanplus(dword addr);
void draw24bpp_mode1_scanplus(dword addr);
void draw24bpp_mode2_scanplus(dword addr);


void draw32bpp_border(void);
void draw32bpp_mode0(dword addr);
void draw32bpp_mode1(dword addr);
void draw32bpp_mode2(dword addr);
void draw32bpp_border_double(void);
void draw32bpp_mode0_double(dword addr);
void draw32bpp_mode1_double(dword addr);
void draw32bpp_mode2_double(dword addr);
void draw32bpp_border_half(void);
void draw32bpp_mode0_half(dword addr);
void draw32bpp_mode1_half(dword addr);
void draw32bpp_mode2_half(dword addr);

void draw32bpp_border_scanplus(void);
void draw32bpp_mode0_scanplus(dword addr);
void draw32bpp_mode1_scanplus(dword addr);
void draw32bpp_mode2_scanplus(dword addr);


#endif
