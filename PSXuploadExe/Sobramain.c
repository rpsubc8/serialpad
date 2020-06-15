//Autor: ackerman
//upload PsExe PAD port with ARDUINO
//Buttons R1,R2,L1,L2
//Jaime Jose Gavin Sierra

#include <kernel.h>
#include <libapi.h>
#include <sys/types.h>	//these are the various include files that need to be included
#include <libetc.h>		//note that the order they are included in does matter in some cases!
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <strings.h>
#include "pad.h"
#include "kk.h"
//also note that while this example uses the Gs functions, these are high level functions
//and should not be used if it can be helped as they are quite slow and consume lots of memory.
//they will however do while you are learning as they simplify things and help you to understand
//the process that you go through to print to the screen.


#define OT_LENGTH (10) //this is the 'length' of the OT (ordering tabble)
					   //for more info on ordering tables, check out ot.txt on my 'info' page

#define PACKETMAX (2048) //these are the graphics area contants
#define PACKETMAX2 (PACKETMAX*24) //allows you to select how many gfx objects the psx should handle
					              //in this particular example

#define SCREEN_WIDTH 320 //contants to determine the height and width of the screen resolution
//#define SCREEN_HEIGHT 240
#define SCREEN_HEIGHT 256
#define tope_texto 1024
#define booleana unsigned char
#define TRUE 1
#define FALSE 0
#define H_SIZE 2048




GsOT myOT[2]; //an array of 2 OT's this is how your double buffering is implemented
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH];
PACKET GPUPacketArea[2][PACKETMAX2]; //also 2 gfx packet areas for double buffering


int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed

char gb_cadLog[200]="\0";
char cadPAD[]="0000";
char cadPAD14[]="00000000000000";
char gb_cadKeyBuf[18]="";
int gb_contKeyBuf=0;
u_char gb_bufferFrame[256]; //trama
int gb_cont_bufferFrame=0;
u_long pad;
u_char globalNewData=0;
u_char gb_Q7=0;
u_char gb_Q6=0;
u_char gb_Q5=0;
u_char gb_Q4=0;
u_char gb_Q3=0;
u_char gb_Q2=0;
u_char gb_Q1=0;
u_char gb_Q0=0;
u_char gb_std=0;
u_char gb_std_antes=0;
u_char gb_d0[8]={0,0,0,0,0,0,0,0};
u_char gb_d1[8]={0,0,0,0,0,0,0,0};
u_char gb_d2[8]={0,0,0,0,0,0,0,0};

