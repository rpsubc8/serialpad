//Autor: ackerman
//upload PsExe PAD port with ARDUINO
//Buttons R1,R2,L1,L2
//Jaime Jose Gavin Sierra

//Pad1L2          (1<< 0)  0
//Pad1R2          (1<< 1)  1
//Pad1L1          (1<< 2)  2
//Pad1R1          (1<< 3)  3
//Pad1tri         (1<< 4)  4
//Pad1crc         (1<< 5)  5
//ad1x            (1<< 6)  6
//Pad1sqr         (1<< 7)  7
//Pad1Select      (1<< 8)  8
//Pad1Start       (1<<11)  9
//Pad1Up          (1<<12) 10
//Pad1Right       (1<<13) 11
//Pad1Down        (1<<14) 12
//Pad1Left        (1<<15) 13

#include <kernel.h>
#include <libapi.h>
#include <libpad.h>
#include <sys/types.h>	//these are the various include files that need to be included
#include <libetc.h>		//note that the order they are included in does matter in some cases!
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <strings.h>
#include "pad.h"
//#include "kk.h"
//#include "demo1.h" //Para comparar serie con datos memoria
//u_char main2[20428]; //Lo defino para que compile bien, hay que quitarlo
u_char main2[10]; //Lo defino para que compile bien, hay que quitarlo

#define OT_LENGTH (10) //this is the 'length' of the OT (ordering tabble)
					   //for more info on ordering tables, check out ot.txt on my 'info' page

#define PACKETMAX (2048) //these are the graphics area contants
#define PACKETMAX2 (PACKETMAX*24) //allows you to select how many gfx objects the psx should handle
					              //in this particular example
								  
#define SCREEN_WIDTH 320 //contants to determine the height and width of the screen resolution
//Modo PAL
//#define SCREEN_HEIGHT 256
//Modo NTSC
#define SCREEN_HEIGHT 240
#define tope_texto 1024
#define booleana unsigned char
#define TRUE 1
#define FALSE 0
#define H_SIZE 2048								  


GsOT myOT[2]; //an array of 2 OT's this is how your double buffering is implemented
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH];
PACKET GPUPacketArea[2][PACKETMAX2]; //also 2 gfx packet areas for double buffering



//Defines
#define IMASK					*((unsigned int*)0x1F801074)
#define IPENDING				*((unsigned int*)0x1F801070)

//Copied from pad.c
#define PADSIO_DATA(x)	*((unsigned char*)(0x1f801040 + (x<<4)))
#define PADSIO_STATUS(x)	*((unsigned short*)(0x1f801044 + (x<<4)))
#define PADSIO_MODE(x)	*((unsigned short*)(0x1f801048 + (x<<4)))
#define PADSIO_CTRL(x)	*((unsigned short*)(0x1f80104a + (x<<4)))
#define PADSIO_BAUD(x)	*((unsigned short*)(0x1f80104e + (x<<4)))

#define PAD_NONE 0xFF
#define PAD_DIGITAL	0x41
#define PAD_ANALOG 0x73
#define PAD_FLIGHT 0x53
#define PAD_MOUSE 0x12


char gb_cadLog[200]="\0";
u_long gb_pad=0; //El estado de botones
int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed
u_char gb_std=0;
u_char gb_std_antes=0;
u_char gb_cont_bit=0;
u_char gb_bufferFrame[256]; //trama
int gb_cont_bufferFrame=0;

u_char globalNewData=0;
u_char gb_byte=0; //El byte de dato
u_char gb_bytes[6]={0,0,0,0,0,0}; //Varios bytes
u_char gb_error=0;
unsigned int gb_size_psExe=0;
unsigned int gb_address_psExe=0;
unsigned int gb_cont_byte_psExe=0;
u_char gb_receivedHead= 0; //No se ha recibido aun la cabecera
int gb_type = -1;
int gb_speed = -1;
int gb_CRC = -1;
u_char gb_launch_exe= 0;
unsigned int gb_address_psExe_cont=0;
unsigned char gb_BeginData = 0;

u_char *gb_p_address; //Puntero a memoria PSX
struct EXEC exe;

static u_char padbuf[2][34]; //Mandos analogico
//u_char* padi = padbuf[0];


