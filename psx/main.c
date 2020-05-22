//Autor: ackerman
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
//#include "barco.h"
#include "raton.h"
#include "btnall.h"
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
//#define SCREEN_HEIGHT 240
#define SCREEN_HEIGHT 256


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
u_char flag_sizefile0=0;
u_char flag_sizefile1=0;
u_char flag_datafile=0;
char cadLog[200]="\0";
u_long sizeFile=0;
u_long contDataFile=0;
char globalNewData=0;
u_char bufferFile[65536];

u_char gb_Q7=0; //8 bits keyboard no strobe
u_char gb_Q6=0; //8 bits keyboard no strobe
u_char gb_Q5=0; //6 bits keyboard
u_char gb_Q4=0; //5 bits keyboard
u_char gb_Q3=0; //4 bits botones
u_char gb_Q2=0;
u_char gb_Q1=0;
u_char gb_Q0=0;
u_char gb_std=0;
u_char gb_std_antes=0;
u_char gb_cont_bit=0; //contador bits
char gb_cadKeyBuf[18]="";
int gb_contKeyBuf=0;

int gb_mouseX= SCREEN_WIDTH>>1;
int gb_mouseY= SCREEN_HEIGHT>>1;
int gb_mouse_btnLeft=0;
int gb_mouse_btnRight=0;
//int gb_contBtnBorrar=0;
int gb_mouse_Xequal=0;     //cont mismo movimiento X
int gb_mouse_Yequal=0;     //cont mismo movimiento Y
int gb_mouseXmoveBefore=0; //movimiento anterior x 0 1 -1
int gb_mouseYmoveBefore=0; //movimiento anterior y 0 1 -1

int gb_mouse_max_equalX = 16; 
int gb_mouse_max_equalY = 16;
int gb_mouse_max_incX = 4; //Inc X al repetirse movimiento X
int gb_mouse_max_incY = 4; //Inc Y al repetirse movimiento Y

#define tope_buffer 128000
#define tope_texto 1024
#define booleana unsigned char
#define TRUE 1
#define FALSE 0
//#define RATON_ADDRESS DecodificaBinario(&raton)
#define RATON_ADDRESS raton_array
//#define BTNFAST_ADDRESS DecodificaBinario(&btnfast)
#define BTNALL_ADDRESS btnall_array
unsigned char bufferAux[tope_buffer];
GsIMAGE	ratonImg;
GsSPRITE ratonSprite;

GsIMAGE	btnAllImg;
GsSPRITE btnFast;
GsSPRITE btnNormal;
GsSPRITE btnSlow;
GsSPRITE btnInvertX;
GsSPRITE btnInvertY;
GsSPRITE btnLeftHand;

u_char gb_pushBtnFast = 0;
u_char gb_pushBtnNormal = 0;
u_char gb_pushBtnSlow = 0;
u_char gb_pushBtnInvertX = 0;
u_char gb_pushBtnInvertY = 0;
u_char gb_pushBtnLeftHand = 0;

u_char gb_action_mouse_click = 0; //Realizar accion raton 

int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed

/***************** prototypes *******************/
void InitGraphics(void);		  //this method sets up gfx for printing to screen
void DisplayAll(int);			  //this displays the contents of the OT
void HandlePad(void);
void HandlePadTwoBtn(void);       //2 botones R1 y R2 convierte 14 botones
void HandleKeyboardTwoBtn(void);  //2 botones R1 y R2 convierte teclado
void HandleKeyboardFourBtn(void); //4 botones R1,R2,L1,L2 convierte teclado
void HandleKeyboardFourBtnNoStrobe(void); //4 botones R1,R2,L1,L2 sin Strobe convierte teclado
void HandleMouseFourBtn(void);    //4 botones R1,R2,L1,L2 convierte raton
void HandleMouseFourBtnNoStrobe(void);    //4 botones R1,R2,L1,L2 convierte raton rapido
void pollKeyboardTwoBtn(void);
void pollKeyboardFourBtn(void);
void pollKeyboardFourBtnNoStrobe(void);
void pollMouseFourBtn(void);
void pollMouseFourBtnNoStrobe(void);
void pollPADTwoBtn(void);
void pollCMDpad(void);
unsigned char *DecodificaBinario(struct TBinarioHex *dato);
void InitSprite(GsIMAGE *im, GsSPRITE *sp,int sxIni,int syIni,int sWidth,int sHeight);
void LoadAllSprites(void);
void PaintBtnAll(void);
void PaintMouse(void);
int main(void);


//***************************************************************************************
void InitSprite(GsIMAGE *im, GsSPRITE *sp,int sxIni,int syIni,int sWidth,int sHeight) 
{
	int bits;
	int widthCompression;
	RECT myRect;
		
	bits=im->pmode&0x03;
	if (bits==0) widthCompression=4;
	else if (bits==1) widthCompression=2;
	else if (bits==2) widthCompression=1;
	//else if (bits==3) printf("\nunsupported file format (24bit tim)!\n");
	
	myRect.x = im->px;
	myRect.y = im->py;	
	myRect.w = im->pw;
	myRect.h = im->ph;
	LoadImage( &myRect, im->pixel );		//loads image data to frame buffer
	
	
	//JJ ini   
/*    myRect.x = im->cx;                                    // x pos in frame buffer
    myRect.y = im->cy;                                    // y pos in frame buffer
    myRect.w = im->cw;                                    // width of CLUT
    myRect.h = im->ch;                                    // height of CLUT
    LoadImage(&myRect, im->clut);      	
	//sp->attribute = (bits<<24); // 16 bit CLUT, all options off (0x1 = 8-bit, 0x2 = 16-bit)
	sp->attribute = 0x1;
	sp->x = 0;                                         // draw at x coord 0
    sp->y = 0;                                         // draw at y coord 0
    sp->w = im->pw;                                       // width of sprite
    sp->h = im->ph;                               // height of sprite
	// texture page | texture mode (0 4-bit, 1 8-bit, 2 16-bit), semi-transparency rate, texture x, texture y in the framebuffer
	//0 4bitsclut 1 8bitsclut 2 16bitsclut
	//0,1,2,3 semitransparente
    sp->tpage=GetTPage(im->pmode, 0, im->px, im->py);
    sp->r = 128;                                       // RGB Data
    sp->g = 128;
    sp->b = 128;
    sp->u=0;                                           // position within timfile for sprite
    sp->v=0;                                           
    sp->cx = im->cx;                              // CLUT location x coord
    sp->cy = im->cy;                              // CLUT location y coord    
    sp->mx = 0;                                        // rotation x coord
    sp->my = 0;                                        // rotation y coord
    sp->scalex = ONE;                                  // scale x coord (ONE = 100%)
    sp->scaley = ONE;                                  // scale y coord (ONE = 100%)
    sp->rotate = 0;                                    // degrees to rotate   
	*/
	//JJ fin
	
	//printf("\nimage bit type =%d\n",bits);

	sp->attribute = (bits<<24);
	sp->x = SCREEN_WIDTH/2-((im->pw*widthCompression)/2);	//center sprite's x value
	sp->y = SCREEN_HEIGHT/2-im->ph/2;			//center sprite's y value	
	//sp->w = im->pw*widthCompression;
	//sp->h = im->ph;
	sp->w = sWidth;
	sp->h = sHeight;
	sp->tpage=GetTPage(bits, 0, im->px, im->py);	
	//sp->u=0;
	//sp->v=0;
    sp->u= sxIni;
	sp->v= syIni;	
	if (bits==0||bits==1) {
		//checks if image is 4 or 8 bit
		myRect.x = im->cx;
		myRect.y = im->cy;
		myRect.w = im->cw;
		myRect.h = im->ch;
		LoadImage( &myRect, im->clut );		//loads clut to frame buffer if needed
		sp->cx=im->cx;
		sp->cy=im->cy;
	}
	sp->r=128;
	sp->g=128;
	sp->b=128;
	sp->mx=0;//(im->pw*widthCompression)/2;
	sp->my=0;//im->ph/2;
	sp->scalex=ONE;
	sp->scaley=ONE;
	sp->rotate=0;
	
}