unsigned short int gb_d13bytes_8[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //1 mando 14 botones
u_char gb_cont_13_bytes = 0;
u_char gb_d3bytes_8[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //4 botones cable mando
u_char gb_cont_3_bytes = 0;

u_char gb_cont_bit=0;
unsigned int gb_size_psExe=0;
unsigned int gb_address_psExe=0;
unsigned int gb_cont_byte_psExe=0;
u_char gb_firstBeginExe=0;
u_char gb_error=0;
u_char gb_receivedHead= 0; //No se ha recibido aun la cabecera
int gb_type = -1;
int gb_speed = -1;
int gb_CRC = -1;
u_char gb_launch_exe= 0;
unsigned int gb_address_psExe_cont=0;
//u_long gb_currentTick;
//u_long gb_lastTick;
//u_long gb_tiempo=-1;
unsigned int gb_cont_error=0;
u_char gb_src_value_error = 0;
u_char gb_value_error = 0;
int gb_cont_i;

struct EXEC exe;

/***************** prototypes *******************/
void InitGraphics(void);		  //this method sets up gfx for printing to screen
void DisplayAll(int);			  //this displays the contents of the OT
void HandleFourBtn(void);
void HandleFourBtnFast(void);
void PollFourBtnFast(void);
void PollReceivedHeadFourBtnFast(void);
void PollFourBtn(void);
void Handle14Btns(void);
void Poll14Btns(void);
void Handle14BtnsFast(void);
void Poll14BtnsFast(void);
void Handle13Bytes(void);
void Poll13Bytes(void);
void PollReceivedHeadFourBtn(void);
void CargaPrograma(struct EXEC *exep);
int main(void);

//************
//* 13 bytes *
//*********************************************************************************
void Handle13Bytes()
{
 unsigned int a0,a1,a2,a3,a4,a5,a6,a7; //8 bits
 unsigned int b0,b1,b2,b3; //4 bits
 unsigned int auxCRC=0; 
  
 pad=PadRead(0);
 if (pad&Pad1tri) gb_std=1; else gb_std=0;
 if (pad&Pad1crc) a0=1; else a0=0;
 if (pad&Pad1x) a1=1; else a1=0;
 if (pad&Pad1sqr) a2=1; else a2=0;
 if (pad&Pad1L2) a3=1; else a3=0;
 if (pad&Pad1R2) a4=1; else a4=0;
 if (pad&Pad1L1) a5=1; else a5=0;
 if (pad&Pad1R1) a6=1; else a6=0;
 if (pad&Pad1Start) a7=1; else a7=0;
 if (pad&Pad1Select) b0=1; else b0=0;
 if (pad&Pad1Up) b1=1; else b1=0;
 if (pad&Pad1Right) b2=1; else b2=0;
 if (pad&Pad1Down) b3=1; else b3=0;
 if (pad&Pad1Left) auxCRC=1; else auxCRC=0;
  
 if (gb_std == 0)
 {	 
  gb_d13bytes_8[gb_cont_13_bytes] = a0|(a1<<1)|(a2<<2)|(a3<<3)|(a4<<4)|(a5<<5)|(a6<<6)|(a7<<7)|(b0<<8)|(b1<<9)|(b2<<10)|(b3<<11)|(auxCRC<<12);
 }
 else
 {	 
  if (gb_std_antes == 0)
  {   	    
   gb_cont_13_bytes ++;
   gb_d13bytes_8[gb_cont_13_bytes] = a0|(a1<<1)|(a2<<2)|(a3<<3)|(a4<<4)|(a5<<5)|(a6<<6)|(a7<<7)|(b0<<8)|(b1<<9)|(b2<<10)|(b3<<11)|(auxCRC<<12);
   gb_cont_13_bytes ++;
   if (gb_cont_13_bytes>7)
   {
    gb_cont_13_bytes=0;
    globalNewData = 1;
   }
  }
 }
 if (gb_std != gb_std_antes)
  gb_std_antes = gb_std;
}

//********************************************************************************************
void Poll13Bytes()
{    
 //0 Bits 8 4 1 - 13   a0 a8  a12
 //1      8 4 1 - 13   a1 a8  a12
 //0      8 4 1 - 13   a2 a9  a12
 //1      8 4 1 - 13   a3 a9  a12
 //0      8 4 1 - 13   a4 a10 a12
 //1      8 4 1 - 13   a5 a10 a12
 //0      8 4 1 - 13   a6 a11 a12
 //1      8 4 1 - 13   a7 a11 a12
 unsigned char d[13]; 
 unsigned char i;
 u_char *p_address; 
 p_address = (u_char *)gb_address_psExe;
 
 if (gb_error == 1)
 {
  sprintf (gb_cadLog,"\nERROR id %d %d src %x value %x",gb_cont_error,gb_cont_i,gb_src_value_error,gb_value_error);
  return;
 }
	
 if (globalNewData == 1)
 {//Tenemos 13 bytes de datos
  globalNewData = 0;    
  for (i=0;i<8;i++)
  {	  
   d[i] = gb_d13bytes_8[i] & 0xFF; //8 bytes   
  }  
  //0x0F FF
  //    0 1111 1111 1111
  d[8]= (
         ((gb_d13bytes_8[0]>>8)&0x0F)
		 |((gb_d13bytes_8[1]>>4)&0xF0)
		)&0xFF; //1 byte
  d[9]= (
         ((gb_d13bytes_8[2]>>8)&0x0F)
		 |((gb_d13bytes_8[3]>>4)&0xF0)
		)&0xFF; //1 byte
  d[10]= (
          ((gb_d13bytes_8[4]>>8)&0x0F)
		  |((gb_d13bytes_8[5]>>4)&0xF0)
		 )&0xFF; //1 byte
  d[11]= (
          ((gb_d13bytes_8[6]>>8)&0x0F)
		  |((gb_d13bytes_8[7]>>4)&0xF0)
		 )&0xFF; //1 byte
  d[12]= (
  ((gb_d13bytes_8[0]>>12)&0x01)
  |((gb_d13bytes_8[1]>>11)&0x02)
  |((gb_d13bytes_8[2]>>10)&0x04)
  |((gb_d13bytes_8[3]>>9)&0x08)
  |((gb_d13bytes_8[4]>>8)&16)
  |((gb_d13bytes_8[5]>>7)&32)
  |((gb_d13bytes_8[6]>>6)&64)
  |((gb_d13bytes_8[7]>>5)&128)
  )&0xFF;
  for (i=0;i<13;i++)
  {
   //gb_bufferFrame[gb_cont_bufferFrame+i] = d[i];      
   p_address[gb_address_psExe_cont] = d[i];
   if (main2[gb_address_psExe_cont] != p_address[gb_address_psExe_cont])
   {
	gb_error = 1;
	gb_cont_error = gb_address_psExe_cont;
	gb_value_error = p_address[gb_address_psExe_cont];
	gb_src_value_error = main2[gb_address_psExe_cont];
	gb_cont_i = i;
	sprintf (gb_cadLog,"\nERROR id %d d%d src %x value %x",gb_cont_error,gb_cont_i,gb_src_value_error,gb_value_error);
	//return;
	break;
	//return;
   }
   gb_address_psExe_cont++;   
  }
  gb_cont_byte_psExe += 13;
  if (gb_size_psExe>0 && gb_cont_byte_psExe>0 && gb_cont_byte_psExe >= gb_size_psExe)
   gb_launch_exe = 1;
  //sprintf (gb_cadLog,"\n%x%x%x%x%x%x%x%x%x%x%x%x%x",d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7],d[8],d[9],d[10],d[11],d[12]);
 }
 //sprintf (gb_cadLog,"\n%x %d/%d",gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe); 
 sprintf (gb_cadLog,"\nbps %d Typ%d %x %d/%d",gb_speed,gb_type,gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe);	 
}

//***************
//* 14 Btn Fast *
//***********************************************************
void Handle14BtnsFast()
{
 //Triangulo Circulo X Cuadrado L2 R2 L1 R1 Start Select Arriba Derecha Abajo Izquierda
 //  D        X      Z    S      E T  W  R  V     C      Up     Right   Down  Left 
 //Send 13 bits
 // D0 - 8 bits 4 bits (12 bits) - 1 bit CRC bit 0
 // D1 - 4 bits 8 bits (12 bits) - 1 bit CRC bit 0
 u_char a0,a1,a2,a3,a4,a5,a6,a7; //8 bits
 u_char b0,b1,b2,b3; //4 bits
 u_char auxCRC=0;
 //a0=a1=a2=a3=a4=a5=a6=a7=b0=b1=b2=b3=0;
 pad=PadRead(0);
 if (pad&Pad1tri) gb_std=1; else gb_std=0;
 if (pad&Pad1crc) a0=1; else a0=0;
 if (pad&Pad1x) a1=1; else a1=0;
 if (pad&Pad1sqr) a2=1; else a2=0;
 if (pad&Pad1L2) a3=1; else a3=0;
 if (pad&Pad1R2) a4=1; else a4=0;
 if (pad&Pad1L1) a5=1; else a5=0;
 if (pad&Pad1R1) a6=1; else a6=0;
 if (pad&Pad1Start) a7=1; else a7=0;
 if (pad&Pad1Select) b0=1; else b0=0;
 if (pad&Pad1Up) b1=1; else b1=0;
 if (pad&Pad1Right) b2=1; else b2=0;
 if (pad&Pad1Down) b3=1; else b3=0;
 if (pad&Pad1Left) auxCRC=1; else auxCRC=0;
 if (gb_std == 0)
 {
  gb_d0[0] = a0;
  gb_d0[1] = a1;
  gb_d0[2] = a2;
  gb_d0[3] = a3;
  gb_d0[4] = a4;
  gb_d0[5] = a5;
  gb_d0[6] = a6;
  gb_d0[7] = a7;
  gb_d1[0] = b0;
  gb_d1[1] = b1;
  gb_d1[2] = b2;
  gb_d1[3] = b3;	  
 }    
 else
 {	 
  if (gb_std_antes == 0)
  {
   gb_d1[4] = a0;
   gb_d1[5] = a1;
   gb_d1[6] = a2;
   gb_d1[7] = a3;
   gb_d2[0] = a4;
   gb_d2[1] = a5;
   gb_d2[2] = a6;
   gb_d2[3] = a7;
   gb_d2[4] = b0;
   gb_d2[5] = b1;
   gb_d2[6] = b2;
   gb_d2[7] = b3;   
   if (auxCRC != a0)
    gb_error=1;
   globalNewData = 1;
  }
 }
 if (gb_std != gb_std_antes)
  gb_std_antes = gb_std;  
 
 //cadPAD14[12]=a0+48;
 //cadPAD14[11]=a1+48;
 //cadPAD14[10]=a2+48;
 //cadPAD14[9]=a3+48;
 //cadPAD14[8]=a4+48;
 //cadPAD14[7]=a5+48;
 //cadPAD14[6]=a6+48;
 //cadPAD14[5]=a7+48;
 //cadPAD14[4]=b0+48;
 //cadPAD14[3]=b1+48; 
 //cadPAD14[2]=b2+48;
 //cadPAD14[1]=b3+48;
 //cadPAD14[0]=gb_std+48;
 //cadPAD14[14]='\0'; 
 //}
}

//***************************************************
void Poll14BtnsFast()
{
 //Convierte 14 bits(12 bits data) modo rapido
 u_char aux0=0;    
 u_char aux1=0;
 u_char aux2=0;
 unsigned int auxSize=0;
 if (gb_error == 1)
 {
  sprintf(gb_cadLog,"\nERROR!");	 
  //return;
 }
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  aux0 = gb_d0[0] | (gb_d0[1]<<1) | (gb_d0[2]<<2) | (gb_d0[3]<<3) | (gb_d0[4]<<4) | (gb_d0[5]<<5)| (gb_d0[6]<<6)| (gb_d0[7]<<7);
  aux1 = gb_d1[0] | (gb_d1[1]<<1) | (gb_d1[2]<<2) | (gb_d1[3]<<3) | (gb_d1[4]<<4) | (gb_d1[5]<<5)| (gb_d1[6]<<6)| (gb_d1[7]<<7);
  aux2 = gb_d2[0] | (gb_d2[1]<<1) | (gb_d2[2]<<2) | (gb_d2[3]<<3) | (gb_d2[4]<<4) | (gb_d2[5]<<5)| (gb_d2[6]<<6)| (gb_d2[7]<<7);
  gb_bufferFrame[gb_cont_bufferFrame] = aux0;
  gb_bufferFrame[gb_cont_bufferFrame+1] = aux1;
  gb_bufferFrame[gb_cont_bufferFrame+2] = aux2;
  gb_cont_bufferFrame += 3;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 7;
  //0 Low size bytes multiple 2048
  //1 High size bytes multiple 2048
  //1,2,3,4 Addres memory psEXE
  //6 ... Bytes psExe  
  if (gb_cont_bufferFrame == 6)
  {
   auxSize = (gb_bufferFrame[1]<<8) | gb_bufferFrame[0];
   gb_size_psExe = (auxSize * 2048);
   gb_address_psExe = (gb_bufferFrame[5]<<24) | (gb_bufferFrame[4]<<16) | (gb_bufferFrame[3]<<8) | gb_bufferFrame[2];
   gb_firstBeginExe = 1;
   gb_cont_byte_psExe = -1;
  }
  if (gb_firstBeginExe == 1)
   gb_cont_byte_psExe +=3;
 }
 sprintf (gb_cadLog,"\n%x %d/%d",gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe); 
}




//**********
//* 14 Btn *
//***********************************************************
void Handle14Btns()
{
 //Triangulo Circulo X Cuadrado L2 R2 L1 R1 Start Select Arriba Derecha Abajo Izquierda
 //  D        X      Z    S      E T  W  R  V     C      Up     Right   Down  Left 
 //Send 13 bits
 // 8 bits 4 bits (12 bits) - 1 bit CRC bit 0
 // 4 bits 8 bits (12 bits) - 1 bit CRC bit 0
 u_char a0,a1,a2,a3,a4,a5,a6,a7; //8 bits
 u_char b0,b1,b2,b3; //4 bits
 u_char auxCRC=0;
 a0=a1=a2=a3=a4=a5=a6=a7=b0=b1=b2=b3=0;
 pad=PadRead(0);
 if (pad&Pad1tri) gb_std=1; else gb_std=0;
 if (pad&Pad1crc) a0=1; else a0=0;
 if (pad&Pad1x) a1=1; else a1=0;
 if (pad&Pad1sqr) a2=1; else a2=0;
 if (pad&Pad1L2) a3=1; else a3=0;
 if (pad&Pad1R2) a4=1; else a4=0;
 if (pad&Pad1L1) a5=1; else a5=0;
 if (pad&Pad1R1) a6=1; else a6=0;
 if (pad&Pad1Start) a7=1; else a7=0;
 if (pad&Pad1Select) b0=1; else b0=0;
 if (pad&Pad1Up) b1=1; else b1=0;
 if (pad&Pad1Right) b2=1; else b2=0;
 if (pad&Pad1Down) b3=1; else b3=0;
 if (pad&Pad1Left) auxCRC=1; else auxCRC=0;
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   switch (gb_cont_bit)
   {
	case 0: gb_d0[0] = a0;
	 gb_d0[1] = a1;
	 gb_d0[2] = a2;
	 gb_d0[3] = a3;
	 gb_d0[4] = a4;
	 gb_d0[5] = a5;
	 gb_d0[6] = a6;
	 gb_d0[7] = a7;
	 gb_d1[0] = b0;
	 gb_d1[1] = b1;
	 gb_d1[2] = b2;
	 gb_d1[3] = b3;
	 if (auxCRC != a0)
	  gb_error=1;
	 break;
	case 1: gb_d1[4] = a0;
	 gb_d1[5] = a1;
	 gb_d1[6] = a2;
	 gb_d1[7] = a3;
	 gb_d2[0] = a4;
	 gb_d2[1] = a5;
	 gb_d2[2] = a6;
	 gb_d2[3] = a7;
	 gb_d2[4] = b0;
	 gb_d2[5] = b1;
	 gb_d2[6] = b2;
	 gb_d2[7] = b3;
	 if (auxCRC != a0)
	  gb_error=1;
     break;
	default: break;
   }
   gb_cont_bit ++;
   if (gb_cont_bit>1)
   {
    gb_cont_bit = 0;
    globalNewData = 1;   
   }
   cadPAD14[12]=a0+48;
   cadPAD14[11]=a1+48;
   cadPAD14[10]=a2+48;
   cadPAD14[9]=a3+48;
   cadPAD14[8]=a4+48;
   cadPAD14[7]=a5+48;
   cadPAD14[6]=a6+48;
   cadPAD14[5]=a7+48;
   cadPAD14[4]=b0+48;
   cadPAD14[3]=b1+48; 
   cadPAD14[2]=b2+48;
   cadPAD14[1]=b3+48;
   cadPAD14[0]=gb_std+48;
   cadPAD14[14]='\0';
  }
 }
}

