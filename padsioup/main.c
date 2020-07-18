//Autor: ackerman
//upload PsExe PAD CUSTOM SIO port with ARDUINO
//Jaime Jose Gavin Sierra
#include <kernel.h>
#include <libapi.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <strings.h>
#include "demo1.h" //Para comparar serie con datos memoria

#define OT_LENGTH (10)					   

#define PACKETMAX (2048)
#define PACKETMAX2 (PACKETMAX*24)
								  
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define tope_texto 1024
#define booleana unsigned char
#define TRUE 1
#define FALSE 0
#define H_SIZE 2048								  

GsOT myOT[2];
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH];
PACKET GPUPacketArea[2][PACKETMAX2];

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

char gb_cadLog[200]="TEST\0";
u_long gb_pad=0; //El estado de botones
int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed
u_char gb_std=0;
u_char gb_std_antes=0;
u_char gb_cont_bit=0;
u_char gb_bufferFrame[256]; //trama
u_char * gb_ptrFrameData = &gb_bufferFrame[3]; //Apunta a los datos
u_char gb_bufferFrame2[256]; //buffer auxiliar
int gb_cont_bufferFrame=0;

u_char globalNewData=0;
u_char gb_byte=0; //El byte de dato
u_char gb_bytes[6]={0,0,0,0,0,0}; //Varios bytes
u_char gb_error=0;
unsigned int gb_size_psExe=0;
unsigned int gb_address_psExe=0;
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
unsigned int gb_contNoFlagHeadBlock = 0; //Contador a saltar de Cabecera relleno


void InitGraphics(void);
void DisplayAll(int);

void CargaPrograma(struct EXEC *exep);
void SendData(int pad_n, unsigned char *in, unsigned char *out, int len);
void TestPADSIO(void);
void PollSpeed12Flag(void);
void PollHead(void);
int main(void);

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

//***********************************************************************
void PollSpeed12Flag()
{
 //u_char * buff = &gb_bufferFrame[3];
 u_char i;
 u_char aux;
 for (i=0;i<16;i+=2)
 {
  aux = (gb_ptrFrameData[i]|(gb_ptrFrameData[i+1]<<4));
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != aux)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],aux);
   }
  }
  gb_p_address[gb_address_psExe_cont] = aux;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
 }
 if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
  gb_launch_exe = 1; 
}

//***********************************************************************
void PollSpeed13NoFlag()
{
 //u_char * buff = &gb_bufferFrame[3];	
 u_char i; 
 u_char aux;
/* for (i=0;i<8;i++)
 {
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != gb_bufferFrame[i])
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],gb_bufferFrame[i]);
   }
  }	 
  gb_p_address[gb_address_psExe_cont] = gb_bufferFrame[i];
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
 }
 if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
  gb_launch_exe = 1; 	
*/
 for (i=0;i<16;i+=2)
 {
  aux = (gb_ptrFrameData[i]|(gb_ptrFrameData[i+1]<<4));
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != aux)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],aux);
   }
  }	 
  gb_p_address[gb_address_psExe_cont] = aux;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
 }
 if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
  gb_launch_exe = 1; 	
}

//***********************************************************************
void PollHeadSpeed12()
{ 
/* gb_size_psExe = (gb_bufferFrame[0]|((gb_bufferFrame[1])<<8)|((gb_bufferFrame[2])<<16));
 gb_speed = (gb_bufferFrame[3]&0x3F);
 gb_type = (gb_bufferFrame[3]>>6)&0x03;
 gb_address_psExe = (0x80<<24) | (gb_bufferFrame[6]<<16) | (gb_bufferFrame[5]<<8) | (gb_bufferFrame[4]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera   
 //gb_cont_bit = 0; //Fuerzo a resetear contador      
 gb_p_address = (u_char *)gb_address_psExe;   	
 */
 //u_char * buff = &gb_bufferFrame[3];
 gb_size_psExe = ((gb_ptrFrameData[0]|(gb_ptrFrameData[1]<<4))|((gb_ptrFrameData[2]|(gb_ptrFrameData[3]<<4))<<8)|((gb_ptrFrameData[4]|(gb_ptrFrameData[5]<<4))<<16));
 gb_speed = ((gb_ptrFrameData[6]|(gb_ptrFrameData[7]<<4))&0x3F);
 gb_type = ((gb_ptrFrameData[6]|(gb_ptrFrameData[7]<<4))>>6)&0x03;
 gb_address_psExe = (0x80<<24) | ((gb_ptrFrameData[12]|(gb_ptrFrameData[13]<<4))<<16) | ((gb_ptrFrameData[10]|(gb_ptrFrameData[11]<<4))<<8) | (gb_ptrFrameData[8]|(gb_ptrFrameData[9]<<4));
 gb_receivedHead = 1; //Ya se ha recibido la cabecera
 //gb_cont_bit = 0; //Fuerzo a resetear contador      
 gb_p_address = (u_char *)gb_address_psExe;   	 
}


