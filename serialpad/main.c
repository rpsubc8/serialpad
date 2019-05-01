//Autor: ackerman
//EA1HLX
//Jaime Jose Gavin Sierra
/**
	a simple program for printing to the psx screen
*/

#include <sys/types.h>		//these are the various include files that need to be included
#include <libetc.h>		//note that the order they are included in does matter in some cases!
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <strings.h>
#include "pad.h"
/*
also note that while this example uses the Gs functions, these are high level functions
and should not be used if it can be helped as they are quite slow and consume lots of memory.
they will however do while you are learning as they simplify things and help you to understand
the process that you go through to print to the screen.
*/

#define OT_LENGTH (10)			//this is the 'length' of the OT (ordering tabble)
					//for more info on ordering tables, check out ot.txt on my 'info' page

#define PACKETMAX (2048)		//these are the graphics area contants
#define PACKETMAX2 (PACKETMAX*24)	//allows you to select how many gfx objects the psx should handle
					//in this particular example

#define SCREEN_WIDTH 320		//contants to determine the height and width of the screen resolution
#define SCREEN_HEIGHT 240

GsOT myOT[2];				//an array of 2 OT's this is how your double buffering is implemented
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH];
PACKET GPUPacketArea[2][PACKETMAX2];	//also 2 gfx packet areas for double buffering

u_long pad;
u_char datoPAD;
char cadPAD[]="00000000";
char letra=' ';
char bufferLetra[200]="\0";
u_char contBuffer=0;
u_char STD_antes=0;
u_char flag_cmd=0;
u_char flag_file=0;
u_char flag_sizefile=0;
u_char flag_datafile=0;
char cadLog[200]="\0";
u_long sizeFile=0;
u_long contDataFile=0;
char globalNewData=0;

/***************** prototypes *******************/
void InitGraphics(void);			//this method sets up gfx for printing to screen
void DisplayAll(int);			//this displays the contents of the OT
void HandlePad(void);
int main(void);

void pollCMDpad(){	
if (globalNewData==1)
{
 if (flag_cmd==0)
 {
  if (datoPAD==255){ flag_cmd=1; strcpy(cadLog,"CMD"); }
 }
 else
 {//Ya tenemos flag cmd
  if (flag_file==0)
  {
   if (datoPAD==2){ flag_file=1; strcpy(cadLog,"CMD FILE"); }
  }
  else
  { //Ya hay flagfile
   if (flag_sizefile==0)
   {
	 flag_sizefile=1; 
	 sizeFile=datoPAD;
	 sprintf(cadLog,"CMD FILE %d bytes",sizeFile);
   }
   else
   { //ya hay flagsizefile
    if (flag_datafile==0)
	{	  
	 flag_datafile=1;
	 contDataFile=0;
	 contDataFile++;
	 sprintf(cadLog,"CMD FILE %d bytes %d",sizeFile,contDataFile);
	}
	else
	{//ya hay flag_datafile
	 sprintf(cadLog,"CMD FILE %d bytes %d",sizeFile,contDataFile);
     contDataFile++;
	}
   }
  }
 }
  //letra=(char)(datoPAD);
}
}

/***************** functions ********************/
int main(void) {
	int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed
	
	InitGraphics();			//this method sets up gfx for printing to screen
	
	FntLoad(960, 256);		//this loads the font gfx to the vram
	FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc
						
	while (1) {				//infinite loop
		activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
		GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
		GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents
		
		//once the OT contents have been cleared you can add new objects to the OT
		//so that they will be displayed next time DisplayAll is called
		
		pollCMDpad(datoPAD);

        FntPrint("Serial PAD\n PAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);
		
		//FntPrint("Serial PAD\n PAD %s %d %d\nLetra %c %s",cadPAD,datoPAD,pad,letra,bufferLetra);
		//this just adds the string to the screen printing buffer
		//to actually print this to screen yu have to flush the contents of this buffer
		//this is done in displayall method
		
		DisplayAll(activeBuffer);		//this dispalys the OT contents to screen		
		HandlePad();					//this handles button presses on the controller pad 1
	}
	return 0;		//when program is finished return from it
}

void InitGraphics(void) {
	//this method sets up gfx for printing to screen
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0); //initialises the graphics system
	//no Gs* * * functions will work unless GsInitGraph() has been called
	GsDefDispBuff(0, 0 , 0, SCREEN_HEIGHT); //defines double buffer attributes
	//buffer 0's top left coordinate becomes (0,0) & buffer 1's coordinate becomes (0, y resolution)

	myOT[0].length = OT_LENGTH; //sets OT length for each OT
	myOT[1].length = OT_LENGTH;
	myOT[0].org = myOT_TAG[0]; //gets top address of GsOT_TAG table
	myOT[1].org = myOT_TAG[1];
	GsClearOt(0,0,&myOT[0]); //initialises ordering table
	GsClearOt(0,0,&myOT[1]);
	
	PadInit(0);	
}