//***************************************************
void Poll14Btns()
{
 //Convierte 14 bits(12 bits data)
 u_char aux0=0;    
 u_char aux1=0;
 u_char aux2=0;
 unsigned int auxSize=0;
 if (gb_error == 1)
 {
  sprintf(gb_cadLog,"\nERROR!");	 
  return;
 }
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  aux0 = gb_d0[0] | (gb_d0[1]<<1) | (gb_d0[2]<<2) | (gb_d0[3]<<3) | (gb_d0[4]<<4) | (gb_d0[5]<<5)| (gb_d0[6]<<6)| (gb_d0[7]<<7);
  aux1 = gb_d1[0] | (gb_d1[1]<<1) | (gb_d1[2]<<2) | (gb_d1[3]<<3) | (gb_d1[4]<<4) | (gb_d1[5]<<5)| (gb_d1[6]<<6)| (gb_d1[7]<<7);
  aux2 = gb_d2[0] | (gb_d2[1]<<1) | (gb_d2[2]<<2) | (gb_d2[3]<<3) | (gb_d2[4]<<4) | (gb_d2[5]<<5)| (gb_d2[6]<<6)| (gb_d2[7]<<7);
  gb_bufferFrame[gb_cont_bufferFrame] = aux0;
  gb_bufferFrame[gb_cont_bufferFrame+1] = aux1;
  gb_bufferFrame[gb_cont_bufferFrame+2] = aux2;
  gb_cont_bufferFrame += 3;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 7;
  //0 Low size bytes multiple 2048
  //1 High size bytes multiple 2048
  //1,2,3,4 Addres memory psEXE
  //6 ... Bytes psExe  
  if (gb_cont_bufferFrame == 6)
  {
   auxSize = (gb_bufferFrame[1]<<8) | gb_bufferFrame[0];
   gb_size_psExe = (auxSize * 2048);
   gb_address_psExe = (gb_bufferFrame[5]<<24) | (gb_bufferFrame[4]<<16) | (gb_bufferFrame[3]<<8) | gb_bufferFrame[2];
   gb_firstBeginExe = 1;
   gb_cont_byte_psExe = -1;
  }
  if (gb_firstBeginExe == 1)
   gb_cont_byte_psExe +=3;
 }
 sprintf (gb_cadLog,"\n%x %d/%d",gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe); 
}

