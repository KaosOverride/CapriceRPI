/*

File manager from NK's Snes 9x port to GP2x and adapted to Caprice GP2x by KaosOverride

Menu System for CapriceGP2x by KaosOverride based on NK's file manager GUI

*/

#include <unistd.h>
#include <dirent.h>
#include <sstream>
#include "bool.h"
#include "cap32.h"
#include "cap32defs.h"
#include "crtc.h"
#include "tape.h"
#include "tape.h"
#include "z80.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "autotype.h"
#include "gui/gui_bmp.h"
#include "vjoystick.h"
#include "videodraw/SDL_rotozoom.h"
#include "videodraw/SDL_rotozoom.c"


//externals
extern SDL_Surface *video_surface,*back_surface;

extern t_CPC CPC;
extern t_drive driveA;

extern bool have_DSK;
extern bool have_SNA;
extern bool have_TAP;

extern byte *pbTapeImage;
extern int cpc_tapeturbo;
extern int cpc_tapespeeding;

extern int emulatorend;

extern int dwXScale,dwYScale;

SDL_Surface *portada, *montaje_zoom, *montaje,*menu,*fich,*zipo,*carga,*carpa, *fondo, *snapa , *tapa, *disca = NULL;
SDL_Surface *key_normal, *key_press, *keyboard_surface = NULL;
SDL_Rect montaje_region;

int keyboard_pos=1;
int keyboard_show=0;
int keyboard_col=7;
int keyboard_row=2;
#include "vkeydefs.h"

int nopcion,vismenu=0;


void menu_blit (void)
{


	//AQUI SE HA DE BLITEAR CON ZOOM

  montaje_zoom = zoomSurface(montaje,dwXScale,dwYScale,0);


/*
  montaje_region.x=64;
  montaje_region.y=25;
  montaje_region.w=640;
  montaje_region.h=480;
*/
  	montaje_region.x=(back_surface->w-montaje_zoom->w)/2;
  	montaje_region.y=(video_surface->h-montaje_zoom->h)/2;
  	montaje_region.w=montaje_zoom->w;
  	montaje_region.h=montaje_zoom->h;



   SDL_BlitSurface(montaje_zoom, NULL, video_surface, &montaje_region);  //180-240
   SDL_UpdateRect(video_surface, 0,0,0,0);
   if (montaje_zoom != NULL) SDL_FreeSurface( montaje_zoom); 


}



/*
================================================================================================================
                                              FUENTES
================================================================================================================
*/


static TTF_Font *font = NULL;
//static TTF_Font *smallfont = NULL;

static void shutdownfonts(void);

void setupfonts_file(void)
{
	if (TTF_Init() < 0)
		printf("Font error: %s\n", TTF_GetError());

	font = TTF_OpenFont("gui/font.ttf", 16);

	if (font == NULL)
		printf("Font error: %s\n", TTF_GetError());

//	smallfont = TTF_OpenFont("gui/font.ttf", 8);
//	if (smallfont == NULL)
//		printf("Font error: %s", TTF_GetError());

//	atexit(shutdownfonts);
}

#include "gui/gui_ttf.h"

void setupfonts(void)
{
	if (TTF_Init() < 0)
		printf("Font error: %s\n", TTF_GetError());

//	font = TTF_OpenFont("gui/font.ttf", 16);
	font = TTF_OpenFontRW(SDL_RWFromConstMem(font_ttf,font_ttf_size),1, 16);

	if (font == NULL)
		printf("Font error: %s\n", TTF_GetError());

//	smallfont = TTF_OpenFont("gui/font.ttf", 8);
//	if (smallfont == NULL)
//		printf("Font error: %s", TTF_GetError());

//	atexit(shutdownfonts);
}

static void shutdownfonts(void)
{
//	fontinuse = NULL;
	
	//TTF_CloseFont(smallfont);
	//smallfont = NULL;

	TTF_CloseFont(font);
	font = NULL;

	TTF_Quit();
}

static SDL_Color getfontcolor(int r, int g, int b)
{
	SDL_Color col;
	col.r = r;
	col.g = g;
	col.b = b;
	return col;
}

static void displaytext( int tx, int ty, const char *texto, SDL_Color textcolor)
{
       SDL_Surface *textsurface;
//       fontinuse=font;
	SDL_Rect dest;
	int textwidth, textheight;

	if (TTF_SizeText(font, texto, &textwidth, &textheight) < 0)
		printf("Font error: %s\n", TTF_GetError());

	if (textwidth >= 320 || textheight >= 240)
		printf("Font is too big (%dx%d)\n", textwidth, textheight);

	dest.x = tx; //- textwidth/2;
	dest.y = ty; //- textheight/2;
	dest.w = textwidth;
	dest.h = textheight;


	//textsurface = TTF_RenderText_Solid(font, texto, textcolor);
	textsurface = TTF_RenderText_Blended(font, texto, textcolor);

	if (!textsurface)
		printf("Font error: %s\n", TTF_GetError());

	SDL_BlitSurface(textsurface, NULL, montaje, &dest);  //PONER A DONDE!!!
        //SDL_UpdateRects(montaje, 1, &dest);
        SDL_Delay(5);
	SDL_FreeSurface(textsurface);
//	SDL_UpdateRect(video_surface, 0, 0, 0, 0);  //PONER A DONDE!!!
}

/*

Ejemplo

void showpausedtext(void)
{
	displaytext(x,y,"PAUSED", getfontcolor(0, 168, 224)); 
}*/



/*
================================================================================================================
                                              MENU

                                              Part of code from NK's SNES9x port's ROM loader

================================================================================================================

*/



#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFDIR) == _S_IFDIR)
#endif

#define MAX_NAME 256
#define MAX_ENTRY 512
#define LIST_COLORKEY 0xFEFE

     //
     
     

int skin_no;
int use_icon;
int needreset;
int draw_pos, cur_pos, redraw;
int draw_menupos, cur_menupos;


static char cur_path[_MAX_PATH];
static char open_path[_MAX_PATH];
//static char skin_path[_MAX_PATH];
//static char work_path[_MAX_PATH];

struct filelist {
	char name[MAX_NAME + 1];
	int isdir;
} files[MAX_ENTRY];
static int nfiles;


enum {    
	EXT_SNAP,
	EXT_DSK,
	EXT_TAPE,
	EXT_ZIPPED,
	EXT_UNKNOWN  
};
const struct {
	char szExt[4];
	int nExtId;
} stExtentions[] = {        
	{ "sna", EXT_SNAP },
	{ "dsk", EXT_DSK },
	{ "cdt", EXT_TAPE },
	{ "zip", EXT_ZIPPED },
	{ "\0", EXT_UNKNOWN }
};
     

//=========================================================
//               FILE MANAGEMENT
//=========================================================


int getExtId( char *szFilePath) {
	char *pszExt;
	int i;
	if (pszExt = strrchr(szFilePath, '.')) {
		pszExt++;
		for (i = 0; stExtentions[i].nExtId != EXT_UNKNOWN; i++) {
			if (!strcasecmp(stExtentions[i].szExt,pszExt)) {
				return stExtentions[i].nExtId;
			}
		}
	}
// Special
	if(!strcasecmp(szFilePath, "IPL.TXT"))
		return EXT_UNKNOWN;

	return EXT_UNKNOWN;
}


int cmpFile(struct filelist *a, struct filelist *b)
{
	if(a->isdir == b->isdir){
		return strcasecmp(a->name, b->name);
	} else if(a->isdir)
		return -1;
	else
		return 1;
}

void sort(int left, int right) {
	struct filelist tmp, pivot;
	int i, p;
	
	if (left < right) {
		pivot = files[left];
		p = left;
		for (i=left+1; i<=right; i++) {
			if (cmpFile(&files[i],&pivot)<0){
				p=p+1;
				tmp=files[p];
				files[p]=files[i];
				files[i]=tmp;
			}
		}
		files[left] = files[p];
		files[p] = pivot;
		sort(left, p-1);
		sort(p+1, right);
	}
}

