

#define SDL_JoyFire1		0
#define SDL_JoyFire2		1
#define SDL_JoyFire3		2
#define SDL_JoyFire4		3
#define SDL_JoyFire5		4
#define SDL_JoyFire6		5
//#define SDL_JoyFire7		6

/*// PC virtual Joystick
*/
#define PC_JOY1_UP		1
#define PC_JOY1_DOWN		2
#define PC_JOY1_LEFT		3
#define PC_JOY1_RIGHT		4
#define PC_JOY1_FIRE1		5
#define PC_JOY1_FIRE2		6
#define PC_JOY1_FIRE3		7

#define PC_JOY2_UP		8
#define PC_JOY2_DOWN		9
#define PC_JOY2_LEFT		10
#define PC_JOY2_RIGHT		11
#define PC_JOY2_FIRE1		12
#define PC_JOY2_FIRE2		13
#define PC_JOY2_FIRE3		14

#define PC_JOY3_UP		15
#define PC_JOY3_DOWN		16
#define PC_JOY3_LEFT		17
#define PC_JOY3_RIGHT		18
#define PC_JOY3_FIRE1		19
#define PC_JOY3_FIRE2		20
#define PC_JOY3_FIRE3		21

#define PC_JOY4_UP		22
#define PC_JOY4_DOWN		23
#define PC_JOY4_LEFT		24
#define PC_JOY4_RIGHT		25
#define PC_JOY4_FIRE1		26
#define PC_JOY4_FIRE2		27
#define PC_JOY4_FIRE3		28


/*		//--------------------------------------------------------------------------------------
		//                  Aqui empieza mi joy real -> joy virtual
		//--------------------------------------------------------------------------------------

*/




//int pcjoy_matrix [6][6];
void pcjoy_reset (void);
void pcjoy_update ( SDL_Event gevent);
//int pcjoy_pressed_davec2 (int pcjoy_dir);   
//int pcjoy_pressed_davec3 (int pcjoy_dir);   
int pcjoy_pressed (int pcjoy_dir);   