//*********
//* 4 Btn *
//***********************************************************
void HandleFourBtnFast()
{
 //0 a0 b0 c0
 //1 a1 b1 c1
 //0 a2 b2 c2
 //1 a3 b3 c3
 //0 a4 b4 c4
 //1 a5 b5 c5
 //0 a6 b6 c6
 //1 a7 b7 c7
 //R1,R2,L1,L2 bit Data 3 bits 8 lecturas 3 bytes
 u_char a0,a1,a2; 
 pad=PadRead(0);
 if (pad&Pad1R1) gb_std=1; else gb_std=0;
 if (pad&Pad1R2) a0=1; else a0=0;
 if (pad&Pad1L1) a1=1; else a1=0;
 if (pad&Pad1L2) a2=1; else a2=0;
 
 if (gb_std == 0)
 {
  gb_d3bytes_8[gb_cont_3_bytes]= a0|(a1<<1)|(a2<<2);
 }
 else
 {
  if (gb_std_antes == 0)
  {
   gb_cont_3_bytes ++;
   gb_d3bytes_8[gb_cont_3_bytes]= a0|(a1<<1)|(a2<<2);
   gb_cont_3_bytes ++;
   if (gb_cont_3_bytes > 7)
   {
	gb_cont_3_bytes = 0;
    globalNewData = 1;
   }
  }
 }
 if (gb_std != gb_std_antes)
  gb_std_antes = gb_std;
}

