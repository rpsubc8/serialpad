//*********************************
//* Fake SPI PSX                  * 
//* Author: ackerman              *
//* Arduino IDE 1.8.11            *
//* CUSTOM MSG 0x1 PSXSDK PSYQ    *
//* PACK 56 bytes 448 bits        *
//* 64 bytes send 56 bytes        *
//* Vel 22: 50 ms 8960 baud (*)   * 
//* Vel 23: 25 ms 17920 baud (#)  *
//* 128 nibbles                   *
//* Espera 550 ms modo 22         *
//* Espera 275 ms modo 23         *
//* Vel 22 - 25                   *
//* Vel 24 - 25                   *
//*********************************
//Arduino SPI Slave for Playstation 2
//Based on busslave's work form 2012 
//Arduino pin |  AVR pin | PSX pin
//
//9 PB1       |    15    | 9 ACK     (green)
//10 -SS      |    16    | 6 ATT     (yellow)
//11 MOSI     |    17    | 2 COMMAND (orange)
//12 MISO     |    18    | 1 DATA    (brown)
//13 SCK      |    19    | 7 CLOCK   (blue)
//                       | 4 GND

#include "Arduino.h"
//#include <SoftwareSerial.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000
//#define ARDUINO 101

#define SPI_PORT PORTB
#define SPI_PINS PINB
#define SPI_DDR  DDRB
#define SPI_PINS  PINB

#define ACK_PIN   1 //PB1
#define ATT_PIN   2 //~SS
#define CMD_PIN   3 //MOSI
#define DATA_PIN  4 //MISO
#define CLK_PIN   5 //SCK

#define max_buf 1450
#define gb_speed_baud 115200
uint8_t gbDelay=25; //Tiempo
uint16_t gb_cont_buf=0;
uint8_t gb_buf[max_buf];
uint8_t gb_aux=0;
uint8_t gb_flipflop=0x01;

//JJ
//#define PSX_ACK_PORT  PORTB
//#define PSX_ACK_DDR   DDRB
//#define PSX_ACK_PIN   PINB
//#define PSX_ACK_BIT   (1<<0)

uint8_t gbUseFlag=1;

#define DATA_LEN 67

//CUSTOM MSG
volatile uint8_t data_buff[DATA_LEN]=   {0x41,0x00, 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02, 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02, 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02, 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, 0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02, 0xFF};
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00};
    
volatile uint8_t curr_byte=0;

uint8_t gb_cont_nibble=0;
uint8_t gb_nibble=0;

void SetFlag(uint8_t aux);
void EnviaDatos(void);
void Speed23ms17NoFlag(void);
void Speed23ms18NoFlag(void);
void Speed23ms20NoFlag(void);
void Speed23ms25NoFlag(void);
void Speed22ms17Flag(void);
void Speed22ms18Flag(void);
void Speed22ms20Flag(void);
void Speed22ms25Flag(void);
uint8_t CharHexToDec(char a);
void ResetAll(void);
void setup();
void loop();

//*************************************
void ResetAll(){
 data_buff[1] = 0x00; //Desactivo
 gb_cont_buf= 0;
 gb_flipflop= 0x01;
 gbUseFlag=1;//Pongo flag defecto
 gb_aux=0;
 gbDelay=25; 
 gb_cont_nibble=0;
}

//*************************************
uint8_t CharHexToDec(char a)
{uint8_t aReturn=0;
 if (a>=97) aReturn= a-87; //ABCDEF
 else if (a>=65) aReturn= a-55; //abcdef
  else if (a>=48) aReturn=a-48; //0123456789
 return aReturn;
}


//*****************************************************
void setup() {    
  Serial.begin(gb_speed_baud);  
     
  //SPI_PORT setup. 
  SPI_DDR |= (1<<ACK_PIN); // output
  SPI_PORT |= (1<<ACK_PIN); // set HIGH
  SPI_DDR |= (1<<DATA_PIN); // output  
  SPI_PORT |= (1<<DATA_PIN); // set HIGH   
  
  //SPI setup.
  PRR &= ~(1<<PRSPI);  // Set to 0 to ensure power to SPI module.
  //SPSR|=(1<<SPI2X);    // Fosc/32. @16MHz==500KHz.
  SPCR|=(1<<SPR1);   // Fosc/64. @16MHz==250KHz.
  SPCR|=(1<<CPHA);     // Setup @ leading edge, sample @ falling edge.
  SPCR|=(1<<CPOL);     // Leading edge is falling edge, trailing edge is rising edge.
  SPCR &= ~(1<<MSTR);  // MSTR bit is zero, SPI is slave.
  SPCR|=(1<<DORD);     // Byte is transmitted LSB first, MSB last.
  SPCR|=(1<<SPE);      // Enable SPI.
  SPCR|=(1<<SPIE);     // Enable Serial Transfer Complete (STC) interrupt.
  SPDR=0xFF;
  
  sei(); // Enable global interrupts.  
  ResetAll();
}