//********************************************************
void PollSpeed14Flag()
{
 u_char i;
 u_char aux;
 for (i=0;i<14;i++)
 {
  aux = (gb_bufferFrame2[i]);
  if (gb_address_psExe_cont <(gb_size_psExe-30))
  {
   if (main2[gb_address_psExe_cont] != aux)
   {   
    gb_error = 1;
    sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],aux);
   }
  }
  gb_p_address[gb_address_psExe_cont] = aux;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
 }
 if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
  gb_launch_exe = 1; 
}

//********************************************************
void PollHeadSpeed14()
{  
 gb_size_psExe = (gb_bufferFrame2[0]|((gb_bufferFrame2[1])<<8)|((gb_bufferFrame2[2])<<16));
 gb_speed = (gb_bufferFrame2[3]&0x3F);
 gb_type = (gb_bufferFrame2[3]>>6)&0x03;
 gb_address_psExe = (0x80<<24) | (gb_bufferFrame2[6]<<16) | (gb_bufferFrame2[5]<<8) | (gb_bufferFrame2[4]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe;   	 
}

//********************************************************
void DecodeSIOBuffer()
{unsigned char i;
 for (i=0;i<14;i++)
  gb_bufferFrame2[i] = gb_ptrFrameData[i];
 gb_bufferFrame2[0] |= ((gb_ptrFrameData[14]&0x01)<<7);
 gb_bufferFrame2[1] |= ((gb_ptrFrameData[14]&0x02)<<6);
 gb_bufferFrame2[2] |= ((gb_ptrFrameData[14]&0x04)<<5);
 gb_bufferFrame2[3] |= ((gb_ptrFrameData[14]&0x08)<<4);
 gb_bufferFrame2[4] |= ((gb_ptrFrameData[14]&0x10)<<3);
 gb_bufferFrame2[5] |= ((gb_ptrFrameData[14]&0x20)<<2);
 gb_bufferFrame2[6] |= ((gb_ptrFrameData[14]&0x40)<<1);
 
 gb_bufferFrame2[7] |= ((gb_ptrFrameData[15]&0x01)<<7);
 gb_bufferFrame2[8] |= ((gb_ptrFrameData[15]&0x02)<<6);
 gb_bufferFrame2[9] |= ((gb_ptrFrameData[15]&0x04)<<5);
 gb_bufferFrame2[10]|= ((gb_ptrFrameData[15]&0x08)<<4);
 gb_bufferFrame2[11]|= ((gb_ptrFrameData[15]&0x10)<<3);
 gb_bufferFrame2[12]|= ((gb_ptrFrameData[15]&0x20)<<2);
 gb_bufferFrame2[13]|= ((gb_ptrFrameData[15]&0x40)<<1);
}

//Send data to PAD_SIO
void SendData(int pad_n, unsigned char *in, unsigned char *out, int len)
{	int x;
	int y;
	int i=0;
	int j=0;
	unsigned int cont_timeout=0;
	
	PADSIO_MODE(0) = 0x0D;
	PADSIO_BAUD(0) = 0x88;
	
	if(pad_n == 1) PADSIO_CTRL(0) = 0x3003; else PADSIO_CTRL(0) = 0x1003;		
	for(y=0;y<400;y++);	//Slight delay before first transmission
	for(x = 0; x < len; x++)
	{		
		while((PADSIO_STATUS(0) & 4) == 0); //Wait for TX ready		
		PADSIO_DATA(0) = in[i++];		

		//Read RX status flag
		cont_timeout=0;
		while(((PADSIO_STATUS(0) & 2) == 0)&&(cont_timeout<3584)) cont_timeout++;					
		out[j++] = PADSIO_DATA(0);		
	}		
    //sprintf (gb_cadLog,"Test %04x %04x %04x %08x",PADSIO_BAUD(0),PADSIO_MODE(0),PADSIO_CTRL(0),PADSIO_STATUS(0));
	PADSIO_CTRL(0) = 0;
}

//***********************************************************************
void TestPADSIO(void)
{
 //Recivo FF4100x0x0x0
 unsigned char i;
 //,cont_dest; 
 //unsigned char Rcv[34];
 char TempString[128];
 unsigned char DataToSend[] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; 
 //SendData(0, DataToSend, Rcv, 20);
 
 if (gb_error == 1)
  return;
 SendData(0, DataToSend, gb_bufferFrame, 20);
	 
 //printf (gb_cadLog,"Test %02x%02x%02x%02x%02x%02x%02x%02x",Rcv[0],Rcv[1],Rcv[2],Rcv[3],Rcv[4],Rcv[5],Rcv[6],Rcv[7]);
 //if ((Rcv[2] & 0x01)==0x01) gb_std=1; else gb_std=0;
 if ((gb_bufferFrame[2] & 0x01)==0x01) gb_std=1; else gb_std=0;
 
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  //if (gb_std == 1) //Lo uso cuando es con flag
  {
   if (gb_receivedHead==0)
   {
    //PollHeadSpeed12();
	DecodeSIOBuffer(); PollHeadSpeed14(); //Modo 14,15	
   }
   else
   {
    gb_contNoFlagHeadBlock++; //Para saltarse relleno linea
	//if (gb_contNoFlagHeadBlock<42) //Para velocidad 14 sin flag saltar 42 lecturas relleno linea
	// return; //Lo uso relleno
	//if (gb_contNoFlagHeadBlock<21) //Para velocidad 13 con flag saltar 21 lecturas relleno linea
	// return; ////Para velocidad 13 conflag saltar 21 lecturas relleno linea
	//if (gb_contNoFlagHeadBlock<11) //Para velocidad 14 con flag saltar 11 lecturas relleno linea
	// return; ////Para velocidad 13 conflag saltar 14 lecturas relleno linea	 
	if (gb_contNoFlagHeadBlock<42) //Para velocidad 15 sin flag saltar 42 lecturas relleno linea
	 return; ////Para velocidad 15 sin flag saltar 15 lecturas relleno linea	 	 
	switch (gb_speed)
	{
	 default: break;
	 case 12: PollSpeed12Flag(); break;	 
	 case 13: PollSpeed13NoFlag(); break;	 
	 case 14: DecodeSIOBuffer();PollSpeed14Flag(); break;
	 case 15: DecodeSIOBuffer();PollSpeed14Flag(); break;	
	}	
   }   
  }
 }
 if (gb_error == 0)
 {
  sprintf(gb_cadLog, "%08x %d/%d\n%d %d",gb_p_address,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type);
 }
}

