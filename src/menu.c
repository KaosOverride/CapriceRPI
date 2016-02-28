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

extern int CPC_Joystate[8];
extern char CPC_Joynames[8][12];
extern int CPC_Joys [4];
extern void pcjoy_assign(int joy, int mode);

extern SDL_Surface *video_surface,*back_surface;

extern t_CPC CPC;
extern t_drive driveA;
extern t_drive driveB;

extern bool have_DSK;
extern bool have_SNA;
extern bool have_TAP;

extern byte *pbTapeImage;
extern int cpc_tapeturbo;
extern int cpc_tapespeeding;
extern int cpc_dskautorun;
extern int cpc_tapeautorun;
extern int BootDiskRunCount;
extern int BootTapeRunCount;

extern int CPC_render_mode;
extern int CPC_max_render_mode;
extern char CPC_render_mode_desc[3][15];

extern int vid_index;
extern int CPC_max_vid_mode;
extern vid_mode videomodes[4];

extern int emulatorend;

extern int dwXScale,dwYScale;

SDL_Surface *splash,*portada, *montaje_zoom, *montaje,*menu,*fich,*zipo,*carga,*carpa, *fondo, *snapa , *tapa, *disca = NULL;
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

	montaje_zoom = zoomSurface(montaje,dwXScale,dwYScale,0);
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
                                              FONTS
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

	SDL_BlitSurface(textsurface, NULL, montaje, &dest);
        //SDL_UpdateRects(montaje, 1, &dest);
        SDL_Delay(5);
	SDL_FreeSurface(textsurface);
//	SDL_UpdateRect(video_surface, 0, 0, 0, 0); 
}

/*


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
	int ExtId;
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
	if ((pszExt = strrchr(szFilePath, '.')) )
	{
		pszExt++;
		for (i = 0; stExtentions[i].nExtId != EXT_UNKNOWN; i++)
		{
			if (!strcasecmp(stExtentions[i].szExt,pszExt))
			{
				return stExtentions[i].nExtId;
			}
		}
	}

// Special
/*	if(!strcasecmp(szFilePath, "IPL.TXT"))
		return EXT_UNKNOWN;
*/
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