void getDir(const char *path, int updir) {
	strcpy(open_path, path);
	
	while(1) {
		DIR *fd;
		char prev_dir[MAX_NAME];
		int prev_len = 0;

		if(updir && (strcasecmp(open_path, "/mnt/") == 0) ) {
			return;
		}
		if(updir) {
			int i;
			for(i = strlen(open_path)-1; i>0 ; i--) {
				if((open_path[i-1] == '/') || (open_path[i-1] == '\\')) {
					strcpy(prev_dir, &open_path[i]);
					prev_len = strlen(prev_dir);
					while( (prev_len > 0) &&
						((prev_dir[prev_len-1] == '/') || (prev_dir[prev_len-1] == '\\')) ) {
						prev_dir[prev_len-1] = '\0';
						--prev_len;
					}
					if(prev_len != 0) {
						prev_dir[prev_len++] = '/';
						prev_dir[prev_len] = '\0';
//						printf("Filer: Prev Dir: %s\n", prev_dir);
					}
					open_path[i] = '\0';
					updir = 0;
					break;
				}
			}
			if(updir){
				return;
			}
		}

		fd = opendir(open_path);
		if(fd != NULL) {
			struct dirent *direntp;
			struct stat statbuf;
			char fullname[_MAX_PATH];

			nfiles = 0;
			while( (nfiles < MAX_ENTRY) && (direntp = readdir(fd)) != NULL ){
				if(direntp->d_name[0] == '.') continue;
				sprintf(fullname, "%s%s", open_path, direntp->d_name);
				if(stat(fullname, &statbuf) == 0) {
					files[nfiles].isdir = S_ISDIR(statbuf.st_mode);
					if(files[nfiles].isdir) {
						strcpy(files[nfiles].name, direntp->d_name);
						strcat(files[nfiles].name, "/");
						nfiles++;
					} else if(getExtId(direntp->d_name) != EXT_UNKNOWN) {
						strcpy(files[nfiles].name, direntp->d_name);
						nfiles++;
					}
				}
			}
			closedir(fd);

			sort(0, nfiles-1);

			cur_pos = 0;
			draw_pos = 0;
			redraw = 1;
			if(prev_len != 0){
				int i;
				for(i=0;i<nfiles;i++){
					if(strcasecmp(files[i].name, prev_dir) == 0) {
						cur_pos = i;
						draw_pos = i - 2;
						if((draw_pos + 5) > nfiles) draw_pos = nfiles - 5;
						if(draw_pos < 0) draw_pos = 0;
						break;
					}
				}
			}

			strcpy(cur_path, open_path);
			//printf("Filer: Get Dir %s(%d).\n", open_path, nfiles);

			return;
		}
		updir = 1;
	}
}


//=========================================================
//               GENERAL FILE LOADER
//=========================================================



void drawlist(char *extension) 
{
	int y = 50;
	int i;
	SDL_Rect dstrect;
	dstrect.x = 20;
	if(nfiles > 5) 
	{
		if((draw_pos + 4) < cur_pos) 	draw_pos = cur_pos - 4;
		if(draw_pos > cur_pos) 		draw_pos = cur_pos;
	}

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
   	SDL_UpdateRect(montaje, 0,0,0,0);

		switch(getExtId(extension)) 
		{
		case EXT_TAPE:
				displaytext( 98, 25 ," CASSETE LOAD", getfontcolor(250, 250, 0));
				break;
		case EXT_DSK:
				displaytext( 98, 25 ," FLOPPY LOAD", getfontcolor(250, 250, 0));
				break;
		case EXT_SNAP:
				displaytext( 98, 25 ,"SNAPSHOT LOAD", getfontcolor(250, 250, 0));
				break;
		}

//	SDL_FillRect(list, NULL, LIST_COLORKEY);

	for(i=draw_pos;(i<nfiles) && (y < 200);i++, y+=25)
	{

	if(i == cur_pos)     	displaytext( 50, y ,files[i].name, getfontcolor(250, 0, 0)); 
//			printU8(list, 50, y + 8, files[i].name, 0x0000);
	else
//			printU8(list, 50, y + 8, files[i].name, 0xFFFF);
     	displaytext( 50, y ,files[i].name, getfontcolor(250, 250, 0));

	dstrect.y = y;

	if(files[i].isdir)
		SDL_BlitSurface(carpa, NULL, montaje, &dstrect);

		else
		switch(getExtId(files[i].name)) 
		{

			case EXT_DSK:
				SDL_BlitSurface(disca, NULL, montaje, &dstrect);
				break;

			case EXT_TAPE:
				SDL_BlitSurface(tapa, NULL, montaje, &dstrect);
				break;
			case EXT_SNAP:
				SDL_BlitSurface(snapa, NULL, montaje, &dstrect);
				break;
			case EXT_ZIPPED:
				SDL_BlitSurface(zipo, NULL, montaje, &dstrect);
			default:
				SDL_BlitSurface(fich, NULL, montaje, &dstrect);
				break;

		}
	}

   SDL_UpdateRect(montaje, 0,0,0,0);
//	SDL_BlitSurface(list, NULL, video_surface, NULL);

}



int eventloop(void) {
	SDL_Event event;
	int flag = 0;
	static int move_dir = 0;
	static Uint32 move_tick = 0;
	move_dir = 0;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
//////////////

    case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

				move_dir = 0;
				pcjoy_update (event);

				if ( (pcjoy_pressed (PC_JOY1_UP)) || (pcjoy_pressed (PC_JOY2_UP))) 
					{		//Action UP
					cur_pos--;
					redraw=1;
					move_dir = -1;
					move_tick = 0;
					break;
					};

				if ( (pcjoy_pressed (PC_JOY1_DOWN)) || (pcjoy_pressed (PC_JOY2_DOWN))) 
					{		//Action DOWN
					cur_pos++;
					redraw=1;			     
					move_dir = 1;
					move_tick = 0;
					break;	
					};

				if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
					{		//Action LEFT
					move_dir = 0;
					getDir(open_path, 1);
					break;
					};

				if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
					{		//Action RIGHT
					if(files[cur_pos].isdir){
						strcat(open_path, files[cur_pos].name);
						getDir(open_path, 0);
						move_dir = 0;
						cur_pos = 0;
						draw_pos = 0;
						redraw = 1;
					} else {
						flag = 1;
					}
					break;
					};

				if (event.type == SDL_JOYBUTTONDOWN )		
				{
				switch (event.jbutton.button)  	
				{
					case SDL_JoyFire1:		//Accion seleccionar
						if(files[cur_pos].isdir)
						{
							strcat(open_path, files[cur_pos].name);
							getDir(open_path, 0);
							move_dir = 0;
							cur_pos = 0;
							draw_pos = 0;
							redraw = 1;
						} else {
							flag = 1;
						}
						break;
					case SDL_JoyFire2:
						move_dir = 0;
						getDir(open_path, 1);
						break;
					case SDL_JoyFire4:		// Accion salir ;
						flag = 2;
						break;
				}
			}
         		break;

///////////////
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_PAGEUP:
        					cur_pos=cur_pos-5;
        					redraw=1;
//						move_dir = -1;
//						move_tick = 0;
						break;
					case SDLK_PAGEDOWN:
						cur_pos=cur_pos+5;
						redraw=1;
//						move_dir = 1;
//						move_tick = 0;
						break;
				

			case SDLK_UP:
				cur_pos--;
				redraw=1;
				move_dir = -1;
				move_tick = 0;
				break;
			case SDLK_DOWN:
				cur_pos++;
				redraw=1;
				move_dir = 1;
				move_tick = 0;
				break;
			case SDLK_LEFT:
				move_dir = 0;
				getDir(open_path, 1);
				break;
			case SDLK_RIGHT:
			case SDLK_RETURN:
				if(files[cur_pos].isdir)
				{
					strcat(open_path, files[cur_pos].name);
					getDir(open_path, 0);
					move_dir = 0;
					cur_pos = 0;
					draw_pos = 0;
					redraw = 1;
				} else {
					flag = 1;
				}
				break;
			case SDLK_ESCAPE:
			case 'q':
				flag = 2;
				break;
/*			case 'z':
				prevskin();
				break;
			case 'x':
				nextskin();
				break;*/
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
			case SDLK_DOWN:
				move_dir = 0;
				move_tick = 0;
				break;
			default:
				break;
			}
			break;
		}
	}
/*
	if( (move_dir != 0) && ((move_tick + 70) < SDL_GetTicks()) ) {

		static int boost = 0;
		static Uint32 boost_tick = 0;
		int old_cur_pos = cur_pos;

		if( (boost) || ((move_tick + 200) < SDL_GetTicks()) ) {
			cur_pos += move_dir;
			if(cur_pos >= nfiles) cur_pos =  nfiles - 1;
			if(cur_pos < 0) cur_pos = 0;
			redraw = (old_cur_pos != cur_pos);

			if(move_tick == 0) {
				boost_tick = SDL_GetTicks();
				boost = 0;
			}
			move_tick = SDL_GetTicks();
		}

		if( (!boost) && ((boost_tick + 300) < SDL_GetTicks()) ) {
			boost = 1;
		}
		
	}*/
			if(cur_pos >= nfiles) cur_pos =  nfiles - 1;
			if(cur_pos < 0) cur_pos = 0;
	return flag;
}



//=========================================================
//               FILE LOADER
//=========================================================