//********************************************************************************
unsigned char* DecodificaBinario(struct TBinarioHex *dato){
//Decodifica un buffer en formato RLE o sin codificar y devuelve el nuevo buffer
//Si no se puede decoficar nos da NULL
//Coger sin preguntar el bufferAux para los datos auxiliares
 unsigned int i,j,cont,inicio;
 unsigned char aux,color,baseColor,colorRepetido;
/* for (i=0;i<tope_buffer;i++)
  bufferAux[i]= 0;*/
// bzero(bufferAux,tope_buffer); //Ocupa mas byte k el for
 switch (dato->tipoCodificacion){
  case sin_codificacion: return (dato->buffer);break;
  case tim_rle4: inicio= 0x40; baseColor= 192; colorRepetido= 64; break;
  case tim_rle8: inicio= 0x220; baseColor= 192; colorRepetido= 64; break;
  case tim_rle4_2: inicio= 0x40; baseColor= 224; colorRepetido= 32; break;
  case tim_rle8_2: inicio= 0x220; baseColor= 224; colorRepetido= 32; break;
  case tim_rle4_3: inicio= 0x40; baseColor= 240; colorRepetido= 16; break;
  case tim_rle8_3: inicio= 0x220; baseColor= 240; colorRepetido= 16; break;
  case tim_rle4_4: inicio= 0x40; baseColor= 248; colorRepetido= 8; break;
  case tim_rle8_4: inicio= 0x220; baseColor= 248; colorRepetido= 8; break;  
  case bin_rle8: inicio= 0; baseColor= 192; colorRepetido= 64; break;
  case bin_rle8_2: inicio=0; baseColor= 224; colorRepetido= 32; break;
  case bin_rle8_3: inicio=0; baseColor= 240; colorRepetido= 16; break;
  case bin_rle8_4: inicio=0; baseColor= 248; colorRepetido= 8; break;
 }; 
 
//bcopy(bufferAux,dato->buffer,inicio);  //Ocupan mas bytes k el for
//memmove (bufferAux,dato->buffer,inicio);
 for (i=0;i<inicio;i++)
  bufferAux[i]= dato->buffer[i];
 i= inicio;
 j= inicio;
 while ((i<(dato->tamCodificado))&&(j<tope_buffer)){
  aux = dato->buffer[i];
  if (aux<baseColor){
   bufferAux[j]= aux;
   i++; j++;
  }
  else{
   inicio= aux-baseColor+1;
   if (inicio>colorRepetido) inicio= colorRepetido; //Por si acaso
   for (cont=0;cont<inicio;cont++){
    bufferAux[j]= dato->buffer[i+1];
    j++;
   }
   i+=2;
  } 
 }
 return (bufferAux);
}

//**********************************************************
void HandleMouseFourBtnNoStrobe()
{
 //R1,R2,L1,L2 bit Data 4 bits Mouse Fast
 //u_char auxBit0,auxBit1,auxBit2,auxBit3;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) gb_Q0=1; else gb_Q0=0;
 if (pad&Pad1R2) gb_Q1=1; else gb_Q1=0;
 if (pad&Pad1L1) gb_Q2=1; else gb_Q2=0;
 if (pad&Pad1L2) gb_Q3=1; else gb_Q3=0;
 
 if (gb_Q0==1 || gb_Q1==1 || gb_Q2==1 || gb_Q3==1) 
 {  
  //cadPAD[0]=gb_std + 48;
  cadPAD[3]=gb_Q0+48; 
  cadPAD[2]=gb_Q1+48;
  cadPAD[1]=gb_Q2+48;
  cadPAD[0]=gb_Q3+48;
  cadPAD[4]='\0'; //Muestro solo 4 bits
  globalNewData=1; //Nuevo dato  
 }
 else
 {
  cadPAD[3]=48;
  cadPAD[2]=48;
  cadPAD[1]=48;
  cadPAD[0]=48;
  cadPAD[4]='\0'; //Muestro solo 4 bits
 }
}