/***************** prototypes *******************/
void PollAnalogPAD();
void InitGraphics(void);		  //this method sets up gfx for printing to screen
void DisplayAll(int);			  //this displays the contents of the OT
void HandleFourBtn(void);
void PollFourBtn(void);
void HandleFourBtn2PAD(void);
void PollFourBtn2PAD(void);
void Handle3bytes14Btn(void);
void Poll3bytes14Btn(void);
void Handle1byte9Btn(void);
void Poll1byte9Btn(void);
void Handle1byte9BtnNoFlag(void);
void Poll1byte9BtnNoFlag(void);
void Handle5bytesAnalog(void);
void Poll5bytesAnalog(void);
void Handle3bytesAnalog(void); //Speed 8
void Poll3bytesAnalog(void);
void HandleSpeed10Flag(void); //Speed 10
void PollSpeed10Flag(void);
void HandleSpeed11NoFlag(void); //Speed 11
void PollSpeed11NoFlag(void);
void Handle3bytesAnalogNoFlag(void); //Speed 9
void Poll3bytesAnalogNoFlag(void);
void Handle6bytes14Btn2PAD(void);
void Poll6bytes14Btn2PAD(void);
//void Handle3bytes14BtnNotSync(void);
//void Poll3bytes14BtnNotSync(void);
void HandleFourBtnNotSync(void); //Falla
void PollFourBtnNotSync(void); //Falla
void PollReceivedHeadFourBtn(void);


//void SendData(int pad_n, unsigned char *in, unsigned char *out, int len);
//void TestSend();
void CargaPrograma(struct EXEC *exep);
int main(void);


//Send data to PAD_SIO
/*void SendData(int pad_n, unsigned char *in, unsigned char *out, int len)
{int x,y,i;	
	PADSIO_MODE(0) = 0x0D;
	PADSIO_BAUD(0) = 0x88;	
	if(pad_n == 1) PADSIO_CTRL(0) = 0x3003; else PADSIO_CTRL(0) = 0x1003;		
	for(y=0;y<400;y++);	//Slight delay before first transmission
	for(x = 0; x < len; x++)
	{		
		while((PADSIO_STATUS(0) & 4) < 1);//Wait for TX ready		
		//while((PADSIO_STATUS(0) & 0x04) == 0x00);//Wait for TX ready		
		PADSIO_DATA(0) = *in;
		in++;		
		while((PADSIO_STATUS(0) & 2) < 1);//Read RX status flag		
		*out = PADSIO_DATA(0);
		out++;
	}	
	PADSIO_CTRL(0) = 0;
}*/

//***********************************************************************
/*void TestSend()
{
 unsigned char DataToSend[] =  {1, 0x42, 0, 0, 0, 0, 0, 0, 0};
 unsigned char ReceivedData[16];
 SendData(0, DataToSend, ReceivedData, sizeof(DataToSend));
 sprintf (gb_cadLog,"Test %02x%02x%02x%02x",ReceivedData[0],ReceivedData[1],ReceivedData[2],ReceivedData[3]);
}*/

//***********************************************************************
void PollAnalogPAD()
{  
 //gb_pad=PadRead(0);            
 //PadGetState(0x00); 
 //PadInfoMode(0,InfoModeCurID,0); 
 //PadGetState(0);
 //PadInfoMode(0,InfoModeCurExID,0); 
 if (padbuf[0][0] == 0)  
  gb_pad = ~((padbuf[0][2]<<8) | (padbuf[0][3]));	 
 else  
  gb_pad = 0;
}

//***********************************************************************
void CargaPrograma(struct EXEC *exep){
 char hbuf[H_SIZE];	// Buffer for EXE head
 struct XF_HDR *head; 
 u_char *p_address = (u_char *)gb_address_psExe; 
 //memcpy (hbuf, main2,H_SIZE); //Para cargar de memoria
 memcpy (hbuf, p_address,H_SIZE); //Carga de serie
 head = (struct XF_HDR *)hbuf;
 memcpy((void *)exep, (void *)&head->exec, sizeof(struct EXEC));
 //memcpy((void *)exep->t_addr,&main2[H_SIZE],exep->t_size); //Carga de memoria
 memcpy((void *)exep->t_addr,&p_address[H_SIZE],exep->t_size);  //Carga de serie
}

//************************************************************************
//No funciona
/*void Handle3bytes14BtnNotSync()
{//Sin sincronismo rapido
 //Izquierda Abajo Derecha Arriba Start Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 //Left      Down  Right   Up     V     C         S     Z  X        D       R  W  T  E  
 //0 1 byte 4 bits - 1 bit CRC
 //1 1 byte 4 bits - 1 bit CRC
 if (gb_error == 1)
  return;
 gb_pad=PadRead(0);
 if (gb_pad&Pad1Left) gb_std=1; else gb_std=0;
 
 if (gb_std == 0)
 {
  gb_bytes[0] = gb_pad & 0xFF; 
  gb_bytes[1] = ((gb_pad>>8)&0x01)|((gb_pad>>10)&0x0F);	 	  
  gb_std_antes = 0;
 }
 else
 {
  if (gb_std_antes == 0)
  {
   gb_std_antes = 1;
   gb_bytes[2] = gb_pad & 0xFF;
   gb_bytes[1] = gb_bytes[1]|(((gb_pad>>4)&0x10)|((gb_pad>>6)&0xF0));   
   gb_cont_bit++;
   globalNewData = 1;	   
  }	 
 }
 
}
*/

