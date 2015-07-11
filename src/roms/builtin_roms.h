#include "cpc464.h"
#include "cpc664.h"
#include "cpc6128.h"
#include "amsdos.h"

//int CPCmodels[3];


int setup_builtin_roms (int model) {

unsigned int *ROMoffset;
switch (model)
	{
	case 0:	ROMoffset=(unsigned int*)&cpc464_rom[0];
		break;
	case 1:	ROMoffset=(unsigned int*)&cpc664_rom[0];
		break;
	case 2:	ROMoffset=(unsigned int*)&cpc6128_rom[0];
		break;
	default:
		return 0;
	}


	memcpy(pbROMlo,ROMoffset,32768);
	return true;
}


//16384