//****************************************************
void pollMouseFourBtnNoStrobe()
{
 //Convierte 4 bits raton
 u_char aux=0;
 char car=' ';
 char car2=' ';
  
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3);
  car =' ';
  if (gb_Q0 == 1 && gb_Q1 == 1)
  {
   car = 'L';
   if (gb_mouseXmoveBefore == -1)
   {
	gb_mouse_Xequal++;
	if (gb_mouse_Xequal>gb_mouse_max_equalX)	
	 gb_mouseX-= gb_mouse_max_incX;	
    else
	 gb_mouseX--;
   }
   else
   {
	gb_mouseXmoveBefore = -1;
	gb_mouse_Xequal = 0;
    gb_mouseX--;   
   }
  }
  else
  {
   if (gb_Q0 == 1 && gb_Q1 == 0)
   {
    car = 'R';
    if (gb_mouseXmoveBefore == 1)
    {
	 gb_mouse_Xequal++;
	 if (gb_mouse_Xequal>gb_mouse_max_equalX )	
	  gb_mouseX+= gb_mouse_max_incX;	
     else
	  gb_mouseX++;
    }
    else
    {
 	 gb_mouseXmoveBefore = 1;
	 gb_mouse_Xequal = 0;
     gb_mouseX++;   
    }	
   }
   else
   {
    if (gb_Q0 == 0 && gb_Q1 == 0)
    {
     car = ' ';
	 gb_mouseXmoveBefore = 0;
	 gb_mouse_Xequal=0;
    }  
   }
  }

  //Movimiento Y
  if (gb_Q2 == 1 && gb_Q3 == 1)
  {
   car2 = 'D';
   if (gb_mouseYmoveBefore == -1)
   {
	gb_mouse_Yequal++;
	if (gb_mouse_Yequal>gb_mouse_max_equalY)	
	 gb_mouseY+= gb_mouse_max_incY;	
    else
     gb_mouseY++;
   }
   else
   {
	gb_mouseYmoveBefore = -1;
	gb_mouse_Yequal = 0;
    gb_mouseY++;   
   }     
  }  
  if (gb_Q2 == 1 && gb_Q3 == 0)
  {
   car2 = 'U';
   if (gb_mouseYmoveBefore == 1)
   {
	gb_mouse_Yequal++;
	if (gb_mouse_Yequal>gb_mouse_max_equalY)	
	 gb_mouseY-= gb_mouse_max_incY;
    else
     gb_mouseY--;
   }
   else
   {
	gb_mouseYmoveBefore = 1;
	gb_mouse_Yequal = 0;
    gb_mouseY--;
   }      
  }
  if (gb_Q2 == 0 && gb_Q3 == 0)
  {
   car2 = ' ';
   gb_mouseYmoveBefore = 0;
   gb_mouse_Yequal=0;
  }
      
  //Botones
  if ((gb_Q0 == 0)&&(gb_Q1 == 1))
   gb_mouse_btnLeft = 1;
  else
   gb_mouse_btnLeft = 0;
  if ((gb_Q2 == 0)&&(gb_Q3 == 1))
   gb_mouse_btnRight = 1;  
  else
   gb_mouse_btnRight = 0;
  
  
  if (gb_mouseX<0)
   gb_mouseX=0;
  else
  {
   if (gb_mouseX>319)
    gb_mouseX=319;
  }
  if (gb_mouseY<0)
   gb_mouseY=0;
  else
  {
   if (gb_mouseY>199)
    gb_mouseY=199;
  }  
     
  gb_cadKeyBuf[gb_contKeyBuf]= car;
  gb_cadKeyBuf[gb_contKeyBuf+1]= car2;
  gb_cadKeyBuf[gb_contKeyBuf+2]= '\0';
  gb_contKeyBuf+=2;
  if (gb_contKeyBuf>1)
   gb_contKeyBuf=0;
  sprintf(cadLog,"%x %s X:%03d Y:%03d B:%d%d",aux,gb_cadKeyBuf,gb_mouseX,gb_mouseY,gb_mouse_btnLeft,gb_mouse_btnRight);
 }
 else
 {
//  gb_contBtnBorrar++;
//  if (gb_contBtnBorrar>50)
//  {
//   gb_contBtnBorrar=0;   
   gb_mouse_btnLeft = 0; //Reseteamos para lectura
   gb_mouse_btnRight = 0;
   sprintf(cadLog,"%x %s X:%03d Y:%03d B:%d%d",aux,gb_cadKeyBuf,gb_mouseX,gb_mouseY,gb_mouse_btnLeft,gb_mouse_btnRight);
//  }  
 }	
}

//**********************************************************
void HandleMouseFourBtn()
{
 //R1 Sync R2,L1,L2 bit Data First 3 bits Mouse, Second 3 bits R2,L1,L2
 u_char auxBit0,auxBit1,auxBit2;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) gb_std=1; else gb_std=0;
 if (pad&Pad1R2) auxBit0=1; else auxBit0=0;
 if (pad&Pad1L1) auxBit1=1; else auxBit1=0;
 if (pad&Pad1L2) auxBit2=1; else auxBit2=0;
 
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   cadPAD[0]=gb_std + 48;
   switch (gb_cont_bit)
   {
	 case 0: gb_Q0 = auxBit0;
	  gb_Q1 = auxBit1;
	  gb_Q2 = auxBit2;
	  cadPAD[7]=gb_Q0+48; 
	  cadPAD[6]=gb_Q1+48;
	  cadPAD[5]=gb_Q2+48;
	  break;
	 case 1: gb_Q3 = auxBit0;
	  gb_Q4 = auxBit1;
	  gb_Q5 = auxBit2;
	  cadPAD[4]=gb_Q3+48; 
	  cadPAD[3]=gb_Q4+48;
	  cadPAD[2]=gb_Q5+48;
	  break;
	 default: break;
   }
   gb_cont_bit++;
   if (gb_cont_bit>1)
   {
	gb_cont_bit=0;	
	globalNewData=1; //Nuevo dato
   }
  }
 } 
}