//************************************************************************
//No funciona
/*void Poll3bytes14BtnNotSync()
{
 Poll3bytes14Btn(); //Se puede usar el mismo
}*/


//************************************************************************
void Handle6bytes14Btn2PAD()
{//6 bytes 2 PADS
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0);
 if (gb_pad&Pad1Left) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   switch (gb_cont_bit)
   {
    case 0: gb_bytes[0] = gb_pad & 0xFF; 
	 gb_bytes[1] = ((gb_pad>>8)&0x01)|((gb_pad>>10)&0x0F);
	 gb_bytes[3] = (gb_pad>>16) & 0xFF; //PAD2 byte 4
	 gb_bytes[4] = ((gb_pad>>24)&0x01)|((gb_pad>>26)&0x0F); //PAD 2 byte 5
	 break;
    case 1: gb_bytes[2] = gb_pad & 0xFF;
	 gb_bytes[1] = gb_bytes[1]|(((gb_pad>>4)&0x10)|((gb_pad>>6)&0xF0));
	 gb_bytes[5] = (gb_pad >>16)& 0xFF; //PAD 2 byte 6
	 gb_bytes[4] = gb_bytes[4]|(((gb_pad>>20)&0x10)|((gb_pad>>22)&0xF0));//PAD 2 byte 5
  	 break;    
    default: break;
   }   
   gb_cont_bit ++;
   if (gb_cont_bit>1)
   {	   
    gb_cont_bit = 0;
    globalNewData = 1;
    //gb_error = !((gb_pad & 0x04)==(gb_byte & 0x04)); //CRC		
   } 
  }
 }
}

//************************************************************************
void Poll6bytes14Btn2PAD()
{
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<6;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     gb_error = 1;
     sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;   
  }

  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
	
}


//************************************************************************
void Handle1byte9Btn()
{
 //Recibe 1 byte 9 botones con Sync
 //PSX  ePSXe   Function  
 //Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 // C         S     Z  X        D       R  W  T  E   
 //Pad1L2          (1<< 0)  0
 //Pad1R2          (1<< 1)  1
 //Pad1L1          (1<< 2)  2
 //Pad1R1          (1<< 3)  3
 //Pad1tri         (1<< 4)  4
 //Pad1crc         (1<< 5)  5
 //Pad1x           (1<< 6)  6
 //Pad1sqr         (1<< 7)  7
 //Pad1Select      (1<< 8)  8  
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0);
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   gb_byte = gb_pad & 0xFF;   
   globalNewData = 1;
  }
 }
}

//************************************************************************
void Poll1byte9Btn()
{
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != gb_byte)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
   }
  }
  gb_p_address[gb_address_psExe_cont] = gb_byte;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
  
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}



//************************************************************************
void Handle5bytesAnalog()
{
 //Recibe 5 byte 9 botones y 4 bytes Analogico con Sync
 //PSX  ePSXe   Function  
 //Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 // C         S     Z  X        D       R  W  T  E   
 //Pad1L2          (1<< 0)  0
 //Pad1R2          (1<< 1)  1
 //Pad1L1          (1<< 2)  2
 //Pad1R1          (1<< 3)  3
 //Pad1tri         (1<< 4)  4
 //Pad1crc         (1<< 5)  5
 //Pad1x           (1<< 6)  6
 //Pad1sqr         (1<< 7)  7
 //Pad1Select      (1<< 8)  8  
 //if (gb_error == 1)
 // return;
 //ReadPads();
    
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   gb_bytes[0] = gb_pad & 0xFF;
   gb_bytes[1] = padbuf[0][4]; //Right X
   gb_bytes[2] = padbuf[0][5]; //Right Y
   gb_bytes[3] = padbuf[0][6]; //Left X
   gb_bytes[4] = padbuf[0][7]; //Left Y
   globalNewData = 1;
  }
 }
}

//************************************************************************
void Poll5bytesAnalog()
{//Analogico 5 bytes
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<5;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     //gb_error = 1;
     //sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void Handle3bytesAnalog()
{//Recibe 3 bytes Analogico con Sync Speed 8
 //4 Right X, 5 Right Y, 6 Left X, 7 Left Y
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0; 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   gb_bytes[0] = ((padbuf[0][4]&0x0F)|((padbuf[0][5]<<4)&0xF0));
   gb_bytes[1] = ((padbuf[0][6]&0x0F)|((padbuf[0][7]<<4)&0xF0));
   //gb_bytes[3] = ((padbuf[0][4]>>4)&0x03)|((padbuf[0][5]>>2)&0x0C)|(padbuf[0][6]&0x30)|((padbuf[0][7]<<2)&0xC0);
   globalNewData = 1;
  }
 }
}