//****************************************************************
void PollFourBtnFast()
{
 //0 a0 b0 c0
 //1 a1 b1 c1
 //0 a2 b2 c2
 //1 a3 b3 c3
 //0 a4 b4 c4
 //1 a5 b5 c5
 //0 a6 b6 c6
 //1 a7 b7 c7
 u_char d[3]={0,0,0};
 u_char i;
 u_char *p_address = (u_char *)gb_address_psExe;
 if (gb_error == 1)
 {
  sprintf (gb_cadLog,"\nERROR id %d %d src %x value %x",gb_cont_error,gb_cont_i,gb_src_value_error,gb_value_error);
  return;
 }
 
 if (globalNewData == 1)
 {
  globalNewData = 0;
  //Tenemos 3 bytes de datos
  d[0] = (gb_d3bytes_8[0]&0x01)
         |((gb_d3bytes_8[1]&0x01)<<1)
		 |((gb_d3bytes_8[2]&0x01)<<2)
		 |((gb_d3bytes_8[3]&0x01)<<3)
		 |((gb_d3bytes_8[4]&0x01)<<4)
		 |((gb_d3bytes_8[5]&0x01)<<5)
		 |((gb_d3bytes_8[6]&0x01)<<6)
		 |((gb_d3bytes_8[7]&0x01)<<7);
  d[1] = ((gb_d3bytes_8[0]&0x02)>>1)
          |(gb_d3bytes_8[1]&0x02)
		  |((gb_d3bytes_8[2]&0x02)<<1)
		  |((gb_d3bytes_8[3]&0x02)<<2)
		  |((gb_d3bytes_8[4]&0x02)<<3)
		  |((gb_d3bytes_8[5]&0x02)<<4)
		  |((gb_d3bytes_8[6]&0x02)<<5)
		  |((gb_d3bytes_8[7]&0x02)<<6);
  d[2] = ((gb_d3bytes_8[0]&0x04)>>2)
          |((gb_d3bytes_8[1]&0x04)>>1)
		  |(gb_d3bytes_8[2]&0x04)
		  |((gb_d3bytes_8[3]&0x04)<<1)
		  |((gb_d3bytes_8[4]&0x04)<<2)
		  |((gb_d3bytes_8[5]&0x04)<<3)
		  |((gb_d3bytes_8[6]&0x04)<<4)
		  |((gb_d3bytes_8[7]&0x04)<<5);  
  for (i=0;i<3;i++)
  {
   //gb_bufferFrame[gb_cont_bufferFrame+i] = d[i];
   p_address[gb_address_psExe_cont] = d[i];
   if (main2[gb_address_psExe_cont] != p_address[gb_address_psExe_cont])
   {
	gb_error = 1;
	gb_cont_error = gb_address_psExe_cont;
	gb_value_error = p_address[gb_address_psExe_cont];
	gb_src_value_error = main2[gb_address_psExe_cont];
	gb_cont_i = i;
	sprintf (gb_cadLog,"\nERROR id %d d%d src %x value %x",gb_cont_error,gb_cont_i,gb_src_value_error,gb_value_error);	
	break;	   
   }
   gb_address_psExe_cont++;
  }
  //gb_cont_bufferFrame += 3;
  //if (gb_cont_bufferFrame>200)
  // gb_cont_bufferFrame = 0;
  gb_cont_byte_psExe += 3;
  if (gb_size_psExe>0 && gb_cont_byte_psExe>0 && gb_cont_byte_psExe >= gb_size_psExe)
   gb_launch_exe = 1;  
  //sprintf (gb_cadLog,"\n%x%x%x",d[0],d[1],d[2]);  
 }
 sprintf (gb_cadLog,"\nbps %d Typ%d %x %d/%d",gb_speed,gb_type,gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe);	  
}

