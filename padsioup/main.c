//Autor: ackerman
//upload PsExe PAD CUSTOM SIO port with ARDUINO
//Jaime Jose Gavin Sierra

//ARDUINO conectado a 0 (puerto 1), 1 (puerto 2)
//MANDO PSX conectado a 0 (puerto 1), 1 (puerto 2)
//TEST usar mando seleccion de velocidades
//#define MODE_VIDEO_NTSC
#define MODE_VIDEO_PAL
#define GB_USEPORTARDUINO 1
#define GB_USEPORTPADPSX 0
#define GB_USEPADTESTSPEED 1
#define maxTimeOutChange 1200
#define maxTimeOutRemovePAD 600
#define PAD_UP (1<<12)
#define PAD_DOWN (1<<14)
#define PAD_LEFT (1<<15)
#define PAD_RIGHT (1<<13)
#define PAD_CRC (1<<5)
#define PAD_X (1<<6)
#define PAD_SELECT (1<<8)
#define PAD_START (1<<11)

#include <kernel.h>
#include <libapi.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <strings.h>
//#include "demo1.h" //Para comparar serie con datos memoria
u_char main2[10];//Para comparar serie con datos memoria

#define OT_LENGTH (10)					   

#define PACKETMAX (2048)
#define PACKETMAX2 (PACKETMAX*24)
								  
#define SCREEN_WIDTH 320
//NTSC
#ifdef MODE_VIDEO_NTSC
 #define SCREEN_HEIGHT 240
#endif
//PAL
#ifdef MODE_VIDEO_PAL
 #define SCREEN_HEIGHT 256
#endif 
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

//Timeout envio padport 400 va muy bien
#define maxTimeOut 300
//Actualiza texto carga cada x fps
#define maxFpsUpdate 40
unsigned char gb_DataToSend20[20] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; //Speed 12,13,14,15 
unsigned char gb_DataToSend36[36] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; //Speed 16,17,18,19
unsigned char gb_DataToSend68[68] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; //Speed 20,21,22,23
unsigned char gb_DataToSend132[132] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; //Speed 24,25
//unsigned char gb_DataToSend260[260] = {1,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0}; //Speed 28,29

u_char gb_contfps=0;
char gb_cadLog[1024]="TEST\0";
char gb_cadPrev[64]="\0";
int activeBuffer=0;		//variable used to hold the buffer that is currently being displayed
u_char gb_std=0;
u_char gb_std_antes=0;
u_char gb_bufferFrame[1024]; //trama
u_char * gb_ptrFrameData = &gb_bufferFrame[3]; //Apunta a los datos
//u_char gb_bufferFrame2[1024]; //buffer auxiliar

u_char globalNewData=0;
u_char gb_byte=0; //El byte de dato
u_char gb_error=0;
unsigned int gb_size_psExe=0;
unsigned int gb_address_psExe=0;
u_char gb_receivedHead= 0; //No se ha recibido aun la cabecera
short int gb_type = -1;
short int gb_speed = -1;
int gb_CRC = -1;
u_char gb_launch_exe= 0;
unsigned int gb_address_psExe_cont=0;
unsigned char gb_BeginPulse = 0;

u_char *gb_p_address; //Puntero a memoria PSX
struct EXEC exe;

unsigned int gb_contNoFlagHeadBlock = 0; //Contador a saltar de Cabecera relleno
u_char gb_useFlag = 0; //usar modo flag
#ifdef GB_USEPADTESTSPEED
 unsigned short gb_pad_antes;
 unsigned short gb_pad; //Botones del mando 16 botones
 unsigned short gb_pad_speed[14]={1280,2560 ,2240,4480, 2560,5120, 4480,8960, 5120,10240, 8960,17920, 10240,20480};//Velocidades de 12 a 25 
 u_char gb_currentPadSpeed = 12;
 u_char gb_endSelect = 0; //Aun no ha saleccionado
#endif

unsigned short int gb_contChange = 0;
u_char gb_resetPSXSerial = 0;
u_char gb_loop = 1;
u_char gb_autoPAD = 0;
u_char gb_padDisconnect = 0;
u_char gbWaitRemovePAD = 0;
enum enumTypeFile{
 fileTIM, fileVAG, fileVAB, fileTMD, fileHMD, fileSEQ, fileEXE, fileOther, fileCEL, fileBGD, filePMD, fileNone
};

enum enumTypeFile gb_launch_file = fileNone;


void InitGraphics(void);
void DisplayAll(int);

void CargaPrograma(struct EXEC *exep);
void SendData(int pad_n, unsigned char *in, unsigned char *out, int len);
#ifdef GB_USEPADTESTSPEED
 void ReadPad(void);
 void SelectSpeed(void);
#endif
void TestPADSIO(void);
void PollSpeed12Flag(void);
void PollHead(void);
void ResetLoader(void);
void ResetPSXSERIAL(void);
void CheckFileData();
int main(void);

//*****************************************
void CheckFileData()
{//Check TIM,VAG,TMD,EXE,etc..
 gb_launch_file = fileNone;
 switch (gb_p_address[0])
 {
  case 0x10: gb_launch_file= fileTIM; break;
  case 0x22: gb_launch_file= fileCEL; break;
  case 0x23: gb_launch_file= fileBGD; break;
  case 0x41: gb_launch_file= fileTMD; break;
  case 0x42: gb_launch_file= filePMD; break;
  case 0x50:
   if ((gb_p_address[1]==0x53)&&(gb_p_address[2]==0x2D)&&(gb_p_address[3]==0x58)) gb_launch_file= fileEXE;   
   else
    if ((gb_p_address[1]==0)&&(gb_p_address[2]==0)&&(gb_p_address[3]==0)) gb_launch_file= fileHMD;
	else gb_launch_file = fileOther;
   break;
  case 0x53: gb_launch_file= fileSEQ; break;
  case 0x56: gb_launch_file= fileVAG; break;
  case 0x70: gb_launch_file= fileVAB; break;
  default: gb_launch_file= fileOther; break;
 }
 
 switch (gb_launch_file)
 {
  case fileTIM: sprintf(gb_cadPrev,"TIM OK"); break;
  case fileCEL: sprintf(gb_cadPrev,"CEL OK"); break;
  case fileBGD: sprintf(gb_cadPrev,"BGD OK"); break;
  case fileTMD: sprintf(gb_cadPrev,"TMD OK"); break;
  case fileEXE: sprintf(gb_cadPrev,"EXE OK"); break;
  case fileHMD: sprintf(gb_cadPrev,"HMD OK"); break;
  case fileSEQ: sprintf(gb_cadPrev,"SEQ OK"); break;
  case fileVAG: sprintf(gb_cadPrev,"VAG OK"); break;
  case fileVAB: sprintf(gb_cadPrev,"VAB OK"); break;
  case filePMD: sprintf(gb_cadPrev,"PMD OK"); break;
  default: sprintf(gb_cadPrev,"OTHER OK"); break;
 }
}