int fileloader( char *out, char *ext )
{


int flag;
static char pathtmp[_MAX_PATH+1]="./";  //SAFE CLEANUP
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
			switch(getExtId(ext)) {
			case EXT_TAPE:
                          //pathtmp="./tape/"; //
                          strcpy(pathtmp, "./tape/");
                          break;
			case EXT_DSK:
                          //pathtmp="./disc/";//
                          strcpy(pathtmp, "./disc/");
                          break;
			case EXT_SNAP:
                          //pathtmp="./snap/";//
                          strcpy(pathtmp, "./snap/");
                          break;
			default:
                          strcpy(pathtmp, "./");
                          //pathtmp="./";
			}          

	getDir(pathtmp, 0);
	cur_pos = 0;
	draw_pos = 0;
	redraw = 1;
	while((flag = eventloop()) == 0)
	{
		if(redraw) 
		{
			drawlist(ext);
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}
//	printf("Saliendo del selector con: %s\n", cur_path );

	if(flag == 1) 
	{
		// Load SNAP...
		if(carga) 
		{
			if( (carga->w < 320) || (carga->h < 240) ) 
			{
				SDL_Rect dstrect;
				dstrect.w = carga->w;
				dstrect.h = carga->h;
				if(dstrect.w < 320)	dstrect.x = (video_surface->w - dstrect.w) >> 1;
				else			dstrect.x = 0;

				if(dstrect.h < 240)	dstrect.y = (video_surface->h - dstrect.h) >> 1;
				else			dstrect.y = 0;

				SDL_BlitSurface(carga, NULL, video_surface, &dstrect);
			} else {
				SDL_BlitSurface(carga, NULL, video_surface, NULL);
			}
            		SDL_UpdateRect(video_surface, 0, 0,0,0); 
			SDL_Delay(1000);

		}


		strcpy(out, open_path);
		strcat(out, files[cur_pos].name);


//	printf("Cargando: %s\n", out );
	
//		snapshot_load (out);
		
		
//		if(getExtId(files[cur_pos].name) == EXT_ZIPPED)
//			strcat(out, files[cur_pos].name);

		return 1;
	}//flag=1

	out[0] = '\0';
	return 0;
}


struct textmenu{
       char text[20];
               };


// TAPE MAIN MENU

//=========================================================
//               MENU SYSTEM
//=========================================================


enum {    
	MENU_DISK,
	MENU_TAPE,
	MENU_SNAP,
	MENU_OPT,
	MENU_SET,
	MENU_UNKNOWN  
};


int maxopt_menucurrent=6; //Max items in current menu

int opt_menutape=1;
static int maxopt_menutape=6; //Max items in menu
textmenu text_menutape[8];
int option_menutape[8] ;


int opt_menudisc=1;
static int maxopt_menudisc=5; //Max items in menu
textmenu text_menudisc[8];
int option_menudisc[8] ;


int opt_menusnap=1;
static int maxopt_menusnap=2; //Max items in menu
textmenu text_menusnap[8];
int option_menusnap[8] ;


int opt_menuconfig=1;
static int maxopt_menuconfig=5; //Max items in menu
textmenu text_menuconfig[8];
int option_menuconfig[8] ;


int opt_settingsconfig=1;
static int maxopt_settingsconfig=3; //Max items in menu
textmenu text_settingsconfig[8];
int option_settingsconfig[8] ;



//EVALUATORS

int menu_eval_tape (int cur_menupos){
    
                   int flag=0;     
                 switch (cur_menupos)
                 {
                  case 0:  //1 Load tape
                               {
                               char tape_filename[_MAX_PATH];
                               fileloader(tape_filename, ".cdt");   // No verificamos errores¿¿?¿?¿?¿? :(

                               cur_menupos = 0;
                               draw_menupos = 0;
                               char tapepath[_MAX_PATH + 1];
                               strcpy(tapepath, tape_filename);
                               if(!tape_insert(tapepath)) {
                                                          splitPathFileName(tapepath, tapepath, tape_filename);
                                                          strcpy(CPC.tape_path, tapepath);
                                                          strcpy(CPC.tape_file, tape_filename); //Se supone k aki se graba sin path¿?
                                                          CPC.tape_zip = 0;
                                                          have_TAP = true;
                                                          }
                               //redraw=1;
                               }

                               //flag =1;
                       break;
                  case 1:  //2 Autotype
                           if (!option_menutape[2]){
                                    switch (CPC.model) {
                                        case 0:
                                           AutoType_SetString("RUN\"\n\r  ", FALSE);
                                           break;
                                        case 1:
                                        case 2:
                                           AutoType_SetString("|TAPE\n\rRUN\"\n\r  ", FALSE);
                                        break;
                                        }
                           }else{
                                 AutoType_Init();
                                           //AutoType_SetString("\0", TRUE);
                           }                                 
                               option_menutape[2]=!option_menutape[2];
                               //redraw=1;
                               break;

                  case 2:  //3 play tape
                                 if (pbTapeImage) {
                                 if (CPC.tape_play_button) {
                                    CPC.tape_play_button = 0;
                                 } else {
                                    CPC.tape_play_button = 0x10;
                                 }
                                 }
                               vismenu =0;
                               CPC.paused=0; 
                               audio_resume();
                               flag=1;
                       break;

                  case 3:  //4 rewind
                       Tape_Rewind();
                 	   option_menutape[3]=(CPC.tape_play_button != 0);
                       //redraw=1;
                       break;
                       
                  case 4:  //5 memory
                       Tape_Rewind(); //Reset Tape state
                       tape_eject();  //NULLify the tape. Rewind before ejecting to reset state. May crash if not
                 	   option_menutape[3]=(CPC.tape_play_button != 0);
                       //redraw=1;
                       break;
                     
                  case 5:  //6 Frameskip when motor on
                           option_menutape[6]=!option_menutape[6];
                           cpc_tapeturbo=option_menutape[6];
                       //redraw=1;
                       break;
                       
                  
                 };
return flag;

}

//   SNAPSHOT EVALUATOR


const char rolltable[49]="789<-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789<-ABCDE";
#define TABLELENGH  38

int saveSnapGUI ()
{
char snapfilename[_MAX_PATH];
char pathtmp[_MAX_PATH];
int tablepos,namepos,exitloop=0;
SDL_Event event;

tablepos=5;
strcpy(pathtmp, "./snap/");

strcpy(snapfilename,CPC.snap_file);

      if (snapfilename[0] == '\0')  strcpy (snapfilename,"SNAP");
while (exitloop==0)
     {
      SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
      SDL_UpdateRect(montaje, 0,0,0,0);
      displaytext( 70, 25 ,"SAVE SNAPSHOT", getfontcolor(250, 250, 0)); 			
      displaytext( 60, 45 ,snapfilename, getfontcolor(250, 250, 0)); 			
    
      {  //ISOLATE 1

                   char linekeys[13];
                   char tmpline[6]="ABCDE";
                   char myposchar[2];
                   myposchar[0]=rolltable[tablepos];
                   myposchar[1]='\0';

                   //Prepare prefix
                   strncpy(tmpline,&rolltable[tablepos-5],5);
                   tmpline[5]='\0';
                   if (tmpline!= NULL)             displaytext( 40, 100 ,tmpline, getfontcolor(250, 250, 0)); 			
    
                   displaytext( 100, 100 ,myposchar, getfontcolor(250,0, 0)); 			
    
                   //Prepare Sufix
                   strcpy (tmpline,"ZXCVB");                   
                   strncpy(tmpline,&rolltable[tablepos+1],5) ;
                   tmpline[5]='\0';


      if (tmpline!= NULL)             displaytext( 120, 100 ,tmpline, getfontcolor(250, 250, 0)); 			

                   menu_blit();

      //EVENTS

      	while(SDL_PollEvent(&event)) {
		switch(event.type) {

//////////////

    case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

                  pcjoy_update (event);
                  if ( (pcjoy_pressed (PC_JOY1_UP)) || (pcjoy_pressed (PC_JOY2_UP))) 
												{		//Action UP
												};
									if ( (pcjoy_pressed (PC_JOY1_DOWN)) || (pcjoy_pressed (PC_JOY2_DOWN))) 
												{		//Action DOWN
												};
                  if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
												{		//Action LEFT
													tablepos--;

												};
									if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
												{		//Action RIGHT
													tablepos++;
													};
					
							if (event.type == SDL_JOYBUTTONDOWN )		
							{		
							 switch (event.jbutton.button)  	
									{	 
									case SDL_JoyFire1:
											//Accion seleccionar
											break;
									case SDL_JoyFire4:
											// Accion salir ;
											exitloop=1;
											break;
									}
							}								
         break;

///////////////			
			
			
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) 
            {

			case SDLK_LEFT:
                 tablepos--;
//                 redraw=1;
//				move_dir = -1;
//				move_tick = 0;
				break;
			case SDLK_RIGHT:
			     tablepos++;
//                 redraw=1;			     
//				move_dir = 1;
//				move_tick = 0;
				break;
			default:
				break;
            }
		case SDL_KEYUP:
			switch (event.key.keysym.sym) 
            {
			case SDLK_LEFT:
			case SDLK_RIGHT:
//				move_dir = 0;
//				move_tick = 0;
				break;
			case SDLK_SPACE: 
			     exitloop=1;
//                 redraw=1;			     
//				move_dir = 1;
//				move_tick = 0;
				break;
			default:
				break;
			}

			default:
				break;

	} //END SWITCH EVENT TYPE SDL

if ( tablepos > 42 ) tablepos=5;

if ( tablepos < 5 ) tablepos=42;

      
      } // While are events SDL

  }//ISOLATE 1


 }//While exitloop
 return 0;
 
}//saveSnapGUI 