//****************************************************************
void PollReceivedHeadFourBtnFast()
{
 //0 a0 b0 c0
 //1 a1 b1 c1
 //0 a2 b2 c2
 //1 a3 b3 c3
 //0 a4 b4 c4
 //1 a5 b5 c5
 //0 a6 b6 c6
 //1 a7 b7 c7
 u_char d[3]={0,0,0}; //Leo 9 bytes de cabecera
 u_char i;
 u_char auxCRC=0; 
 if (globalNewData == 1)
 {
  globalNewData = 0;
  //Tenemos 3 bytes de datos
  d[0] = (gb_d3bytes_8[0]&0x01)
         |((gb_d3bytes_8[1]&0x01)<<1)
		 |((gb_d3bytes_8[2]&0x01)<<2)
		 |((gb_d3bytes_8[3]&0x01)<<3)
		 |((gb_d3bytes_8[4]&0x01)<<4)
		 |((gb_d3bytes_8[5]&0x01)<<5)
		 |((gb_d3bytes_8[6]&0x01)<<6)
		 |((gb_d3bytes_8[7]&0x01)<<7);
  d[1] = ((gb_d3bytes_8[0]&0x02)>>1)
         |(gb_d3bytes_8[1]&0x02)
		 |((gb_d3bytes_8[2]&0x02)<<1)
		 |((gb_d3bytes_8[3]&0x02)<<2)
		 |((gb_d3bytes_8[4]&0x02)<<3)
		 |((gb_d3bytes_8[5]&0x02)<<4)
		 |((gb_d3bytes_8[6]&0x02)<<5)
		 |((gb_d3bytes_8[7]&0x02)<<6);
  d[2] = ((gb_d3bytes_8[0]&0x04)>>2)
         |((gb_d3bytes_8[1]&0x04)>>1)
		 |(gb_d3bytes_8[2]&0x04)
		 |((gb_d3bytes_8[3]&0x04)<<1)
		 |((gb_d3bytes_8[4]&0x04)<<2)
		 |((gb_d3bytes_8[5]&0x04)<<3)
		 |((gb_d3bytes_8[6]&0x04)<<4)
		 |((gb_d3bytes_8[7]&0x04)<<5);

  for (i=0;i<3;i++)  
   gb_bufferFrame[gb_cont_bufferFrame+i] = d[i];    
  gb_cont_bufferFrame += 3;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 10;
  if (gb_cont_bufferFrame == 9)
  {   
   gb_size_psExe = gb_bufferFrame[0] | (gb_bufferFrame[1]<<8) | (gb_bufferFrame[2]<<16);
   gb_speed = gb_bufferFrame[3];
   gb_type = gb_bufferFrame[4];
   gb_address_psExe = (0x80<<24) | (gb_bufferFrame[7]<<16) | (gb_bufferFrame[6]<<8) | gb_bufferFrame[5];
   for (i=0;i<9;i++)
    auxCRC = auxCRC|gb_bufferFrame[i];
   if (gb_bufferFrame[8] == auxCRC)
	gb_CRC = 0;
   else
	gb_CRC = -1;
   gb_receivedHead = 1; //Ya se ha recibido la cabecera	  
  }
 }
 sprintf (gb_cadLog,"\nbps %d Typ%d %x %d",gb_speed,gb_type,gb_address_psExe,gb_size_psExe);
 //sprintf (gb_cadLog,"\n%d",gb_cont_bufferFrame);
}
//****************************************************************
void HandleFourBtn()
{
 //R1,R2,L1,L2 bit Data 4 bits Read 3 bits
 //R1,R2,L1,L2 bit Data First 4 bits Data, Second 4 bits R1,R2,L1,L2
 //Bit 3 - 1 - Nibble High Bit 3
 //Bit 3 - 0 - Nibble Low
 //Bit 3 - 1 - Nibble Low
 u_char auxBit0,auxBit1,auxBit2; 
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
   switch (gb_cont_bit)
   {
    case 0: gb_Q0 = auxBit0;   
     gb_Q1 = auxBit1;
  	 gb_Q2 = auxBit2;
     break;
    case 1: gb_Q3 = auxBit0;
     gb_Q4 = auxBit1;
  	 gb_Q5 = auxBit2;
     break;
    case 2: gb_Q6 = auxBit0;
     gb_Q7 = auxBit1;	
     break;
    default: break;
   }
   gb_cont_bit ++;
   if (gb_cont_bit>2)
   {
    gb_cont_bit = 0;
    globalNewData = 1;
   }
   cadPAD[3]=auxBit0+48; 
   cadPAD[2]=auxBit1+48;
   cadPAD[1]=auxBit2+48;
   cadPAD[0]=gb_std+48;
   cadPAD[4]='\0'; //Muestro solo 4 bits  
  }
 }
}

