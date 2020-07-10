//*********************************
//* Fake SPI PSX                  * 
//* Author: ackerman              *
//* Arduino IDE 1.8.11            *
//* Mode 10: 50 ms 480 baud (*,)  * 
//* Mode 11: 25 ms 960 baud (#,)  *
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
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000
#define ARDUINO 101

#define SPI_PORT PORTB
#define SPI_PINS PINB
#define SPI_DDR  DDRB
#define SPI_PINS  PINB

#define ACK_PIN   1 //PB1
#define ATT_PIN   2 //~SS
#define CMD_PIN   3 //MOSI
#define DATA_PIN  4 //MISO
#define CLK_PIN   5 //SCK


//Analogico
#define max_buf 512
//#define gb_speed_baud 57600
#define gb_speed_baud 38400
//#define gb_speed_baud 9600
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


//INI Prueba PSX REAL PAD modo analogico
//En el retorno de linea dejo 00 en los botones
//1 byte flechas
//2 byte gatillos
//0x00,0x80 No parpadea
//0000 Select apretado y resto botones
//1000 Select sin apretar y resto botones
#define DATA_LEN 9
#define begin_buttons0 2
#define begin_buttons1 3
volatile uint8_t data_buff[DATA_LEN]={0x73,0x5A,0x00,0x00,0x01,0x02,0x03,0x04,0xFF};
volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//FIN Prueba PSX REAL PAD modo analogico
    
volatile uint8_t curr_byte=0;
volatile uint8_t next_byte=0;

uint8_t gb_cont_nibble=0;
uint8_t gb_nibble=0;

void Speed10ms25Flag(void);
void Speed11ms16NoFlag(void);
void Speed11ms17NoFlag(void);
void Speed11ms18NoFlag(void);
void Speed11ms20NoFlag(void);
void Speed11ms25NoFlag(void);
uint8_t CharHexToDec(char a);
void setup();
void loop();

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
  // Begin Serial communication with Pi
  //Serial.begin(38400);
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
  
  //En modo analogico
  data_buff[begin_buttons0] = 0x00; //Para evitar parpadeo desconexion Flechas y Select
  data_buff[begin_buttons1] = 0x0F; //Los gatillos  
}


/* SPI interrupt */
ISR(SPI_STC_vect) { 

  // Byte received from the master
  uint8_t inbyte = SPDR;  
  
  // FIX ME : Manage other cases ? (other commands from the master?)
  //if (SPDR == command_buff[curr_byte])
  if (inbyte == command_buff[curr_byte])  
  {
    // We put the next byte in the buffer, to be send along the next clock cycle
    SPDR = data_buff[curr_byte++];
    //curr_byte++; //Optimizado
    
    // We need to ACK (PS2 Protocol)
    if (curr_byte < DATA_LEN) 
    { // ACK goes low.
      SPI_PORT &= ~(1<<PORTB1);
      //_delay_us(1); // For 10µseconds Quitar delay o 1 us para funcionar
      SPI_PORT |= (1<<PORTB1);      
    }     
    else 
    { // Except for the last packet
      SPDR = 0xFF;      
      curr_byte = 0;      
    }   
  } 
  else 
  {    
    SPDR = 0xFF;    
    curr_byte = 0;    
  }
  
}

//***************************************************************
void Speed10ms25Flag()
{for (uint8_t i=0;i<gb_cont_buf;i+=5)
 {memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = 0x0E; //Select activo
  _delay_us(25000);
  data_buff[3] = 0x0F;
  data_buff[2] = 0x01;
  _delay_us(25000);
 }
}

//***************************************************************
void Speed11ms16NoFlag()
{for (uint16_t i=0;i<gb_cont_buf;i+=5){
  memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = gb_flipflop;
  //_delay_us(16666.6666);
  //_delay_us(16666.666666666666666666666666667);
  _delay_us(16666);
  gb_flipflop = ((!gb_flipflop)&0x01);}
}

//***************************************************************
void Speed11ms17NoFlag()
{for (uint16_t i=0;i<gb_cont_buf;i+=5){
  memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = gb_flipflop;
  _delay_us(17000);
  gb_flipflop = ((!gb_flipflop)&0x01);}
}

//***************************************************************
void Speed11ms18NoFlag()
{for (uint16_t i=0;i<gb_cont_buf;i+=5){
  memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = gb_flipflop;
  _delay_us(18000);
  gb_flipflop = ((!gb_flipflop)&0x01);}
}

//***************************************************************
void Speed11ms20NoFlag()
{for (uint16_t i=0;i<gb_cont_buf;i+=5){
  memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = gb_flipflop;
  _delay_us(20000);
  gb_flipflop = ((!gb_flipflop)&0x01);}
}

//***************************************************************
void Speed11ms25NoFlag()
{for (uint16_t i=0;i<gb_cont_buf;i+=5){
  memcpy(&data_buff[3],&gb_buf[i],5);
  data_buff[begin_buttons0] = gb_flipflop;
  _delay_us(25000);
  gb_flipflop = ((!gb_flipflop)&0x01);}
}

//***************************************************************
void loop() {
 if(Serial.available()>=1)
 {  
  gb_aux = Serial.read();
//  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=13 && gb_aux!=32 &&
//      gb_aux!=36 && gb_aux!=37 && gb_aux!=38 && gb_aux!=40 && gb_aux!=41 &&
//      gb_aux!=35 && gb_aux!=42
//      )
  if (gb_aux>42)
  {
   if ((gb_cont_nibble & 0x01) == 0x00)
    gb_nibble = (CharHexToDec(gb_aux)&0x0F);
   else
    gb_buf[gb_cont_buf++]= ((CharHexToDec(gb_aux)<<4)|gb_nibble);
   gb_cont_nibble++;
  }
 }

 //Velocidad
 switch (gb_aux)
 {
  default: break;     
  case 36: gbDelay = 25; break; //$
  case 37: gbDelay = 20; break; //%
  case 38: gbDelay = 18; break; //&
  case 40: gbDelay = 17; break; //(
  case 41: gbDelay = 16; break; //)
 }
 
 if ((gb_aux==42) || (gb_aux==35))
 {    
  gb_cont_buf= 0;
  gb_flipflop= 0x01;  
  //Mando analogico
  data_buff[begin_buttons0] = 0x00; //Ponemos a 0  
  data_buff[begin_buttons1] = 0x0F;
  //Fin mando analogico
  gb_cont_nibble=0;
  switch (gb_aux)
  {   
   case 42: gbUseFlag=1; break; //*
   case 35: gbUseFlag=0; break; //#
   default: gbUseFlag=1; break;  
  }  
  gb_aux=0;
 }
 else
 {
  if (gb_aux==10)
  {
   gb_aux =0;
   if (gbUseFlag == 1)
   {//Con flag
    switch (gbDelay)
    {
     default: break;
     case 25: Speed10ms25Flag(); break;
    }
   }
   else
   {//Sin flag
    switch (gbDelay)
    {
     default: break;
     case 16: Speed11ms16NoFlag(); break;
     case 17: Speed11ms17NoFlag(); break;
     case 18: Speed11ms18NoFlag(); break;
     case 20: Speed11ms20NoFlag(); break;
     case 25: Speed11ms25NoFlag(); break;
    }    
   }
   gb_cont_buf=0;    
  }
 }
 
} 