//**********************************************************
void pollMouseFourBtn()
{
 //Convierte 6 bits raton
 u_char aux=0;
 char car=' ';
 char car2=' ';
  
 if (globalNewData == 1)
 {
  globalNewData = 0;  	
  //aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4) | (gb_Q5<<5);
  car =' ';
  if (gb_Q0 == 1 && gb_Q1 == 1)
  {
   car = 'L';
   gb_mouseX--;
  }
  else
  {
   if (gb_Q0 == 1 && gb_Q1 == 0)
   {
    car = 'R';
	gb_mouseX++;
   }
   else
   {
    if (gb_Q0 == 0 && gb_Q1 == 0)
    {
     car = ' ';
    }  
   }
  }
  
  if (gb_Q2 == 1 && gb_Q3 == 1)
  {
   car2 = 'D';
   gb_mouseY++;
  }  
  if (gb_Q2 == 1 && gb_Q3 == 0)
  {
   car2 = 'U';
   gb_mouseY--;
  }
  if (gb_Q2 == 0 && gb_Q3 == 0)
  {
   car2 = ' ';
  }
    
  gb_mouse_btnLeft = gb_Q4;
  gb_mouse_btnRight = gb_Q5;  
  
  if (gb_mouseX<0)
   gb_mouseX=0;
  else
  {
   if (gb_mouseX>39)
    gb_mouseX=39;
  }
  if (gb_mouseY<0)
   gb_mouseY=0;
  else
  {
   if (gb_mouseY>24)
    gb_mouseY=24;
  }  
     
  gb_cadKeyBuf[gb_contKeyBuf]= car;
  gb_cadKeyBuf[gb_contKeyBuf+1]= car2;
  gb_cadKeyBuf[gb_contKeyBuf+2]= '\0';
  gb_contKeyBuf+=2;
  if (gb_contKeyBuf>16)
   gb_contKeyBuf=0;
  sprintf(cadLog,"MOUSE4BTN %d %c\n%s\nX:%d Y:%d Btn:%d%d",aux,car,gb_cadKeyBuf,gb_mouseX,gb_mouseY,gb_mouse_btnLeft,gb_mouse_btnRight);
 }
 else
 {
//  gb_contBtnBorrar++;
//  if (gb_contBtnBorrar>50)
//  {
//   gb_contBtnBorrar=0;   
   gb_mouse_btnLeft = 0; //Reseteamos para lectura
   gb_mouse_btnRight = 0;
   sprintf(cadLog,"MOUSE4BTN %d %c\n%s\nX:%d Y:%d Btn:%d%d",aux,car,gb_cadKeyBuf,gb_mouseX,gb_mouseY,gb_mouse_btnLeft,gb_mouse_btnRight);
//  }  
 }
}




//**********************************************************
void HandleKeyboardFourBtnNoStrobe()
{
 //R1,R2,L1,L2 bit Data First 4 bits Keyboard, Second 4 bits R1,R2,L1,L2
 //Bit 7 - 1 - Nibble High Bit 3
 //Bit 7 - 0 - Nibble Low
 u_char auxBit0,auxBit1,auxBit2,auxBit3;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) auxBit0=1; else auxBit0=0;
 if (pad&Pad1R2) auxBit1=1; else auxBit1=0;
 if (pad&Pad1L1) auxBit2=1; else auxBit2=0;
 if (pad&Pad1L2) auxBit3=1; else auxBit3=0; 
   
 if (auxBit0==1 || auxBit1==1 || auxBit2==1 || auxBit3==1)
 {
  //cadPAD[0]=gb_std + 48;
  if (gb_cont_bit == 1 && auxBit3 == 1)
   return; //Es el mismo dato anterior
  gb_cont_bit= auxBit3;
  switch (gb_cont_bit)
  {
   case 0: gb_Q0 = auxBit0;
	gb_Q1 = auxBit1;
	gb_Q2 = auxBit2;
	gb_Q3 = auxBit3;
	cadPAD[7]=gb_Q0+48; 
	cadPAD[6]=gb_Q1+48;
	cadPAD[5]=gb_Q2+48;
	cadPAD[4]=gb_Q3+48;
	break;
   case 1: gb_Q4 = auxBit0;
	gb_Q5 = auxBit1;
	gb_Q6 = auxBit2;
	gb_Q7 = auxBit3;
	cadPAD[3]=gb_Q4+48; 
	cadPAD[2]=gb_Q5+48;
	cadPAD[1]=gb_Q6+48;
	cadPAD[0]=gb_Q7+48;
	break;
   default: break;	  
  }  
  if (gb_cont_bit == 1)
  {
   //gb_cont_bit=0;
   //if ((gb_Q0 == gb_Q4) && (gb_Q1 == gb_Q5) && (gb_Q2 == gb_Q6) && (gb_Q3 == gb_Q7))
   // globalNewData=0; //Es repetido
   //else
   globalNewData=1; //Nuevo dato
  }  
 } 
}

//**********************************************************
void pollKeyboardFourBtnNoStrobe()
{
 //Convierte 8 bits keyboard letras fast No Strobe
 u_char aux=0;
 char car=' ';
 if (globalNewData == 1)
 {
  globalNewData = 0;  	
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4) | (gb_Q5<<5)| (gb_Q6<<6) | (gb_Q7<<7);
  switch (aux)
  {
   case 129: car= 'A'; break; //1000 0001 0x81 129 A
   case 130: car= 'B'; break; //1000 0010 0x82 130 B
   case 131: car= 'C'; break; //1000 0011 0x83 131 C
   case 132: car= 'D'; break; //1000 0100 0x84 132 D
   case 133: car= 'E'; break; //1000 0101 0x85 133 E
   case 134: car= 'F'; break; //1000 0110 0x86 134 F
   case 135: car= 'G'; break; //1000 0111 0x87 135 G
   case 145: car= 'H'; break; //1001 0001 0x91 145 H
   case 146: car= 'I'; break; //1001 0010 0x92 146 I
   case 147: car= 'J'; break; //1001 0011 0x93 147 J
   case 148: car= 'K'; break; //1001 0100 0x94 148 K
   case 149: car= 'L'; break; //1001 0101 0x95 149 L
   case 150: car= 'M'; break; //1001 0110 0x96 150 M
   case 151: car= 'N'; break; //1001 0111 0x97 151 N
   case 161: car= 'N'; break; //1010 0001 0xA1 161 Ñ  
   case 162: car= 'O'; break; //1010 0010 0xA2 162 O
   case 163: car= 'P'; break; //1010 0011 0xA3 163 P
   case 164: car= 'Q'; break; //1010 0100 0xA4 164 Q
   case 165: car= 'R'; break; //1010 0101 0xA5 165 R
   case 166: car= 'S'; break; //1010 0110 0xA6 166 S
   case 167: car= 'T'; break; //1010 0111 0xA7 167 T
   case 177: car= 'U'; break; //1011 0001 0xB1 177 U
   case 178: car= 'V'; break; //1011 0010 0xB2 178 V
   case 179: car= 'W'; break; //1011 0011 0xB3 179 W
   case 180: car= 'X'; break; //1011 0100 0xB4 180 X
   case 181: car= 'Y'; break; //1011 0101 0xB5 181 Y
   case 182: car= 'Z'; break; //1011 0110 0xB6 182 Z    
   case 183: car= ' '; break; //1011 0111 0xB7 183  
   case 193: car= '0'; break; //1100 0001 0xC1 193 0 
   case 194: car= '1'; break; //1100 0010 0xC2 194 1 
   case 195: car= '2'; break; //1100 0011 0xC3 195 2    
   case 196: car= '3'; break; //1100 0100 0xC4 196 3
   case 197: car= '4'; break; //1100 0101 0xC5 197 4
   case 198: car= '5'; break; //1100 0110 0xC6 198 5 
   case 199: car= '6'; break; //1100 0111 0xC7 199 6  
   case 209: car= '7'; break; //1101 0001 0xD1 209 7 
   case 210: car= '8'; break; //1101 0010 0xD2 210 8 
   case 211: car= '9'; break; //1101 0011 0xD3 211 9
   case 212: car= '.'; break; //1101 0100 0xD4 212 .
   case 213: car= ','; break; //1101 0101 0xD5 213 ,
   case 214: car= '-'; break; //1101 0110 0xD6 214 -
   case 215: car= '+'; break; //1101 0111 0xD7 215 +      
  
   case 225: car= '*'; break; //1110 0001 0xE1 225 *
   case 226: car= '/'; break; //1110 0010 0xE2 226 /
   default: car = ' '; break;
  }
  //if (car != ' ')
  {
   gb_cadKeyBuf[gb_contKeyBuf]= car;
   gb_cadKeyBuf[gb_contKeyBuf+1]= '\0';
   gb_contKeyBuf++;
   if (gb_contKeyBuf>16)
    gb_contKeyBuf=0;
   sprintf(cadLog,"KEYBOARD4BTNFAST %d %c\n%s",aux,car,gb_cadKeyBuf);  
  }
 }	
}