void getDir(const char *path, int updir, int filter) 
{
	strcpy(open_path, path);


	while(1) {
		DIR *fd;
		char prev_dir[MAX_NAME];
		int prev_len = 0;

/*		if(updir && (strcasecmp(open_path, "/mnt/") == 0) ) {
			return;
		} */
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
						//printf("Filer: Prev Dir: %s\n", prev_dir);
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
			while( (nfiles < MAX_ENTRY) && (direntp = readdir(fd)) != NULL )
			{
				if(direntp->d_name[0] == '.') continue;
				sprintf(fullname, "%s%s", open_path, direntp->d_name);

				if(stat(fullname, &statbuf) == 0) 
					{
					files[nfiles].isdir = S_ISDIR(statbuf.st_mode);


					if(files[nfiles].isdir) 
					{
						strcpy(files[nfiles].name, direntp->d_name);
						strcat(files[nfiles].name, "/");
						nfiles++;
						} else {
							int myext;
							myext=getExtId(direntp->d_name);
							if ((myext == filter) || (myext == EXT_ZIPPED)) 
							//(myext != EXT_UNKNOWN) 
							{
							//remove not relevant ext
							strcpy(files[nfiles].name, direntp->d_name);
							files[nfiles].ExtId=myext;
							nfiles++;
							}
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


//GETdir filtered
void getDirFilter(const char *path, int filterExtID, const char *filter) 
{
 int updir=0;
 strcpy(open_path, path);
 while(1) 
 {
	DIR *fd;
	char prev_dir[MAX_NAME];
	int prev_len = 0;

	if(updir && (strcasecmp(open_path, "/") == 0) ) 
	{
		return;
	}

	if(updir) 
	{
		int i;
		for(i = strlen(open_path)-1; i>0 ; i--) 
		{
			if((open_path[i-1] == '/') || (open_path[i-1] == '\\')) 
			{
				strcpy(prev_dir, &open_path[i]);
				prev_len = strlen(prev_dir);

				while( (prev_len > 0) &&
				  ((prev_dir[prev_len-1] == '/') || 
				  (prev_dir[prev_len-1] == '\\')) ) 
				{
					prev_dir[prev_len-1] = '\0';
					--prev_len;
				}

				if(prev_len != 0) 
				{
					prev_dir[prev_len++] = '/';
					prev_dir[prev_len] = '\0';
//					printf("Filer: Prev Dir: %s\n", prev_dir);
				}
				open_path[i] = '\0';
				updir = 0;
				break;
			}
		}

		if(updir)
		{
			return;
		}
	}

////
	fd = opendir(open_path);

	if(fd != NULL) 
	{
		struct dirent *direntp;
		struct stat statbuf;
		char fullname[_MAX_PATH];

		nfiles = 0;
		while( (nfiles < MAX_ENTRY) && (direntp = readdir(fd)) != NULL )
		{
			if(direntp->d_name[0] == '.') continue;
			sprintf(fullname, "%s%s", open_path, direntp->d_name);
			if(stat(fullname, &statbuf) == 0) 
				{
				files[nfiles].isdir = S_ISDIR(statbuf.st_mode);
				if(!files[nfiles].isdir) 
				 {
					int myext;
					myext=getExtId(direntp->d_name);
					//compare filter and filename, and extensionID
					//printf("GetDirFilter: Compare:%s - %s\n",filter,direntp->d_name);
					if ((myext == filterExtID) && ((!strncasecmp(direntp->d_name,filter,strlen(filter))))) //TROLOLO
					//(myext != EXT_UNKNOWN) 
					{

						//remove not relevant ext
						strcpy(files[nfiles].name, direntp->d_name);
						files[nfiles].ExtId=myext;
						nfiles++;
					}
				 }
				}
		}//while
	closedir(fd);

	sort(0, nfiles-1);

	cur_pos = 0;
	draw_pos = 0;
	redraw = 1;

	if(prev_len != 0)
	{
		int i;
		for(i=0;i<nfiles;i++)
		{
			if(strcasecmp(files[i].name, prev_dir) == 0)
			{
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
	}// fd!=NULL

 updir = 1; //Try up dir
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
		switch(files[i].ExtId) 
//		switch(getExtId(files[i].name)) 
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
				break;
			default:
				SDL_BlitSurface(fich, NULL, montaje, &dstrect);
				break;

		}
	}

   SDL_UpdateRect(montaje, 0,0,0,0);
//	SDL_BlitSurface(list, NULL, video_surface, NULL);

}



int eventloop(int ext) {
	SDL_Event event;
	int flag = 0;
//	static int move_dir = 0;
//	static Uint32 move_tick = 0;
//	move_dir = 0;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
//////////////

    case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

				//move_dir = 0;
				pcjoy_update (event);

				if  (pcjoy_pressed (PC_JOY_UP,event.jaxis.which) ) 
					{		//Action UP
					cur_pos--;
					redraw=1;
					//move_dir = -1;
					//move_tick = 0;
					break;
					};

				if  (pcjoy_pressed (PC_JOY_DOWN,event.jaxis.which)) 
					{		//Action DOWN
					cur_pos++;
					redraw=1;			     
					//move_dir = 1;
					//move_tick = 0;
					break;	
					};

				if  (pcjoy_pressed (PC_JOY_LEFT,event.jaxis.which))
					{		//Action LEFT
					//move_dir = 0;
					if (ext!=EXT_UNKNOWN) getDir(open_path, 1,ext);//filter
					break;
					};

				if  (pcjoy_pressed (PC_JOY_RIGHT,event.jaxis.which)) 
					{		//Action RIGHT
					if(files[cur_pos].isdir){
						strcat(open_path, files[cur_pos].name);
						getDir(open_path, 0,ext);
						//move_dir = 0;
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
							getDir(open_path, 0,ext);
							//move_dir = 0;
							cur_pos = 0;
							draw_pos = 0;
							redraw = 1;
						} else {
							flag = 1;
						}
						break;
					case SDL_JoyFire2:
						//move_dir = 0;
						if (ext!=EXT_UNKNOWN) getDir(open_path, 1,ext);
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
				//move_dir = -1;
				//move_tick = 0;
				break;
			case SDLK_DOWN:
				cur_pos++;
				redraw=1;
				//move_dir = 1;
				//move_tick = 0;
				break;
			case SDLK_LEFT:
				//move_dir = 0;
				if (ext!=EXT_UNKNOWN) getDir(open_path, 1,ext);
				break;
			case SDLK_RIGHT:
			case SDLK_RETURN:
				if(files[cur_pos].isdir)
				{
					strcat(open_path, files[cur_pos].name);
					getDir(open_path, 0,ext);
					//move_dir = 0;
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
				//move_dir = 0;
				//move_tick = 0;
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
		
	}
*/
			if(cur_pos >= nfiles) cur_pos =  nfiles - 1;
			if(cur_pos < 0) cur_pos = 0;
	return flag;
}



//=========================================================
//               FILE LOADER
//=========================================================


int fileloader( char *out, char *ext )
{


	int flag,fext;
	static char pathtmp[_MAX_PATH+1]="./";  //SAFE CLEANUP

//	SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
	fext=getExtId(ext);
	switch(fext) 
	{
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

	getDir(pathtmp, 0,fext);
	cur_pos = 0;
	draw_pos = 0;
	redraw = 1;
	while((flag = eventloop(fext)) == 0)
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


//=========================================================
//               MENU SYSTEM
//=========================================================

struct textmenu{
       char text[20];
               };

enum {    
	MENU_DISK,
	MENU_TAPE,
	MENU_SNAP,
	MENU_OPT,
	MENU_SET,
	MENU_SCR,
	MENU_JOY,
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
static int maxopt_menusnap=3; //Max items in menu
textmenu text_menusnap[8];
int option_menusnap[8] ;

/*
int opt_menuconfig=1;
static int maxopt_menuconfig=5; //Max items in menu
textmenu text_menuconfig[8];
int option_menuconfig[8] ;
*/

int opt_menuscreen=1;
static int maxopt_menuscreen=4; //Max items in menu
textmenu text_menuscreen[8];
int option_menuscreen[8] ;


int opt_menujoy=1;
static int maxopt_menujoy=5; //Max items in menu
textmenu text_menujoy[8];
int option_menujoy[8] ;


int opt_settingsconfig=1;
static int maxopt_settingsconfig=6; //Max items in menu
textmenu text_settingsconfig[8];
unsigned int option_settingsconfig[8] ;



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
                               CPC_TAPE_load(tapepath);
                               redraw=1;
                               }

                               //flag =1;
				break;

                  case 1:  //2 Eject tape
				Tape_Rewind(); //Reset Tape state
				tape_eject();  //NULLify the tape. Rewind before ejecting to reset state. May crash if not
				option_menutape[3]=(CPC.tape_play_button != 0);
				CPC.tape_path[0]='\0';
				CPC.tape_file[0]='\0';
				have_TAP=0;
				redraw=1;
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
   //                            redraw=1;
                               flag=1;
				break;

                  case 3:  //4 rewind
                       		Tape_Rewind();
                 	   	option_menutape[3]=(CPC.tape_play_button != 0);
                       		//redraw=1;
                       		break;

                  case 4:  //5 Autotype
				option_menutape[5]=!option_menutape[5];
				cpc_tapeautorun=option_menutape[5];
				redraw=1;
                               break;

                  case 5:  //6 Frameskip when motor on
                           option_menutape[6]=!option_menutape[6];
                           cpc_tapeturbo=option_menutape[6];
                       //redraw=1;
                       break;
                 };
return flag;

}

//   SNAPSHOT System


//SAVE SNA
int saveSnap ()
{
 char snapfilename[_MAX_PATH];
 char pathtmp[_MAX_PATH];
// int tablepos,namepos,exitloop=0;
 int exitloop=0;
 struct stat st = {0};
 int fmkdir=0;
 int savefail=100;

 snapfilename[0]='\0';

 strcpy(pathtmp, "./snap/save");


 if (stat(pathtmp, &st) == -1) fmkdir=mkdir(pathtmp, 0777); //No exist  ./snap/save -> create

 if (fmkdir == -1) //Error creating snap/save, check ./save
 {
	strcpy(pathtmp, "./save");
	if (stat(pathtmp, &st) == -1) 
	 {
		fmkdir=mkdir(pathtmp, 0777); 	//No exist  ./snap/save -> create
		savefail=1; //Cannot create ./save. SUPERFAIL 1
	 }
 }

strcat(pathtmp,"/");

if (have_DSK)
	{
	strcpy(snapfilename,CPC.drvA_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_DSK_");
	}
   else if (have_TAP) 
	{
	strcpy(snapfilename,CPC.tape_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_CDT_");
	}
   else if (have_SNA) 
	{
	strcpy(snapfilename,CPC.snap_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_SNA_");
	}



 if (snapfilename[0] == '\0')  strcpy (snapfilename,"USER_SNA_");

//printf("SaveSNA: Snapfilename:%s\n",snapfilename);

 while (exitloop==0)
     {
	int flag=0;
//	SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
//	SDL_UpdateRect(montaje, 0,0,0,0);

	getDirFilter(pathtmp,EXT_SNAP,snapfilename);
	//ADD NEW FILE OPTION
	if (nfiles<5)
	{
		strcpy(files[nfiles].name,"NEW FILE...");
		files[nfiles].isdir=0;
		files[nfiles].ExtId=EXT_UNKNOWN;
		nfiles++;
	}
	cur_pos = 0;
	draw_pos = 0;
	redraw = 1;
	while((flag = eventloop(EXT_UNKNOWN)) == 0)
	{
		if(redraw) 
		{
 			char tmpname[_MAX_PATH];
			strcpy(tmpname,"FILE: ");
			strcat(tmpname,snapfilename);
			strcat(tmpname,"XX.sna");
//                	displaytext( 40, 180 ,tmpname, getfontcolor(250, 0 , 250));
			drawlist(".");
			displaytext( 70, 25 ,"SAVE SNAPSHOT", getfontcolor(250, 250, 0));
                	displaytext( 40, 180 ,tmpname, getfontcolor(250, 0 , 250));
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 2) 
		{
		savefail=2; //CANCELED
		exitloop=1;
		}
	if(flag == 1) 
	{
		char tmpfilename[_MAX_PATH];
		// SAVE SNAP...
		if (strcmp(files[nfiles].name,"NEW FILE..."))
			{
			int findex;
			char tmpstr[6]; //AQUI

			for (findex=0;findex<5;findex++)
			 {
				strcpy(tmpfilename, pathtmp);
				strcat(tmpfilename,snapfilename);
        			sprintf(tmpstr,"%02d",findex);
				strcat(tmpfilename,tmpstr);
				strcat(tmpfilename,".sna");

				if (!stat (tmpfilename, &st) == 0)
				 {
				 break; 
				 }

			 }

			} else
			{
			strcpy(tmpfilename, pathtmp);
			strcat(tmpfilename, files[cur_pos].name);
//snapfilename
			}
			 strcpy(snapfilename,tmpfilename);
			//GOT FILENAME
			savefail=0;  //No error
			exitloop=1;

	}//flag=1

     }//While exitloop

if (!savefail)
	{
	//ask
	int qexit,qopt;
 	SDL_Event qevent;

	qopt=0;
	qexit=0;

	while (!qexit)
	  {
		SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
		SDL_UpdateRect(montaje, 0,0,0,0);
		displaytext( 70, 100 ,"ARE YOU SURE?", getfontcolor(250, 250, 0));

		displaytext( 70, 125 ,"OK!", getfontcolor(250, 250*qopt, 0));
		displaytext( 120, 125 ,"CANCEL", getfontcolor(250, 250*(!qopt), 0));
		menu_blit();

		//SDL EVALUATE
		while(SDL_PollEvent(&qevent)) 
		 {
			int which_joy;
			switch(qevent.type)
			 {
			 case SDL_JOYAXISMOTION:  // Handle Joystick Motion 
			 case SDL_JOYBUTTONUP:
			 case SDL_JOYBUTTONDOWN:
                  		pcjoy_update (qevent);
				if (qevent.type==SDL_JOYAXISMOTION)
					which_joy=qevent.jaxis.which;
					else
					which_joy=qevent.jbutton.which;

				if (pcjoy_pressed (PC_JOY_LEFT,which_joy))
				 {
					qopt=0;
				 }
				if (pcjoy_pressed (PC_JOY_RIGHT, which_joy))
				 {
					qopt=1;
				 }
				if (qevent.type == SDL_JOYBUTTONDOWN )
				 {
				 switch (qevent.jbutton.button)
				  {
					case SDL_JoyFire1:
						//Accion seleccionar
						if (!qopt)
						{
						 //SAVESNAP
						snapshot_save(snapfilename);
						}
						else savefail=2; //CANCELED

						qexit=1;
						break;

					case SDL_JoyFire4:
						// Accion salir ;
						qexit=1;
						break;
				  }
				}
			case SDL_KEYDOWN:
			 switch (qevent.key.keysym.sym) 
            		  {
				case SDLK_LEFT:
					qopt=0;
						break;

				case SDLK_RIGHT:
					qopt=1;
						break;
				case SDLK_RETURN:
				case SDLK_SPACE: 
						//Accion seleccionar
						if (!qopt)
						{
						 //SAVESNAP
						snapshot_save(snapfilename);
						}
						else savefail=2; //CANCELED

						qexit=1;
						break;

				case SDLK_ESCAPE:
				case 'q':
						qexit=1;
						break;
				default:
						break;
			    }

			 } //Switch
		}//While event

	  }
	//save
	}


char snapmsg[60];

switch (savefail)
  {
	case 0:  //OK
		strcpy (snapmsg, "SNA Save OK!");
		break;
	case 1:  //Dir not creatabl
		strcpy (snapmsg, "Directory error!");

		break;
	case 2:  //Dir not creatabl
		strcpy (snapmsg, "Canceled saving!");

		break;
	default:

		break;

  }


if (savefail > 0)
	{
	//ask
	int qexit;
 	SDL_Event qevent;

	qexit=0;

	while (!qexit)
	  {
		SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
		SDL_UpdateRect(montaje, 0,0,0,0);
		displaytext( 70, 100 ,snapmsg, getfontcolor(250, 250, 0));

		displaytext( 100, 125 ,"OK!", getfontcolor(250,0, 0));
		menu_blit();



		//SDL EVALUATE
		while(SDL_PollEvent(&qevent)) 
		 {
			switch(qevent.type)
			 {
			 case SDL_JOYBUTTONDOWN:
				 switch (qevent.jbutton.button)
				  {
					case SDL_JoyFire1:
					case SDL_JoyFire4:
						qexit=1;
						break;
				  }
				break;

			case SDL_KEYDOWN:
			 switch (qevent.key.keysym.sym) 
            		  {

				case SDLK_RETURN:
				case SDLK_SPACE: 
				case SDLK_ESCAPE:
				case 'q':
						//Accion seleccionar
						qexit=1;
						break;
				default:
						break;
			    }

			 } //Switch
		}//While event

	  }
	//save
	}




return 0;
}//saveSnapGUI 

//LOAD SNAP

int loadSnap ()
{
 char snapfilename[_MAX_PATH];
 char pathtmp[_MAX_PATH];
// int tablepos,namepos,exitloop=0;
 int exitloop=0;
 struct stat st = {0};
 int fmkdir=0;
 int loadfail=100;

 snapfilename[0]='\0';

 strcpy(pathtmp, "./snap/save");


 if (stat(pathtmp, &st) == -1) fmkdir=mkdir(pathtmp, 0777); //No exist  ./snap/save -> create

 if (fmkdir == -1) //Error creating snap/save, check ./save
 {
	strcpy(pathtmp, "./save");
	if (stat(pathtmp, &st) == -1) 
	 {
		fmkdir=mkdir(pathtmp, 0777); 	//No exist  ./snap/save -> create
		loadfail=1; //Cannot create ./save. SUPERFAIL 1
	 }
 }

strcat(pathtmp,"/");

if (have_DSK)
	{
	strcpy(snapfilename,CPC.drvA_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_DSK_");
	}
   else if (have_TAP) 
	{
	strcpy(snapfilename,CPC.tape_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_CDT_");
	}
   else if (have_SNA) 
	{
	strcpy(snapfilename,CPC.snap_file);
	snapfilename[strlen(snapfilename)-4]='\0';
	strcat(snapfilename,"_SNA_");
	}

 if (snapfilename[0] == '\0')  strcpy (snapfilename,"USER_SNA_");

 while (exitloop==0)
     {
	int flag=0;
//	SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
//	SDL_UpdateRect(montaje, 0,0,0,0);

	getDirFilter(pathtmp,EXT_SNAP,snapfilename);
	//ADD NEW FILE OPTION
/*	if (nfiles<5)
	{
		strcpy(files[nfiles].name,"NEW FILE...");
		files[nfiles].isdir=0;
		files[nfiles].ExtId=EXT_UNKNOWN;
		nfiles++;
	}
*/
	cur_pos = 0;
	draw_pos = 0;
	redraw = 1;
	while((flag = eventloop(EXT_UNKNOWN)) == 0)
	{
		if(redraw) 
		{
 			char tmpname[_MAX_PATH];
			strcpy(tmpname,"FILE: ");
			strcat(tmpname,snapfilename);
			strcat(tmpname,"XX.sna");
			drawlist(".");
			displaytext( 70, 25 ,"LOAD SNAPSHOT", getfontcolor(250, 250, 0));
                	displaytext( 40, 180 ,tmpname, getfontcolor(250, 0 , 250));
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 2) 
		{
		loadfail=2; //CANCELED
		exitloop=1;
		}
	if(flag == 1) 
	{
		char tmpfilename[_MAX_PATH];
		// SAVE SNAP...
/*		if (strcmp(files[nfiles].name,"NEW FILE..."))
			{
			int findex;
			char tmpstr[6]; //AQUI

			for (findex=0;findex<5;findex++)
			 {
				strcpy(tmpfilename, pathtmp);
				strcat(tmpfilename,snapfilename);
        			sprintf(tmpstr,"%02d",findex);
				strcat(tmpfilename,tmpstr);
				strcat(tmpfilename,".sna");

				if (!stat (tmpfilename, &st) == 0)
				 {
				 break; 
				 }

			 }

			} else
			{

*/
			strcpy(tmpfilename, pathtmp);
			strcat(tmpfilename, files[cur_pos].name);
//snapfilename
//			}
			strcpy(snapfilename,tmpfilename);
			//GOT FILENAME
			loadfail=0;  //No error
			exitloop=1;

	}//flag=1

     }//While exitloop

if (!loadfail) CPC_SNA_load(snapfilename);
/*
if (!savefail)
	{


	//ask
	int qexit,qopt;
 	SDL_Event qevent;

	qopt=0;
	qexit=0;

	while (!qexit)
	  {
		SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
		SDL_UpdateRect(montaje, 0,0,0,0);
		displaytext( 70, 100 ,"ARE YOU SURE?", getfontcolor(250, 250, 0));

		displaytext( 70, 125 ,"OK!", getfontcolor(250, 250*qopt, 0));
		displaytext( 120, 125 ,"CANCEL", getfontcolor(250, 250*(!qopt), 0));
		menu_blit();

		//SDL EVALUATE
		while(SDL_PollEvent(&qevent)) 
		 {
			switch(qevent.type)
			 {
			 case SDL_JOYAXISMOTION:  // Handle Joystick Motion 
			 case SDL_JOYBUTTONUP:
			 case SDL_JOYBUTTONDOWN:
                  		pcjoy_update (qevent);
				if ( (pcjoy_pressed (PC_JOY1_LEFT)) || (pcjoy_pressed (PC_JOY2_LEFT))) 
				 {
					qopt=0;
				 }
				if ( (pcjoy_pressed (PC_JOY1_RIGHT)) || (pcjoy_pressed (PC_JOY2_RIGHT))) 
				 {
					qopt=1;
				 }
				if (qevent.type == SDL_JOYBUTTONDOWN )
				 {
				 switch (qevent.jbutton.button)
				  {
					case SDL_JoyFire1:
						//Accion seleccionar
						if (!qopt)
						{
						 //SAVESNAP
						snapshot_save(snapfilename);
						}
						else savefail=2; //CANCELED

						qexit=1;
						break;

					case SDL_JoyFire4:
						// Accion salir ;
						qexit=1;
						break;
				  }
				}
			case SDL_KEYDOWN:
			 switch (qevent.key.keysym.sym) 
            		  {
				case SDLK_LEFT:
					qopt=0;
						break;

				case SDLK_RIGHT:
					qopt=1;
						break;
				case SDLK_RETURN:
				case SDLK_SPACE: 
						//Accion seleccionar
						if (!qopt)
						{
						 //SAVESNAP
						snapshot_save(snapfilename);
						}
						else savefail=2; //CANCELED

						qexit=1;
						break;

				case SDLK_ESCAPE:
				case 'q':
						qexit=1;
						break;
				default:
						break;
			    }

			 } //Switch
		}//While event

	  }
	//save
	}


char snapmsg[60];

switch (savefail)
  {
	case 0:  //OK
		strcpy (snapmsg, "SNA Save OK!");
		break;
	case 1:  //Dir not creatabl
		strcpy (snapmsg, "Directory error!");

		break;
	case 2:  //Dir not creatabl
		strcpy (snapmsg, "Canceled saving!");

		break;
	default:

		break;

  }


if (savefail > 0)
	{
	//ask
	int qexit;
 	SDL_Event qevent;

	qexit=0;

	while (!qexit)
	  {
		SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
		SDL_UpdateRect(montaje, 0,0,0,0);
		displaytext( 70, 100 ,snapmsg, getfontcolor(250, 250, 0));

		displaytext( 100, 125 ,"OK!", getfontcolor(250,0, 0));
		menu_blit();



		//SDL EVALUATE
		while(SDL_PollEvent(&qevent)) 
		 {
			switch(qevent.type)
			 {
			 case SDL_JOYBUTTONDOWN:
				 switch (qevent.jbutton.button)
				  {
					case SDL_JoyFire1:
					case SDL_JoyFire4:
						qexit=1;
						break;
				  }
				break;

			case SDL_KEYDOWN:
			 switch (qevent.key.keysym.sym) 
            		  {

				case SDLK_RETURN:
				case SDLK_SPACE: 
				case SDLK_ESCAPE:
				case 'q':
						//Accion seleccionar
						qexit=1;
						break;
				default:
						break;
			    }

			 } //Switch
		}//While event

	  }
	//save
	}
*/



return 0;
}//LoadSnap 



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
				CPC_SNA_load(snappath);
                               redraw=1;
                               flag =1;
                               }
                               break;
                case 1:  //2 Save snap
                               {
                               saveSnap();
                               redraw=1;
                               flag =1;
                               }

                               break;
                case 2:  //3 Fastload snap
                               {
                               loadSnap();
                               redraw=1;
                               flag =1;
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
			char discpath[_MAX_PATH + 1];

			fileloader(disc_filename, ".dsk");
			cur_menupos = 0;
			draw_menupos = 0;
			strcpy(discpath, disc_filename);

			CPC_DSK_load(discpath, 'A'); 

			redraw=1;
			//flag =1;
			}
			break;

		case 1:  //2 eject disk
			dsk_eject(&driveA);
			CPC.drvA_path[0]='\0';
			CPC.drvA_file[0]='\0';
			redraw=1;
			break;
		case 2:  //3 Autrun disc
			option_menudisc[3]=!option_menudisc[3];
			cpc_dskautorun=option_menudisc[3];
			break;
		case 3:  //4 Load disc B
			{
			char disc_filename[_MAX_PATH];
			char discpath[_MAX_PATH + 1];

			fileloader(disc_filename, ".dsk");   // No verificamos errores¿¿?¿?¿?¿? :(
			cur_menupos = 0;
			draw_menupos = 0;
			strcpy(discpath, disc_filename);

			CPC_DSK_load(discpath, 'B'); 

			redraw=1;
			//flag =1;
			}
			break;

		case 4:  //5 eject disk
			dsk_eject(&driveB);
			CPC.drvB_path[0]='\0';
			CPC.drvB_file[0]='\0';
			redraw=1;
			break;
                 };

return flag;

}


/*
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
                       load_settingsconfig();   // :(
                           flag =1;
                       break;

                  case 4:  //5 reset
                           needreset=1;
                           flag =1;
                       break;
                       
                  
                 };
                 
return flag;

}
*/

//EVAL SCREEN CONFIG

int menu_eval_screen (int cur_menupos){
    
                   int flag=0;     

                 switch (cur_menupos)
                 {
                  case 0:  //1 Color/Green monitor
                       option_menuscreen[1]=!option_menuscreen[1];
                       break;

                  case 1:  //2 frameskip
                       option_menuscreen[2]++;
                       if (option_menuscreen[2] > 2 ) option_menuscreen[2]=0;
                       break;

                  case 2:  //3 vid mode
			option_menuscreen[3]++;
			if (option_menuscreen[3] > CPC_max_vid_mode ) option_menuscreen[3]=0;
			break;

                  case 3:  //4 render
     			if (option_menuscreen[3]>1)
			{
			option_menuscreen[4]++;
			if (option_menuscreen[4] > CPC_max_render_mode ) option_menuscreen[4]=0;
			}
//CPC_render_mode;

                       break;

                  case 4:  //5 back
//                           needreset=1;
                           flag =1;
                       break;
                 };

return flag;

}

//EVAL JOYSTICK

extern void SDL_JoystickReload(int debug);

int menu_eval_joystick (int cur_menupos){
    
                   int flag=0;     

                 switch (cur_menupos)
                 {
                  case 0:
                  case 1:
                  case 2:
                  case 3:  //4 render
			//cur_menupos=actual joystick
			{
			int jmodeind=CPC_Joys[cur_menupos];
			int oldjmode=jmodeind;
			char jdone=0;
			while (!jdone)
			{
				if (CPC_Joystate[jmodeind])
				{
				jmodeind++;
				if (jmodeind >7) jmodeind=0;
				}
				else
				jdone=1;
			}
			CPC_Joystate[oldjmode]=0;
			CPC_Joys[cur_menupos]=jmodeind;
			CPC_Joystate[jmodeind]=1;
			pcjoy_assign(cur_menupos,CPC_Joys[cur_menupos]);

                       }
			break;

                  case 4:  //5 back
			SDL_JoystickReload(0);
                        //flag =1;
                       break;
                 };

return flag;

}


//EVAL SETTINGS

int load_menuscreen();   // predefinition
int load_menujoy();   // predefinition

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

                  case 2:  //3 audio
                       option_settingsconfig[3]=!option_settingsconfig[3];
                       break;

                  case 3:  //4 Screen setup
			load_menuscreen();
			maxopt_menucurrent=maxopt_settingsconfig;
			break;
                  case 4:  //5 joystick
			load_menujoy();
			maxopt_menucurrent=maxopt_settingsconfig;
			break;
                  case 5:  //6 reset
                           needreset=1;
                           flag =1;
			   break;

                 };

return flag;

}





//EVALUATION MAIN LOOP


int eventloop_menu(int menu_type) {
	SDL_Event event;
	int make_option=0;
	int flag = 0;
//	static int move_dir = 0;
//	move_dir = 0;

	while(SDL_PollEvent(&event)) 
	{int which_joy;
	switch(event.type) 
		{

//////////////

		case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
				//move_dir = 0;
				pcjoy_update (event);
				if (event.type==SDL_JOYAXISMOTION)
					which_joy=event.jaxis.which;
					else
					which_joy=event.jbutton.which;


				if (pcjoy_pressed (PC_JOY_UP, which_joy))
				{		//Action UP
					cur_menupos--;
					redraw=1;
					//move_dir = -1;
					break;
				};
				if (pcjoy_pressed (PC_JOY_DOWN, which_joy))
				{		//Action DOWN
					cur_menupos++;
					redraw=1;			     
					//move_dir = 1;
					break;
				};
                  		if (pcjoy_pressed (PC_JOY_LEFT,which_joy))
				{		//Action LEFT
					flag = 1;
				};
				if (pcjoy_pressed (PC_JOY_RIGHT,which_joy))
				{		//Action RIGHT
					redraw=1;
					make_option=1;
					//move_dir = 0;
					break;
				};

				if (event.type == SDL_JOYBUTTONDOWN )
				{
				 switch (event.jbutton.button)
					{
					case SDL_JoyFire1:	//Accion seleccionar
						redraw=1;
						make_option=1;
						//move_dir = 0;
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
					//move_dir = -1;
					break;
				case SDLK_DOWN:
					cur_menupos++;
					redraw=1;
					//move_dir = 1;
					break;
				case SDLK_RETURN:
				case SDLK_RIGHT:
					redraw=1;
					make_option=1;
					//move_dir = 0;
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
					//move_dir = 0;
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
                        case MENU_SCR:
                                  flag=menu_eval_screen(cur_menupos);
                                  redraw=1;
                                  break;
                        case MENU_JOY:
                                  flag=menu_eval_joystick(cur_menupos);
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

        if ((opt_menutape == 5) or (opt_menutape == 6) )
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
//TAPE Status:

	dstrect.x = 16;


	if (CPC.tape_file[0]=='\0')
		{
                displaytext( 25, 205 ,"--", getfontcolor(250, 0 , 250));
		displaytext( 55, 205 ,"NO TAPE", getfontcolor(250, 0 , 250));
		}
		else
		{
		dstrect.y = 205;
		SDL_BlitSurface(tapa, NULL, montaje, &dstrect);
		displaytext( 55, 205 ,CPC.tape_file, getfontcolor(250, 0 , 250));
		}
    SDL_UpdateRect(montaje, 0,0,0,0);
}


int load_menutape( )
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
	option_menutape[5]=cpc_tapeautorun;
	option_menutape[6]=cpc_tapeturbo;
    strcpy(text_menutape[1].text,"Load Tape");
    strcpy(text_menutape[2].text,"Eject Tape");
    strcpy(text_menutape[3].text,"Play Tape");
    strcpy(text_menutape[4].text,"Rewind Tape");
    strcpy(text_menutape[5].text,"Autorun Tape");
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
//	SDL_Rect dstrect;
//	dstrect.x = 20;

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

              
		//dstrect.y = y;

		opt_menusnap++;
	}   
    SDL_UpdateRect(montaje, 0,0,0,0);
}


int load_menusnap( )
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
    strcpy(text_menusnap[3].text,"Fastload Snapshot");
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
	//dstrect.x = 20;

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

        if ((opt_menudisc == 3)  )
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


               
		//dstrect.y = y;

		opt_menudisc++;
	}   




//Drive Status:

	dstrect.x = 16;
                displaytext( 40, 180 ,"A:", getfontcolor(250, 0 , 250));
	if (CPC.drvA_file[0]=='\0')
		{
                displaytext( 25, 180 ,"--", getfontcolor(250, 0 , 250));
		displaytext( 55, 180 ,"NO DISC", getfontcolor(250, 0 , 250));
		}
		else
		{
		dstrect.y = 180;
		SDL_BlitSurface(disca, NULL, montaje, &dstrect);
		displaytext( 55, 180 ,CPC.drvA_file, getfontcolor(250, 0 , 250));
		}
                displaytext( 40, 205 ,"B:", getfontcolor(250, 0 , 250));
	if (CPC.drvB_file[0]=='\0')
		{
                displaytext( 25, 205 ,"--", getfontcolor(250, 0 , 250));
		displaytext( 55, 205 ,"NO DISC", getfontcolor(250, 0 , 250));
		}
		else
		{
		dstrect.y = 205;
		SDL_BlitSurface(disca, NULL, montaje, &dstrect);
		displaytext( 55, 205 ,CPC.drvB_file, getfontcolor(250, 0, 250));
		}


    SDL_UpdateRect(montaje, 0,0,0,0);
}


int load_menudisc( )
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
	option_menudisc[3]=cpc_dskautorun;
	option_menudisc[4]=0;
	option_menudisc[5]=0;
	option_menudisc[6]=0;
    strcpy(text_menudisc[1].text,"A:Load Disc");
    strcpy(text_menudisc[2].text,"A:Eject Disc");
    strcpy(text_menudisc[3].text,"A:Autorun Disc");
    strcpy(text_menudisc[4].text,"B:Load Disc");
    strcpy(text_menudisc[5].text,"B:Eject Disc");
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



//MENU SCREEN


void draw_menuscreen(void) {
	int y = 50;
	int i;
//	SDL_Rect dstrect;
//	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 105, 25 ,"SCREEN SETUP", getfontcolor(250, 250, 0));
	opt_menuscreen=1;
	for(i=draw_menupos;(opt_menuscreen<=maxopt_menuscreen) && (y < 250);i++, y+=25){
		int color_menu =0;
        if(i == cur_menupos) {color_menu=0;} else {color_menu=250;};
        displaytext( 50, y ,text_menuscreen[opt_menuscreen].text, getfontcolor(250, color_menu, 0)); 
        char tmpstr[6];
        sprintf(tmpstr,"%d",option_menuscreen[opt_menuscreen]);
        switch (opt_menuscreen)
              {
                 case 1:
                       {switch (option_menuscreen[opt_menuscreen]) 
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
                        displaytext( 250, y ,tmpstr, getfontcolor(250, color_menu, 0));
                        break;
                 case 3:
			displaytext( 250, y ,videomodes[option_menuscreen[opt_menuscreen]].Vid_mode_desc, getfontcolor(250, color_menu, 0));

			break;
                 case 4:
     			if (option_menuscreen[3]>1)
			{
                        displaytext( 220, y ,CPC_render_mode_desc[option_menuscreen[opt_menuscreen]], getfontcolor(250, color_menu, 0));
                        } else
			displaytext( 250, y , "NONE" , getfontcolor(250, color_menu, 0));
                        break;
                 default:
                         break;   

              }

//		dstrect.y = y;

		opt_menuscreen++;
	}   SDL_UpdateRect(montaje, 0,0,0,0);
}



int load_menuscreen( )
{

int flag;
// char *path;
	SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  	SDL_UpdateRect(montaje, 0,0,0,0);
    	needreset=0;
	option_menuscreen[1]=!CPC.scr_tube;
	option_menuscreen[2]=CPC.scr_fskip;
	option_menuscreen[3]=vid_index;//vidmode
	option_menuscreen[4]=CPC_render_mode;
	option_menuscreen[5]=0;
	option_menuscreen[6]=0;
    strcpy(text_menuscreen[1].text,"Color monitor");
    strcpy(text_menuscreen[2].text,"Frameskip");
    strcpy(text_menuscreen[3].text,"Video mode:");
    strcpy(text_menuscreen[4].text,"Renderer");
    strcpy(text_menuscreen[5].text,"Back <<--");
    strcpy(text_menuscreen[6].text,"------------");

    maxopt_menucurrent=maxopt_menuscreen;
    
	cur_menupos = 0;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_SCR)) == 0) {
		if(redraw) {
			draw_menuscreen();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}

	if(flag == 1) {  // IF flag is not 0 or 1, then abort settings change

	CPC.scr_tube=!option_menuscreen[1];
	CPC.scr_fskip=option_menuscreen[2];
	vid_index=option_menuscreen[3];
	CPC_render_mode=option_menuscreen[4];
	video_set_palette();
	if (needreset==1){
                  emulator_reset(false);
	}

		return 1;
	}//flag=1


	return 0;
}



//MENU JOYSTICKS

enum {
	JOY_JA,
	JOY_JB,
	JOY_JG,
	JOY_JO,
	JOY_T3,
	JOY_T4,
	JOY_OPQA,
	JOY_CURSOR
};


void draw_menujoy(void) {
	int y = 50;
	int i;
//	SDL_Rect dstrect;
//	dstrect.x = 20;
	int cr,cg,cb;
	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 105, 25 ,"JOYSTICK SETUP", getfontcolor(250, 250, 0));
	opt_menujoy=1;
	for(i=draw_menupos;(opt_menujoy<=maxopt_menujoy) && (y < 250);i++, y+=25){
		int color_menu =0;
        if(i == cur_menupos) 
		{
		cr=250;
		cg=0;
		cb=0;
		} else
		{ 
	        if((SDL_NumJoysticks()<=i) && (i<4))
			{
			cr=128;
			cg=128;
			cb=128;
			} else
			{
			cr=250;
			cg=250;
			cb=0;
			};
		};
        displaytext( 50, y ,text_menujoy[opt_menujoy].text, getfontcolor(cr, cg, cb)); 
//        char tmpstr[6];
//        sprintf(tmpstr,"%d",option_menujoy[opt_menujoy-1]);
        switch (opt_menujoy)
              {
                 case 1:
                 case 2:
                 case 3:
                 case 4:
//		        if((SDL_NumJoysticks()<=i) && (i<4))
//                        displaytext( 150, y ,"OFF", getfontcolor(cr, cg, cb)); 
//                        else
			displaytext( 150, y ,CPC_Joynames[CPC_Joys[opt_menujoy-1]], getfontcolor(cr, cg, cb)); 
			break;
                 default:
                         break;   

              }


//		dstrect.y = y;

		opt_menujoy++;
	}   SDL_UpdateRect(montaje, 0,0,0,0);
}



int load_menujoy( )
{

int indx;
int flag;
// char *path;
	SDL_BlitSurface( fondo, NULL , montaje, NULL ); 
  	SDL_UpdateRect(montaje, 0,0,0,0);
    	needreset=0;

    strcpy(text_menujoy[1].text,"J1");
    strcpy(text_menujoy[2].text,"J2");
    strcpy(text_menujoy[3].text,"J3");
    strcpy(text_menujoy[4].text,"J4");
    strcpy(text_menujoy[5].text,"Reload Joysticks");
    strcpy(text_menujoy[6].text,"------------");

    maxopt_menucurrent=maxopt_menujoy;
    
	cur_menupos = 4;
	draw_menupos = 0;
	redraw = 1;
	
	while((flag = eventloop_menu(MENU_JOY)) == 0) {
		if(redraw) {
			draw_menujoy();
			menu_blit();
			//SDL_Delay (1000);
			redraw = 0;
		}
	}


	if(flag == 1)   // IF flag is not 1, then abort settings change
	{
/*		if (needreset==1){
                  emulator_reset(false);
		}
*/
		return 1;
	}//flag=1


	return 0;
}



//___________________________________________
// SETTINGS CONFIG

void draw_settingsconfig(void) {
	int y = 50;
	int i;
//	SDL_Rect dstrect;
//	dstrect.x = 20;

	SDL_BlitSurface(fondo, NULL, montaje, NULL);
	   SDL_UpdateRect(montaje, 0,0,0,0);
	displaytext( 105, 25 ,"CPC SETTINGS", getfontcolor(250, 250, 0));
	opt_settingsconfig=1;
	for(i=draw_menupos;(opt_settingsconfig<=maxopt_settingsconfig) && (y < 250);i++, y+=25)
	{
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

                 case 2:
                        sprintf(tmpstr,"%d",option_settingsconfig[opt_settingsconfig]+64);
                        displaytext( 250, y ,tmpstr, getfontcolor(250, color_menu, 0));
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

                 default:
                         break;   
              }

//		dstrect.y = y;

		opt_settingsconfig++;
	}   SDL_UpdateRect(montaje, 0,0,0,0);
}



int load_settingsconfig( )
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
	option_settingsconfig[3]=CPC.snd_enabled;
	option_settingsconfig[4]=0;
	option_settingsconfig[5]=0;
	option_settingsconfig[6]=0;
    strcpy(text_settingsconfig[1].text,"CPC Model");
    strcpy(text_settingsconfig[2].text,"Memory");
    strcpy(text_settingsconfig[3].text,"Audio");
    strcpy(text_settingsconfig[4].text,"Screen setup   ->");
    strcpy(text_settingsconfig[5].text,"Joystick setup ->");
    strcpy(text_settingsconfig[6].text,"Reset CPC");
//    strcpy(text_settingsconfig[6].text,"----------------");


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

//	if(flag > 0) {  //???

	if (CPC.model != option_settingsconfig[1]) needreset=1;
	if (CPC.ram_size != (option_settingsconfig[2]+64)) needreset=1;

    	CPC.model=option_settingsconfig[1];
	CPC.ram_size=option_settingsconfig[2]+64;
	CPC.snd_enabled=option_settingsconfig[3];
	if (needreset==1){

			emulator_shutdown();
			//emulator_reset(false);
			emulator_init();
	                  }
		return 1;
//	}//flag=1


return 0;
}


//___________________________________________

/*
================================================================================================================
                                              ICON MENU
================================================================================================================
*/

void intro_cap(bool active)
{
if (active)
{
   SDL_BlitSurface(splash, NULL, montaje, NULL);
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
   //SDL_UpdateRects(video_surface, 1, &Offset); 
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
	int which_joy;

	switch (icoevent.type)
	{
							//////////////

		case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:
			if (icoevent.type==SDL_JOYAXISMOTION)
			which_joy=icoevent.jaxis.which;
			else
			which_joy=icoevent.jbutton.which;

			pcjoy_update (icoevent);
			if  (pcjoy_pressed (PC_JOY_LEFT,which_joy))
				{		//Action LEFT
				sele=ICOMENU_LEFT;
				break;
				};
			if (pcjoy_pressed (PC_JOY_RIGHT,which_joy))
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
                                                load_menudisc();   // :(
						if (!cpc_dskautorun) BootDiskRunCount=0; //If no autorun, disable it...
						vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 1:
                                                load_menutape();            // :(
						if (!cpc_tapeautorun) BootTapeRunCount=0; //If no autorun, disable it...
                                                vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 2:
                                               load_menusnap();            // :(
                                                vismenu =0;
                                                CPC.paused=0;
                                                audio_resume();
                                               break;
                                          case 3:
                                                load_settingsconfig();   // :(
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
#ifndef NOTPI
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
#else
 return 200;
#endif

}



void menu_init()
{
//SDL_Surface *menu,*fich,*zipo,*carga,*carpa = NULL;


menu = load_image_include(menucpc_bmp, menucpc_bmp_size);// "gui/menucpc.bmp");
	if (menu == NULL)
		printf("Menu error: %s", SDL_GetError());

#ifndef NOTPI
if (WhichPI()>128)
        {
	portada= load_image_include(portada_RPI2_bmp,portada_RPI2_bmp_size );//"gui/portada.bmp"); 
        }else
        {
	portada= load_image_include(portada_RPI1_bmp,portada_RPI1_bmp_size );//"gui/portada.bmp"); 
        } 
#else
portada= load_image_include(portada_RPI_bmp,portada_RPI_bmp_size );//"gui/portada.bmp"); 
#endif

	if (portada == NULL)
		printf("Menu error: %s", SDL_GetError());

splash= load_image_include(portada_RPI_bmp,portada_RPI_bmp_size );//"gui/portada.bmp"); 
	if (splash == NULL)
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

	if (splash != NULL) SDL_FreeSurface( splash ); 

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
int which_joy;
   switch (kevent.type) {

  
//KEYS

//////JOY START

		case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:

		pcjoy_update (kevent);
		if (kevent.type==SDL_JOYAXISMOTION)
			which_joy=kevent.jaxis.which;
			else
			which_joy=kevent.jbutton.which;

		if (pcjoy_pressed (PC_JOY_UP,which_joy))
			{		//Action UP
                        if (!(vkey_pressed)) vkey_move=VKEY_UP ;

			break;
			};
		if (pcjoy_pressed (PC_JOY_DOWN,which_joy))
			{		//Action DOWN
                        if (!(vkey_pressed)) vkey_move=VKEY_DOWN ;

			break;
			};
		if (pcjoy_pressed (PC_JOY_LEFT,which_joy))
			{		//Action LEFT
                        if (!(vkey_pressed)) vkey_move=VKEY_LEFT ;

			break;
			};
		if (pcjoy_pressed (PC_JOY_RIGHT,which_joy))
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
