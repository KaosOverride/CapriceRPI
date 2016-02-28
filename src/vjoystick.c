#include "vjoystick.h"


int CPC_Joys[4];
int CPC_Joystate[8]={0,0,0,0,0,0,0,0};
char CPC_Joynames[8][12]={
        "CPC J1",
        "CPC J2",
        "KEY ROW7",
        "KEY ROW5",
        "Multi J3",
        "Multi J4",
        "OPQA KEYS",
        "CURSOR KEYS"};



typedef struct {
	void (*Up) (int value);
	void (*Down) (int value);
	void (*Left) (int value);
	void (*Right) (int value);
	void (*Fire1) (int value);
	void (*Fire2) (int value);
	void (*Fire3) (int value);
} JoyHandler;

JoyHandler JoyFuncs[4];

int pcjoy_matrix[4][4][3]= {
	//Joy 1
	{{0,0,0},//UPS
	{0,0,0},//MIDDLES
	{0,0,0},//DOWNS
	{0,0,0}},//FIRES
	//Joy 2
	{{0,0,0},//UPS
	{0,0,0},//MIDDLES
	{0,0,0},//DOWNS
	{0,0,0}},//FIRES
	//Joy 3
	{{0,0,0},//UPS
	{0,0,0},//MIDDLES
	{0,0,0},//DOWNS
	{0,0,0}},//FIRES
	//Joy 4
	{{0,0,0},//UPS
	{0,0,0},//MIDDLES
	{0,0,0},//DOWNS
	{0,0,0}}//FIRES


	};




 void pcjoy_reset (void)
{
	memset(pcjoy_matrix, 0, sizeof(pcjoy_matrix)); 
}


void pcjoy_update (SDL_Event gevent )
{
int pcjoy_move=0;
if (gevent.type ==	SDL_JOYBUTTONDOWN) pcjoy_move=1;
	

if (gevent.type ==	SDL_JOYAXISMOTION)
	{     //DO AXIX MOTIONS

//	if ( ( event.jaxis.value < -3000 ) || (event.jaxis.value > 3000 ) ) 

        if( gevent.jaxis.axis == 1) 
        {
            /* Up-Down movement code goes here */

			//_UP:
			pcjoy_matrix [gevent.jaxis.which][0][1] = (gevent.jaxis.value < -3000);
			//DOWN:
			pcjoy_matrix [gevent.jaxis.which][2][1] = (gevent.jaxis.value > 3000);
	}

        if( gevent.jaxis.axis == 0) 
        {
            /* Left-right movement code goes here */

			//_LEFT:
			pcjoy_matrix [gevent.jaxis.which][1][0] = (gevent.jaxis.value < -3000);
			//RIGHT:
			pcjoy_matrix [gevent.jaxis.which][1][2] = (gevent.jaxis.value > 3000);

	}



  }
else
  {
//printf("-Joy%d : Button %d Action %d\n", gevent.jbutton.which,gevent.jbutton.button , pcjoy_move);
	

		switch (gevent.jbutton.button)
		{ 

			case SDL_JoyFire1:
				pcjoy_matrix [gevent.jbutton.which][3][0] = pcjoy_move;
				break;
			case SDL_JoyFire2:
				pcjoy_matrix [gevent.jbutton.which][3][1] = pcjoy_move;
				break;
			case SDL_JoyFire3:
				pcjoy_matrix [gevent.jbutton.which][3][2] = pcjoy_move;
				break;
		}
  }
/*  sys_printf("Evento: %d\n\n",pcjoy_move);*/
}



int pcjoy_pressed  (int pcjoy_dir, int which)
{
	switch ( pcjoy_dir )
	{
	case PC_JOY_FIRE1:
		{
		return (pcjoy_matrix [which][3][0]);
		break;
		}
	case PC_JOY_FIRE2:
		{
		return (pcjoy_matrix [which][3][1]);
		break;
		}
	case PC_JOY_FIRE3:
		{
		return (pcjoy_matrix [which][3][2]);
		break;
		}
	case PC_JOY_UP:
		{
		return ( (pcjoy_matrix [which][0][0])  | (pcjoy_matrix[which][0][1]) | (pcjoy_matrix[which][0][2]) ); 
		break;
		}
	case PC_JOY_DOWN:
		{
		return ( (pcjoy_matrix [which][2][0]) | (pcjoy_matrix[which][2][1]) | ( pcjoy_matrix[which][2][2]) ); 
		break;
		}
	case PC_JOY_RIGHT:
		{
		return ( (pcjoy_matrix [which][0][2]) | (pcjoy_matrix[which][1][2]) | (pcjoy_matrix[which][2][2]) ); 
		break;
		}
	case PC_JOY_LEFT:
		{
		return ( (pcjoy_matrix [which][0][0])  | (pcjoy_matrix[which][1][0]) | (pcjoy_matrix[which][2][0]) ); 
		break;
		}

	}
  return 0; 
}