//**********************************************************
void HandleKeyboardFourBtn()
{
 //R1 Sync R2,L1,L2 bit Data First 3 bits Keyboard, Second 3 bits R2,L1,L2
 u_char auxBit0,auxBit1,auxBit2;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) gb_std=1; else gb_std=0;
 if (pad&Pad1R2) auxBit0=1; else auxBit0=0;
 if (pad&Pad1L1) auxBit1=1; else auxBit1=0;
 if (pad&Pad1L2) auxBit2=1; else auxBit2=0;
 
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   cadPAD[0]=gb_std + 48;
   switch (gb_cont_bit)
   {
	 case 0: gb_Q0 = auxBit0;
	  gb_Q1 = auxBit1;
	  gb_Q2 = auxBit2;
	  cadPAD[7]=gb_Q0+48; 
	  cadPAD[6]=gb_Q1+48;
	  cadPAD[5]=gb_Q2+48;
	  break;
	 case 1: gb_Q3 = auxBit0;
	  gb_Q4 = auxBit1;
	  gb_Q5 = auxBit2;
	  cadPAD[4]=gb_Q3+48; 
	  cadPAD[3]=gb_Q4+48;
	  cadPAD[2]=gb_Q5+48;
	  break;
	 default: break;
   }
   gb_cont_bit++;
   if (gb_cont_bit>1)
   {
	gb_cont_bit=0;
	globalNewData=1; //Nuevo dato
   }
  }
 } 
}

//**********************************************************
void pollKeyboardFourBtn()
{
 //Convierte 6 bits keyboard letras
 u_char aux=0;
 char car=' ';
 if (globalNewData == 1)
 {
  globalNewData = 0;  	
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4) | (gb_Q5<<5);
  switch (aux)
  {
   case 0: car= 'A'; break;
   case 1: car= 'B'; break;
   case 2: car= 'C'; break;
   case 3: car= 'D'; break;
   case 4: car= 'E'; break;
   case 5: car= 'F'; break;
   case 6: car= 'G'; break;
   case 7: car= 'H'; break;
   case 8: car= 'I'; break;
   case 9: car= 'J'; break;
   case 10: car= 'K'; break;
   case 11: car= 'L'; break;
   case 12: car= 'M'; break;
   case 13: car= 'N'; break;
   case 14: car= 'N'; break;
   case 15: car= 'O'; break;
   case 16: car= 'P'; break;
   case 17: car= 'Q'; break;
   case 18: car= 'R'; break;
   case 19: car= 'S'; break;
   case 20: car= 'T'; break;
   case 21: car= 'U'; break;
   case 22: car= 'V'; break;
   case 23: car= 'W'; break;
   case 24: car= 'X'; break;
   case 25: car= 'Y'; break;
   case 26: car= 'Z'; break;
   case 27: car= '.'; break;
   case 28: car= ','; break;
   case 29: car= ':'; break;
   case 30: car= '['; break;
   case 31: car= ' '; break;
   default: break;
  }
  gb_cadKeyBuf[gb_contKeyBuf]= car;
  gb_cadKeyBuf[gb_contKeyBuf+1]= '\0';
  gb_contKeyBuf++;
  if (gb_contKeyBuf>16)
   gb_contKeyBuf=0;
  sprintf(cadLog,"KEYBOARD4BTN %d %c\n%s",aux,car,gb_cadKeyBuf);  
 }	
}

//**********************************************************
void HandleKeyboardTwoBtn()
{
 //R1 Sync R2 bit Data  5 bits Keyboard
 u_char auxBit;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) gb_std=1; else gb_std=0;
 if (pad&Pad1R2) auxBit=1; else auxBit=0;
  
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   cadPAD[0]=gb_std + 48;
   switch (gb_cont_bit)
   {
	 case 0: gb_Q0 = auxBit; cadPAD[7]=gb_Q0+48; break;
	 case 1: gb_Q1 = auxBit; cadPAD[6]=gb_Q1+48; break;
	 case 2: gb_Q2 = auxBit; cadPAD[5]=gb_Q2+48; break;
	 case 3: gb_Q3 = auxBit; cadPAD[4]=gb_Q3+48; break;
	 case 4: gb_Q4 = auxBit; cadPAD[3]=gb_Q4+48; break;
	 default: break;
   }
   gb_cont_bit++;
   if (gb_cont_bit>4)
   {
	gb_cont_bit=0;	
	globalNewData=1; //Nuevo dato
   }
  }
 }
}