//************************************************************************
void Poll3bytesAnalog()
{//Analogico 3 bytes Speed 8
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<2;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     gb_error = 1;
     sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_bytes[i]);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void HandleSpeed10Flag()
{//Recibe 4 bytes Analogico con Sync Speed 10
 //4 Right X, 5 Right Y, 6 Left X, 7 Left Y
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0; 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   gb_bytes[0] = ((padbuf[0][4]&0x0F)|((padbuf[0][5]<<4)&0xF0));
   gb_bytes[1] = ((padbuf[0][6]&0x0F)|((padbuf[0][7]<<4)&0xF0));
   gb_bytes[2] = ((padbuf[0][4]>>4)&0x03)
                 |(((padbuf[0][5]>>2)&0x0C))
	 		     |(padbuf[0][6]&0x30)
	 		     |((padbuf[0][7]<<2)&0xC0);				
   gb_bytes[3] = (gb_pad&0x0F)
                 |((padbuf[0][4]>>2)&0x10)
				 |((padbuf[0][5]>>1)&0x20)
				 |((padbuf[0][6])&0x40)
				 |((padbuf[0][7]<<1)&0x80);
   globalNewData = 1;
   //gb_bytes[2] = ((padbuf[0][4]>>4)&0x03)
   //              |(((padbuf[0][5]>>4)&0x03)<<2)
   //    		   |(((padbuf[0][6]>>4)&0x03)<<4)
   //		  	   |(((padbuf[0][7]>>4)&0x03)<<6);   
  }
 }
}

//************************************************************************
void PollSpeed10Flag()
{//Analogico 4 bytes Speed 10
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<4;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     gb_error = 1;
     sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_bytes[i]);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void HandleSpeed11NoFlag()
{//Recibe 4 bytes Analogico con Sync Speed 11
 //4 Right X, 5 Right Y, 6 Left X, 7 Left Y
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0; 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  gb_bytes[0] = ((padbuf[0][4]&0x0F)|((padbuf[0][5]<<4)&0xF0));
  gb_bytes[1] = ((padbuf[0][6]&0x0F)|((padbuf[0][7]<<4)&0xF0));
  gb_bytes[2] = ((padbuf[0][4]>>4)&0x03)
                |(((padbuf[0][5]>>2)&0x0C))
			    |(padbuf[0][6]&0x30)
			    |((padbuf[0][7]<<2)&0xC0);
  gb_bytes[3] = (gb_pad&0x0F)
                |((padbuf[0][4]>>2)&0x10)
			    |((padbuf[0][5]>>1)&0x20)
			    |((padbuf[0][6])&0x40)
			    |((padbuf[0][7]<<1)&0x80);
  globalNewData = 1;
  /*gb_bytes[2] = ((padbuf[0][4]>>4)&0x03)
                |(((padbuf[0][5]>>4)&0x03)<<2)
			    |(((padbuf[0][6]>>4)&0x03)<<4)
			    |(((padbuf[0][7]>>4)&0x03)<<6);
			 */  
 }
}

//************************************************************************
void PollSpeed11NoFlag()
{//Analogico 4 bytes Speed 11 No flag
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<4;i++)
  {
   //if (gb_address_psExe_cont <(gb_size_psExe-30))
   //{
   // if (main2[gb_address_psExe_cont] != gb_bytes[i])
   // {   
   //  gb_error = 1;
   //  sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_bytes[i]);
   // }
   //}
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void Handle3bytesAnalogNoFlag()
{//Recibe 3 bytes Analogico con Sync Speed 9
 //4 Right X, 5 Right Y, 6 Left X, 7 Left Y
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0; 
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  gb_bytes[0] = ((padbuf[0][4]&0x0F)|((padbuf[0][5]<<4)&0xF0));
  gb_bytes[1] = ((padbuf[0][6]&0x0F)|((padbuf[0][7]<<4)&0xF0));  
  globalNewData = 1;
 }
}