//***************************************************
void PollFourBtn()
{
 //Convierte 4 bits(3 bits data)
 u_char aux=0;   
  
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4) | (gb_Q5<<5)| (gb_Q6<<6)| (gb_Q7<<7);
  
  gb_bufferFrame[gb_cont_bufferFrame] = aux;
  gb_cont_bufferFrame++;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 6;
  //0 size bytes multiple 2048
  //1,2,3,4 Addres memory psEXE
  //5 ... Bytes psExe  
  if (gb_cont_bufferFrame == 5)
  {
   gb_size_psExe = (gb_bufferFrame[0] * 2048);
   gb_address_psExe = (gb_bufferFrame[4]<<24) | (gb_bufferFrame[3]<<16) | (gb_bufferFrame[2]<<8) | gb_bufferFrame[1];
   gb_firstBeginExe = 1;
   gb_cont_byte_psExe = -1;
  }
  if (gb_firstBeginExe == 1)
   gb_cont_byte_psExe++;
 }
 sprintf (gb_cadLog,"\n%x %d/%d ",gb_address_psExe,gb_size_psExe,gb_cont_byte_psExe);
}


//***************************************************
void PollReceivedHeadFourBtn()
{
 //Convierte 4 bits(3 bits data)
 u_char aux=0;     
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  aux = gb_Q0 | (gb_Q1<<1) | (gb_Q2<<2) | (gb_Q3<<3) | (gb_Q4<<4) | (gb_Q5<<5)| (gb_Q6<<6)| (gb_Q7<<7);
  
  gb_bufferFrame[gb_cont_bufferFrame] = aux;
  gb_cont_bufferFrame++;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 8;
  //0,1 size bytes multiple 2048
  //2,3,4,5 Addres memory psEXE  
  if (gb_cont_bufferFrame == 7)
  {   
   gb_size_psExe = ((gb_bufferFrame[0]|((gb_bufferFrame[1]&0x03)<<8)) << 11);
   gb_speed = (gb_bufferFrame[1]>>2)&0x0F;
   gb_type = (gb_bufferFrame[1]>>6)&0x03;
   gb_address_psExe = (gb_bufferFrame[5]<<24) | (gb_bufferFrame[4]<<16) | (gb_bufferFrame[3]<<8) | gb_bufferFrame[2];
   gb_receivedHead = 1; //Ya se ha recibido la cabecera
   //gb_currentTick = GetRCnt(0);   
   //gb_tiempo = gb_lastTick-gb_currentTick;
  }
 }
 sprintf (gb_cadLog,"\nbps %d Typ%d %x %d",gb_speed,gb_type,gb_address_psExe,gb_size_psExe);	
}