int menu_eval_snap (int cur_menupos){
    
                   int flag=0;                
                 switch (cur_menupos)
                 {
                  case 0:  //1 Load snap
                               {
                               char snap_filename[_MAX_PATH];
                               fileloader(snap_filename, ".sna");   // No verificamos errores¿¿?¿?¿?¿? :(
                               cur_menupos = 0;
                               draw_menupos = 0;
                               char snappath[_MAX_PATH + 1];
                               strcpy(snappath, snap_filename);
                               if(!snapshot_load(snappath)) 
                                  {
                                     splitPathFileName(snappath, snappath, snap_filename);
                                     strcpy(CPC.snap_path, snappath);
                                     strcpy(CPC.snap_file, snap_filename);
                                     CPC.snap_zip = 0;
                                     have_SNA = true;
//                                     printf("Cargando: \"%s\"\n", CPC.snap_file);
                                  }

                               redraw=1;
                               flag =1;
                               }
                               
                               break;
                               
                case 1:  //1 Save snap
                               {
                               saveSnapGUI();
    
                                      /*
                               char snap_filename[_MAX_PATH];
                               fileloader(snap_filename, ".sna");   // No verificamos errores¿¿?¿?¿?¿? :(
                               cur_menupos = 0;
                               draw_menupos = 0;
                               char snappath[_MAX_PATH + 1];
                               strcpy(snappath, snap_filename);
                               printf("PREVIO CARGA SNAP: \n");
                               if(!snapshot_load(snappath)) 
                                  {
                                     splitPathFileName(snappath, snappath, snap_filename);
                                     strcpy(CPC.snap_path, snappath);
                                     strcpy(CPC.snap_file, snap_filename);
                                     CPC.snap_zip = 0;
                                     have_SNA = true;
                                     printf("Cargando: \"%s\"\n", CPC.snap_file);
                                  }
                               printf("CARGADO SNAP: \n");

                               redraw=1;
                               //flag =1;
                               */
                               }
                               
                               break;


                 };
                 
return flag;

}


int menu_eval_disc (int cur_menupos){
    
                   int flag=0;                
                 switch (cur_menupos)
                 {
                  case 0:  //1 Load disc
                               {
                               char disc_filename[_MAX_PATH];
                               fileloader(disc_filename, ".dsk");   // No verificamos errores¿¿?¿?¿?¿? :(
                               cur_menupos = 0;
                               draw_menupos = 0;
                               char discpath[_MAX_PATH + 1];
                               strcpy(discpath, disc_filename);
                               if(!dsk_load(discpath, &driveA, 'A')) 
                                  {
                                     splitPathFileName(discpath, discpath, disc_filename);
                                     strcpy(CPC.drvA_path, discpath);
                                     strcpy(CPC.drvA_file, disc_filename);
                                     CPC.drvA_zip = 0;
                                     have_DSK = true;
//                                     printf("Cargando: \"%s\"\n", CPC.drvA_file);
                                  }

                               redraw=1;
                               //flag =1;
                               }
                               break;
                               

               case 1:  //2 Autotype
                           if (!option_menudisc[2])
                           {
                              AutoType_SetString("CAT\n\r", FALSE);
                           }
                           else
                           {
                                 AutoType_Init();
                                           //AutoType_SetString("\0", TRUE);
                           }                                 
                               option_menudisc[2]=!option_menudisc[2];
            //                   redraw=1;
                               flag =1;
                               break;
               case 2:  //2 Autotype TEMPORAL RUN"DISC
                           if (!option_menudisc[3])
                           {
                           AutoType_SetString("RUN\"DISC\n\r", FALSE);
                           }
                           else
                           {
                                 AutoType_Init();
                                           //AutoType_SetString("\0", TRUE);
                           }                                 
                               option_menudisc[3]=!option_menudisc[3];
            //                   redraw=1;
                               flag =1;
                               break;

               case 3:  //2 Autotype TEMPORAL RUN"DISK
                           if (!option_menudisc[4])
                           {
                           AutoType_SetString("RUN\"DISK\n\r", FALSE);
                           }
                           else
                           {
                                 AutoType_Init();
                                           //AutoType_SetString("\0", TRUE);
                           }                                 
                               option_menudisc[4]=!option_menudisc[4];
            //                   redraw=1;
                               flag =1;
                               break;
               case 4:  //2 Autotype TEMPORAL |CPM
                           if (!option_menudisc[5])
                           {
                           AutoType_SetString("|CPM\n\r", FALSE);
                           }
                           else
                           {
                                 AutoType_Init();
                                           //AutoType_SetString("\0", TRUE);
                           }                                 
                               option_menudisc[5]=!option_menudisc[5];
            //                   redraw=1;
                               flag =1;
                               break;



                 };
                 
return flag;

}

int carga_settingsconfig();   // predefinition


int menu_eval_config (int cur_menupos){
    
                   int flag=0;     

                 switch (cur_menupos)
                 {
                  case 0:  //1 audio
                       option_menuconfig[1]=!option_menuconfig[1];
                       break;
                       
                  case 1:  //2 frameskip
                       option_menuconfig[2]++;
                       if (option_menuconfig[2] > 2 ) option_menuconfig[2]=0;
                       break;
                       
                  case 2:  //3 speed
                       break;
                       
                  case 3:  //4 model
                       carga_settingsconfig();   // :(
                           flag =1;
                       break;

                  case 4:  //5 reset
                           needreset=1;
                           flag =1;
                       break;
                       
                  
                 };
                 
return flag;

}

int menu_eval_settings (int cur_settingspos){
    
                   int flag=0;     

                 switch (cur_settingspos)
                 {
                  case 0:  //1 model
                       option_settingsconfig[1]++;
                       if (option_settingsconfig[1] > 2 ) option_settingsconfig[1]=0;
                       break;


                  case 1:  //2 memory
                       if (option_settingsconfig[2]==0)
                              {option_settingsconfig[2]=64;
                              }else{
                               option_settingsconfig[2]=option_settingsconfig[2]*2;
                               if (option_settingsconfig[2]>512) option_settingsconfig[2]=0;
                               if ((option_settingsconfig[1]==2) && (option_settingsconfig[2]==0) ) option_settingsconfig[2]=64;
                               }
                       break;

                  case 2:  //3 COLOR
                       option_settingsconfig[3]=!option_settingsconfig[3];
                       break;
                       
                  
                 };
                 
return flag;

}





//EVALUATION MAIN LOOP


int eventloop_menu(int menu_type) {
	SDL_Event event;
	int make_option=0;
	int flag = 0;
	static int move_dir = 0;
	move_dir = 0;

	while(SDL_PollEvent(&event)) 
	{
	switch(event.type) 
		{

//////////////

		case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
				move_dir = 0;
				pcjoy_update (event);
				if ( (pcjoy_pressed (PC_JOY1_UP)) || (pcjoy_pressed (PC_JOY2_UP))) 
				{		//Action UP
					cur_menupos--;
					redraw=1;
					move_dir = -1;
					break;
				};
				if ( (pcjoy_pressed (PC_JOY1_DOWN)) || (pcjoy_pressed (PC_JOY2_DOWN))) 
				{		//Action DOWN
					cur_menupos++;
					redraw=1;			     
					move_dir = 1;
					break;
				};
                  		if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
				{		//Action LEFT
					flag = 1;
				};
				if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
				{		//Action RIGHT
					redraw=1;
					make_option=1;
					move_dir = 0;
					break;
				};

				if (event.type == SDL_JOYBUTTONDOWN )
				{
				 switch (event.jbutton.button)
					{
					case SDL_JoyFire1:	//Accion seleccionar
						redraw=1;
						make_option=1;
						move_dir = 0;
						break;
					case SDL_JoyFire4:	// Accion salir ;
						flag = 2;
						break;
					}
				}
         			break;

///////////////

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_UP:
					cur_menupos--;
					redraw=1;
					move_dir = -1;
					break;
				case SDLK_DOWN:
					cur_menupos++;
					redraw=1;
					move_dir = 1;
					break;
				case SDLK_RETURN:
				case SDLK_RIGHT:
					redraw=1;
					make_option=1;
					move_dir = 0;
					break;
				case SDLK_LEFT:
					flag = 1;
					break;
				case SDLK_ESCAPE:
				case 'q':
					flag = 2;
					break;
/*				case 'z':
					prevskin();
					break;
				case 'x':
					nextskin();
					break;*/
				default:
					break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_DOWN:
					move_dir = 0;
					break;
				default:
					break;
				}
			break;
		}



 	if (make_option !=0)
	{
 	make_option=0;

                 switch (menu_type){
                        case MENU_DISK:
                                  flag=menu_eval_disc(cur_menupos);
                                  redraw=1;
                                  break;
                        case MENU_TAPE:
                                  flag=menu_eval_tape(cur_menupos);
                                  redraw=1;
                                  break;
                        case MENU_SNAP:
                                  flag=menu_eval_snap(cur_menupos);
                                  redraw=1;
                                  break;
                        case MENU_OPT:
                                  flag=menu_eval_config(cur_menupos);
                                  redraw=1;
                                  break;
                        case MENU_SET:
                                  flag=menu_eval_settings(cur_menupos);
                                  redraw=1;
                                  break;
                                  }
             }

	}


			if(cur_menupos >= maxopt_menucurrent) cur_menupos =  0;
			if(cur_menupos < 0) cur_menupos = maxopt_menucurrent-1;
	return flag;
}




// CUSTOM MENUS