#ifdef GB_USEPADTESTSPEED
 //***********************************************************************
 void ReadPad()
 {u_char DataToSend[] = {1,0x42,0,0,0,0,0,0,0};//Leer mando
  u_char ReceivedData[16];
  SendData(GB_USEPORTPADPSX, DataToSend, ReceivedData, sizeof(DataToSend));
  gb_pad = ~((ReceivedData[3] << 8) | ReceivedData[4]);
  if (ReceivedData[1] == PAD_NONE)
   gb_padDisconnect = 1;   
  else 
   gb_padDisconnect = 0;
 }

 //***********************************************************************
 void SelectSpeed()
 {
  u_char auxRun = 0;
  if (gb_pad_antes != gb_pad)
  {
   gb_pad_antes = gb_pad;
   if (gb_pad & PAD_DOWN) gb_currentPadSpeed--;
   if (gb_pad & PAD_UP) gb_currentPadSpeed++;
   if (gb_pad & PAD_LEFT) gb_currentPadSpeed-=10;
   if (gb_pad & PAD_RIGHT) gb_currentPadSpeed+=10;   
   if (gb_pad & PAD_CRC) gb_autoPAD=1;   
   if (gb_pad & PAD_X) auxRun=1;         
   if (gb_pad & PAD_START)
   {gb_loop=0;
	gb_resetPSXSerial=1;//Llamamos a PSXSERIAL
   }
   if (gb_pad & PAD_SELECT) ResetLoader();//Reiniciamos nuestro cargador
   if (gb_currentPadSpeed<12) gb_currentPadSpeed=12;
   if (gb_currentPadSpeed>25) gb_currentPadSpeed=25;
  }

  if ((gb_autoPAD == 1) && (gb_padDisconnect == 1)) auxRun=1;

  if ((gb_currentPadSpeed & 0x01)==0x00) gb_useFlag = 1; //Es par es con flag
  else gb_useFlag = 0;
  
  if (auxRun == 1)
  {gb_endSelect = 1;
   gb_speed = gb_currentPadSpeed;	   	  
  }
   
  sprintf(gb_cadLog, "V1    %d - %d BAUD(FLAG %d)\n\nX(OK) O(AUTO) START(RST) SEL(INI)",gb_currentPadSpeed,gb_pad_speed[(gb_currentPadSpeed-12)],gb_useFlag);
 }
#endif

//***********************************************************************
void ResetLoadNextFile()
{
 gb_endSelect=1;
 gbWaitRemovePAD=0;
 gb_contChange=0;
 gb_error=0;
 gb_BeginPulse=0;
 gb_receivedHead=0;
 gb_contNoFlagHeadBlock=0;
 gb_autoPAD=0;
 gb_padDisconnect=0;
 gb_address_psExe_cont=0;
 gb_launch_exe=0;
 gb_launch_file = fileNone;
 
 gb_contfps=0;
 globalNewData=0;
 gb_byte=0;
 gb_size_psExe=0;
 gb_address_psExe=0;
 //gb_type=-1;
 //gb_speed=-1;
 //gb_useFlag=0;
 //gb_currentPadSpeed=12;
 gb_resetPSXSerial=0;
 gb_loop=1;
}

//***********************************************************************
void ResetLoader()//Reset Loader
{
 sprintf(gb_cadPrev,"");
 gb_endSelect=0;
 gbWaitRemovePAD=0;
 gb_contChange=0;
 gb_error=0;
 gb_BeginPulse=0;
 gb_receivedHead=0;
 gb_contNoFlagHeadBlock=0;
 gb_autoPAD=0;
 gb_padDisconnect=0;
 gb_address_psExe_cont=0;
 gb_launch_exe=0; 
 gb_launch_file = fileNone;
 
 gb_contfps=0;
 globalNewData=0;
 gb_byte=0;
 gb_size_psExe=0;
 gb_address_psExe=0;
 gb_type=-1;
 gb_speed=-1;
 gb_useFlag=0;
 gb_currentPadSpeed=12;
 gb_resetPSXSerial=0;
 gb_loop=1;
 
 //ResetGraph(3);
 //StopCallback();
 //EnterCriticalSection(); 
 //__asm__("j 0x80199C84");//Direccion del pc0 cpe2x
}

//***********************************************************************
void ResetPSXSERIAL()//Reset PSXSerial
{ResetGraph(3);
 StopCallback();
 EnterCriticalSection();
 //__asm__("j 0x801ecd94");//v1.3
 //__asm__("j 0x801EAAE8");//v1.2
 __asm__("j 0x801e75a8");//v1 funciona 
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
 {gb_loop=0;
  gb_launch_exe = 1;
 }
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
 {gb_loop=0;
  gb_launch_exe = 1; 	
 }
}

