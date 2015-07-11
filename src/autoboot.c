
 void CPC_BootStartTape (){ 

			if (pbTapeImage) {
				CPC.tape_play_button = 0x10; 
				switch (CPC.model) {
					case 0:
						AutoType_SetString("RUN\"\n\r  ", FALSE);
						break;
					case 1:
					case 2:
						AutoType_SetString("|TAPE\n\rRUN\"\n\r  ", FALSE);
                                        	break;
                                        }
			}
 
}


#define FORMAT_DATA	0xC1;
#define FORMAT_SYSTEM	0x41;
#define FORMAT_IBM	0x01;


int CPC_BootStartDisk (t_drive bootdisc)
{

	int booterror;
	unsigned char *sectdata=NULL;
	unsigned char cmp;
	unsigned int in;

	typedef struct{
	 unsigned char filename[8];
	 unsigned char ext[8];
	 unsigned char blocks[16];
	} ams_file;

	ams_file filelist[64];
	unsigned char dformat;
	unsigned char dtrack;
	unsigned char dsector;

	//is CPM

	for (in=0;in<bootdisc.track[0][0].sectors;in++)
	{
		if (bootdisc.track[0][0].sector[in].CHRN[2] == 0x41)
		{
		sectdata=bootdisc.track[0][0].sector[in].data;
		}
	}


	if (sectdata!=NULL) //got 0x41 sector?
	{
	cmp=sectdata[0];
	for (in=1;in<512;in++)
		{
			if (sectdata[in]!=cmp)
			{
			AutoType_SetString("|CPM\n\r", FALSE);
			printf ("\nIs CP/M Boot!!\n");
			return 1;
			}
		}
	}

	// So is no CPM...

	//dual head not allowed
	if (bootdisc.sides>0) return 0;

	// only analize  standard SYSTEM,DATA,IBM!!!!
	//What I got??

	dformat=NULL;
/*
	for (in=0;in<bootdisc.track[0][0].sectors;in++)
	{
		switch (bootdisc.track[0][0].sector[in].CHRN[2])
		{
		  case FORMAT_DATA:
		  case FORMAT_SYSTEM:
		  case FORMAT_IBM:
					dformat=bootdisc.track[0][0].sector[in].CHRN[2];
					break;
		  default:
					break;
		}

	}

	//if not SYS,DAT,IBM, then...
	if (dformat==NULL) return 0;
	
	//fill the file list
	switch (bootdisc.track[0][0].sector[in].CHRN[2])
	{
	  case FORMAT_DATA:
				dtrack=0;
				break;
	  case FORMAT_SYSTEM:
				dtrack=2;
				break;

	  case FORMAT_IBM:
				dtrack=1;
				break;
	}

	//lets look for  bootfile in first 

	for (in=0;in<bootdisc.track[dtrack][0].sectors;in++)
	{
	}
*/
	//TO-DOOOO

return 0;

}