/*								*/
/*		Joystick modes					*/
/*								*/

//  ALPHA MODE

void Joy_Alpha_Up (int value)
{
   if ( value ) {cpc_key_press (0x90); } else {cpc_key_unpress (0x90);};
}

void Joy_Alpha_Down (int value)
{
   if ( value ) {cpc_key_press (0x91); } else {cpc_key_unpress (0x91);};
}

void Joy_Alpha_Left (int value)
{
   if ( value ) {cpc_key_press (0x92); } else {cpc_key_unpress (0x92);};
}

void Joy_Alpha_Right (int value)
{
   if ( value ) {cpc_key_press (0x93); } else {cpc_key_unpress (0x93);};
}

void Joy_Alpha_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x94); } else {cpc_key_unpress (0x94);};
}

void Joy_Alpha_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x95); } else {cpc_key_unpress (0x95);};
}

void Joy_Alpha_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x96); } else {cpc_key_unpress (0x96);};
}

// BETA MODE


void Joy_Beta_Up (int value)
{
   if ( value ) {cpc_key_press (0x60); } else {cpc_key_unpress (0x60);};
}

void Joy_Beta_Down (int value)
{
   if ( value ) {cpc_key_press (0x61); } else {cpc_key_unpress (0x61);};
}

void Joy_Beta_Left (int value)
{
   if ( value ) {cpc_key_press (0x62); } else {cpc_key_unpress (0x62);};
}

void Joy_Beta_Right (int value)
{
   if ( value ) {cpc_key_press (0x63); } else {cpc_key_unpress (0x63);};
}

void Joy_Beta_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x64); } else {cpc_key_unpress (0x64);};
}

void Joy_Beta_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x65); } else {cpc_key_unpress (0x65);};
}

void Joy_Beta_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x66); } else {cpc_key_unpress (0x66);};
}

// GAMMA MODE


void Joy_Gamma_Up (int value)
{
   if ( value ) {cpc_key_press (0x70); } else {cpc_key_unpress (0x70);};
}

void Joy_Gamma_Down (int value)
{
   if ( value ) {cpc_key_press (0x71); } else {cpc_key_unpress (0x71);};
}

void Joy_Gamma_Left (int value)
{
   if ( value ) {cpc_key_press (0x72); } else {cpc_key_unpress (0x72);};
}

void Joy_Gamma_Right (int value)
{
   if ( value ) {cpc_key_press (0x73); } else {cpc_key_unpress (0x73);};
}

void Joy_Gamma_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x74); } else {cpc_key_unpress (0x74);};
}

void Joy_Gamma_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x75); } else {cpc_key_unpress (0x75);};
}

void Joy_Gamma_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x76); } else {cpc_key_unpress (0x76);};
}

// OMEGA MODE


void Joy_Omega_Up (int value)
{
   if ( value ) {cpc_key_press (0x50); } else {cpc_key_unpress (0x50);};
}

void Joy_Omega_Down (int value)
{
   if ( value ) {cpc_key_press (0x51); } else {cpc_key_unpress (0x51);};
}

void Joy_Omega_Left (int value)
{
   if ( value ) {cpc_key_press (0x52); } else {cpc_key_unpress (0x52);};
}

void Joy_Omega_Right (int value)
{
   if ( value ) {cpc_key_press (0x53); } else {cpc_key_unpress (0x53);};
}

void Joy_Omega_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x54); } else {cpc_key_unpress (0x54);};
}

void Joy_Omega_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x55); } else {cpc_key_unpress (0x55);};
}

void Joy_Omega_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x56); } else {cpc_key_unpress (0x56);};
}


// TOTO3

void Joy_ToTo3_Up (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x01;} else {ToToJoy3=ToToJoy3 & 0xFE;};
}

void Joy_ToTo3_Down (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x02;} else {ToToJoy3=ToToJoy3 & 0xFD;};
}

void Joy_ToTo3_Left (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x04;} else {ToToJoy3=ToToJoy3 & 0xFB;};
}

void Joy_ToTo3_Right (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x08;} else {ToToJoy3=ToToJoy3 & 0xF7;};
}

void Joy_ToTo3_Fire1 (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x10;} else {ToToJoy3=ToToJoy3 & 0xEF;};
}

void Joy_ToTo3_Fire2 (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x20;} else {ToToJoy3=ToToJoy3 & 0xDF;};
}

