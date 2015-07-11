#include "vjoystick.h"


int pcjoy_matrix[4][3]= {
	{0,0,0},//UPS
	{0,0,0},//MIDDLES
	{0,0,0},//DOWNS
	{0,0,0},//FIRES
	};

int pcjoy_matrix2[4][3]= {
	{0,0,0},
	{0,0,0},
	{0,0,0},
	{0,0,0},
	};

 void pcjoy_reset (void)
{
	memset(pcjoy_matrix, 0, sizeof(pcjoy_matrix)); 
	memset(pcjoy_matrix2, 0, sizeof(pcjoy_matrix2)); 
}


void pcjoy_update (SDL_Event gevent )
{
int pcjoy_move=0;
if (gevent.type ==	SDL_JOYBUTTONDOWN) pcjoy_move=1;
	

if (gevent.type ==	SDL_JOYAXISMOTION)
	{     //DO AXIX MOTIONS

//	if ( ( event.jaxis.value < -3000 ) || (event.jaxis.value > 3000 ) ) 
	switch (gevent.jaxis.which)
    {
			  //JOY1
			case 0:
        if( gevent.jaxis.axis == 1) 
        {
            /* Up-Down movement code goes here */

						//_UP:
						pcjoy_matrix [0][1] = (gevent.jaxis.value < -3000);
						//DOWN:
						pcjoy_matrix [2][1] = (gevent.jaxis.value > 3000);
  	
				}

        if( gevent.jaxis.axis == 0) 
        {
            /* Left-right movement code goes here */
  
						//_LEFT:
						pcjoy_matrix [1][0] = (gevent.jaxis.value < -3000);
						//RIGHT:
						pcjoy_matrix [1][2] = (gevent.jaxis.value > 3000);

				}
				break;
				
				//JOY2
			case 1:
        if( gevent.jaxis.axis == 1) 
        {
            /* Up-Down movement code goes here */

						//_UP:
						pcjoy_matrix2 [0][1] = (gevent.jaxis.value < -3000);
						//DOWN:
						pcjoy_matrix2 [2][1] = (gevent.jaxis.value > 3000);
  	
				}

        if( gevent.jaxis.axis == 0) 
        {
            /* Left-right movement code goes here */
  
						//_LEFT:
						pcjoy_matrix2 [1][0] = (gevent.jaxis.value < -3000);
						//RIGHT:
						pcjoy_matrix2 [1][2] = (gevent.jaxis.value > 3000);

				}
				break;
    }
		
	}
	else
	{
	
	switch (gevent.jbutton.which)
		{

	        printf("-Joy%d : Button %d Action %d\n", gevent.jbutton.which,gevent.jbutton.button , pcjoy_move);

			case 0: //Joy 1
					switch (gevent.jbutton.button)
					{ 

						case SDL_JoyFire1:
							pcjoy_matrix [3][0] = pcjoy_move;
							break;
						case SDL_JoyFire2:
							pcjoy_matrix [3][1] = pcjoy_move;
							break;
						case SDL_JoyFire3:
							pcjoy_matrix [3][2] = pcjoy_move;
							break;
					}
					break;
			case 1: //Joy 2
					switch (gevent.jbutton.button)
					{ 

						case SDL_JoyFire1:
							pcjoy_matrix2 [3][0] = pcjoy_move;
							break;
						case SDL_JoyFire2:
							pcjoy_matrix2 [3][1] = pcjoy_move;
							break;
						case SDL_JoyFire3:
							pcjoy_matrix2 [3][2] = pcjoy_move;
							break;


					}
					break;
		}
	}
/*  sys_printf("Evento: %d\n\n",pcjoy_move);*/
}



int pcjoy_pressed  (int pcjoy_dir)
{
	switch ( pcjoy_dir )
	{
	case PC_JOY1_FIRE1:
		{
		return (pcjoy_matrix [3][0]);
		break;
		}
	case PC_JOY1_FIRE2:
		{
		return (pcjoy_matrix [3][1]);
		break;
		}
	case PC_JOY1_FIRE3:
		{
		return (pcjoy_matrix [3][2]);
		break;
		}
	case PC_JOY1_UP:
		{
		return ( (pcjoy_matrix [0][0])  | (pcjoy_matrix[0][1]) | (pcjoy_matrix[0][2]) ); 
		break;
		}
	case PC_JOY1_DOWN:
		{
		return ( (pcjoy_matrix [2][0]) | (pcjoy_matrix[2][1]) | ( pcjoy_matrix[2][2]) ); 
		break;
		}
	case PC_JOY1_RIGHT:
		{
		return ( (pcjoy_matrix [0][2]) | (pcjoy_matrix[1][2]) | (pcjoy_matrix[2][2]) ); 
		break;
		}
	case PC_JOY1_LEFT:
		{
		return ( (pcjoy_matrix [0][0])  | (pcjoy_matrix[1][0]) | (pcjoy_matrix[2][0]) ); 
		break;
		}

		//JOY 2

	case PC_JOY2_FIRE1:
		{
		return (pcjoy_matrix2 [3][0]);
		break;
		}
	case PC_JOY2_FIRE2:
		{
		return (pcjoy_matrix2 [3][1]);
		break;
		}
	case PC_JOY2_FIRE3:
		{
		return (pcjoy_matrix2 [3][2]);
		break;
		}

	case PC_JOY2_UP:
		{
		return ( (pcjoy_matrix2 [0][0])  | (pcjoy_matrix2[0][1]) | (pcjoy_matrix2[0][2]) ); 
		break;
		}
	case PC_JOY2_DOWN:
		{
		return ( (pcjoy_matrix2 [2][0]) | (pcjoy_matrix2[2][1]) | ( pcjoy_matrix2[2][2]) ); 
		break;
		}
	case PC_JOY2_RIGHT:
		{
		return ( (pcjoy_matrix2 [0][2]) | (pcjoy_matrix2[1][2]) | (pcjoy_matrix2[2][2]) ); 
		break;
		}
	case PC_JOY2_LEFT:
		{
		return ( (pcjoy_matrix2 [0][0])  | (pcjoy_matrix2[1][0]) | (pcjoy_matrix2[2][0]) ); 
		break;
		}





	}
	return 0; 
}

	/*
		case SDL_JOYAXISMOTION:
		case SDL_JOYBUTTONUP:
		case SDL_JOYBUTTONDOWN:





       	case 6://GP2X_BUTTON_UP:
		pcjoy_matrix2 [1][0] = pcjoy_move;
        	break;
        case 7://GP2X_BUTTON_DOWN:
		pcjoy_matrix2 [1][2] = pcjoy_move;
        	break;
       	case 8://GP2X_BUTTON_LEFT:
		pcjoy_matrix2 [0][1] = pcjoy_move;
                break;
        case 9://GP2X_BUTTON_RIGHT:
		pcjoy_matrix2 [2][1] = pcjoy_move;
        	break;
       	case 10://GP2X_BUTTON_UPLEFT:
		pcjoy_matrix2 [0][0] = pcjoy_move;
        	break;
        case 11://GP2X_BUTTON_UPRIGHT:
		pcjoy_matrix2 [2][0] = pcjoy_move;
        	break;
       	case 12://GP2X_BUTTON_DOWNLEFT:
		pcjoy_matrix2 [0][2] = pcjoy_move;
        	break;
        case 13://GP2X_BUTTON_DOWNRIGHT:
		pcjoy_matrix2 [2][2] = pcjoy_move;
        	break;





	*/


		//--------------------------------------------------------------------------------------
		//                  Aqui a cabamos mi joy real -> joy virtual
		//--------------------------------------------------------------------------------------
