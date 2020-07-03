//******************************
//* Fake SPI PSX               *
//* Author: ackerman           *
//* Mode 0: 50 ms 60 baud  (*) *
//* Mode 1: 25 ms 120 baud (#) *
//* Mode 2: 50 ms 160 baud (+) *
//* Mode 3: 25 ms 320 baud (,) *
//******************************

//En binario SPI Comunicaciones (2 bytes)
//Bit
//0   Select Button    (0=Pressed, 1=Released)
//1   L3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
//2   R3/Joy-button    (0=Pressed, 1=Released/None/Disabled) ;analog mode only
//3   Start Button     (0=Pressed, 1=Released)
//4   Joypad Up        (0=Pressed, 1=Released)
//5   Joypad Right     (0=Pressed, 1=Released)
//6   Joypad Down      (0=Pressed, 1=Released)
//7   Joypad Left      (0=Pressed, 1=Released)
//8   L2 Button        (0=Pressed, 1=Released) (Lower-left shoulder)
//9   R2 Button        (0=Pressed, 1=Released) (Lower-right shoulder)
//10  L1 Button        (0=Pressed, 1=Released) (Upper-left shoulder)
//11  R1 Button        (0=Pressed, 1=Released) (Upper-right shoulder)
//12  /\ Button        (0=Pressed, 1=Released) (Triangle, upper button)
//13  () Button        (0=Pressed, 1=Released) (Circle, right button)
//14  >< Button        (0=Pressed, 1=Released) (Cross, lower button)
//15  [] Button        (0=Pressed, 1=Released) (Square, left button)

//Botones en la PSX real
//Pad1L2          (1<< 0)  0
//Pad1R2          (1<< 1)  1
//Pad1L1          (1<< 2)  2
//Pad1R1          (1<< 3)  3
//Pad1tri         (1<< 4)  4
//Pad1crc         (1<< 5)  5
//Pad1x           (1<< 6)  6
//Pad1sqr         (1<< 7)  7
//Pad1Select      (1<< 8)  8
//Pad1Start       (1<<11)  9
//Pad1Up          (1<<12) 10
//Pad1Right       (1<<13) 11
//Pad1Down        (1<<14) 12
//Pad1Left        (1<<15) 13

//Botones conversor USB PSX botones controlador en Windows
//triangulo - 1
//Circulo   - 2
//X         - 3
//Cuadrado  - 4
//L2        - 5
//R2        - 6
//L1        - 7
//R1        - 8
//Start     - 9
//Select    - 10
//Arriba    - 13
//Derecha   - 14
//Abajo     - 15
//Izquierda - 16

//Analogico
//Stick izquierdo - 11
//Stick derecho   - 12

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

//Digital
//#define DATA_LEN 5
//Analogico
//#define DATA_LEN 9
//#define DATA_LEN 10

//#define LED_PIN 7

//#define _DEBUG 0

#define max_buf 256
#define gb_speed_baud 38400
uint8_t gbDelay=25; //Tiempo
uint8_t gb_cont_buf=0;
uint8_t gb_buf[max_buf];
uint8_t gb_aux=0;
uint8_t gb_flipflop=0x01;


//JJ
//#define PSX_ACK_PORT  PORTB
//#define PSX_ACK_DDR   DDRB
//#define PSX_ACK_PIN   PINB
//#define PSX_ACK_BIT   (1<<0)

//uint8_t gb_aux0=0;
//uint8_t gb_aux1=0;
uint8_t gb_low=0;
uint8_t gb_high=0;
uint8_t gbUseFlag=1;
uint8_t gbUseReadNumNibble=1; //Lee 1 nibble puerto serie logica positiva


//En ordenador usb psx adaptador funciona esto en el modo analogico
//volatile uint8_t data_buff[DATA_LEN]={0x53,0x53,0x5A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; //sin parpadeo analogico
//char control_data[2] = {0xFF,0xFF};
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //No parpadeo analogico
//Para digital en usb psx adaptador funciona esto en modo digital
//volatile uint8_t data_buff[DATA_LEN]={0xFF,0x41,0x5A,0xFF,0xFF}; // Reply: nothing pressed digital OK sin parpadeo Digital
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00}; //No parpadeo digital


//char control_data[2] = {0xFF,0xFF};
//char control_data[2] = {0x00,0x00}; //JJ