void Joy_ToTo3_Fire3 (int value)
{
if ( value ) {ToToJoy3=ToToJoy3 | 0x40;} else {ToToJoy3=ToToJoy3 & 0xBF;};
}

// TOTO4


void Joy_ToTo4_Up (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x01;} else {ToToJoy4=ToToJoy4 & 0xFE;};
}

void Joy_ToTo4_Down (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x02;} else {ToToJoy4=ToToJoy4 & 0xFD;};
}

void Joy_ToTo4_Left (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x04;} else {ToToJoy4=ToToJoy4 & 0xFB;};
}

void Joy_ToTo4_Right (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x08;} else {ToToJoy4=ToToJoy4 & 0xF7;};
}

void Joy_ToTo4_Fire1 (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x10;} else {ToToJoy4=ToToJoy4 & 0xEF;};
}

void Joy_ToTo4_Fire2 (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x20;} else {ToToJoy4=ToToJoy4 & 0xDF;};
}

void Joy_ToTo4_Fire3 (int value)
{
if ( value ) {ToToJoy4=ToToJoy4 | 0x40;} else {ToToJoy4=ToToJoy4 & 0xBF;};
}

// OPQA MODE


void Joy_Opqa_Up (int value)
{
   if ( value ) {cpc_key_press (0x83); } else {cpc_key_unpress (0x83);};
}

void Joy_Opqa_Down (int value)
{
   if ( value ) {cpc_key_press (0x85); } else {cpc_key_unpress (0x85);};
}

void Joy_Opqa_Left (int value)
{
   if ( value ) {cpc_key_press (0x42); } else {cpc_key_unpress (0x42);};
}

void Joy_Opqa_Right (int value)
{
   if ( value ) {cpc_key_press (0x33); } else {cpc_key_unpress (0x33);};
}

void Joy_Opqa_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x57); } else {cpc_key_unpress (0x57);};
}

void Joy_Opqa_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x27); } else {cpc_key_unpress (0x27);};
}

void Joy_Opqa_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x11); } else {cpc_key_unpress (0x11);};
}

// CURSOR MODE


void Joy_Cursor_Up (int value)
{
   if ( value ) {cpc_key_press (0x00); } else {cpc_key_unpress (0x00);};
}

void Joy_Cursor_Down (int value)
{
   if ( value ) {cpc_key_press (0x02); } else {cpc_key_unpress (0x02);};
}

void Joy_Cursor_Left (int value)
{
   if ( value ) {cpc_key_press (0x10); } else {cpc_key_unpress (0x10);};
}

void Joy_Cursor_Right (int value)
{
   if ( value ) {cpc_key_press (0x01); } else {cpc_key_unpress (0x01);};
}

void Joy_Cursor_Fire1 (int value)
{
   if ( value ) {cpc_key_press (0x22); } else {cpc_key_unpress (0x22);};
}

void Joy_Cursor_Fire2 (int value)
{
   if ( value ) {cpc_key_press (0x06); } else {cpc_key_unpress (0x06);};
}

void Joy_Cursor_Fire3 (int value)
{
   if ( value ) {cpc_key_press (0x17); } else {cpc_key_unpress (0x17);};
}




////////////////////////////////////////////////////////

void pcjoy_assign_Alpha (int joy)
{
	JoyFuncs[joy].Up	=Joy_Alpha_Up;
	JoyFuncs[joy].Down	=Joy_Alpha_Down;
	JoyFuncs[joy].Left	=Joy_Alpha_Left;
	JoyFuncs[joy].Right	=Joy_Alpha_Right;
	JoyFuncs[joy].Fire1	=Joy_Alpha_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Alpha_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Alpha_Fire3;
	CPC_Joys[joy]=JMODE_ALPHA;
	CPC_Joystate[JMODE_ALPHA]=1;
}

void pcjoy_assign_Beta (int joy)
{
	JoyFuncs[joy].Up	=Joy_Beta_Up;
	JoyFuncs[joy].Down	=Joy_Beta_Down;
	JoyFuncs[joy].Left	=Joy_Beta_Left;
	JoyFuncs[joy].Right	=Joy_Beta_Right;
	JoyFuncs[joy].Fire1	=Joy_Beta_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Beta_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Beta_Fire3;
	CPC_Joys[joy]=JMODE_BETA;
	CPC_Joystate[JMODE_BETA]=1;
}

void pcjoy_assign_Gamma (int joy)
{
	JoyFuncs[joy].Up	=Joy_Gamma_Up;
	JoyFuncs[joy].Down	=Joy_Gamma_Down;
	JoyFuncs[joy].Left	=Joy_Gamma_Left;
	JoyFuncs[joy].Right	=Joy_Gamma_Right;
	JoyFuncs[joy].Fire1	=Joy_Gamma_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Gamma_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Gamma_Fire3;
	CPC_Joys[joy]=JMODE_GAMMA;
	CPC_Joystate[JMODE_GAMMA]=1;
}