//************************************************************************
void Poll3bytesAnalogNoFlag()
{//Analogico 3 bytes Speed 9
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<2;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     gb_error = 1;
     sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_bytes[i]);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void Handle1byte9BtnNoFlag()
{//25 ms
 //Recibe 1 byte 9 botones con Sync
 //PSX  ePSXe   Function  
 //Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 // C         S     Z  X        D       R  W  T  E   
 //Pad1L2          (1<< 0)  0
 //Pad1R2          (1<< 1)  1
 //Pad1L1          (1<< 2)  2
 //Pad1R1          (1<< 3)  3
 //Pad1tri         (1<< 4)  4
 //Pad1crc         (1<< 5)  5
 //Pad1x           (1<< 6)  6
 //Pad1sqr         (1<< 7)  7
 //Pad1Select      (1<< 8)  8  
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0);
 if (gb_pad&Pad1Select) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  gb_byte = gb_pad & 0xFF;
  globalNewData = 1;
 }
}

//************************************************************************
void Poll1byte9BtnNoFlag()
{//25 ms
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != gb_byte)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
   }
  }
  gb_p_address[gb_address_psExe_cont] = gb_byte;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
  
  
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void Handle3bytes14Btn()
{
 //Recibe 3 bytes 14 botones con Sync
 //PSX  ePSXe   Function  
 //Izquierda Abajo Derecha Arriba Start Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 //Left      Down  Right   Up     V     C         S     Z  X        D       R  W  T  E   
 //Pad1L2          (1<< 0)  0
 //Pad1R2          (1<< 1)  1
 //Pad1L1          (1<< 2)  2
 //Pad1R1          (1<< 3)  3
 //Pad1tri         (1<< 4)  4
 //Pad1crc         (1<< 5)  5
 //Pad1x           (1<< 6)  6
 //Pad1sqr         (1<< 7)  7
 //Pad1Select      (1<< 8)  8
 //                      9
 //	   				    10
 //Pad1Start       (1<<11)  9
 //Pad1Up          (1<<12) 10
 //Pad1Right       (1<<13) 11
 //Pad1Down        (1<<14) 12
 //Pad1Left        (1<<15) 13 
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0);
 if (gb_pad&Pad1Left) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   switch (gb_cont_bit)
   {
    case 0: gb_bytes[0] = gb_pad & 0xFF; 
	 gb_bytes[1] = ((gb_pad>>8)&0x01)|((gb_pad>>10)&0x0F);	 
	 break;
    case 1: gb_bytes[2] = gb_pad & 0xFF;
	 gb_bytes[1] = gb_bytes[1]|(((gb_pad>>4)&0x10)|((gb_pad>>6)&0xF0));
  	 break;    
    default: break;
   }   
   gb_cont_bit ++;
   if (gb_cont_bit>1)
   {	   
    gb_cont_bit = 0;
    globalNewData = 1;
    //gb_error = !((gb_pad & 0x04)==(gb_byte & 0x04)); //CRC		
   } 
  }
 }
}

//************************************************************************
void Poll3bytes14Btn()
{
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0; 
  for (i=0;i<3;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     //gb_error = 1;
     //sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
   if ((gb_type == 1)&&(gb_address_psExe_cont==128))
    gb_address_psExe_cont= 2048;   
  }

  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}

//************************************************************************
void HandleFourBtn2PAD()
{
 //Recibe 2 byte 4 botones con Sync 
 //PAD1
 //L2 -   E  - Data Bit 0,3,6 3 bits
 //R2 -   T  - Data Bit 1,4,7 3 bits
 //L1 -   W  - Data Bit 2,5,x 2 bits
 //R1 -   R  - sync
 //PAD2
 //L2 -   0  - Data Bit 0,4,x 4 bits
 //R2 -   1  - Data Bit 1,5,x 4 bits
 //L1 -   2  - Data Bit 2,6,x 4 bits
 //R1 -   3  - Data Bit 3,7,x 
 //Pad2L2          (1<<16) 
 //Pad2R2          (1<<17)
 //Pad2L1          (1<<18)
 //Pad2R1          (1<<19)
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0); 
 if (gb_pad&Pad1R1) gb_std=1; else gb_std=0;
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   switch (gb_cont_bit)
   {
    case 0: gb_bytes[0] = gb_pad & 0x07;
	 gb_bytes[1] = (gb_pad >> 16)&0x0F;
	 break;
    case 1: gb_bytes[0] |= ((gb_pad & 0x07)<<3);
	 gb_bytes[1] |= ((gb_pad >> 12)&0xF0);
	 break;
    case 2: gb_bytes[0] |= ((gb_pad & 0x03)<<6); break;
    default: break;
   }   
   gb_cont_bit ++;
   if (gb_cont_bit>2)
   {	   
    gb_cont_bit = 0;
    globalNewData = 1;
    //gb_error = !((gb_pad & 0x04)==(gb_byte & 0x04)); //CRC		
   } 
  }
 } 
}