//SPI interrupt
ISR(SPI_STC_vect) { 
  // Byte received from the master
  uint8_t inbyte = SPDR;    
  // FIX ME : Manage other cases ? (other commands from the master?)
  //if (SPDR == command_buff[curr_byte])
  //if (inbyte == command_buff[curr_byte])  
  if ((curr_byte==0 && inbyte == 1)||(curr_byte>0 && inbyte == 0))
  {
    // We put the next byte in the buffer, to be send along the next clock cycle
    SPDR = data_buff[curr_byte++];
    //curr_byte++; //Optimizado
    
    // We need to ACK (PS2 Protocol)
    if (curr_byte < DATA_LEN) 
    { // ACK goes low.
      SPI_PORT &= ~(1<<PORTB1);
      //_delay_us(2); // For 10µseconds Quitar delay o 1 us para funcionar
      SPI_PORT |= (1<<PORTB1);      
    }     
    else 
    { // Except for the last packet
      SPDR = 0xFF;      
      //_delay_us(2);
      curr_byte = 0;      
    }   
  } 
  else 
  {    
    SPDR = 0xFF;   
    //_delay_us(2); 
    curr_byte = 0;    
  }
  
  //Serial.print(inbyte,HEX);  
  //Serial.print(" ");
  //Serial.flush();
}

//***************************************************************
void Speed22ms17Flag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = 0x01; //Activo
  _delay_us(17000);
  data_buff[1] = 0x00; //Desactivo
  _delay_us(17000);
 }
}

//***************************************************************
void Speed22ms18Flag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = 0x01; //Activo
  _delay_us(18000);
  data_buff[1] = 0x00; //Desactivo
  _delay_us(18000);
 }
}

//***************************************************************
void Speed22ms20Flag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = 0x01; //Activo
  _delay_us(20000);
  data_buff[1] = 0x00; //Desactivo
  _delay_us(20000);
 }
}

//***************************************************************
void Speed22ms25Flag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = 0x01; //Activo
  _delay_us(25000);
  data_buff[1] = 0x00; //Desactivo
  _delay_us(25000);
 }
}




//***************************************************************
void Speed23ms17NoFlag() //Envio 65 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = gb_flipflop; //Activo
  _delay_us(17000);
  gb_flipflop = ((!gb_flipflop)&0x01);
 }
}

//***************************************************************
void Speed23ms18NoFlag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = gb_flipflop; //Activo
  _delay_us(18000);
  gb_flipflop = ((!gb_flipflop)&0x01);
 }
}

//***************************************************************
void Speed23ms20NoFlag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = gb_flipflop; //Activo
  _delay_us(20000);
  gb_flipflop = ((!gb_flipflop)&0x01);
 }
}

//***************************************************************
void Speed23ms25NoFlag() //Envio 64 bytes 128 nibbles(quitamos parte alta fallo bit 7)
{for (uint16_t i=0;i<gb_cont_buf;i+=64)
 {memcpy(&data_buff[2],&gb_buf[i],64);
  data_buff[1] = gb_flipflop; //Activo
  _delay_us(25000);
  gb_flipflop = ((!gb_flipflop)&0x01);
 }
}


//***************************************************************
void EnviaDatos()
{
 gb_aux =0;
 if (gbUseFlag == 1)
 {//Con flag
  switch (gbDelay)
  {
   default: break;
   case 17: Speed22ms17Flag(); break;
   case 18: Speed22ms18Flag(); break;
   case 20: Speed22ms20Flag(); break;
   case 25: Speed22ms25Flag(); break;
  }
 }
 else
 {//Sin flag
  switch (gbDelay)
  {
   default: break;   
   case 17: Speed23ms17NoFlag(); break;
   case 18: Speed23ms18NoFlag(); break;
   case 20: Speed23ms20NoFlag(); break;
   case 25: Speed23ms25NoFlag(); break;   
  }    
 }
 gb_cont_buf=0;    
}

//***************************************************************
void SetFlag(uint8_t aux)
{
 gb_cont_buf= 0;
 gb_flipflop= 0x01;
 if (aux == 1)
  data_buff[1] = 0x00; //Desactivo
 gb_cont_nibble=0; 
 gbUseFlag=aux; //Ponemos Flag o no
 gb_aux=0;
}

//***************************************************************
void loop() {
 if(Serial.available()>=1)
 {
  gb_aux = Serial.read();  
  if (gb_aux>42)
  {    
   if ((gb_cont_nibble & 0x01) == 0x00)
    gb_nibble = (CharHexToDec(gb_aux)&0x0F);
   else
   {
    gb_buf[gb_cont_buf++]= ((CharHexToDec(gb_aux)<<4)|gb_nibble);
    if (gb_cont_buf>=max_buf) gb_cont_buf = 0;    
   }
   gb_cont_nibble++;
  }
 }

 //Velocidad
 switch (gb_aux)
 {
  default: break;
  case 10: EnviaDatos(); break; //Nueva linea envio datos
  case 36: gbDelay = 25; gb_aux=0;break; //$
  case 37: gbDelay = 20; gb_aux=0;break; //%
  case 38: gbDelay = 18; gb_aux=0;break; //&
  case 40: gbDelay = 17; gb_aux=0;break; //(
  case 41: gbDelay = 16; gb_aux=0; break; //)
  case 42: SetFlag(1); break; //* Flag  
  case 35: SetFlag(0); break; //#  
 }
} 