//**********************************************************
void pollKeyboardTwoBtn()
{
 //Convierte 5 bits keyboard letras
 u_char aux=0;
 char car=' ';
 if (globalNewData == 1)
 {
  globalNewData = 0;  	
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4);
  switch (aux)
  {
   case 0: car= 'A'; break;
   case 1: car= 'B'; break;
   case 2: car= 'C'; break;
   case 3: car= 'D'; break;
   case 4: car= 'E'; break;
   case 5: car= 'F'; break;
   case 6: car= 'G'; break;
   case 7: car= 'H'; break;
   case 8: car= 'I'; break;
   case 9: car= 'J'; break;
   case 10: car= 'K'; break;
   case 11: car= 'L'; break;
   case 12: car= 'M'; break;
   case 13: car= 'N'; break;
   case 14: car= 'N'; break;
   case 15: car= 'O'; break;
   case 16: car= 'P'; break;
   case 17: car= 'Q'; break;
   case 18: car= 'R'; break;
   case 19: car= 'S'; break;
   case 20: car= 'T'; break;
   case 21: car= 'U'; break;
   case 22: car= 'V'; break;
   case 23: car= 'W'; break;
   case 24: car= 'X'; break;
   case 25: car= 'Y'; break;
   case 26: car= 'Z'; break;
   case 27: car= '.'; break;
   case 28: car= ','; break;
   case 29: car= ':'; break;
   case 30: car= '['; break;
   case 31: car= ' '; break;
   default: break;
  }
  gb_cadKeyBuf[gb_contKeyBuf]= car;
  gb_cadKeyBuf[gb_contKeyBuf+1]= '\0';
  gb_contKeyBuf++;
  if (gb_contKeyBuf>16)
   gb_contKeyBuf=0;
  sprintf(cadLog,"KEYBOARD2BTN %d %c\n%s",aux,car,gb_cadKeyBuf);  
 }	
}

//**********************************************************
void pollPADTwoBtn()
{	
 //Convierte 4 bits 14 botones PSX a letras 
 u_char aux=0;
 char car=' ';
 if (globalNewData == 1)
 {
  globalNewData = 0;  	
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3);
  switch (aux)
  {
   case 0: car= 'C'; break;
   case 1: car= 'V'; break;
   case 2: car= 'E'; break;
   case 3: car= 'W'; break;
   case 4: car= '^'; break;
   case 5: car= '<'; break;
   case 6: car= '>'; break;
   case 7: car= '.'; break;
   case 8: car= 'T'; break;
   case 9: car= 'R'; break;
   case 10: car= 'D'; break;
   case 11: car= 'S'; break;
   case 12: car= 'X'; break;
   case 13: car= 'Z'; break;
   default: break;
  }
  gb_cadKeyBuf[gb_contKeyBuf]= car;
  gb_cadKeyBuf[gb_contKeyBuf+1]= '\0';
  gb_contKeyBuf++;
  if (gb_contKeyBuf>16)
   gb_contKeyBuf=0;
  sprintf(cadLog,"PAD2BTN %d %c\n%s",aux,car,gb_cadKeyBuf);  
 }
}

//**********************************************************
void pollCMDpad(){	
 int i;
 char auxcad[200]="\0";
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
    if (flag_sizefile0==0)
    {
 	 flag_sizefile0=1; 
	 sizeFile=datoPAD;
	 sprintf(cadLog,"CMD FILE0 %d B",sizeFile);
    }
    else
    { //ya hay flagsizefile
     if (flag_sizefile1==0)
  	 {
 	  flag_sizefile1=1; 
  	  sizeFile=sizeFile+(datoPAD<<8);
	  sprintf(cadLog,"CMD FILE1 %d B",sizeFile);		
	 }
	 else
	 {
      if (flag_datafile==0)
	  {	  
 	   flag_datafile=1;
	   contDataFile=0;
	   bufferFile[contDataFile]=datoPAD;
	   contDataFile++;	   
	   sprintf(cadLog,"CMD FILE %d B %d",sizeFile,contDataFile);
	  }
	  else
	  {//ya hay flag_datafile
       if (contDataFile>=65535)
		contDataFile=0;
       bufferFile[contDataFile]=datoPAD;
	   contDataFile++;
       if (contDataFile>=sizeFile)
	   {		
		sprintf(cadLog,"CMD FILE %d B %d FULL\n",sizeFile,contDataFile);
        flag_cmd=0;
        flag_file=0;
        flag_sizefile0=0;
        flag_sizefile1=0;
        flag_datafile=0;
        contDataFile=0;		
	   }
	   else
		sprintf(cadLog,"CMD FILE %d B %d",sizeFile,contDataFile);		  
       
	  }
	 }
    }
   }
  }
  //letra=(char)(datoPAD);
 }
}

//*******************************************************
void PaintMouse()
{
 //ship.x = (gb_mouseX<<3); //40x25
 //ship.y = (gb_mouseY*10);
 ratonSprite.x = gb_mouseX;
 ratonSprite.y = gb_mouseY;		//Dibuja raton despues sobreescribe primer plano		
 GsSortSprite(&ratonSprite, &myOT[activeBuffer], 0);
}