//************************************************************************
//Falla
void HandleFourBtnNotSync()
{//4 botones 2 bytes sin sincronia
 // 0: estado raro posible 1
 // 1: 1 3 bits   Primer byte
 // 2: 0 3 bits
 // 3: 1 2 bits
 // 4: 0 3 bits   Segundo byte
 // 5: 1 3 bits
 // 6: 0 2 bits 
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0); 
 if (gb_pad&Pad1R1) gb_std=1; else gb_std=0;


 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  switch (gb_cont_bit)
  {
   case 0: break;   
   case 1: gb_byte = gb_pad & 0x07; 
    break;
   case 2: gb_byte |= ((gb_pad & 0x07)<<3);
    break;
   case 3: gb_byte |= ((gb_pad & 0x03)<<6);     
	globalNewData = 1;
	gb_cont_bit=1;
	return;
	break;
   default: break;
  }
  gb_cont_bit++;   	  
 }
   	 
 
  /*switch (gb_cont_bit)
  {  
   case 0: break;  
   case 1: gb_bytes[0] = gb_pad & 0x07; break; 
   case 2: gb_bytes[0] |= ((gb_pad & 0x07)<<3); break;
   case 3: gb_bytes[0] |= ((gb_pad & 0x03)<<6); break;
   case 4: gb_bytes[1] = gb_pad & 0x07; break;
   case 5: gb_bytes[1] |= ((gb_pad & 0x07)<<3); break;   
   case 6: gb_bytes[1] |= ((gb_pad & 0x03)<<6); break;
   case 7:    gb_cont_bit = 0;
	globalNewData = 1;
	return;
   default: break;   
  }
     
 if (gb_std_antes != gb_std)
 {
  gb_std_antes = gb_std;
  gb_cont_bit ++;
 }*/
}

//************************************************************************
//Falla
void PollFourBtnNotSync()
{//1 PAD 4 botones 2 bytes Sin sincronismo
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0;
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != gb_byte)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
   }
  }
  gb_p_address[gb_address_psExe_cont] = gb_byte;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;  

  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void PollFourBtn2PAD()
{//Lee 
 u_char i=0;
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0;  
  for (i=0;i<2;i++)
  {
   if (gb_address_psExe_cont <(gb_size_psExe-30))
   {
    if (main2[gb_address_psExe_cont] != gb_bytes[i])
    {   
     gb_error = 1;
     sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
	 return;
    }
   }
   gb_p_address[gb_address_psExe_cont] = gb_bytes[i];
   gb_address_psExe_cont++;
  }
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}


//************************************************************************
void HandleFourBtn()
{
 //Recibe 1 byte 4 botones con Sync 
 //PSX  ePSXe   Function  
 //L2 -   E  - Data Bit 0,3,6 3 bits
 //R2 -   T  - Data Bit 1,4,7 3 bits
 //L1 -   W  - Data Bit 2,5,x 2 bits
 //R1 -   R  - sync
 if (gb_error == 1)
  return;
 //gb_pad=PadRead(0);
 if (gb_pad&Pad1R1) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {  
  gb_std_antes = gb_std;
  if (gb_std == 1)
  {
   switch (gb_cont_bit)
   {
    case 0: gb_byte = gb_pad & 0x07; break;
    case 1: gb_byte |= ((gb_pad & 0x07)<<3); break;
    case 2: gb_byte |= ((gb_pad & 0x03)<<6); break;
    default: break;
   }   
   gb_cont_bit ++;
   if (gb_cont_bit>2)
   {	   
    gb_cont_bit = 0;
    globalNewData = 1;
    //gb_error = !((gb_pad & 0x04)==(gb_byte & 0x04)); //CRC		
   } 
  }
 }
}

//************************************************************************
void PollReceivedHeadFourBtn()
{
 //Convierte 4 bits(3 bits data) 
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0;      
  gb_bufferFrame[gb_cont_bufferFrame] = gb_byte;
  gb_cont_bufferFrame++;
  if (gb_cont_bufferFrame>200)
   gb_cont_bufferFrame = 8; 
  if (gb_cont_bufferFrame == 7)
  {   
   gb_size_psExe = (gb_bufferFrame[0]|((gb_bufferFrame[1])<<8)|((gb_bufferFrame[2])<<16));
   gb_speed = (gb_bufferFrame[3]&0x3F);
   gb_type = (gb_bufferFrame[3]>>6)&0x03;
   gb_address_psExe = (0x80<<24) | (gb_bufferFrame[6]<<16) | (gb_bufferFrame[5]<<8) | (gb_bufferFrame[4]);
   gb_receivedHead = 1; //Ya se ha recibido la cabecera
   
   gb_cont_bit = 0; //Fuerzo a resetear contador
   //gb_std_antes = gb_std;
   
   gb_p_address = (u_char *)gb_address_psExe;   
   gb_BeginData = 0; //Buscar silencio
  }
 }
}