void pcjoy_assign_Omega (int joy)
{
	JoyFuncs[joy].Up	=Joy_Omega_Up;
	JoyFuncs[joy].Down	=Joy_Omega_Down;
	JoyFuncs[joy].Left	=Joy_Omega_Left;
	JoyFuncs[joy].Right	=Joy_Omega_Right;
	JoyFuncs[joy].Fire1	=Joy_Omega_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Omega_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Omega_Fire3;
	CPC_Joys[joy]=JMODE_OMEGA;
	CPC_Joystate[JMODE_OMEGA]=1;
}

void pcjoy_assign_ToTo3 (int joy)
{
	JoyFuncs[joy].Up	=Joy_ToTo3_Up;
	JoyFuncs[joy].Down	=Joy_ToTo3_Down;
	JoyFuncs[joy].Left	=Joy_ToTo3_Left;
	JoyFuncs[joy].Right	=Joy_ToTo3_Right;
	JoyFuncs[joy].Fire1	=Joy_ToTo3_Fire1;
	JoyFuncs[joy].Fire2	=Joy_ToTo3_Fire2;
	JoyFuncs[joy].Fire3	=Joy_ToTo3_Fire3;
	CPC_Joys[joy]=JMODE_TOTO3;
	CPC_Joystate[JMODE_TOTO3]=1;
}


void pcjoy_assign_ToTo4 (int joy)
{
	JoyFuncs[joy].Up	=Joy_ToTo4_Up;
	JoyFuncs[joy].Down	=Joy_ToTo4_Down;
	JoyFuncs[joy].Left	=Joy_ToTo4_Left;
	JoyFuncs[joy].Right	=Joy_ToTo4_Right;
	JoyFuncs[joy].Fire1	=Joy_ToTo4_Fire1;
	JoyFuncs[joy].Fire2	=Joy_ToTo4_Fire2;
	JoyFuncs[joy].Fire3	=Joy_ToTo4_Fire3;
	CPC_Joys[joy]=JMODE_TOTO4;
	CPC_Joystate[JMODE_TOTO4]=1;
}

void pcjoy_assign_Cursor (int joy)
{
	JoyFuncs[joy].Up	=Joy_Cursor_Up;
	JoyFuncs[joy].Down	=Joy_Cursor_Down;
	JoyFuncs[joy].Left	=Joy_Cursor_Left;
	JoyFuncs[joy].Right	=Joy_Cursor_Right;
	JoyFuncs[joy].Fire1	=Joy_Cursor_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Cursor_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Cursor_Fire3;
	CPC_Joys[joy]=JMODE_CURSOR;
	CPC_Joystate[JMODE_CURSOR]=1;
}

void pcjoy_assign_Opqa (int joy)
{
	JoyFuncs[joy].Up	=Joy_Opqa_Up;
	JoyFuncs[joy].Down	=Joy_Opqa_Down;
	JoyFuncs[joy].Left	=Joy_Opqa_Left;
	JoyFuncs[joy].Right	=Joy_Opqa_Right;
	JoyFuncs[joy].Fire1	=Joy_Opqa_Fire1;
	JoyFuncs[joy].Fire2	=Joy_Opqa_Fire2;
	JoyFuncs[joy].Fire3	=Joy_Opqa_Fire3;
	CPC_Joys[joy]=JMODE_OPQA;
	CPC_Joystate[JMODE_OPQA]=1;
}



void pcjoy_assign(int joy, int mode)
{

switch (mode)
	{
	case JMODE_ALPHA:
		pcjoy_assign_Alpha(joy);
		break;
	case JMODE_BETA:
		pcjoy_assign_Beta (joy);
		break;
	case JMODE_GAMMA:
		pcjoy_assign_Gamma (joy);
		break;
	case JMODE_OMEGA:
		pcjoy_assign_Omega (joy);
		break;
	case JMODE_TOTO3:
		pcjoy_assign_ToTo3(joy);
		break;
	case JMODE_TOTO4:
		pcjoy_assign_ToTo4(joy);
		break;
	case JMODE_CURSOR:
		pcjoy_assign_Cursor (joy);
		break;
	case JMODE_OPQA:
		pcjoy_assign_Opqa (joy);
		break;

	default:
		break;
	}

}

void pcjoy_init (void)
{
	pcjoy_reset();
	pcjoy_assign(0,JMODE_ALPHA);
	pcjoy_assign(1,JMODE_BETA);
	pcjoy_assign(2,JMODE_TOTO3);
	pcjoy_assign(3,JMODE_TOTO4);
}

