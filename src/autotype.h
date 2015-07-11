/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2003
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef __AUTOTYPE_HEADER_INCLUDED__
#define __AUTOTYPE_HEADER_INCLUDED__

#ifndef EXTERN_C   
   #ifdef __cplusplus
      #define EXTERN_C    extern "C"
   #else
      #define EXTERN_C    extern
   #endif
#endif

void ASCII_to_CPC(int nASCII, BOOL bKeyDown);

/* auto-type is active and is "typing" */
#define AUTOTYPE_ACTIVE 0x01
/* auto-type is performing key release action */
/* if clear, auto-type is performing key press action */
#define AUTOTYPE_RELEASE 0x02
/* if set, auto-type is waiting for first keyboard scan to be done */
#define AUTOTYPE_WAITING 0x04

typedef struct
{
	char ch;
	/* the string; as ascii characters to type */
	const char *sString;
	/* current position within the string */
	int nPos;
	/* number of characters remaining to type */
	int nCountRemaining;
	/* number of frames to waste before continuing */
	int nFrames;

	unsigned long nFlags;
}  AUTOTYPE;

EXTERN_C void AutoType_Init();
EXTERN_C void AutoType_Term();
EXTERN_C BOOL AutoType_Active();

/* set the string to auto type */
EXTERN_C void AutoType_SetString( const char *sString, BOOL bWaitInput);

/* execute this every emulated frame; even if it will be skipped */
EXTERN_C void AutoType_Update(void);

#endif