/*
//INI Prueba emulador PAD modo digital
#define DATA_LEN 5
#define begin_buttons0 2
#define begin_buttons1 3
volatile uint8_t data_buff[DATA_LEN]={0xFF,0x41,0x5A,0xFF,0xFF}; // Reply: nothing pressed digital OK sin parpadeo Digital
volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00}; //No parpadeo digital
//FIN Prueba emulador PAD modo digital
*/

//INI Prueba PSX REAL PAD modo digital
//1 byte flechas
//2 byte gatillos
//0x00,0x80 No parpadea
//0000 Select apretado y resto botones
//1000 Select sin apretar y resto botones
#define DATA_LEN 5
#define begin_buttons0 2
#define begin_buttons1 3
volatile uint8_t data_buff[DATA_LEN]={0x41,0x5A,0xFF,0xFF,0xFF}; // Reply: nothing pressed digital OK sin parpadeo Digital
volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00}; //No parpadeo digital
//FIN Prueba PSX REAL PAD modo digital


//volatile uint8_t data_buff[DATA_LEN]={0x41,0x5A,0x00,0x00,0xFF}; // Reply: nothing pressed. parpadea
//Digital volatile uint8_t data_buff[DATA_LEN]={0xFF,0x41,0x5A,0xFF,0xFF}; // Reply: nothing pressed digital OK sin parpadeo Digital
//volatile uint8_t data_buff[DATA_LEN]={0xFF,0x41,0x5A,0xFF,0xFF}; //Digital OK
//volatile uint8_t data_buff[DATA_LEN]={0x41,0x5A,0xFF,0xFF,0xFF}; //Digital OK
//volatile uint8_t data_buff[DATA_LEN]={0x73,0x5A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; // parpadeo analogico
//volatile uint8_t data_buff[DATA_LEN]={0x73,0x73,0x5A,0xFF,0x80,0x80,0x80,0x80,0xFF}; //sin parpadeo analogico funciona ne padtest y PC
//volatile uint8_t data_buff[DATA_LEN]={0x73,0x73,0x5A,0xFF,0xFF,0x80,0x80,0x80,0x80,0xFF}; //sin parpadeo analogico funciona ne padtest y PC
//volatile uint8_t data_buff[DATA_LEN]={0x73,0x5A,0xFF,0xFF,0x80,0x80,0x80,0x80,0xFF}; //sin parpadeo analogico
//volatile uint8_t data_buff[DATA_LEN]={0x41,0x5A,0xFF,0xFF,0xFF}; // parpadeo digital
//Digital volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00}; //No parpadeo digital
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //No parpadeo analogico
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //No parpadeo analogico
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //No parpadeo analogico
//volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00};
    
volatile uint8_t curr_byte=0;
volatile uint8_t next_byte=0;

uint8_t gb_cont_nibble=0;
uint8_t gb_nibble=0;
uint8_t CharHexToDec(char a);
void setup();
void loop();
//static void ack_jj(void);

//*************************************
uint8_t CharHexToDec(char a)
{
 uint8_t aReturn=0;
 if (a>=97) aReturn= a-87; //ABCDEF
 else if (a>=65) aReturn= a-55; //abcdef
  else if (a>=48) aReturn=a-48; //0123456789
 return aReturn;
}


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

  //data_buff[begin_buttons0] = 0x01; //Para evitar parpadeo desconexion Flechas y Select
  data_buff[begin_buttons0] = 0x00; //Para evitar parpadeo desconexion Flechas y Select
  data_buff[begin_buttons1] = 0xFF; //Los gatillos
  //Serial.print("Ini");
  //delay(1000);
}

//static void ack_jj()
//{
//  _delay_us(1);
//  // pull acknowledge
//  PSX_ACK_PORT &= ~PSX_ACK_BIT;
//  PSX_ACK_DDR |= PSX_ACK_BIT;
//  _delay_us(3);
//  // release acknowledge
//  PSX_ACK_DDR &= ~PSX_ACK_BIT;
//}


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
      _delay_us(2); // For 10µseconds            
      SPI_PORT |= (1<<PORTB1);      
      //ack_jj();      
    }     
    else 
    { // Except for the last packet
      SPDR = 0xFF;      
      curr_byte = 0;
      //ack_jj();
    }   
  } 
  else 
  {    
    SPDR = 0xFF;    
    curr_byte = 0;      
    //ack_jj();
  }
  
}