//************************************************************************
void PollFourBtn()
{
 if (gb_error == 1)
  return;
 if (globalNewData == 1)
 {
  globalNewData = 0;
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != gb_byte)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_byte);
   }
  }
  gb_p_address[gb_address_psExe_cont] = gb_byte;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;
  //gb_address_psExe_cont+= 1920;
  if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
   gb_launch_exe = 1;
 }
}

/***************** functions ********************/
int i=0;
int port0,port1,info0,info1;

int main(void) 
{	
 InitGraphics();			//this method sets up gfx for printing to screen	
 FntLoad(960, 256);		//this loads the font gfx to the vram
 //FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
 //x y ini, width heigh, clear, max char
 FntOpen(8, 8, 300, 24, 0, 128);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc    	
 DrawSync(0);    
 //while (1) 
 //gb_address_psExe = 0x80010000;
 while (gb_launch_exe == 0)
 {				//infinite
  activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
  GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
  GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents
  
  
  PollAnalogPAD();  
  if (gb_receivedHead == 0)
  {
   if (gb_BeginData == 0)
   {//Busca que este en silencio
    //gb_pad=PadRead(0);
	if ((gb_pad&Pad1R1)==0)
	 gb_BeginData = 1;
   }
   else
   {	   
    HandleFourBtn();
    PollReceivedHeadFourBtn();
   }   
  }
  else	  	  
  {
//   if (gb_BeginData == 0)
//   {//Busca que este en silencio
//    //gb_pad=PadRead(0);
//	if ((gb_pad&Pad1R1)==0)
//	 gb_BeginData = 1;
//    //if ((gb_pad&Pad1Left)==0)
//	// gb_BeginData = 1;
//   }
//   else
   {	
    switch (gb_speed)
	{
	 case 0: HandleFourBtn(); //4 Botones mando modificado
      PollFourBtn();
	  break;
	 case 1: HandleFourBtnNotSync(); //Falla 4 Botones mando modificado Sin sincronia
      PollFourBtnNotSync();   //Falla
	  break;
	 case 2: Handle1byte9Btn(); //9 Botones 1 mando fake spi 50 ms
      Poll1byte9Btn();
	  break;
	 case 3: Handle1byte9BtnNoFlag(); //9 Botones 1 mando fake spi 25 ms
      Poll1byte9BtnNoFlag();
	  break;	  	  
	 case 4: Handle3bytes14Btn(); //14 Botones 1 mando fake spi
      Poll3bytes14Btn();
	  break;
	 case 6: Handle5bytesAnalog(); //Mando analogico
	  Poll5bytesAnalog();
	  break;
	 case 8: Handle3bytesAnalog(); //Mando analogico
	  Poll3bytesAnalog();
	  break;	  
	 case 9: Handle3bytesAnalogNoFlag(); //Mando analogico
	  Poll3bytesAnalogNoFlag();
	  break;	  	  
	 case 10: HandleSpeed10Flag(); //Mando analogico
	  PollSpeed10Flag();
	  break;
	 case 11: HandleSpeed11NoFlag(); //Mando analogico
	  PollSpeed11NoFlag();
	  break;
	 default:HandleFourBtn(); //4 Botones mando modificado
      PollFourBtn();
      break;	  
	}

    //HandleFourBtn2PAD(); //4 Botones mando modificado 2 PAD
    //PollFourBtn2PAD();          
    //Handle6bytes14Btn2PAD(); //14 botones 2 PAD 6 bytes
    //Poll6bytes14Btn2PAD();
    //Handle3bytes14BtnNotSync(); //No funciona 14 Botones 1 mando fake spi fast
    //Poll3bytes14BtnNotSync();   
   }  
  } 
  
  
  //port0= PadGetState(0x00);
  //port1= PadGetState(0x01);
  //PadGetState(0x02);
  //PadGetState(0x03);
     
  //info0= PadInfoMode(0,InfoModeCurID,0);
  //info1= PadInfoMode(1,InfoModeCurID,0);
  //PadInfoMode(2,InfoModeCurID,0);
  //PadInfoMode(4,InfoModeCurID,0);
  
  //TestSend();
 
  //Handle5bytesAnalog();
  if (gb_error != 1)        
   //sprintf (gb_cadLog,"%x %d/%d\n%d %d %04x",gb_address_psExe,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type,(gb_pad&0xFFFF));
   //sprintf (gb_cadLog,"%x %d/%d\n%d %d %04x %d %d",gb_address_psExe,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type,(gb_pad&0xFFFF),gb_std,gb_cont_bit);
//   sprintf (gb_cadLog,"%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x %d %d %d %d",
//   (padbuf[0][0]&0xFF),(padbuf[0][1]&0xFF),(padbuf[0][2]&0xFF),(padbuf[0][3]&0xFF),
//   (padbuf[0][4]&0xFF),(padbuf[0][5]&0xFF),(padbuf[0][6]&0xFF),(padbuf[0][7]&0xFF),
//   (padbuf[0][8]&0xFF),(padbuf[0][9]&0xFF),(padbuf[0][10]&0xFF),(padbuf[0][11]&0xFF),
//   (padbuf[0][12]&0xFF),(padbuf[0][13]&0xFF),(padbuf[0][14]&0xFF),(padbuf[0][15]&0xFF),
//   (padbuf[0][16]&0xFF),(padbuf[0][17]&0xFF),(padbuf[0][18]&0xFF),(padbuf[0][19]&0xFF),
//   (padbuf[0][20]&0xFF),(padbuf[0][21]&0xFF),(padbuf[0][22]&0xFF),(padbuf[0][23]&0xFF),
//   (padbuf[0][24]&0xFF),(padbuf[0][25]&0xFF),(padbuf[0][26]&0xFF),(padbuf[0][27]&0xFF),
//   (padbuf[0][28]&0xFF),(padbuf[0][29]&0xFF),(padbuf[0][30]&0xFF),(padbuf[0][31]&0xFF),
//   (padbuf[0][32]&0xFF),(padbuf[0][33]&0xFF),
//   port0,port1,info0,info1
//   );
  //sprintf (gb_cadLog,"%x %d/%d\n%d %d",gb_address_psExe,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type);
  sprintf (gb_cadLog,"%x %d/%d\n%d %d %04x %02x%02x%02x%02x",gb_address_psExe,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type,(gb_pad&0xFFFF),(padbuf[0][4]&0xff),(padbuf[0][5]&0xff),(padbuf[0][6]&0xff),(padbuf[0][7]&0xff));
   //sprintf (gb_cadLog,"%x %d/%d\nBeg%d STD%d Vel%d %x",gb_address_psExe,gb_size_psExe,gb_address_psExe_cont,gb_BeginData,gb_std,gb_speed,gb_pad);
  FntPrint("%s",gb_cadLog);
  
  DisplayAll(activeBuffer);		//this displays the OT contents to screen		
 }
 
 CargaPrograma(&exe); //Prepara exec
 ResetGraph(3);
 //PadStop(); //Solo usar con PadInit, con directport no
 PadStopCom();
 StopCallback();
 for (i=0;i<100;i++)
  VSync(0); //Esperamos 2 segundos 50x2
 EnterCriticalSection(); 
 Exec(&exe,1,0); //Fin exec
 
 return 0;		//when program is finished return from it
} 