void draw_menutape(void) {
	int y = 50;
	int i;
	int color_menu;
	SDL_Rect dstrect;
	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 70, 25 ,"TAPE MANAGEMENT", getfontcolor(250, 250, 0)); 			
	opt_menutape=1;
	for(i=0;(opt_menutape<=maxopt_menutape) && (y < 250);i++, y+=25)
    {
		if (i == cur_menupos) {color_menu=0;} else {color_menu=250;};		
		displaytext( 50, y ,text_menutape[opt_menutape].text, getfontcolor(250, color_menu, 0)); 
        char tmpstr[6]; //AQUI
        sprintf(tmpstr,"%d",option_menutape[opt_menutape]);

        if ((opt_menutape == 2) or (opt_menutape == 6) )
              {switch (option_menutape[opt_menutape]) 
                      {
                      case 0:
                          displaytext( 230, y ,"OFF", getfontcolor(250, color_menu, 0));
                          break;
                      case 1:
                          displaytext( 230, y ,"ON", getfontcolor(250, color_menu, 0));
                          break;
                      }
               }



        if (opt_menutape == 3 )
              {switch (option_menutape[3]) 
                      {
                      case 0:
                          displaytext( 230, y ,"STOPPED", getfontcolor(250, color_menu, 0));
                          break;
                      default:
                          displaytext( 230, y ,"PLAYING", getfontcolor(250, color_menu, 0));
                          break;
                      }
               }
                
		dstrect.y = y;

		opt_menutape++;
	}   
    SDL_UpdateRect(montaje, 0,0,0,0);
}


int carga_menutape( )
{

int flag;
// char *path;
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  SDL_UpdateRect(montaje, 0,0,0,0);
	//strcpy(path, "./snap/");
	//getDir(path, 0);
    needreset=0;
	option_menutape[1]=0;
	option_menutape[2]=0;
	option_menutape[3]=(CPC.tape_play_button != 0);
	option_menutape[4]=0;
	option_menutape[5]=0;
	option_menutape[6]=cpc_tapeturbo;
    strcpy(text_menutape[1].text,"Load Tape");
    strcpy(text_menutape[2].text,"Type RUN\"");
    strcpy(text_menutape[3].text,"Play Tape");
    strcpy(text_menutape[4].text,"Rewind Tape");
    strcpy(text_menutape[5].text,"Eject Tape");
    strcpy(text_menutape[6].text,"Tape Turbo");

    maxopt_menucurrent=maxopt_menutape;
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_TAPE)) == 0) {
		if(redraw) {
			draw_menutape();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 1) {  //???
                  if (needreset==1){
                     emulator_reset(false);
                  }

		return 1;
	}//flag=1



	return 0;
}


//SNAPSHOT MAIN MENU



void draw_menusnap(void) {
	int y = 50;
	int i;
	int color_menu;
	SDL_Rect dstrect;
	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 70, 25 ,"SNAP MANAGEMENT", getfontcolor(250, 250, 0)); 			
	opt_menusnap=1;
	for(i=draw_menupos;(opt_menusnap<=maxopt_menusnap) && (y < 250);i++, y+=25)
    {
		if (i == cur_menupos) {color_menu=0;} else {color_menu=250;};		
		displaytext( 50, y ,text_menusnap[opt_menusnap].text, getfontcolor(250, color_menu, 0)); 
        // char tmpstr[6]; AQUI POR AHORA NO HACE FALTA
        //      sprintf(tmpstr,"%d",option_menusnap[opt_menusnap]);

              
		dstrect.y = y;

		opt_menusnap++;
	}   
    SDL_UpdateRect(montaje, 0,0,0,0);
}


int carga_menusnap( )
{

int flag;
// char *path;
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  SDL_UpdateRect(montaje, 0,0,0,0);
	//strcpy(path, "./snap/");
	//getDir(path, 0);
    needreset=0;
	option_menusnap[1]=0;
	option_menusnap[2]=0;
	option_menusnap[3]=0;
	option_menusnap[4]=0;
	option_menusnap[5]=0;
	option_menusnap[6]=0;
    strcpy(text_menusnap[1].text,"Load Snapshot");
    strcpy(text_menusnap[2].text,"Save Snapshot");
    strcpy(text_menusnap[3].text,"--------");
    strcpy(text_menusnap[4].text,"--------");
    strcpy(text_menusnap[5].text,"--------");
    strcpy(text_menusnap[6].text,"--------");

    maxopt_menucurrent=maxopt_menusnap;            
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_SNAP)) == 0) {
		if(redraw) {
			draw_menusnap();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 1) {  //???
                  if (needreset==1){
                     emulator_reset(false);
                  }

		return 1;
	}//flag=1



	return 0;
}





// DISC MAIN MENU


void draw_menudisc(void) {
	int y = 50;
	int i;
	int color_menu;
	SDL_Rect dstrect;
	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 70, 25 ,"DISC MANAGEMENT", getfontcolor(250, 250, 0)); 			
	opt_menudisc=1;
	for(i=draw_menupos;(opt_menudisc<=maxopt_menudisc) && (y < 250);i++, y+=25)
    {
		if (i == cur_menupos) {color_menu=0;} else {color_menu=250;};		
		displaytext( 50, y ,text_menudisc[opt_menudisc].text, getfontcolor(250, color_menu, 0)); 
        // char tmpstr[6]; AQUI POR AHORA NO HACE FALTA
        //      sprintf(tmpstr,"%d",option_menudisc[opt_menudisc]);

        if ((opt_menudisc == 2)  )
              {switch (option_menudisc[opt_menudisc]) 
                      {
                      case 0:
                          displaytext( 230, y ,"OFF", getfontcolor(250, color_menu, 0));
                          break;
                      case 1:
                          displaytext( 230, y ,"ON", getfontcolor(250, color_menu, 0));
                          break;
                      }
               }


               
		dstrect.y = y;

		opt_menudisc++;
	}   
    SDL_UpdateRect(montaje, 0,0,0,0);
}


int carga_menudisc( )
{

int flag;
// char *path;
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  SDL_UpdateRect(montaje, 0,0,0,0);
	//strcpy(path, "./snap/");
	//getDir(path, 0);
    needreset=0;
	option_menudisc[1]=0;
	option_menudisc[2]=0;
	option_menudisc[3]=0;
	option_menudisc[4]=0;
	option_menudisc[5]=0;
	option_menudisc[6]=0;
    strcpy(text_menudisc[1].text,"Load Disc");
    strcpy(text_menudisc[2].text,"Type CAT");
    strcpy(text_menudisc[3].text,"Type RUN\"DISC");
    strcpy(text_menudisc[4].text,"Type RUN\"DISK");
    strcpy(text_menudisc[5].text,"Type |CPM");
    strcpy(text_menudisc[6].text,"--------");

    maxopt_menucurrent=maxopt_menudisc;            
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_DISK)) == 0) {
		if(redraw) {
			draw_menudisc();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 1) {  //???
                  if (needreset==1){
                     emulator_reset(false);
                  }

		return 1;
	}//flag=1



	return 0;
}



// MENU CONFIG

void draw_menuconfig(void) {
	int y = 50;
	int i;
	SDL_Rect dstrect;
	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 105, 25 ,"CAPRICE MENU", getfontcolor(250, 250, 0)); 			
	opt_menuconfig=1;
	for(i=draw_menupos;(opt_menuconfig<=maxopt_menuconfig) && (y < 250);i++, y+=25){
		int color_menu =0;
        if(i == cur_menupos) {color_menu=0;} else {color_menu=250;};
        displaytext( 50, y ,text_menuconfig[opt_menuconfig].text, getfontcolor(250, color_menu, 0)); 
        char tmpstr[6];
        sprintf(tmpstr,"%d",option_menuconfig[opt_menuconfig]);
        switch (opt_menuconfig)
              {
                 case 1:
                       {switch (option_menuconfig[opt_menuconfig]) 
                             {
                        case 0:
                          displaytext( 250, y ,"OFF", getfontcolor(250, color_menu, 0));
                          break;
                        case 1:
                          displaytext( 250, y ,"ON", getfontcolor(250, color_menu, 0));
                          break;
                             }
                        }
                        break;
                 case 2:
                 case 3:
                        displaytext( 250, y ,tmpstr, getfontcolor(250, color_menu, 0));
                        break;
                 case 5:
                 case 4:
                 case 6:                    
                 default:
                         break;   

              }

		dstrect.y = y;

		opt_menuconfig++;
	}   SDL_UpdateRect(montaje, 0,0,0,0);
}



int carga_menuconfig( )
{

int flag;
// char *path;
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  SDL_UpdateRect(montaje, 0,0,0,0);
	//strcpy(path, "./snap/");
	//getDir(path, 0);
    needreset=0;
	option_menuconfig[1]=CPC.snd_enabled;
	option_menuconfig[2]=CPC.scr_fskip;
	option_menuconfig[3]=200;  //FAKEspeed(c) for now
	option_menuconfig[4]=CPC.model;
	option_menuconfig[5]=!CPC.scr_tube;
	option_menuconfig[6]=0;
    strcpy(text_menuconfig[1].text,"Audio");
    strcpy(text_menuconfig[2].text,"Frameskip");
    strcpy(text_menuconfig[3].text,"GP2x Speed (fake)");
    strcpy(text_menuconfig[4].text,"CPC Settings    >>>");
    strcpy(text_menuconfig[5].text,"Reset CPC");
    strcpy(text_menuconfig[6].text,"------------");

    maxopt_menucurrent=maxopt_menuconfig;
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_OPT)) == 0) {
		if(redraw) {
			draw_menuconfig();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 1) {  //???

	CPC.snd_enabled=option_menuconfig[1];
	CPC.scr_fskip=option_menuconfig[2];
//	option_menuconfig[3]=200;  //FAKEspeed(c) for now
//	CPC.model=option_menuconfig[4];
//	CPC.scr_tube=!option_menuconfig[5];
//	video_set_palette();
//	CPC.ram_size=option_menuconfig[5];
if (needreset==1){
                  emulator_reset(false);
                  }

		return 1;
	}//flag=1


	return 0;
}