void DisplayAll(int activeBuffer) {
	//this method contains all the functions needed to display the contents of the OT
	FntFlush(-1);				//flushes font buffers contents from buffer so that they can be printed to screen
	DrawSync(0);
	//this waits till the GPU has finished drawing, as GsSwapDispBuff will not work correctly if drawing is in progress
	VSync(0);					//gsswapdispbuff should be called after beginning a v-blank
	GsSwapDispBuff();				//swap display buffer
	GsSortClear(0,0,0,&myOT[activeBuffer]);		//clears screen to color (0,0,0) and sorts OT ready for drawing
	GsDrawOt(&myOT[activeBuffer]);			//draws the contents of OT to screen
}

/***********************************************/
void HandlePad(void) {
//L1 - W         (sync)
//L2 - E         Q7
//R1 - R         Q6
//R2 - T         Q5
//Triangulo - D  Q4
//Cuadrado - S   Q3
//Circulo - X    Q2
//X   - Z        Q1
//Start - V      Q0	
 u_char Q7,Q6,Q5,Q4,Q3,Q2,Q1,Q0,STD,aux; 
	//handles button presses
	pad=PadRead(0);
	if (pad&Pad1L1) STD=1; else STD=0;	
	if (pad&Pad1L2) Q7=1; else Q7=0;
	if (pad&Pad1R1) Q6=1; else Q6=0;
	if (pad&Pad1R2) Q5=1; else Q5=0;	
	if (pad&Pad1tri) Q4=1; else Q4=0;
	if (pad&Pad1sqr) Q3=1; else Q3=0;
	if (pad&Pad1crc) Q2=1; else Q2=0;
	if (pad&Pad1x) Q1=1; else Q1=0;
	if (pad&Pad1Start) Q0=1; else Q0=0;
	
	cadPAD[7]=Q0+48;
	cadPAD[6]=Q1+48;
	cadPAD[5]=Q2+48;
	cadPAD[4]=Q3+48;
	cadPAD[3]=Q4+48;
	cadPAD[2]=Q5+48;
	cadPAD[1]=Q6+48;
	cadPAD[0]=Q7+48;
	
    globalNewData=0;
	if (STD!=STD_antes)
    {
	 STD_antes=STD;
	 if (STD==1){
	  globalNewData=1;
	  aux=Q0+(Q1<<1)+(Q2<<2)+(Q3<<3)+(Q4<<4)+(Q5<<5)+(Q6<<6)+(Q7<<7);
	  datoPAD=aux;
	  bufferLetra[contBuffer]=aux;
	  contBuffer++;
	  bufferLetra[contBuffer]='\0';
	  if (contBuffer>150)
	   contBuffer=0;
	 }	
	}
	
//	if (pad&Pad1Left) ship.x-=2;		//directional buttons move ship sprite
//	if (pad&Pad1Right) ship.x+=2;
//	if (pad&Pad1Up) ship.y-=2;
//	if (pad&Pad1Down) ship.y+=2;
//if (pad&Pad1R1)||(pad&Pad1L1)) {ship.scalex+=25; ship.scaley+=25;}	//top buttons make ship appear to move
//	if ((pad&Pad1R2)||(pad&Pad1L2)) {ship.scalex-=25; ship.scaley-=25;}	//toward and away from screen
//	if (pad&Pad1x) if(speed<15) speed++;
//	if (pad&Pad1crc) if(speed>1) speed--;
}