void InitGraphics(void) {
	//Prueba JJ
	ResetCallback();//JJ
	ResetGraph(0); //JJ 
	//PadInit(0); //JJ
	//SetVideoMode( MODE_PAL ); //JJ
	SetVideoMode( MODE_NTSC ); //JJ
	SetDispMask(1);				// 0: inhibit display 			
	//this method sets up gfx for printing to screen
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0); //initialises the graphics system	
    // set the graphics mode resolutions (GsNONINTER for NTSC, and GsINTER for PAL)
	//GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);	//PAL
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0);	//NTSC
	//no Gs* * * functions will work unless GsInitGraph() has been called
	GsDefDispBuff(0, 0 , 0, SCREEN_HEIGHT); //defines double buffer attributes
	//buffer 0's top left coordinate becomes (0,0) & buffer 1's coordinate becomes (0, y resolution)
	myOT[0].length = OT_LENGTH; //sets OT length for each OT
	myOT[1].length = OT_LENGTH;
	myOT[0].org = myOT_TAG[0]; //gets top address of GsOT_TAG table
	myOT[1].org = myOT_TAG[1];
	GsClearOt(0,0,&myOT[0]); //initialises ordering table
	GsClearOt(0,0,&myOT[1]);	
	
	
    //Mandos analogico directo
	PadInitDirect(padbuf[0], padbuf[1]);
	PadStartCom();
	gb_pad = 0;		//controller value initialize
	//Fin mandos analogicos directo
	
	
	//Multitap
//	PadInitMtap(padbuf[0],padbuf[1]);
	//InitTAP(padbuf[0],34,padbuf[1],34);
	//VSync(0);
    //StartTAP();
    //VSync(0);
	//ChangeClearPAD(0);

	//StartTAP();
	//PadStartCom();
	//gb_pad = 0;
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