void loop() {

  /* Reads input from Serial connection */
  /*
    We're going to read 2 characters = 2 x 1 byte
    It's going to be read as a char but then translated
    to HEX, taking only the lower byte
  */

 if(Serial.available()>=1)
 {  
  gb_aux = Serial.read();
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13 && gb_aux!=43 && gb_aux!=45 && gb_aux!=42 && gb_aux!=35 && gb_aux!=36 && gb_aux!=37)  
  {
   switch (gbUseReadNumNibble)
   {
    default: break;
    case 1: gb_buf[gb_cont_buf++] = (~(CharHexToDec(gb_aux))&0x0F)|0xF0; break; //Modo 0,1 (* # le llega -)
    case 2: //Modo 2,3 Leemos 8 bits
     if ((gb_cont_nibble & 0x01) == 0x00) 
      gb_nibble = (CharHexToDec(gb_aux)&0x0F);
     else 
      gb_buf[gb_cont_buf++]= ((CharHexToDec(gb_aux)<<4)|gb_nibble);
     gb_cont_nibble++;
     break;
   }
  }
 }

 switch (gb_aux)
 {
  default: break;
  //Numero de nibbles a leer
  case 43: gbUseReadNumNibble= 2; //+ Lee 2 nibbles Modo 2,3 Logica negada botones
   gb_nibble = gb_cont_nibble= gb_cont_buf = gb_aux = 0;
   break; 
  case 45: gbUseReadNumNibble= 1; //- Lee 1 nibble Modo 0,1 logica positiva botones
   gb_cont_buf = gb_aux = 0;
   break;   
  //Velocidad
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
  data_buff[begin_buttons0] = 0x00; //Ponemos a 0  
  //data_buff[begin_buttons0] = 0x00; //Ponemos a 0  
  data_buff[begin_buttons1] = 0xFF;
  gb_cont_nibble=0;
  //Serial.end(); 
  //Serial.begin(gb_speed_baud);
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
   {
    switch (gbUseReadNumNibble)
    {
     default: break;
     case 1: //Modo 0 con flag 50 ms 4 bits 60 baud
      for (uint8_t i=0;i<gb_cont_buf;i++)
      {     
       data_buff[begin_buttons1] = gb_buf[i];
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);           
       data_buff[begin_buttons1] = 0xFF; //Ponemos a 0
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);                   
      }                
      break;
     case 2: //Modo 2 con flag 50 ms 8 bits 160 baud
      for (uint8_t i=0;i<gb_cont_buf;i++)
      {     
       data_buff[begin_buttons1] = gb_buf[i];
       data_buff[begin_buttons0] = 0x00; //Select activo
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);           
       data_buff[begin_buttons1] = 0xFF; //Ponemos a 0       
       data_buff[begin_buttons0] = 0x01; //Select desactivado
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);
      }             
      break;        
    }
   }
   else
   {//Sin flag
    switch (gbUseReadNumNibble)
    {
     default: break;
     case 1: //Modo 1 (25 ms sin flag 120 baud 4 bits)
      for (uint8_t i=0;i<gb_cont_buf;i++)
      {         
       data_buff[begin_buttons1] = gb_buf[i];     
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);     
      }
      break;
     case 2: //Modo 3 (25 ms sin flag 320 baud 8 bits)
      for (uint8_t i=0;i<gb_cont_buf;i++)
      {         
       data_buff[begin_buttons1] = gb_buf[i];     
       data_buff[begin_buttons0] = gb_flipflop;
       for (uint8_t j=0;j<gbDelay;j++) _delay_us(1000);
       gb_flipflop = ((!gb_flipflop)&0x01);
      }
      break;     
    }
   }
   gb_cont_buf=0;    
  }
 }
 
} 

 
 /*Sobra
//Serial.flush();
    //Serial.print("(");   
    //Serial.print(gb_cont_buf);
    //Serial.print(")");      
     //Serial.print(gb_buf[i],HEX);     
     //Serial.print(gb_buf[i+1],HEX);     
     //Fix flick button en PSX real     
     //data_buff[begin_buttons0] = 0x00; //Boton select como flag 11111110
     //data_buff[begin_buttons0] = gb_buf[i];
     //data_buff[begin_buttons1] = gb_buf[i+1]; //Ponemos a 0 los botones sin logica negada                         
     //data_buff[begin_buttons0] = 0xFE; //Boton select como flag 11111110 En real genera Flick                    
   
//if (gb_aux==10)
//Serial.flush();
      //Serial.print(gb_cont_buf);
   //Serial.print(" ");   
   //for (uint8_t i=0;i<gb_cont_buf;i+=4)
      //Serial.end(); 
   //Serial.begin(9600);
   //Serial.print(gb_low,HEX);
    //Serial.print(gb_high,HEX);
   //delay(gbDelay);
   //Serial.print((char)(gb_buf[i]));
    //gb_high= (CharHexToDec(gb_buf[i+3])<<4)|CharHexToDec(gb_buf[i+2]);
    //data_buff[3] = ~(gb_low); //Ponemos a 0 los botones
    //data_buff[2] = ~(0x01); //Boton select como flag
    
    //data_buff[3] = ~(gb_high);
    //delay(gbDelay);   
   //data_buff[3] = ~0x1; //Ponemos a 0 los botones
   //data_buff[4] = 0xFF;
   //data_buff[2] = 0xFE; //Ponemos a 0 los botones
   //data_buff[3] = 0xFF;  

  data_buff[2] = CharHexToDec(gb_aux0);
  data_buff[3] = 0xFF;
  delay(100);

 //Serial.readBytes(control_data, 2);
 //data_buff[2] = lowByte(control_data[0]);
 //data_buff[3] = lowByte(control_data[1]);
 //Serial.print(control_data);
 //Serial.print('|');
 //Serial.print(control_data[0]);
 //Serial.print('|');
 //Serial.print(control_data[1]);
 //Serial.print('|);
 //Serial.flush();
 //data_buff[2]= 0x02;//control_data[0];
 //data_buff[3]= 0x00;//control_data[1];
//}
   
//   flipflop = (flipflop==0xFF)?0x7F:0xFF;
   //data_buff[2] = flipflop;
   //data_buff[3] = flipflop;      
//   data_buff[2]= 0xFF;
//   data_buff[3]= flipflop; //Cuadrado 4   
   //data_buff[4]= flipflop; //Cuadrado 4   
   //data_buff[5]= flipflop; //Cuadrado 4   
   //data_buff[6]= flipflop; //Cuadrado 4   
   //data_buff[4]= 0x00;
   //data_buff[4]= flipflop; //Cuadrado 4        
   //data_buff[5]= flipflop; //Cuadrado 4        
   //data_buff[6]= flipflop; //Cuadrado 4           
   //control_data[0] = flipflop;
   //control_data[1] = flipflop;   

  
  //if (Serial.available() >= 2) {
    
    //Serial.readBytes(control_data, 2); // Read 16 bits

    // DEBUG 
    //if (_DEBUG) {
    //  Serial.print("Command : 0x");
    //  Serial.print(lowByte(control_data[0]), HEX);
    //  Serial.print(" 0x");
    //  Serial.print(lowByte(control_data[1]), HEX);
    //  Serial.println("");
    //}

    // We store the new commands in the data_buff buffer
    //data_buff[2] = lowByte(control_data[0]);
    //data_buff[3] = lowByte(control_data[1]);

    // Led
    //if (lowByte(control_data[0]) != 255 || lowByte(control_data[1]) != 255){
    //  digitalWrite(LED_PIN, HIGH);
    //} else {
    //  digitalWrite(LED_PIN, LOW);
    //}

//    for (uint16_t i=0;i<20000;i++)
//     _delay_us(100);
  //}

}



  // Indicate Reset
  //pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, HIGH);
  //delay(500);
  //digitalWrite(LED_PIN, LOW);
  //delay(70);
  //digitalWrite(LED_PIN, HIGH);
  //delay(70);
  //digitalWrite(LED_PIN, LOW);
  //delay(70);
  //digitalWrite(LED_PIN, HIGH);
  //delay(70);
  //digitalWrite(LED_PIN, LOW);
  //delay(70);
  //digitalWrite(LED_PIN, HIGH);
  //delay(70);
  //digitalWrite(LED_PIN, LOW);

  //if (_DEBUG) {
  //  Serial.println("");
  //  Serial.println("-----------------------------------");
  //  Serial.println("Playstation 2 Protocol initialized.");
  //  Serial.println("-----------------------------------");
  //}

*/