//Sobra
	//cont_dest=0;
	//for (i=0;i<16;i+=2)
	//{
    // gb_bufferFrame[cont_dest]= (Rcv[i+3]&0x0F)|((Rcv[i+4]<<4)&0xF0);
	// cont_dest++;
	//}
	//cont_dest=0;
	//for (i=0;i<16;i+=2)
	//{
    // gb_bufferFrame[cont_dest]= (Rcv[i+3]&0x0F)|((Rcv[i+4]<<4)&0xF0);
	// cont_dest++;
	//}	   


int i=0;

int main(void) 
{	
 InitGraphics();			//this method sets up gfx for printing to screen	
 FntLoad(960, 256);		//this loads the font gfx to the vram
 //FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
 //x y ini, width heigh, clear, max char
 FntOpen(8, 8, 300, 24, 0, 128);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc    	
 DrawSync(0); 
 
 //gb_address_psExe = 0x80010000; 
 //gb_launch_exe = 1;
 
 while (gb_launch_exe == 0)
 {				//infinite
  activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
  GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
  GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents
       
  TestPADSIO();  
  FntPrint("%s",gb_cadLog);
  
  DisplayAll(activeBuffer);		//this displays the OT contents to screen		
 }
 
  
 CargaPrograma(&exe); //Prepara exec 
 ResetGraph(3);
 //PadStopCom();
 StopCallback();
// for (i=0;i<100;i++)
//  VSync(0); //Esperamos 2 segundos 50x2 
 EnterCriticalSection(); 
 Exec(&exe,1,0); //Fin exec
 return 0;		//when program is finished return from it
} 


void InitGraphics(void) {
	//Prueba JJ
	ResetCallback();//JJ
	ResetGraph(0); //JJ 
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
}

//*************************************************
void DisplayAll(int activeBuffer) {
	//this method contains all the functions needed to display the contents of the OT
	FntFlush(-1);				//flushes font buffers contents from buffer so that they can be printed to screen	
	DrawSync(0);
	//this waits till the GPU has finished drawing, as GsSwapDispBuff will not work correctly if drawing is in progress
	VSync(0);
	//gsswapdispbuff should be called after beginning a v-blank
	GsSwapDispBuff();				//swap display buffer
	GsSortClear(0,0,0,&myOT[activeBuffer]);		//clears screen to color (0,0,0) and sorts OT ready for drawing
	GsDrawOt(&myOT[activeBuffer]);			//draws the contents of OT to screen
}