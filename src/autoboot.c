/******************************************************************/
/*                                                                */
/*       CapriceRPI Autoboot routines                             */
/*       Based on Kevin Thacker's Arnold own routines             */
/*                                      KaosOverride 2015         */
/******************************************************************/

// Tape Autoboot  //////////////////////////////////////////////////

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


// Disk Autoboot  //////////////////////////////////////////////////

#define FORMAT_DATA	0xC1
#define FORMAT_SYSTEM	0x41
#define FORMAT_IBM	0x01

typedef struct{
 unsigned char user[1];
 unsigned char filename[8];
 unsigned char ext[3];
 unsigned char extent[4];
 unsigned char blocks[16];
} ams_file;


int validate_filename (ams_file dir_entry, unsigned char *filen)
{
	unsigned int ind,namoff;
	unsigned char tmpEXT[4];
	unsigned char tmpNAME[9];
	unsigned int validEXT,validNAME,NULLext;
	tmpEXT[3]='\0';
	tmpNAME[8]='\0';
	filen[13]=0; //filename score


	for (ind=0;ind<3;ind++)
		{
		tmpEXT[ind]=dir_entry.ext[ind]&127;
		}

	validEXT=0;
	if (strcmp((const char*)tmpEXT,"   ") == 0)
			{
			NULLext=1;
			validEXT=1;
			filen[13]=filen[13]+16; //filename score update
			}
	if (strcmp((const char*)tmpEXT,"BAS") == 0)
			{
			NULLext=0;
			validEXT=1;
			filen[13]=filen[13]+32; //filename score update
			}

	if (strcmp((const char*)tmpEXT,"BIN") == 0)
			{
			NULLext=0;
			validEXT=1;
			filen[13]=filen[13]+8; //filename score update
			}

	if (validEXT==0) return 0;


	for (ind=0;ind<8;ind++)
		{
		   validNAME=0;
		   tmpNAME[ind]=dir_entry.filename[ind];
		   if (tmpNAME[ind]==32) validNAME=1;
		   if (tmpNAME[ind]==33) validNAME=1;
		   if ( (tmpNAME[ind]>43) && (tmpNAME[ind]<57) ) validNAME=1;
		   if ( (tmpNAME[ind]>64) && (tmpNAME[ind]<90) ) validNAME=1;
		   if (tmpNAME[0]==32) validNAME=0; //The exception!!

		}
//	if (validNAME==0) printf("NOT-%s\n",tmpNAME);
//	if (validNAME==1) printf("YES-%s\n",tmpNAME);

	if (validNAME==0) return 0;

	if (strcmp((const char*)tmpNAME,"DISC    ") == 0)
			{
			filen[13]=filen[13]+16; //filename score update
			}
	if (strcmp((const char*)tmpNAME,"DISK    ") == 0)
			{
			filen[13]=filen[13]+16; //filename score update
			}


//Assemble filename string

	namoff=0;

	for (ind=0;ind<8;ind++)
		{
		if (tmpNAME[ind]==32) break;
		filen[ind]=tmpNAME[ind];
		namoff=ind;
		}
	namoff++;

	if (NULLext == 0)
		{
		filen[namoff]='.';
		namoff++;
		for (ind=0;ind<3;ind++){filen[ind+namoff]=tmpEXT[ind];}
		filen[namoff+3]='\0';
		}
		else  filen[namoff]='\0';

	return 1;
}