//***********************************************************************
void PollHeadSpeed1213161720212425(){  
 gb_size_psExe = ((gb_ptrFrameData[0]|(gb_ptrFrameData[1]<<4))|((gb_ptrFrameData[2]|(gb_ptrFrameData[3]<<4))<<8)|((gb_ptrFrameData[4]|(gb_ptrFrameData[5]<<4))<<16));
 gb_speed = (gb_ptrFrameData[6]|(gb_ptrFrameData[7]<<4));
 gb_type = (gb_ptrFrameData[8]|(gb_ptrFrameData[9]<<4));
 gb_address_psExe = (0x80<<24)|((gb_ptrFrameData[14]|(gb_ptrFrameData[15]<<4))<<16)|((gb_ptrFrameData[12]|(gb_ptrFrameData[13]<<4))<<8)|(gb_ptrFrameData[10]|(gb_ptrFrameData[11]<<4));
 gb_receivedHead = 1;//Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe; 
}


//********************************************************
void PollSpeed1415Flag()
{
 u_char i;
 u_char aux;
 for (i=0;i<14;i++)
 {
  //aux = (gb_bufferFrame2[i]);
  aux = gb_ptrFrameData[i]; //optimizado
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
}

//********************************************************
void PollHeadSpeed1415()
{  
 /*gb_size_psExe = (gb_bufferFrame2[0]|((gb_bufferFrame2[1])<<8)|((gb_bufferFrame2[2])<<16));
 gb_speed = gb_bufferFrame2[3];
 gb_type = gb_bufferFrame2[4];
 gb_address_psExe = (0x80<<24) | (gb_bufferFrame2[7]<<16) | (gb_bufferFrame2[6]<<8) | (gb_bufferFrame2[5]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe; */
 gb_size_psExe = (gb_ptrFrameData[0]|((gb_ptrFrameData[1])<<8)|((gb_ptrFrameData[2])<<16));
 gb_speed = gb_ptrFrameData[3];
 gb_type = gb_ptrFrameData[4];
 gb_address_psExe = (0x80<<24) | (gb_ptrFrameData[7]<<16) | (gb_ptrFrameData[6]<<8) | (gb_ptrFrameData[5]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe; 
}

//********************************************************
void DecodeSIOBuffer1415()
{/*unsigned char i;
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
 gb_bufferFrame2[13]|= ((gb_ptrFrameData[15]&0x40)<<1);*/
 //Optimizado
 gb_ptrFrameData[0] |= ((gb_ptrFrameData[14]&0x01)<<7);
 gb_ptrFrameData[1] |= ((gb_ptrFrameData[14]&0x02)<<6);
 gb_ptrFrameData[2] |= ((gb_ptrFrameData[14]&0x04)<<5);
 gb_ptrFrameData[3] |= ((gb_ptrFrameData[14]&0x08)<<4);
 gb_ptrFrameData[4] |= ((gb_ptrFrameData[14]&0x10)<<3);
 gb_ptrFrameData[5] |= ((gb_ptrFrameData[14]&0x20)<<2);
 gb_ptrFrameData[6] |= ((gb_ptrFrameData[14]&0x40)<<1);
 
 gb_ptrFrameData[7] |= ((gb_ptrFrameData[15]&0x01)<<7);
 gb_ptrFrameData[8] |= ((gb_ptrFrameData[15]&0x02)<<6);
 gb_ptrFrameData[9] |= ((gb_ptrFrameData[15]&0x04)<<5);
 gb_ptrFrameData[10]|= ((gb_ptrFrameData[15]&0x08)<<4);
 gb_ptrFrameData[11]|= ((gb_ptrFrameData[15]&0x10)<<3);
 gb_ptrFrameData[12]|= ((gb_ptrFrameData[15]&0x20)<<2);
 gb_ptrFrameData[13]|= ((gb_ptrFrameData[15]&0x40)<<1); 
}


//********************************************************
void PollHeadSpeed1819()
{gb_size_psExe = (gb_ptrFrameData[0]|((gb_ptrFrameData[1])<<8)|((gb_ptrFrameData[2])<<16));
 gb_speed = gb_ptrFrameData[3];
 gb_type = gb_ptrFrameData[4];
 gb_address_psExe = (0x80<<24) | (gb_ptrFrameData[7]<<16) | (gb_ptrFrameData[6]<<8) | (gb_ptrFrameData[5]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe;
}

//********************************************************
void DecodeSIOBuffer1819()
{/*unsigned char i;
 for (i=0;i<28;i++)
  gb_bufferFrame2[i] = gb_ptrFrameData[i];
 gb_bufferFrame2[0] |= ((gb_ptrFrameData[28]&0x01)<<7);
 gb_bufferFrame2[1] |= ((gb_ptrFrameData[28]&0x02)<<6);
 gb_bufferFrame2[2] |= ((gb_ptrFrameData[28]&0x04)<<5);
 gb_bufferFrame2[3] |= ((gb_ptrFrameData[28]&0x08)<<4);
 gb_bufferFrame2[4] |= ((gb_ptrFrameData[28]&0x10)<<3);
 gb_bufferFrame2[5] |= ((gb_ptrFrameData[28]&0x20)<<2);
 gb_bufferFrame2[6] |= ((gb_ptrFrameData[28]&0x40)<<1);
 
 gb_bufferFrame2[7] |= ((gb_ptrFrameData[29]&0x01)<<7);
 gb_bufferFrame2[8] |= ((gb_ptrFrameData[29]&0x02)<<6);
 gb_bufferFrame2[9] |= ((gb_ptrFrameData[29]&0x04)<<5);
 gb_bufferFrame2[10]|= ((gb_ptrFrameData[29]&0x08)<<4);
 gb_bufferFrame2[11]|= ((gb_ptrFrameData[29]&0x10)<<3);
 gb_bufferFrame2[12]|= ((gb_ptrFrameData[29]&0x20)<<2);
 gb_bufferFrame2[13]|= ((gb_ptrFrameData[29]&0x40)<<1);
 
 gb_bufferFrame2[14] |= ((gb_ptrFrameData[30]&0x01)<<7);
 gb_bufferFrame2[15] |= ((gb_ptrFrameData[30]&0x02)<<6);
 gb_bufferFrame2[16] |= ((gb_ptrFrameData[30]&0x04)<<5);
 gb_bufferFrame2[17] |= ((gb_ptrFrameData[30]&0x08)<<4);
 gb_bufferFrame2[18] |= ((gb_ptrFrameData[30]&0x10)<<3);
 gb_bufferFrame2[19] |= ((gb_ptrFrameData[30]&0x20)<<2);
 gb_bufferFrame2[20] |= ((gb_ptrFrameData[30]&0x40)<<1);
 
 gb_bufferFrame2[21] |= ((gb_ptrFrameData[31]&0x01)<<7);
 gb_bufferFrame2[22] |= ((gb_ptrFrameData[31]&0x02)<<6);
 gb_bufferFrame2[23] |= ((gb_ptrFrameData[31]&0x04)<<5);
 gb_bufferFrame2[24] |= ((gb_ptrFrameData[31]&0x08)<<4);
 gb_bufferFrame2[25] |= ((gb_ptrFrameData[31]&0x10)<<3);
 gb_bufferFrame2[26] |= ((gb_ptrFrameData[31]&0x20)<<2);
 gb_bufferFrame2[27] |= ((gb_ptrFrameData[31]&0x40)<<1); */
 
 gb_ptrFrameData[0] |= ((gb_ptrFrameData[28]&0x01)<<7);
 gb_ptrFrameData[1] |= ((gb_ptrFrameData[28]&0x02)<<6);
 gb_ptrFrameData[2] |= ((gb_ptrFrameData[28]&0x04)<<5);
 gb_ptrFrameData[3] |= ((gb_ptrFrameData[28]&0x08)<<4);
 gb_ptrFrameData[4] |= ((gb_ptrFrameData[28]&0x10)<<3);
 gb_ptrFrameData[5] |= ((gb_ptrFrameData[28]&0x20)<<2);
 gb_ptrFrameData[6] |= ((gb_ptrFrameData[28]&0x40)<<1);
 
 gb_ptrFrameData[7] |= ((gb_ptrFrameData[29]&0x01)<<7);
 gb_ptrFrameData[8] |= ((gb_ptrFrameData[29]&0x02)<<6);
 gb_ptrFrameData[9] |= ((gb_ptrFrameData[29]&0x04)<<5);
 gb_ptrFrameData[10]|= ((gb_ptrFrameData[29]&0x08)<<4);
 gb_ptrFrameData[11]|= ((gb_ptrFrameData[29]&0x10)<<3);
 gb_ptrFrameData[12]|= ((gb_ptrFrameData[29]&0x20)<<2);
 gb_ptrFrameData[13]|= ((gb_ptrFrameData[29]&0x40)<<1);
 
 gb_ptrFrameData[14] |= ((gb_ptrFrameData[30]&0x01)<<7);
 gb_ptrFrameData[15] |= ((gb_ptrFrameData[30]&0x02)<<6);
 gb_ptrFrameData[16] |= ((gb_ptrFrameData[30]&0x04)<<5);
 gb_ptrFrameData[17] |= ((gb_ptrFrameData[30]&0x08)<<4);
 gb_ptrFrameData[18] |= ((gb_ptrFrameData[30]&0x10)<<3);
 gb_ptrFrameData[19] |= ((gb_ptrFrameData[30]&0x20)<<2);
 gb_ptrFrameData[20] |= ((gb_ptrFrameData[30]&0x40)<<1);
 
 gb_ptrFrameData[21] |= ((gb_ptrFrameData[31]&0x01)<<7);
 gb_ptrFrameData[22] |= ((gb_ptrFrameData[31]&0x02)<<6);
 gb_ptrFrameData[23] |= ((gb_ptrFrameData[31]&0x04)<<5);
 gb_ptrFrameData[24] |= ((gb_ptrFrameData[31]&0x08)<<4);
 gb_ptrFrameData[25] |= ((gb_ptrFrameData[31]&0x10)<<3);
 gb_ptrFrameData[26] |= ((gb_ptrFrameData[31]&0x20)<<2);
 gb_ptrFrameData[27] |= ((gb_ptrFrameData[31]&0x40)<<1); 
}


//********************************************************
void PollSpeed1819Flag()
{
 u_char i;
 u_char aux;
 for (i=0;i<28;i++)
 {
  //aux = (gb_bufferFrame2[i]);
  aux = (gb_ptrFrameData[i]);
  if (gb_address_psExe_cont <(gb_size_psExe-100))
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
}



//********************************************************
void DecodeSIOBuffer2223()
{/*unsigned char i;
 for (i=0;i<56;i++)
  gb_bufferFrame2[i] = gb_ptrFrameData[i];
 gb_bufferFrame2[0] |= ((gb_ptrFrameData[56]&0x01)<<7);
 gb_bufferFrame2[1] |= ((gb_ptrFrameData[56]&0x02)<<6);
 gb_bufferFrame2[2] |= ((gb_ptrFrameData[56]&0x04)<<5);
 gb_bufferFrame2[3] |= ((gb_ptrFrameData[56]&0x08)<<4);
 gb_bufferFrame2[4] |= ((gb_ptrFrameData[56]&0x10)<<3);
 gb_bufferFrame2[5] |= ((gb_ptrFrameData[56]&0x20)<<2);
 gb_bufferFrame2[6] |= ((gb_ptrFrameData[56]&0x40)<<1);
 
 gb_bufferFrame2[7] |= ((gb_ptrFrameData[57]&0x01)<<7);
 gb_bufferFrame2[8] |= ((gb_ptrFrameData[57]&0x02)<<6);
 gb_bufferFrame2[9] |= ((gb_ptrFrameData[57]&0x04)<<5);
 gb_bufferFrame2[10]|= ((gb_ptrFrameData[57]&0x08)<<4);
 gb_bufferFrame2[11]|= ((gb_ptrFrameData[57]&0x10)<<3);
 gb_bufferFrame2[12]|= ((gb_ptrFrameData[57]&0x20)<<2);
 gb_bufferFrame2[13]|= ((gb_ptrFrameData[57]&0x40)<<1);
 
 gb_bufferFrame2[14] |= ((gb_ptrFrameData[58]&0x01)<<7);
 gb_bufferFrame2[15] |= ((gb_ptrFrameData[58]&0x02)<<6);
 gb_bufferFrame2[16] |= ((gb_ptrFrameData[58]&0x04)<<5);
 gb_bufferFrame2[17] |= ((gb_ptrFrameData[58]&0x08)<<4);
 gb_bufferFrame2[18] |= ((gb_ptrFrameData[58]&0x10)<<3);
 gb_bufferFrame2[19] |= ((gb_ptrFrameData[58]&0x20)<<2);
 gb_bufferFrame2[20] |= ((gb_ptrFrameData[58]&0x40)<<1);
 
 gb_bufferFrame2[21] |= ((gb_ptrFrameData[59]&0x01)<<7);
 gb_bufferFrame2[22] |= ((gb_ptrFrameData[59]&0x02)<<6);
 gb_bufferFrame2[23] |= ((gb_ptrFrameData[59]&0x04)<<5);
 gb_bufferFrame2[24] |= ((gb_ptrFrameData[59]&0x08)<<4);
 gb_bufferFrame2[25] |= ((gb_ptrFrameData[59]&0x10)<<3);
 gb_bufferFrame2[26] |= ((gb_ptrFrameData[59]&0x20)<<2);
 gb_bufferFrame2[27] |= ((gb_ptrFrameData[59]&0x40)<<1); 
 
 
 gb_bufferFrame2[28] |= ((gb_ptrFrameData[60]&0x01)<<7);
 gb_bufferFrame2[29] |= ((gb_ptrFrameData[60]&0x02)<<6);
 gb_bufferFrame2[30] |= ((gb_ptrFrameData[60]&0x04)<<5);
 gb_bufferFrame2[31] |= ((gb_ptrFrameData[60]&0x08)<<4);
 gb_bufferFrame2[32] |= ((gb_ptrFrameData[60]&0x10)<<3);
 gb_bufferFrame2[33] |= ((gb_ptrFrameData[60]&0x20)<<2);
 gb_bufferFrame2[34] |= ((gb_ptrFrameData[60]&0x40)<<1);
 
 gb_bufferFrame2[35] |= ((gb_ptrFrameData[61]&0x01)<<7);
 gb_bufferFrame2[36] |= ((gb_ptrFrameData[61]&0x02)<<6);
 gb_bufferFrame2[37] |= ((gb_ptrFrameData[61]&0x04)<<5);
 gb_bufferFrame2[38] |= ((gb_ptrFrameData[61]&0x08)<<4);
 gb_bufferFrame2[39] |= ((gb_ptrFrameData[61]&0x10)<<3);
 gb_bufferFrame2[40] |= ((gb_ptrFrameData[61]&0x20)<<2);
 gb_bufferFrame2[41] |= ((gb_ptrFrameData[61]&0x40)<<1);
 
 gb_bufferFrame2[42] |= ((gb_ptrFrameData[62]&0x01)<<7);
 gb_bufferFrame2[43] |= ((gb_ptrFrameData[62]&0x02)<<6);
 gb_bufferFrame2[44] |= ((gb_ptrFrameData[62]&0x04)<<5);
 gb_bufferFrame2[45] |= ((gb_ptrFrameData[62]&0x08)<<4);
 gb_bufferFrame2[46] |= ((gb_ptrFrameData[62]&0x10)<<3);
 gb_bufferFrame2[47] |= ((gb_ptrFrameData[62]&0x20)<<2);
 gb_bufferFrame2[48] |= ((gb_ptrFrameData[62]&0x40)<<1);
 
 gb_bufferFrame2[49] |= ((gb_ptrFrameData[63]&0x01)<<7);
 gb_bufferFrame2[50] |= ((gb_ptrFrameData[63]&0x02)<<6);
 gb_bufferFrame2[51] |= ((gb_ptrFrameData[63]&0x04)<<5);
 gb_bufferFrame2[52] |= ((gb_ptrFrameData[63]&0x08)<<4);
 gb_bufferFrame2[53] |= ((gb_ptrFrameData[63]&0x10)<<3);
 gb_bufferFrame2[54] |= ((gb_ptrFrameData[63]&0x20)<<2);
 gb_bufferFrame2[55] |= ((gb_ptrFrameData[63]&0x40)<<1);*/
 
/* unsigned char idDest=56;
 unsigned char idOrigen=0;
 unsigned char i;
 unsigned char idIzq;
 unsigned char mascara;
 unsigned char idShift;
 for (i=0;i<8;i++)
 {  
  mascara = 0x01;
  idShift = 7;
  for (idIzq=0;idIzq<7;idIzq++)
  {   
   gb_ptrFrameData[idOrigen] |= ((gb_ptrFrameData[idDest]&mascara)<<idShift); 
   mascara = mascara<<1;
   idOrigen++;
   idShift--;
  }
  idDest++;
 }*/
 
 gb_ptrFrameData[0] |= ((gb_ptrFrameData[56]&0x01)<<7);
 gb_ptrFrameData[1] |= ((gb_ptrFrameData[56]&0x02)<<6);
 gb_ptrFrameData[2] |= ((gb_ptrFrameData[56]&0x04)<<5);
 gb_ptrFrameData[3] |= ((gb_ptrFrameData[56]&0x08)<<4);
 gb_ptrFrameData[4] |= ((gb_ptrFrameData[56]&0x10)<<3);
 gb_ptrFrameData[5] |= ((gb_ptrFrameData[56]&0x20)<<2);
 gb_ptrFrameData[6] |= ((gb_ptrFrameData[56]&0x40)<<1);
 
 gb_ptrFrameData[7] |= ((gb_ptrFrameData[57]&0x01)<<7);
 gb_ptrFrameData[8] |= ((gb_ptrFrameData[57]&0x02)<<6);
 gb_ptrFrameData[9] |= ((gb_ptrFrameData[57]&0x04)<<5);
 gb_ptrFrameData[10]|= ((gb_ptrFrameData[57]&0x08)<<4);
 gb_ptrFrameData[11]|= ((gb_ptrFrameData[57]&0x10)<<3);
 gb_ptrFrameData[12]|= ((gb_ptrFrameData[57]&0x20)<<2);
 gb_ptrFrameData[13]|= ((gb_ptrFrameData[57]&0x40)<<1);
 
 gb_ptrFrameData[14]|= ((gb_ptrFrameData[58]&0x01)<<7);
 gb_ptrFrameData[15]|= ((gb_ptrFrameData[58]&0x02)<<6);
 gb_ptrFrameData[16]|= ((gb_ptrFrameData[58]&0x04)<<5);
 gb_ptrFrameData[17]|= ((gb_ptrFrameData[58]&0x08)<<4);
 gb_ptrFrameData[18]|= ((gb_ptrFrameData[58]&0x10)<<3);
 gb_ptrFrameData[19]|= ((gb_ptrFrameData[58]&0x20)<<2);
 gb_ptrFrameData[20]|= ((gb_ptrFrameData[58]&0x40)<<1);
 
 gb_ptrFrameData[21]|= ((gb_ptrFrameData[59]&0x01)<<7);
 gb_ptrFrameData[22]|= ((gb_ptrFrameData[59]&0x02)<<6);
 gb_ptrFrameData[23]|= ((gb_ptrFrameData[59]&0x04)<<5);
 gb_ptrFrameData[24]|= ((gb_ptrFrameData[59]&0x08)<<4);
 gb_ptrFrameData[25]|= ((gb_ptrFrameData[59]&0x10)<<3);
 gb_ptrFrameData[26]|= ((gb_ptrFrameData[59]&0x20)<<2);
 gb_ptrFrameData[27]|= ((gb_ptrFrameData[59]&0x40)<<1); 
 
 
 gb_ptrFrameData[28]|= ((gb_ptrFrameData[60]&0x01)<<7);
 gb_ptrFrameData[29]|= ((gb_ptrFrameData[60]&0x02)<<6);
 gb_ptrFrameData[30]|= ((gb_ptrFrameData[60]&0x04)<<5);
 gb_ptrFrameData[31]|= ((gb_ptrFrameData[60]&0x08)<<4);
 gb_ptrFrameData[32]|= ((gb_ptrFrameData[60]&0x10)<<3);
 gb_ptrFrameData[33]|= ((gb_ptrFrameData[60]&0x20)<<2);
 gb_ptrFrameData[34]|= ((gb_ptrFrameData[60]&0x40)<<1);
 
 gb_ptrFrameData[35]|= ((gb_ptrFrameData[61]&0x01)<<7);
 gb_ptrFrameData[36]|= ((gb_ptrFrameData[61]&0x02)<<6);
 gb_ptrFrameData[37]|= ((gb_ptrFrameData[61]&0x04)<<5);
 gb_ptrFrameData[38]|= ((gb_ptrFrameData[61]&0x08)<<4);
 gb_ptrFrameData[39]|= ((gb_ptrFrameData[61]&0x10)<<3);
 gb_ptrFrameData[40]|= ((gb_ptrFrameData[61]&0x20)<<2);
 gb_ptrFrameData[41]|= ((gb_ptrFrameData[61]&0x40)<<1);
 
 gb_ptrFrameData[42]|= ((gb_ptrFrameData[62]&0x01)<<7);
 gb_ptrFrameData[43]|= ((gb_ptrFrameData[62]&0x02)<<6);
 gb_ptrFrameData[44]|= ((gb_ptrFrameData[62]&0x04)<<5);
 gb_ptrFrameData[45]|= ((gb_ptrFrameData[62]&0x08)<<4);
 gb_ptrFrameData[46]|= ((gb_ptrFrameData[62]&0x10)<<3);
 gb_ptrFrameData[47]|= ((gb_ptrFrameData[62]&0x20)<<2);
 gb_ptrFrameData[48]|= ((gb_ptrFrameData[62]&0x40)<<1);
 
 gb_ptrFrameData[49]|= ((gb_ptrFrameData[63]&0x01)<<7);
 gb_ptrFrameData[50]|= ((gb_ptrFrameData[63]&0x02)<<6);
 gb_ptrFrameData[51]|= ((gb_ptrFrameData[63]&0x04)<<5);
 gb_ptrFrameData[52]|= ((gb_ptrFrameData[63]&0x08)<<4);
 gb_ptrFrameData[53]|= ((gb_ptrFrameData[63]&0x10)<<3);
 gb_ptrFrameData[54]|= ((gb_ptrFrameData[63]&0x20)<<2);
 gb_ptrFrameData[55]|= ((gb_ptrFrameData[63]&0x40)<<1);
}


//********************************************************
void PollHeadSpeed2223()
{  
 gb_size_psExe = (gb_ptrFrameData[0]|((gb_ptrFrameData[1])<<8)|((gb_ptrFrameData[2])<<16));
 gb_speed = gb_ptrFrameData[3];
 gb_type = gb_ptrFrameData[4];
 gb_address_psExe = (0x80<<24) | (gb_ptrFrameData[7]<<16) | (gb_ptrFrameData[6]<<8) | (gb_ptrFrameData[5]);
 gb_receivedHead = 1; //Ya se ha recibido la cabecera 
 gb_p_address = (u_char *)gb_address_psExe; 
}

//********************************************************
void PollSpeed2223Flag()
{
 u_char i;
 u_char aux;
 for (i=0;i<56;i++)
 {
  //aux = (gb_bufferFrame2[i]);
  aux = (gb_ptrFrameData[i]);
  if (gb_address_psExe_cont <(gb_size_psExe-100))
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
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
	//for(y=0;y<400;y++);	//Slight delay before first transmission
	for(x = 0; x < len; x++)
	{		
		while((PADSIO_STATUS(0) & 4) == 0); //Wait for TX ready		
		PADSIO_DATA(0) = in[i++];		

		//Read RX status flag
		cont_timeout=0;
		//while(((PADSIO_STATUS(0) & 2) == 0)&&(cont_timeout<3584)) cont_timeout++;
		while(((PADSIO_STATUS(0) & 2) == 0)&&(cont_timeout<maxTimeOut)) cont_timeout++;
		out[j++] = PADSIO_DATA(0);		
	}		
    //sprintf (gb_cadLog,"Test %04x %04x %04x %08x",PADSIO_BAUD(0),PADSIO_MODE(0),PADSIO_CTRL(0),PADSIO_STATUS(0));
	PADSIO_CTRL(0) = 0;
}

//***********************************************************************
void PollSpeed1617Flag()
{ 
 u_char i;
 u_char aux;
 for (i=0;i<32;i+=2)
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
}


//***********************************************************************
void PollSpeed2021Flag()
{ 
 u_char i;
 u_char aux;
 for (i=0;i<64;i+=2)
 {
  aux = (gb_ptrFrameData[i]|(gb_ptrFrameData[i+1]<<4));
  if (gb_address_psExe_cont <(gb_size_psExe-100))
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
}


//***********************************************************************
void PollSpeed2425Flag()
{ 
 u_char i;
 u_char aux;
 for (i=0;i<128;i+=2)
 {
  aux = (gb_ptrFrameData[i]|(gb_ptrFrameData[i+1]<<4));
  //if (gb_address_psExe_cont <(gb_size_psExe-200))
  //{
  // if (main2[gb_address_psExe_cont] != aux)
  // {   
  //  gb_error = 1;
  //  sprintf (gb_cadLog,"\nERROR id %d src %x value %x",gb_address_psExe_cont,main2[gb_address_psExe_cont],aux);
  // }
  //}
  gb_p_address[gb_address_psExe_cont] = aux;
  gb_address_psExe_cont++;
  if ((gb_type == 1)&&(gb_address_psExe_cont==128))
   gb_address_psExe_cont= 2048;   
 }
 if (gb_size_psExe>0 && gb_address_psExe_cont>0 && gb_address_psExe_cont >= gb_size_psExe)
 {
  CheckFileData();  
  if (gb_launch_file == fileEXE)
  {
   gb_loop=0;
   gb_launch_exe = 1;
  }
  else
  {
   ResetLoadNextFile();
   //ResetLoader();
  }
 }
}


//***********************************************************************
void PollSpeed2829Flag()
{ 
 u_char i;
 u_char aux;
 for (i=0;i<256;i+=2)
 {
  aux = (gb_ptrFrameData[i]|(gb_ptrFrameData[i+1]<<4));
  if (gb_address_psExe_cont <(gb_size_psExe-500))
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
 {gb_loop=0;
  gb_launch_exe = 1; 
 }
}


//***********************************************************************
void TestPADSIO(void)
{//Recivo FF4100x0x0x0 
 if (gb_error == 1)
  return;
 switch (gb_speed)
 {
  case 12:case 13:case 14:case 15:
   SendData(GB_USEPORTARDUINO, gb_DataToSend20, gb_bufferFrame, 20);//Speed 12,13,14,15
   break;
  case 16:case 17:case 18:case 19:
   SendData(GB_USEPORTARDUINO, gb_DataToSend36, gb_bufferFrame, 36);//Speed 16,17,18,19
   break;
  case 20:case 21:case 22:case 23:
   SendData(GB_USEPORTARDUINO, gb_DataToSend68, gb_bufferFrame, 68);//Speed 20,21,22,23
   break;
  case 24:case 25: 
   SendData(GB_USEPORTARDUINO, gb_DataToSend132, gb_bufferFrame, 132);//Speed 24,25
   break;
  default:break;
 }
 //SendData(GB_USEPORTARDUINO, gb_DataToSend, gb_bufferFrame, 260); //Speed 28,29
	  
 //if ((Rcv[2] & 0x01)==0x01) gb_std=1; else gb_std=0;
 
 //if (gb_bufferFrame[1] != 0x41) //Mensaje 1
 //{
 // sprintf (gb_cadLog,"Test %02x%02x%02x%02x%02x%02x%02x%02x",gb_bufferFrame[0],gb_bufferFrame[1],gb_bufferFrame[2],gb_bufferFrame[3],gb_bufferFrame[4],gb_bufferFrame[5],gb_bufferFrame[6],gb_bufferFrame[7]);
 // return;
 //}
	 
 
 if ((gb_bufferFrame[2] & 0x01)==0x01) gb_std=1; else gb_std=0;
  
 if (gb_BeginPulse == 0)
 {//Busca que este en silencio Primera vez
  if (gb_std==0)   
   gb_BeginPulse = 1;	     
  return;
 }
  
 if (gb_std != gb_std_antes)
 {
  gb_std_antes = gb_std;
  gb_contChange = 0;
  if (
      ((gb_useFlag == 1) && (gb_std == 1))
	  ||
	  (gb_useFlag == 0)
	 )
  //if (gb_std == 1) //Lo uso cuando es con flag	 
  {
   if (gb_receivedHead==0)
   {
    switch (gb_speed)
	{
     case 12:case 13:case 16:case 17:case 20:case 21:case 24:case 25:
	  PollHeadSpeed1213161720212425(); //Modo 12,13  16,17, tambien 20,21,  24,25  28,29
	  break;
	 case 14:case 15: DecodeSIOBuffer1415();PollHeadSpeed1415(); break;//Modo 14,15
	 case 18:case 19: DecodeSIOBuffer1819();PollHeadSpeed1819(); break;//Modo 18,19
	 case 22:case 23: DecodeSIOBuffer2223();PollHeadSpeed2223(); break;//Modo 22,23
	 default: break;
	}
   }
   else
   {
	#ifdef GB_USEPADTESTSPEED
	 if (gb_speed != gb_currentPadSpeed)
	 {sprintf(gb_cadLog,"ERROR SPEED SELECT %d RECEIVE %d",gb_currentPadSpeed,gb_speed);
	  gb_error = 1;
	  return;
	 }
	#endif
    gb_contNoFlagHeadBlock++; //Para saltarse relleno linea
	switch (gb_speed)
	{
	 default: break;
	 case 12: if (gb_contNoFlagHeadBlock<21) return; break;//Velocidad 12 relleno	  
	 case 13: if (gb_contNoFlagHeadBlock<42) return; break;//Velocidad 13 sin flag saltar 42 lecturas relleno linea
	 case 14: if (gb_contNoFlagHeadBlock<21) return; break;//Velocidad 14 con flag saltar 21 lecturas relleno
	 case 15: if (gb_contNoFlagHeadBlock<42) return; break;//Velocidad 15 sin flag saltar 42 lecturas relleno
     case 16: if (gb_contNoFlagHeadBlock<21) return; break;//Para velocidad 16 con flag saltar 21 lecturas relleno linea
     case 17: if (gb_contNoFlagHeadBlock<42) return; break;//Para velocidad 17 sin flag saltar 42 lecturas relleno linea
     case 18:case 19: if (gb_contNoFlagHeadBlock<21) return; break;//Para velocidad 18,19 21 lecturas relleno linea
	 case 20:case 21: if (gb_contNoFlagHeadBlock<21) return; break;//Velocidad 20 y 21  21 lecturas relleno linea
	 case 22:case 23: if (gb_contNoFlagHeadBlock<11) return; break;//Velocidad 22,23 saltar 10 lecturas relleno linea
	 case 24:case 25: if (gb_contNoFlagHeadBlock<11) return; break; //Velocidad 24,25
	}
	//if (gb_contNoFlagHeadBlock<5) return;//Velocidad 28 salta 10 No funciona
	
	switch (gb_speed)
	{
	 default: break;
	 case 12: PollSpeed12Flag(); break;
	 case 13: PollSpeed13NoFlag(); break;
	 case 14: case 15: DecodeSIOBuffer1415();PollSpeed1415Flag(); break;	 
	 case 16: case 17: PollSpeed1617Flag(); break;	 
	 case 18: case 19: DecodeSIOBuffer1819();PollSpeed1819Flag(); break;
     case 20: case 21: PollSpeed2021Flag(); break;	 
     case 22: case 23: DecodeSIOBuffer2223();PollSpeed2223Flag(); break;	 
     case 24: case 25: PollSpeed2425Flag(); break;	 
     //case 28: PollSpeed2829Flag(); break; //No funciona
	 //case 29: PollSpeed2829Flag(); break; //No funciona
	}	
   }   
  }
 }
  
 if ((gb_contfps++) >= maxFpsUpdate)
 {
  gb_contfps = 0;
  if (gb_error == 0){
   sprintf(gb_cadLog, "%08x %d/%d\n%d %d %d",gb_p_address,gb_size_psExe,gb_address_psExe_cont,gb_speed,gb_type,gb_contChange);
  }  
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



int main(void) 
{	
 InitGraphics();			//this method sets up gfx for printing to screen	
 FntLoad(960, 256);		//this loads the font gfx to the vram
 //FntOpen(32, 32, 256, 200, 0, 512);	//this sets up the fonts printing attributes
 //x y ini, width heigh, clear, max char
 FntOpen(8, 8, 300, 24, 0, 64);	//this sets up the fonts printing attributes
						//eg printing boundries and number of letters to hold in printing buffer etc    	
 DrawSync(0); 
 
 //gb_address_psExe = 0x80010000; 
 //gb_launch_exe = 1; 
 //TestPADSIO(); ///Primer test quitar timeout 
 while (gb_loop == 1)
 {				//infinite
  activeBuffer = GsGetActiveBuff();	//gets the buffer currently being displayed and stores in activeBuffer
  GsSetWorkBase((PACKET*)GPUPacketArea[activeBuffer]);	//sets up the gfx workspace
  GsClearOt(0, 0, &myOT[activeBuffer]);			//clears the OT contents

  #ifdef GB_USEPADTESTSPEED
   if (gb_endSelect == 0)
   {ReadPad();
    SelectSpeed();
   }
  #endif  
  
  if (gb_endSelect == 1)
  {
   gb_contChange++;
   if (gbWaitRemovePAD == 0) //Esperamos quitar mando
   {	
	sprintf(gb_cadLog,"REMOVE PAD CONTROL PORT%d 00:%02d\n%s",(GB_USEPORTPADPSX+1),((maxTimeOutRemovePAD-gb_contChange)/60),gb_cadPrev);
	if (gb_contChange > maxTimeOutRemovePAD)
	{
	 gbWaitRemovePAD = 1;
	 gb_contChange = 0;
	 sprintf(gb_cadLog,"WAIT");
	}
   }
   else
   {
	TestPADSIO();
    if (gb_contChange > maxTimeOutChange) ResetLoader();    
   }
  }
  FntPrint("%s",gb_cadLog);  
  DisplayAll(activeBuffer);		//this displays the OT contents to screen		
 }
 
 if (gb_resetPSXSerial == 1) ResetPSXSERIAL();//Arranca Hitmen Serial
 
 if (gb_launch_exe==1)
 {CargaPrograma(&exe); //Prepara exec 
  ResetGraph(3);
  //PadStopCom();
  StopCallback();
  // for (i=0;i<100;i++)
  //  VSync(0); //Esperamos 2 segundos 50x2 
  EnterCriticalSection(); 
  Exec(&exe,1,0); //Fin exec
 }
 return 0;		//when program is finished return from it
} 


void InitGraphics(void) {
	//Prueba JJ
	ResetCallback();//JJ
	ResetGraph(0); //JJ 
	#ifdef MODE_VIDEO_PAL
	 SetVideoMode( MODE_PAL ); //JJ
	#endif 
	#ifdef MODE_VIDEO_NTSC
	 SetVideoMode( MODE_NTSC ); //JJ
	#endif 
	SetDispMask(1);				// 0: inhibit display 			
	//this method sets up gfx for printing to screen
	//GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0); //initialises the graphics system	
    // set the graphics mode resolutions (GsNONINTER for NTSC, and GsINTER for PAL)
	#ifdef MODE_VIDEO_PAL
	 GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);	//PAL
	#endif
    #ifdef MODE_VIDEO_NTSC	
	 GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsNONINTER|GsOFSGPU, 1, 0);	//NTSC
	#endif 
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