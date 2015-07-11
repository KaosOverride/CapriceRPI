/*

File manager from NK's Snes 9x port to GP2x and adapted to Caprice GP2x by KaosOverride

Menu System for CapriceGP2x by KaosOverride based on NK's file manager GUI

*/


typedef struct {
        int xdesp;
        int xlong;
        int keyval;
} t_vkey;

t_vkey virtualkeys [5][18]=
{
 {   //row 0,  ESC,1,2,3...F7,F8,F9
	{ 0       , 18    , 0x82 } ,  //ESC
	{ 0       , 18    , 0x80 } ,  //1
	{ 0       , 18    , 0x81 } ,  //2
	{ 0       , 18    , 0x71 } ,  //3
	{ 0       , 18    , 0x70 } ,  //4
	{ 0       , 18    , 0x61 } ,  //5
	{ 0       , 18    , 0x60 } ,  //6
	{ 0       , 18    , 0x51 } ,  //7
	{ 0       , 18    , 0x50 } ,  //8
	{ 0       , 18    , 0x41 } ,  //9
	{ 0       , 18    , 0x40 } ,  //0
	{ 0       , 18    , 0x31 } ,  //'
	{ 0       , 18    , 0x30 } ,  //¡
	{ 0       , 18    , 0x97 } ,  //<-
	{ 0       , 18    , 0x20 } ,  //->
	{ 0       , 18    , 0x12 } ,  //F7
	{ 0       , 18    , 0x13 } ,  //F8
	{ 0       , 18    , 0x03 } ,  //F9
},{
     //row 1,  TAB,Q,W,E...F4,F5,F6
	{ 0       , 26    , 0x84 } ,  //TAB
	{ 8       , 18    , 0x83 } ,  //Q
	{ 8       , 18    , 0x73 } ,  //W
	{ 8       , 18    , 0x72 } ,  //E
	{ 8       , 18    , 0x62 } ,  //R
	{ 8       , 18    , 0x63 } ,  //T
	{ 8       , 18    , 0x53 } ,  //Y
	{ 8       , 18    , 0x52 } ,  //U
	{ 8       , 18    , 0x43 } ,  //I
	{ 8       , 18    , 0x42 } ,  //O
	{ 8       , 18    , 0x33 } ,  //P
	{ 8       , 18    , 0x32 } ,  //@
	{ 8       , 18    , 0x23 } ,  //[
	{ 8       , 27    , 0x22 } ,  //RET
	{ 8       , 4     , 0x22 } ,  //NULLLLLLLLLLLLL
	{ 0       , 18    , 0x24 } ,  //F4
	{ 0       , 18    , 0x14 } ,  //F5
	{ 0       , 18    , 0x04 } ,  //F6
},{
	{ 0       , 33    , 0xff } ,//row 2,  CAPS LOCK,A,S,D...F1,F2,F3
	{ 15      , 18    , 0x85 } ,//A
	{ 15      , 18    , 0x74 } ,//S
	{ 15      , 18    , 0x75 } ,//D
	{ 15      , 18    , 0x65 } ,//F
	{ 15      , 18    , 0x64 } ,//G
	{ 15      , 18    , 0x54 } ,//H
	{ 15      , 18    , 0x55 } ,//J
	{ 15      , 18    , 0x45 } ,//K
	{ 15      , 18    , 0x44 } ,//L
	{ 15      , 18    , 0x35 } ,//:
	{ 15      , 18    , 0x34 } ,//;
	{ 15      , 18    , 0x21 } ,//[
	{ 15      , 20    , 0x22 } ,//RETURN
	{ 15      , 4     , 0x22 } ,//RETURN
	{ 0       , 18    , 0x24 } ,//F4
	{ 0       , 18    , 0x14 } ,//F5
	{ 0       , 18    , 0x05 } ,//F6
},{
	{ 0       , 40    , 0xff } ,//row 2,  CAPS LOCK,A,S,D...F1,F2,F3
	{ 22      , 18    , 0x87 } ,//Z
	{ 22      , 18    , 0x77 } ,//X
	{ 22      , 18    , 0x76 } ,//C
	{ 22      , 18    , 0x67 } ,//V
	{ 22      , 18    , 0x66 } ,//B
	{ 22      , 18    , 0x56 } ,//N
	{ 22      , 18    , 0x46 } ,//M
	{ 22      , 18    , 0x47 } ,//<
	{ 22      , 18    , 0x37 } ,//>
	{ 22      , 18    , 0x36 } ,// 
	{ 22      , 18    , 0x26 } ,// /
	{ 22      , 30    , 0xff } ,//RSHIFT
	{ 22      , 18    , 0xff } ,//NOEXIST
	{ 22      , 18    , 0xff } ,//NOEXIST
	{ 0       , 18    , 0x17 } ,//ZERO
	{ 0       , 18    , 0x00 } ,//UP
	{ 0       , 18    , 0x07 } ,//PERIOD

},{
	{ 0       , 40    , 0x00 } ,//row 4,  CTRL,SPACE,...LEFT,DOWN,RIGHT
	{ 22      , 31    , 0x11 } , //COPY
	{ 36      , 153   , 0x57 } ,   //SPACE
	{ 171     , 35    , 0x06 } , //INTRO
	{ 22      , 18    , 0xff } , // NOEXIST
	{ 22      , 18    , 0xff } , // NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 22      , 18    , 0xff } ,// NOEXIST
	{ 0       , 18    , 0x10 } ,//LEFT
	{ 0       , 18    , 0x02 } ,//DOWN
	{ 0       , 18    , 0x01 } //RIGHT
}
};