//___________________________________________
// SETTINGS CONFIG

void draw_settingsconfig(void) {
	int y = 50;
	int i;
	SDL_Rect dstrect;
	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 105, 25 ,"CPC SETTINGS", getfontcolor(250, 250, 0)); 			
	opt_settingsconfig=1;
	for(i=draw_menupos;(opt_settingsconfig<=maxopt_settingsconfig) && (y < 250);i++, y+=25){
		int color_menu =0;
        if(i == cur_menupos) {color_menu=0;} else {color_menu=250;};
        displaytext( 50, y ,text_settingsconfig[opt_settingsconfig].text, getfontcolor(250, color_menu, 0)); 
        char tmpstr[6];
        sprintf(tmpstr,"%d",option_settingsconfig[opt_settingsconfig]);
        switch (opt_settingsconfig)
              {
                 case 1:
                       {switch (option_settingsconfig[opt_settingsconfig]) {
                          case 0:
                              displaytext( 250, y ,"464", getfontcolor(250, color_menu, 0));
                               break;
                          case 1:
                              displaytext( 250, y ,"664", getfontcolor(250, color_menu, 0));
                               break;                               
                          case 2:
                              displaytext( 250, y ,"6128", getfontcolor(250, color_menu, 0));
                               break;
                          }
                       }
                       break;

                 case 3:
                       {switch (option_settingsconfig[opt_settingsconfig]) 
                             {
                        case 0:
                          displaytext( 250, y ,"OFF", getfontcolor(250, color_menu, 0));
                          break;
                        case 1:
                          displaytext( 250, y ,"ON", getfontcolor(250, color_menu, 0));
                          break;
                             }
                        }
                        break;
                 case 2:
                        sprintf(tmpstr,"%d",option_settingsconfig[opt_settingsconfig]+64);
                        displaytext( 250, y ,tmpstr, getfontcolor(250, color_menu, 0));
                        break;
                 case 5:
                 case 4:
                 default:
                         break;   

              }

		dstrect.y = y;

		opt_settingsconfig++;
	}   SDL_UpdateRect(montaje, 0,0,0,0);
}



int carga_settingsconfig( )
{

int flag;
// char *path;
SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  SDL_UpdateRect(montaje, 0,0,0,0);
	//strcpy(path, "./snap/");
	//getDir(path, 0);
    needreset=0;
	option_settingsconfig[1]=CPC.model;
	option_settingsconfig[2]=CPC.ram_size-64;
	option_settingsconfig[3]=!CPC.scr_tube;
	option_settingsconfig[4]=0;
	option_settingsconfig[5]=0;
	option_settingsconfig[6]=0;
    strcpy(text_settingsconfig[1].text,"CPC Model");
    strcpy(text_settingsconfig[2].text,"Memory");
    strcpy(text_settingsconfig[3].text,"Color monitor");
    strcpy(text_settingsconfig[4].text,"-------------");
    strcpy(text_settingsconfig[5].text,"-------------");
    strcpy(text_settingsconfig[6].text,"-------------");

    maxopt_menucurrent=maxopt_settingsconfig;
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_SET)) == 0) {
		if(redraw) {
			draw_settingsconfig();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag > 0) {  //???

	if (CPC.model != option_settingsconfig[1]) needreset=1;
	if (CPC.ram_size != option_settingsconfig[2]) needreset=1;

    CPC.model=option_settingsconfig[1];
	CPC.ram_size=option_settingsconfig[2]+64;
	CPC.scr_tube=!option_settingsconfig[3];
	video_set_palette();
//	option_settingsconfig[3]=200;  //FAKEspeed(c) for now
//	CPC.model=option_settingsconfig[4];
//	CPC.ram_size=option_settingsconfig[5];
if (needreset==1){

			emulator_shutdown();
			//emulator_reset(false);
			emulator_init();


                  }


		return 1;
	}//flag=1


	return 0;
}


//___________________________________________

/*
================================================================================================================
                                              ICON MENU
================================================================================================================
*/

void intro_cap(bool splash)
{
if (splash)
{
   SDL_BlitSurface(portada, NULL, montaje, NULL);
   displaytext( 15, 160 ,NOTE_STRING, getfontcolor(200, 200 , 200));
   displaytext( 15, 180 ,VERSION_STRING, getfontcolor(200, 200 , 200));
   displaytext( 15, 200 ,AUTOR_STRING, getfontcolor(200, 200 , 200));

menu_blit();
SDL_Delay(2000);     
}
}


void dibujamenu ()
{

SDL_Rect menubase,Offset; 
   SDL_BlitSurface(portada, NULL, montaje, NULL);
//#ifndef GP2X
//   SDL_UpdateRects(video_surface, 1, &video_rect);
//#endif   
   
Offset.x = 26; 
Offset.y = 190; //190 a frame completo!!
menubase.x=26;
menubase.y=43;
menubase.w=300;
menubase.h=36;

SDL_BlitSurface( menu, &menubase, montaje, &Offset ); 
   //SDL_UpdateRects(video_surface, 1, &Offset); //TROLOLO1
//menu_blit(); will blit at dibujaopcion() ;)   
}

void dibujaopcion (int opci)
{

SDL_Rect menuopcion,Offset; 
Offset.x = 26+(56*opci); 
Offset.y = 190; //190 a frame completo!!

menuopcion.x=26+(56*opci);
menuopcion.y=7;
menuopcion.w=35;
menuopcion.h=35;

//Emu's screen already blited from dibujamenu()
SDL_BlitSurface( menu, &menuopcion, montaje, &Offset ); 
//   SDL_UpdateRects(video_surface, 1, &Offset);
menu_blit();
}

void icomenu_vis (bool pausescreen)
{
            if (!vismenu) 
               {
               vismenu=1;
               CPC.paused=1;
               audio_pause();
               if (pausescreen)
               {
               SDL_BlitSurface(video_surface, NULL, portada, NULL); // Recicle the intro surface into pause surface
               SDL_UpdateRect(portada, 0, 0,320,240); 
               }
}
      //         }else
      //         {vismenu=0; CPC.paused=0;audio_resume();}

}


#define ICOMENU_EXIT		          1
#define ICOMENU_LEFT              2
#define ICOMENU_RIGHT             3
#define ICOMENU_RUN               4

void icomenu_eval (SDL_Event icoevent)
{
     int sele=0;
     switch (icoevent.type)
            {
							//////////////

    case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

                  pcjoy_update (icoevent);
                  if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
												{		//Action LEFT
                             sele=ICOMENU_LEFT;

												};
									if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
												{		//Action RIGHT
                             sele=ICOMENU_RIGHT;

													break;

												};
					
							if (icoevent.type == SDL_JOYBUTTONDOWN )		
							{		
							 switch (icoevent.jbutton.button)  	
									{	 
									case SDL_JoyFire1:
											//Accion seleccionar
                             sele=ICOMENU_RUN;

											break;
									case SDL_JoyFire4:
											// Accion salir ;
                             sele=ICOMENU_EXIT;
											break;
									}
							}								
         break;

///////////////


							
							
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                 switch ((int)icoevent.key.keysym.sym)
                        {
                        case CAP32_OPTIONS:
                             sele=ICOMENU_EXIT;
                             break;
                        case SDLK_LEFT:
                             sele=ICOMENU_LEFT;
                             break;
                        case SDLK_RIGHT:
                             sele=ICOMENU_RIGHT;
                             break;
                        case SDLK_RETURN:
                             sele=ICOMENU_RUN;
                             break;
                        }                                                 
                 break;
            }                        
                                                 
            switch (sele)
            {
                   case ICOMENU_EXIT:
                        vismenu=0; 
                        CPC.paused=0;
                        audio_resume(); 
                        break;
                   case ICOMENU_LEFT:
                        nopcion--;break;
                   case ICOMENU_RIGHT:
                        nopcion++;
                        break;
                   case ICOMENU_RUN: 
                        {
                                     switch (nopcion)
                                          {
                                          case 0:
                                                carga_menudisc();   // :(
                                                vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 1:
                                                carga_menutape();            // :(
                                                vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 2:
                                               carga_menusnap();            // :(
                                                vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 3:
                                                carga_menuconfig();   // :(
                                                vismenu =0;
                                                CPC.paused=0;
                                                //audio_pause();
                                                audio_resume(); 
                                              break;
                                          case 4: //carga_menuexit();   //:(
                                                  emulatorend=1; //Deberia ser carga_menuexit();para reset o exit
                                                  break;
                                          default:
                                                  break;
                                          }
                     
                     }  

                     }               
}






/*
================================================================================================================
                                              IMAGENES E INICIALIZACION
================================================================================================================
*/



SDL_Surface *load_image_include( const unsigned char *image_array, const long int image_array_size ) 
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = SDL_LoadBMP_RW(SDL_RWFromConstMem (image_array,image_array_size),1);//SDL_LoadBMP( filename.c_str() );

    if( loadedImage != NULL )
    {
        optimizedImage = SDL_DisplayFormat( loadedImage );
        SDL_FreeSurface( loadedImage );
    }
    else
    		printf("Image error: from memory\n" );