int CPC_BootStartDisk (t_drive bootdisc)
{

//	int booterror;
	unsigned char *sectdata=NULL;
	unsigned char cmp;
	unsigned int in,round;
	unsigned char dformat,dtrack=0;
	unsigned char best_score=0;
	unsigned char *best_filename=NULL;

	ams_file directory[64];
	ams_file *dir_pointer=NULL;
	unsigned char filename[64][14];
	memset(filename,0,sizeof(filename));
	memset(directory,0,sizeof(directory));


	//dual head not allowed
	//printf ("...Sides:%d",bootdisc.sides);
	if (bootdisc.sides>0)
		{
		//printf ("...Not single sided disk\n");
		return 0;
		}

	// only analize  standard SYSTEM,DATA,IBM!!!!
	//What I got??

	dformat=0;

	//printf ("...Sectors:");
	for (in=0;in<bootdisc.track[0][0].sectors;in++)
	{

	//printf (" %#02x",bootdisc.track[0][0].sector[in].CHRN[2]);

		switch (bootdisc.track[0][0].sector[in].CHRN[2])
		{
		  case FORMAT_DATA:
				dtrack=0;
				dformat=bootdisc.track[0][0].sector[in].CHRN[2];
					break;
		  case FORMAT_SYSTEM:
				dtrack=2;
				dformat=bootdisc.track[0][0].sector[in].CHRN[2];
					break;
		  case FORMAT_IBM:
				dtrack=1;
				dformat=bootdisc.track[0][0].sector[in].CHRN[2];
					break;
		  default:
					break;
		}

	}
	//printf (" ");

	//if not SYS,DAT,IBM, then...
	if (dformat==0) 
		{
		//printf ("...Not Amsdos disk\n");
		return 0;
		}

	//fill the file list


	for (round=0;round<4;round++)
	{

	for (in=0;in<bootdisc.track[dtrack][0].sectors;in++)
		{
		if (bootdisc.track[dtrack][0].sector[in].CHRN[2] == dformat+round )
			{
			sectdata=bootdisc.track[dtrack][0].sector[in].data;
			dir_pointer=&directory[0+(16*round)];
			memcpy((unsigned char*)dir_pointer,sectdata,512);

			}
		}
	}

	//lets look for  bootfile in first 

	//TEST IN
	//print all the raw file names
	//printf("Files found:\n");

	round=0;
	for (in=0;in<64;in++)
	{	if (directory[in].blocks[0]>0)
			{
			unsigned char *filenpoint;
//			printf("%s.%s\n",directory[in].filename,directory[in].ext);
//			printf("%i\n",directory[in].blocks[0]);
			filenpoint=&filename[round][0];
/*			if (validate_filename(directory[in],filenpoint)) 
				{round ++;
				printf("%s.%s\n",directory[in].filename,directory[in].ext);
				printf("%i\n",directory[in].blocks[0]);

				}
*/
			if (validate_filename(directory[in],filenpoint)) round ++;
			}
	}



	for (in=0;in<64;in++)
	{	if (filename[in][0]!='\0')
			{
//			printf("---%s\n",filename[in]);
			if (filename[in][13]>best_score)
			  {
			  best_filename=filename[in];
			  best_score=filename[in][13];
			  }
			}
	}

//	printf("%s  -->%i\n",best_filename,best_score);

	if (best_score<40)
	{
	//is CPM??

		for (in=0;in<bootdisc.track[0][0].sectors;in++)
		{
			if (bootdisc.track[0][0].sector[in].CHRN[2] == FORMAT_SYSTEM )
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
				return 1;
				}
			}
		}
	// So is no CPM...
	}

	if (best_score > 0)
		{
		char runstring[25]="RUN\"                    ";
		char enterstring[3]="\n\r";

		for (in=0;in<12;in++)
			{
			runstring[in+4]=best_filename[in];
			if (runstring[in+4]=='\0')
				{
				runstring[in+4]=enterstring[0];
				runstring[in+5]=enterstring[1];
				runstring[in+6]='\0';
				break;
				}
			else if (in==11)
				{
				runstring[in+5]=enterstring[0];
				runstring[in+6]=enterstring[1];
				runstring[in+7]='\0';

				}
			}

		AutoType_SetString(runstring, FALSE);
		return 1;

		}


	//TEST OUT


	//TO-DOOOO
//printf ("...No autoboot info found.\n");

return 0;

}
