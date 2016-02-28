/* Joystick modes    */

#define JMODE_ALPHA	0	//JOY 1 keyboard matrix 
#define JMODE_BETA	1	//JOY 2 keyboard matrix
#define JMODE_GAMMA	2	//OTHER line keyboard matrix
#define JMODE_OMEGA	3	//AND OTHER line keyboard matrix
#define JMODE_TOTO3	4	//ToTo's Joy 3
#define JMODE_TOTO4	5	//ToTo's Joy 4
#define JMODE_OPQA	6	//OPQA keys
#define JMODE_CURSOR	7	//Cursor keys

#define SDL_JoyFire1		0
#define SDL_JoyFire2		1
#define SDL_JoyFire3		2
#define SDL_JoyFire4		3
#define SDL_JoyFire5		4
#define SDL_JoyFire6		5
//#define SDL_JoyFire7		6

/*// PC virtual Joystick
*/
#define PC_JOY_UP		1
#define PC_JOY_DOWN		2
#define PC_JOY_LEFT		3
#define PC_JOY_RIGHT		4
#define PC_JOY_FIRE1		5
#define PC_JOY_FIRE2		6
#define PC_JOY_FIRE3		7



/*		//--------------------------------------------------------------------------------------
		//                  Aqui empieza mi joy real -> joy virtual
		//--------------------------------------------------------------------------------------

*/




void pcjoy_reset (void);
void pcjoy_update ( SDL_Event gevent);
int pcjoy_pressed (int pcjoy_dir, int which);   