//    		printf("Image error: %s", filename.c_str() );

    return optimizedImage;


}





void set_transpa (SDL_Surface *source )
{
     SDL_SetColorKey(source,SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(menu->format,236,0,140));
}


unsigned int WhichPI()
{
 FILE * filp;
 unsigned rev;
 char buf[512];
 char term;

 rev = 0;

 filp = fopen ("/proc/cpuinfo", "r");

 if (filp != NULL)
 {
 	while (fgets(buf, sizeof(buf), filp) != NULL)
 	{
 	if (!strncasecmp("revision\t", buf, 9))
 	{
 		if (sscanf(buf+strlen(buf)-5, "%x%c", &rev, &term) == 2)
 		{
 		if (term == '\n') break;
 		rev = 0;
 		}
 	}
 	}
 	fclose(filp);
 }
 return rev;
}



void menu_init()
{
//SDL_Surface *menu,*fich,*zipo,*carga,*carpa = NULL;


menu = load_image_include(menucpc_bmp, menucpc_bmp_size);// "gui/menucpc.bmp");
	if (menu == NULL)
		printf("Menu error: %s", SDL_GetError());
portada= load_image_include(portada_bmp,portada_bmp_size );//"gui/portada.bmp"); 
	if (portada == NULL)
		printf("Menu error: %s", SDL_GetError());
montaje= load_image_include( back_bmp,back_bmp_size );//"gui/back.bmp"); //Dirty trick to gain a backbuffer equal to the background!!
	if (montaje == NULL)
		printf("Menu error: %s", SDL_GetError());
fich = load_image_include( game_bmp,game_bmp_size );//"gui/game.bmp");
	if (fich == NULL)
		printf("Menu error: %s", SDL_GetError());
disca = load_image_include( disc_bmp,disc_bmp_size );//"gui/disc.bmp");
	if (disca == NULL)
		printf("Menu error: %s", SDL_GetError());
tapa = load_image_include(tape_bmp,tape_bmp_size );// "gui/tape.bmp");
	if (tapa == NULL)
		printf("Menu error: %s", SDL_GetError());
snapa = load_image_include( snap_bmp,snap_bmp_size );//"gui/snap.bmp");
	if (snapa == NULL)
		printf("Menu error: %s", SDL_GetError());
zipo = load_image_include( zipped_bmp,zipped_bmp_size );//"gui/zipped.bmp");
	if (zipo == NULL)
		printf("Menu error: %s", SDL_GetError());
carpa = load_image_include( folder_bmp,folder_bmp_size );//"gui/folder.bmp");
	if (carpa == NULL)
		printf("Menu error: %s", SDL_GetError());
carga = load_image_include( loading_bmp,loading_bmp_size );//"gui/loading.bmp");
	if (carga == NULL)
		printf("Menu error: %s", SDL_GetError());
fondo = load_image_include( back_bmp,back_bmp_size );//"gui/back.bmp");
	if (fondo == NULL)
		printf("Menu error: %s", SDL_GetError());


//set_transpa(fondo);
set_transpa(menu);
set_transpa(snapa);
set_transpa(disca);
set_transpa(tapa);
set_transpa(fich);
set_transpa(zipo);
set_transpa(carpa);
set_transpa(carga);
setupfonts();
//list = SDL_CreateRGBSurface(0, 320, 240, 16,
//		0xF800, 0x07E0,	0x001F, 0x0000);
//SDL_SetColorKey(list, SDL_SRCCOLORKEY, LIST_COLORKEY);
}





void menu_shutdown()
{
shutdownfonts();
//Free the images 

	if (portada != NULL) SDL_FreeSurface( portada ); 

	if (snapa != NULL) SDL_FreeSurface( snapa ); 

	if (disca != NULL) SDL_FreeSurface( disca);

	if (tapa != NULL) SDL_FreeSurface( tapa );

	if (fich != NULL) SDL_FreeSurface( fich ); 

	if (zipo != NULL) SDL_FreeSurface( zipo ); 

	if (carga != NULL) SDL_FreeSurface( carga ); 

	if (carpa != NULL) SDL_FreeSurface( carpa); 

	if (fondo != NULL) SDL_FreeSurface( fondo); 

	if (montaje != NULL) SDL_FreeSurface( montaje); 

//	if (montaje_zoom != NULL) SDL_FreeSurface( montaje_zoom); 

	if (carga != NULL) SDL_FreeSurface( menu ); 


//SDL_FreeSurface( list);


 
}



/*

Pasar por las opciones
            switch (event.type)
            {
            case SDL_KEYDOWN:
                 if( event.key.keysym.sym == SDLK_UP ) {jarl=(!(jarl));break;}
                 if( event.key.keysym.sym == SDLK_LEFT ) {nopcion--;break;}
                 if( event.key.keysym.sym == SDLK_RIGHT ) {nopcion++;break;}                 
                //break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
                break;
            }



        if ( vismenu ) 
        {
             
             if  (nopcion<0) nopcion=0;
             if  (nopcion>4) nopcion=4;
             dibujamenu (); 
             dibujaopcion(nopcion);
        }


shutdownfonts(); //Apagar fuentes

*/


/* _________________________________________________________________________
                      Keyboard
*/


int vkey_pressed,vkey_shift_pressed,vkey_control_pressed=0;


void load_keyboard()
{
    	SDL_Surface* tmpImage = NULL;

	//keyboard_surface
	montaje_zoom = load_image_include( key_r_bmp,key_r_bmp_size);

	if (montaje_zoom == NULL)
		printf("Keyboard error: Create virtual surface %s", SDL_GetError());

	tmpImage = zoomSurface(montaje_zoom,dwXScale,dwYScale,0);
//	printf("-Key1-X:%i  Y:%i\n",dwXScale,dwYScale);
	keyboard_surface = SDL_DisplayFormat( tmpImage );


   	if (montaje_zoom != NULL) SDL_FreeSurface( montaje_zoom); 
   	if (tmpImage != NULL) SDL_FreeSurface( tmpImage); 

	//keyboard_no_pressed
	montaje_zoom = load_image_include( key_r_bmp,key_r_bmp_size);

	if (montaje_zoom == NULL)
		printf("Keyboard error: Normal keys bmp %s", SDL_GetError());

	tmpImage = zoomSurface(montaje_zoom,dwXScale,dwYScale,0);
	key_normal = SDL_DisplayFormat( tmpImage );


   	if (montaje_zoom != NULL) SDL_FreeSurface( montaje_zoom); 
   	if (tmpImage != NULL) SDL_FreeSurface( tmpImage); 

	//keyboard_pressed
	montaje_zoom = load_image_include( key_p_bmp,key_p_bmp_size);
	if (montaje_zoom == NULL)
		printf("Keyboard error: Pressed keys bmp %s", SDL_GetError());

	tmpImage = zoomSurface(montaje_zoom,dwXScale,dwYScale,0);
	key_press = SDL_DisplayFormat( tmpImage );

   	if (montaje_zoom != NULL) SDL_FreeSurface( montaje_zoom); 
   	if (tmpImage != NULL) SDL_FreeSurface( tmpImage); 

}



void unload_keyboard()
{

//Free the images 
SDL_FreeSurface( key_press ); 
SDL_FreeSurface( key_normal ); 
SDL_FreeSurface( keyboard_surface); 
 
}

void    blit_pressed_key(int k_row, int k_col, int off_x, int off_y)
{
	SDL_Rect Offset,keybmp;
	keybmp.x=((k_col*17)+virtualkeys[k_row][k_col].xdesp)*dwXScale;
	keybmp.y=k_row*17*dwYScale;
	Offset.w=keybmp.w=(virtualkeys[k_row][k_col].xlong)*dwXScale;
	Offset.h=keybmp.h=18*dwYScale;

	Offset.x=keybmp.x+off_x;
	Offset.y=keybmp.y+off_y;

	 SDL_BlitSurface(key_press, &keybmp, keyboard_surface, &Offset);

}



void show_keys()
{
	int boardoffset_x=0;//7;
	int boardoffset_y=0;//(keyboard_pos*23)+7;

	SDL_Rect Offset;
	Offset.x = boardoffset_x;
	Offset.y = boardoffset_y;

 	SDL_BlitSurface(key_normal, NULL, keyboard_surface, &Offset);

     	//show KEY
    	blit_pressed_key(keyboard_row, keyboard_col, boardoffset_x, boardoffset_y);

    	if (vkey_shift_pressed) blit_pressed_key(3, 0, boardoffset_x, boardoffset_y);
    	if (vkey_control_pressed) blit_pressed_key(4, 0, boardoffset_x, boardoffset_y);
    	if ((keyboard_col==13) && ( (keyboard_row==1) || (keyboard_row==2) ) ) //Is return key selected????
       		{blit_pressed_key(1, 13, boardoffset_x, boardoffset_y);
        	blit_pressed_key(2, 13, boardoffset_x, boardoffset_y);
       		}
  	montaje_region.x=(back_surface->w-keyboard_surface->w)/2;
  	montaje_region.y=(30+(video_surface->h-keyboard_surface->h-26)*keyboard_pos);
  	montaje_region.w=montaje_zoom->w;
  	montaje_region.h=montaje_zoom->h;

 	SDL_BlitSurface(keyboard_surface, NULL, back_surface, &montaje_region);  //180-240
}