//*******************************************************
void PaintBtnAll()
{
 //int auxXini= 8;
 //int auxWidth = 136;
 //int auxYini = 30;
 //int auxHeight = 40;
 u_char aux_pushBtnFast = 0;
 u_char aux_pushBtnNormal = 0;
 u_char aux_pushBtnSlow = 0;
 u_char aux_pushBtnInvertX = 0;
 u_char aux_pushBtnInvertY = 0;
 u_char aux_pushBtnLeftHand = 0;  
  
 if (gb_action_mouse_click == 1)
 {
  if (gb_mouse_btnLeft != 1)
   gb_action_mouse_click = 0;
  else
  {
   GsSortSprite(&btnFast, &myOT[activeBuffer], 0);		
   GsSortSprite(&btnNormal, &myOT[activeBuffer], 0);		
   GsSortSprite(&btnSlow, &myOT[activeBuffer], 0);
   GsSortSprite(&btnInvertX, &myOT[activeBuffer], 0);
   GsSortSprite(&btnInvertY, &myOT[activeBuffer], 0);		
   GsSortSprite(&btnLeftHand, &myOT[activeBuffer], 0);		  
   return;
  }
 }
    
 btnFast.r = btnFast.g = btnFast.b= 40;
 btnNormal.r = btnNormal.g = btnNormal.b= 40;  
 btnSlow.r = btnSlow.g = btnSlow.b= 40;
 btnInvertX.r = btnInvertX.g = btnInvertX.b= 40;
 btnInvertY.r = btnInvertY.g = btnInvertY.b= 40;
 btnLeftHand.r = btnLeftHand.g = btnLeftHand.b= 40;
 //Botones izquierda 
 if (gb_mouseX >= 8 && gb_mouseX <= 144)
 {
  if (gb_mouseY >= 30 && gb_mouseY <= 70)
  {   
   if (gb_mouse_btnLeft == 1)	  
    aux_pushBtnFast = 1;
   else
    btnFast.r = btnFast.g = btnFast.b= 80;
  }
  if (gb_mouseY >= 90 && gb_mouseY <= 130)
  {   
   if (gb_mouse_btnLeft == 1)	
	aux_pushBtnNormal = 1;
   else
    btnNormal.r = btnNormal.g = btnNormal.b= 80;   
  }
  if (gb_mouseY >= 150 && gb_mouseY <= 190)
  {   
   if (gb_mouse_btnLeft == 1)   
	aux_pushBtnSlow = 1;
   else
    btnSlow.r = btnSlow.g = btnSlow.b= 80;     
  }
 }
 else
 {
  //Botones derecha
  if (gb_mouseX >= 176 && gb_mouseX <= 312)
  {
   if (gb_mouseY >= 30 && gb_mouseY <= 70)
   {	
	if (gb_mouse_btnLeft == 1)	  
	 aux_pushBtnInvertX = 1;
	else
     btnInvertX.r = btnInvertX.g = btnInvertX.b= 80;   
   }
   if (gb_mouseY >= 90 && gb_mouseY <= 130)
   {
	if (gb_mouse_btnLeft == 1)	  
	 aux_pushBtnInvertY = 1;
	else
     btnInvertY.r = btnInvertY.g = btnInvertY.b= 80;   
   }
   if (gb_mouseY >= 150 && gb_mouseY <= 190)
   {
	if (gb_mouse_btnLeft == 1)	  
	 aux_pushBtnLeftHand = 1; 
	else
     btnLeftHand.r = btnLeftHand.g = btnLeftHand.b= 80;	  
   }
  }
 }
 
 //Se ha tocado el boton de Rapdio, normal o lento
 if (aux_pushBtnFast == 1)
 {
  gb_pushBtnFast=1;
  gb_pushBtnNormal = 0;
  gb_pushBtnSlow = 0;
  btnNormal.r = btnNormal.g = btnNormal.b = 40;
  btnSlow.r = btnSlow.g = btnSlow.b = 40;
  gb_mouse_max_equalX = 8; 
  gb_mouse_max_equalY = 8;
  gb_mouse_max_incX = 8;
  gb_mouse_max_incY = 8;    
  gb_action_mouse_click = 1;
 }
 
 if (aux_pushBtnNormal == 1)
 {
  gb_pushBtnFast=0;
  gb_pushBtnNormal = 1;
  gb_pushBtnSlow = 0;
  btnFast.r = btnFast.g = btnFast.b = 40;
  btnSlow.r = btnSlow.g = btnSlow.b = 40;      
  gb_mouse_max_equalX = 16; 
  gb_mouse_max_equalY = 16;
  gb_mouse_max_incX = 4;
  gb_mouse_max_incY = 4;  
  gb_action_mouse_click = 1;
 } 

 if (aux_pushBtnSlow == 1)
 {
  gb_pushBtnFast=0;
  gb_pushBtnNormal = 0;
  gb_pushBtnSlow = 1;
  btnFast.r = btnFast.g = btnFast.b = 40;
  btnNormal.r = btnNormal.g = btnNormal.b = 40;
  gb_mouse_max_equalX = 16; 
  gb_mouse_max_equalY = 16;
  gb_mouse_max_incX = 1;
  gb_mouse_max_incY = 1;  
  gb_action_mouse_click = 1;
 } 
 
 if (aux_pushBtnInvertX == 1)
 {
  if (gb_pushBtnInvertX == 1)
   gb_pushBtnInvertX = 0;
  else
   gb_pushBtnInvertX = 1;
  gb_action_mouse_click = 1;
 }
 
 if (aux_pushBtnInvertY ==1)
 {
  if (gb_pushBtnInvertY == 1)
   gb_pushBtnInvertY = 0;
  else
   gb_pushBtnInvertY = 1;
  gb_action_mouse_click = 1;
 }
 
 if (aux_pushBtnLeftHand == 1)
 {
  if (gb_pushBtnLeftHand == 1)
   gb_pushBtnLeftHand = 0;
  else 
   gb_pushBtnLeftHand = 1;
  gb_action_mouse_click = 1;
 }
 
 
 if (gb_pushBtnFast == 1)
  btnFast.r = btnFast.g = btnFast.b = 128;
 if (gb_pushBtnNormal == 1) 
  btnNormal.r = btnNormal.g = btnNormal.b = 128;
 if (gb_pushBtnSlow == 1) 
  btnSlow.r = btnSlow.g = btnSlow.b = 128;
 
 if (gb_pushBtnInvertX == 1)
  btnInvertX.r = btnInvertX.g = btnInvertX.b = 128;
 if (gb_pushBtnInvertY == 1)
  btnInvertY.r = btnInvertY.g = btnInvertY.b = 128;
 if (gb_pushBtnLeftHand == 1)
  btnLeftHand.r = btnLeftHand.g = btnLeftHand.b = 128;	 
	 
 GsSortSprite(&btnFast, &myOT[activeBuffer], 0);		
 GsSortSprite(&btnNormal, &myOT[activeBuffer], 0);		
 GsSortSprite(&btnSlow, &myOT[activeBuffer], 0);
 GsSortSprite(&btnInvertX, &myOT[activeBuffer], 0);
 GsSortSprite(&btnInvertY, &myOT[activeBuffer], 0);		
 GsSortSprite(&btnLeftHand, &myOT[activeBuffer], 0);					
}