//***********************************************************************
void CargaPrograma(struct EXEC *exep){
 char hbuf[H_SIZE];	// Buffer for EXE head
 struct XF_HDR *head; 
 u_char *p_address = (u_char *)gb_address_psExe; 
 //memcpy (hbuf, main2,H_SIZE);
 memcpy (hbuf, p_address,H_SIZE);
 head = (struct XF_HDR *)hbuf;
 memcpy((void *)exep, (void *)&head->exec, sizeof(struct EXEC));
 //memcpy((void *)exep->t_addr,&main2[H_SIZE],exep->t_size);
 memcpy((void *)exep->t_addr,&p_address[H_SIZE],exep->t_size); 
}

//***********************************************************************
//void CopyMemExe()
//{
// void *p_address = (void *)gb_address_psExe;
// //memcpy((void *)gb_address_psExe,&main2,28196); //Copiar todo
// memcpy((void *)gb_address_psExe,&main2[],28196)
//}

/***************** functions ********************/
int main(void) {	
	
	InitGraphics();			//this method sets up gfx for printing to screen
	
	FntLoad(960, 256);		//this loads the font gfx to the vram
	//FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
	//x y ini, width heigh, clear, max char
	FntOpen(8, 8, 300, 24, 0, 64);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc    	
	DrawSync(0);
    
    //gb_lastTick = GetRCnt(0);	
	//while (1) 
	gb_address_psExe = 0x80010000;
	//CopyMemExe();
	//gb_launch_exe = 1;
    while (gb_launch_exe == 0)
	{				//infinite loop
		activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
		GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
		GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents
							
		//once the OT contents have been cleared you can add new objects to the OT
		//so that they will be displayed next time DisplayAll is called

        if (gb_receivedHead == 0)
		{
	     //HandleFourBtn();
		 HandleFourBtnFast();		 
		 PollReceivedHeadFourBtnFast();
		 //PollReceivedHeadFourBtn();
		}
		else
		{
		 //Handle13Bytes();
		 HandleFourBtnFast();
		 gb_address_psExe = 0x80010000;
		 PollFourBtnFast();
		 //Poll13Bytes();
		 //HandleFourBtnFast();
		 //PollFourBtnFast();
		}
		//FntPrint("13B %s %x %s\nBaud %d Type %d",cadPAD14,pad,gb_cadLog,gb_speed,gb_type);
		//FntPrint("13B %s %x %s %d",cadPAD14,pad,gb_cadLog,gb_tiempo);
		FntPrint("13B %s %x %s CRC%d",cadPAD14,pad,gb_cadLog,gb_CRC);
		//HandleFourBtn();
		//PollFourBtn();		
		//FntPrint("4B %s %x %s",cadPAD,pad,gb_cadLog);
		
		//Handle14Btns();
		//Poll14Btns();
		//FntPrint("14B %s %x %s",cadPAD14,pad,gb_cadLog);
		
		//Handle14BtnsFast();
		//Poll14BtnsFast();
		//FntPrint("14BF %s %x %s",cadPAD14,pad,gb_cadLog);				


		
		//FntPrint("Serial PAD\n PAD %s %d %d\nLetra %c %s",cadPAD,datoPAD,pad,letra,bufferLetra);
		//this just adds the string to the screen printing buffer
		//to actually print this to screen yu have to flush the contents of this buffer
		//this is done in displayall method
		
		DisplayAll(activeBuffer);		//this displays the OT contents to screen		
	}
	
	
	////Ini JJ exec	
	gb_address_psExe = 0x80010000;
    CargaPrograma(&exe);
    ResetGraph(3);
    PadStop();
	StopCallback();
	EnterCriticalSection();
     Exec(&exe,1,0);
	//return 0;		//when program is finished return from it	
	//Fin JJ exec
	
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
	
	//PadInit(0);	
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