enum {
	VKEY_NONE,
	VKEY_LEFT,
	VKEY_RIGHT,
	VKEY_UP,
	VKEY_DOWN,
};



EXTERN_C void  cpc_key_press (int cpc_keypress);

EXTERN_C void  cpc_key_unpress (int cpc_keypress);

void eval_keyboard(SDL_Event kevent)
{

int vkey_move=VKEY_NONE;

   switch (kevent.type) {

  
//KEYS

//////JOY START

    case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

		pcjoy_update (kevent);

		if ( (pcjoy_pressed (PC_JOY1_UP)) || (pcjoy_pressed (PC_JOY2_UP))) 
			{		//Action UP
                        if (!(vkey_pressed)) vkey_move=VKEY_UP ;

			break;
			};
		if ( (pcjoy_pressed (PC_JOY1_DOWN)) || (pcjoy_pressed (PC_JOY2_DOWN))) 
			{		//Action DOWN
                        if (!(vkey_pressed)) vkey_move=VKEY_DOWN ;

			break;
			};
		if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
			{		//Action LEFT
                        if (!(vkey_pressed)) vkey_move=VKEY_LEFT ;

			break;
			};
		if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
			{		//Action RIGHT
                        if (!(vkey_pressed)) vkey_move=VKEY_RIGHT ;

			break;
			};

		if (kevent.type == SDL_JOYBUTTONUP )
			{
				switch (kevent.jbutton.button)
				{
					case SDL_JoyFire1:
								cpc_key_unpress (virtualkeys[keyboard_row][keyboard_col].keyval);
								vkey_pressed=0;
								break;

					case SDL_JoyFire2: //SHIFT
								cpc_key_unpress (0x25);
								vkey_pressed=0;
								vkey_shift_pressed=0;
								break;

					case SDL_JoyFire3: //CTRL
								cpc_key_unpress (0x27);
								vkey_pressed=0;
								vkey_control_pressed=0;

								break;
/*					case SDL_JoyFire4:
								break;
					case SDL_JoyFire5:
								break;
					case SDL_JoyFire6:
								break;
*/
				}
			}

		if (kevent.type == SDL_JOYBUTTONDOWN )
			{
				switch (kevent.jbutton.button)
				{
					case SDL_JoyFire1:
	        		                                cpc_key_press (virtualkeys[keyboard_row][keyboard_col].keyval);
        			                                vkey_pressed=1;
								break;
					case SDL_JoyFire2: //SHIFT
								cpc_key_press (0x25);
								vkey_pressed=0;
								vkey_shift_pressed=1;

								break;
					case SDL_JoyFire3: //CONTROL
								cpc_key_press (0x27);
								vkey_pressed=0;
								vkey_control_pressed=1;
								break;
					case SDL_JoyFire4:
								keyboard_pos=!(keyboard_pos);

								break;
					case SDL_JoyFire5:
								keyboard_show=0;
								break;
/*					case SDL_JoyFire6:
								break;
*/
				}
			}




         		break;

//////JOY END




            case SDL_KEYUP:
               {
               switch ((int)kevent.key.keysym.sym) 
                     {
                     case SDLK_LCTRL:
                     case SDLK_RCTRL:
                          cpc_key_unpress (0x27);
                          //cpc_key_unpress (virtualkeys[keyboard_row][keyboard_col].keyval);
                          vkey_pressed=0;
                          vkey_control_pressed=0;
                          break;
                   
                     case SDLK_LSHIFT:
                     case SDLK_RSHIFT:
                          cpc_key_unpress (0x25);
                          //cpc_key_unpress (virtualkeys[keyboard_row][keyboard_col].keyval);
                          vkey_pressed=0;
                          vkey_shift_pressed=0;
                          break;

                     case SDLK_SPACE:
                          cpc_key_unpress (virtualkeys[keyboard_row][keyboard_col].keyval);
                          vkey_pressed=0;
                          break; 
                     }
               break;
               }


            case SDL_KEYDOWN:
               {
                     switch ((int)kevent.key.keysym.sym) 
                     {  
                     case SDLK_LCTRL:
                     case SDLK_RCTRL:
                             cpc_key_press (0x27);
                             //cpc_key_press (virtualkeys[keyboard_row][keyboard_col].keyval);
                             vkey_pressed=0;
                             vkey_control_pressed=1;
                             break;
                     case SDLK_LSHIFT:
                     case SDLK_RSHIFT:
                             cpc_key_press (0x25);
                             //cpc_key_press (virtualkeys[keyboard_row][keyboard_col].keyval);
                             vkey_pressed=0;
                             vkey_shift_pressed=1;
                             break;

                        case SDLK_SPACE:
                                        cpc_key_press (virtualkeys[keyboard_row][keyboard_col].keyval);
                                        vkey_pressed=1;
                             break;
                        case SDLK_LEFT:
                                       if (!(vkey_pressed)) vkey_move=VKEY_LEFT ;
                        		break;
                        case SDLK_RIGHT:
                                       if (!(vkey_pressed)) vkey_move=VKEY_RIGHT;
                        		break;
                        case SDLK_UP:
                                       if (!(vkey_pressed)) vkey_move=VKEY_UP ;
                        		break;
                        case SDLK_DOWN:
                                       if (!(vkey_pressed)) vkey_move=VKEY_DOWN;
                        		break;

                        case CAP32_VKEY: //F9
                           	 if (kevent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
                           	{
                           	   keyboard_pos=!(keyboard_pos);
                           	   } else {
                           	   keyboard_show=0;
                           	}
                           	break;
                        case CAP32_EXIT:
				emulatorend=1;
				break;

                     }
                  break; 
		       }
            case SDL_QUIT:
                 emulatorend=1;
         }
   // GENERAL MOVEMENT
         switch (vkey_move)
                {
                case VKEY_LEFT:
                     {
                     if (keyboard_col>0) keyboard_col--;
                     //EXCEPTION TABLE FOR LEFT
                     if (keyboard_row==4)
                        if ((keyboard_col>3 )&&(keyboard_col<15)) keyboard_col=3;
                     if (keyboard_row==3)
                        if ((keyboard_col>12 )&&(keyboard_col<15)) keyboard_col=12;  // 
                     if (keyboard_row==2)
                        if ((keyboard_col>13 )&&(keyboard_col<15)) keyboard_col=13;  //
                     if (keyboard_row==1)
                        if ((keyboard_col>13 )&&(keyboard_col<15)) keyboard_col=13;  //
                     }
                     break; 
                case VKEY_RIGHT:
                     {
                     if (keyboard_col<17) keyboard_col++;
                     //EXCEPTION TABLE FOR RIGHT 
                     if (keyboard_row==4)
                        if ((keyboard_col>3 )&&(keyboard_col<15)) keyboard_col=15;
                     if (keyboard_row==3)
                        if ((keyboard_col>12 )&&(keyboard_col<15)) keyboard_col=15;  // 
                     if (keyboard_row==2)
                        if ((keyboard_col>13 )&&(keyboard_col<15)) keyboard_col=15;  // 
                     if (keyboard_row==1)
                        if ((keyboard_col>13 )&&(keyboard_col<15)) keyboard_col=15;  //
                     }                                  
                     break;

                case VKEY_UP:
                     {
                     if (keyboard_row>0) keyboard_row--;
                     //EXCEPTION TABLE FOR RIGHT 
                      if (keyboard_row==3){
                         int notmoved=1;
                         if ((keyboard_col>2 )&&(keyboard_col<15)) {keyboard_col=12;notmoved=0;}
                         //if ((keyboard_col>2 )&&(keyboard_col<15)&& (notmoved)) {keyboard_col=12;notmoved=0;}
                         if ((keyboard_col==2) && (notmoved)) {keyboard_col=7;notmoved=0;}
                         if ((keyboard_col==1) && (notmoved)) {keyboard_col=2;notmoved=0;}

                         }
                      if (keyboard_row==2){
                         if ((keyboard_col>12 )&&(keyboard_col<15)) keyboard_col=12;
                         }
                     }         
                     break;
                case VKEY_DOWN:
                     {
                     if (keyboard_row<4) keyboard_row++;
                     //EXCEPTION TABLE FOR RIGHT 
                     if (keyboard_row==4){
                        int notmoved=1;
                        if ((keyboard_col>9 )&&(keyboard_col<15)) {keyboard_col=3;notmoved=0;}
                        if ((keyboard_col>3 )&&(keyboard_col<10) && (notmoved)) {keyboard_col=2;notmoved=0;}
                        if ((keyboard_col>0 )&&(keyboard_col<4) && (notmoved)){keyboard_col=1;notmoved=0;}
                        }

                     if (keyboard_row==3){
                        if ((keyboard_col>12 )&&(keyboard_col<15)) keyboard_col=12;
                        }
  
                       if (keyboard_row==1){
                        if ((keyboard_col>13 )&&(keyboard_col<15)) keyboard_col=13;
                        }
  
                     }         
                     break;
                case VKEY_NONE:
                default:
                     break;         

                }
         //exception vertical moves


}

void GUI_load()
{
             load_keyboard();   
             menu_init();
}

void GUI_unload()
{
             unload_keyboard();   
             menu_shutdown();
}

void GUI_reload()
{
             unload_keyboard();
             menu_shutdown();
             load_keyboard();   
             menu_init();
}