//*******************************
void LoadAllSprites()
{
	GsGetTimInfo((u_long *)(BTNALL_ADDRESS+4), &btnAllImg);	//this prepares the 'ship' sprite for display
	InitSprite(&btnAllImg, &btnFast,0,0,136,40);
	btnFast.mx=0;//shipImg.px/2;
	btnFast.my=0;//shipImg.py/2;
	btnFast.scalex=ONE;//*0.5;
	btnFast.scaley=ONE;//*0.5;
	btnFast.x=0;
	btnFast.y=0;	
		
	InitSprite(&btnAllImg, &btnNormal,0,40,136,40);
	btnNormal.mx=0;
	btnNormal.my=0;
	btnNormal.scalex=ONE;
	btnNormal.scaley=ONE;
	btnNormal.x=0;
	btnNormal.y=0;		
		
	InitSprite(&btnAllImg, &btnSlow,0,80,136,40);
	btnSlow.mx=0;
	btnSlow.my=0;
	btnSlow.scalex=ONE;
	btnSlow.scaley=ONE;
	btnSlow.x=0;
	btnSlow.y=0;			

	InitSprite(&btnAllImg, &btnInvertX,0,120,136,40);
	btnInvertX.mx=0;
	btnInvertX.my=0;
	btnInvertX.scalex=ONE;
	btnInvertX.scaley=ONE;
	btnInvertX.x=0;
	btnInvertX.y=0;	
	
	InitSprite(&btnAllImg, &btnInvertY,0,160,136,40);
	btnInvertY.mx=0;
	btnInvertY.my=0;
	btnInvertY.scalex=ONE;
	btnInvertY.scaley=ONE;
	btnInvertY.x=0;
	btnInvertY.y=0;		
	
	InitSprite(&btnAllImg, &btnLeftHand,0,200,136,40);
	btnLeftHand.mx=0;
	btnLeftHand.my=0;
	btnLeftHand.scalex=ONE;
	btnLeftHand.scaley=ONE;
	btnLeftHand.x=0;
	btnLeftHand.y=0;			
	
    GsGetTimInfo((u_long *)(RATON_ADDRESS+4), &ratonImg);	//this prepares the 'ship' sprite for display	
	InitSprite(&ratonImg, &ratonSprite,0,0,16,22);
	ratonSprite.mx=0;//shipImg.px/2;
	ratonSprite.my=0;//shipImg.py/2;
	ratonSprite.scalex=ONE;//*0.5;
	ratonSprite.scaley=ONE;//*0.5;
	ratonSprite.x= gb_mouseX;
	ratonSprite.y= gb_mouseY;	
	
 btnFast.x = 8;
 btnFast.y = 30;
 btnNormal.x = 8;
 btnNormal.y = 90;
 btnSlow.x = 8;
 btnSlow.y = 150; 
 btnInvertX.x = 176;
 btnInvertX.y = 30; 
 btnInvertY.x = 176;
 btnInvertY.y = 90; 
 btnLeftHand.x = 176;
 btnLeftHand.y = 150; 	
}

/***************** functions ********************/
int main(void) {	
	
	InitGraphics();			//this method sets up gfx for printing to screen
	
	FntLoad(960, 256);		//this loads the font gfx to the vram
	//FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
	//x y ini, width heigh, clear, max char
	FntOpen(8, 8, 300, 16, 0, 64);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc    
	LoadAllSprites(); //Load Mouse cursor and buttons
	DrawSync(0);
						
	while (1) {				//infinite loop
		activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
		GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
		GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents
							
		//once the OT contents have been cleared you can add new objects to the OT
		//so that they will be displayed next time DisplayAll is called

		HandleMouseFourBtnNoStrobe();
		pollMouseFourBtnNoStrobe();
		FntPrint("4N %s %x %s",cadPAD,pad,cadLog);
				
        PaintMouse();  //Show Mouse					
		PaintBtnAll(); //Show All Buttons		
						
		//FntPrint("Serial PAD\n PAD %s %d %d\nLetra %c %s",cadPAD,datoPAD,pad,letra,bufferLetra);
		//this just adds the string to the screen printing buffer
		//to actually print this to screen yu have to flush the contents of this buffer
		//this is done in displayall method
		
		//DisplayAll(activeBuffer);		//this dispalys the OT contents to screen				
		
		//HandlePadTwoBtn();
		//pollPADTwoBtn();
		//FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);			    
		
		//HandleKeyboardTwoBtn();
		//pollKeyboardTwoBtn();
		//FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);			    
		
		//HandleKeyboardFourBtn();
		//pollKeyboardFourBtn();
		//FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);			    		
		
		//HandleMouseFourBtn();
		//pollMouseFourBtn();
		//FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);

		
		//HandleKeyboardFourBtnNoStrobe();
		//pollKeyboardFourBtnNoStrobe();
		//FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);			    		
		
//      HandlePad();					//this handles button presses on the controller pad 1
//		pollCMDpad(datoPAD);		
//      FntPrint("SERIALPAD %s %d %d\n%s",cadPAD,datoPAD,pad,cadLog);			    

		DisplayAll(activeBuffer);		//this displays the OT contents to screen		
	}
	return 0;		//when program is finished return from it
}

void InitGraphics(void) {
	//Prueba JJ
	ResetCallback();//JJ
	ResetGraph(0); //JJ 
	PadInit(0); //JJ
	SetVideoMode( MODE_PAL ); //JJ
	SetDispMask(1);				// 0: inhibit display 	
	
	
	//this method sets up gfx for printing to screen
	//GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0); //initialises the graphics system	
    // set the graphics mode resolutions (GsNONINTER for NTSC, and GsINTER for PAL)
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	
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

//*************************************************
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

//***********************************************
void HandlePadTwoBtn(void)
{
 //R1 Sync R2 bit Data   4 bits 14 botones
 u_char auxBit;
 
 globalNewData=0;
 pad=PadRead(0);
 if (pad&Pad1R1) gb_std=1; else gb_std=0;
 if (pad&Pad1R2) auxBit=1; else auxBit=0;
  
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   cadPAD[0]=gb_std + 48;
   switch (gb_cont_bit)
   {
	 case 0: gb_Q0 = auxBit; cadPAD[7]=gb_Q0+48; break;
	 case 1: gb_Q1 = auxBit; cadPAD[6]=gb_Q1+48; break;
	 case 2: gb_Q2 = auxBit; cadPAD[5]=gb_Q2+48; break;
	 case 3: gb_Q3 = auxBit; cadPAD[4]=gb_Q3+48; break;
	 default: break;
   }
   gb_cont_bit++;
   if (gb_cont_bit>3)
   {
	//gb_Q0 = 0;
	//gb_Q1 = 0;
	//gb_Q2 = 0;
	//gb_Q3 = 0;
	gb_cont_bit=0;
	
	globalNewData=1; //Nuevo dato
   }
  }
 }  
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
