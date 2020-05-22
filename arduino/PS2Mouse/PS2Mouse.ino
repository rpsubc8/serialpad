
//Ackerman
//PSX Mouse
//Arduino Nano CLK pin 2, Data pin 4
//USB Pinout (Left to Right, USB symbol up)
//4: GND
//3: Clk
//2: Data
//1: Vcc

#include "PS2Mouse.h"

#define pinR1 7
#define pinR2 8
#define pinL1 5
#define pinL2 6

#define DataPin 4
#define IRQpin 2

//PS2Mouse mouse(6,5);
PS2Mouse mouse(IRQpin,DataPin); //Clock Data

void SendDataMouseFourBtnNoStrobe(int xpos,int ypos, uint8_t mLeft, uint8_t mRight, uint8_t aDelay);
//void SendDataMouseFourBtn(int xpos,int ypos, uint8_t mLeft, uint8_t mRight, uint8_t aDelay, uint8_t syncDelay);
void setup(void);
void loop(void);

void setup()
{
  pinMode(pinR1, OUTPUT);
  pinMode(pinR2, OUTPUT);
  pinMode(pinL1, OUTPUT);
  pinMode(pinL2, OUTPUT);
    
  Serial.begin(9600);
  while(!Serial);
  Serial.print("Setup...");

  mouse.begin();
  Serial.println("complete!");

  digitalWrite(pinR1,LOW);
  digitalWrite(pinR2,LOW);
  digitalWrite(pinL1,LOW);
  digitalWrite(pinL2,LOW);  
}

//uint8_t contLine =0;
//uint16_t xposAntes=0;
//uint16_t yposAntes=0;
//uint8_t mouseLeftAntes=0;
//uint8_t mouseRightAntes=0;

void loop(){
  uint8_t stat;
  int x,y;
  uint8_t btnL;
  uint8_t btnR;
  mouse.getPosition(stat,x,y);
  btnL = stat & 1;
  btnR = ((stat & 2)==2)?1:0;
  //if ((x != 0) && (y != 0))
  {   
   //SendDataMouseFourBtn(x,y,btnL,btnR,25, 25);
   SendDataMouseFourBtnNoStrobe(x,y,btnL,btnR,25);
   //Serial.print(btnL);
   //Serial.print(btnR);
   //Serial.print(" ");
   //Serial.print(x, DEC);
   //Serial.print(",");
   //Serial.print(y, DEC);
   //Serial.print('|');
   //contLine++;
   //if (contLine>10)
   //{
   // Serial.println();
   // contLine=0;
   //}   
  }
  /*if (x != xposAntes) 
   xposAntes= x; 
  if (y != yposAntes) 
   yposAntes= y; 
   */
  //if (btnL != mouseLeftAntes)
  // mouseLeftAntes= btnL;
  //if (btnR != mouseRightAntes)
  // mouseRightAntes= btnR; 
   
  //Serial.print(stat, BIN);
  //Serial.print("\tx=");
  //Serial.print(x, DEC);
  //Serial.print("\ty=");
  //Serial.println(y, DEC);
  //Serial.print(stat, BIN);

  //delay(1000);  
  //delay(20);  
}

//*********************************************************************************************************
void SendDataMouseFourBtnNoStrobe(int xpos, int ypos, uint8_t mLeft, uint8_t mRight, uint8_t aDelay)
{
 //PSX  ePSXe   Function 
 //R1 -   R   - Data Bit 0
 //R2 -   T   - Data Bit 1
 //L1 -   W   - Data Bit 2
 //L2 -   E   - Data Bit 3
 //2 bits X 
 // 00 quieto
 // 01 derecha
 // 11 izquierda
 // 10 boton izquierdo
 //2 bits Y
 // 00 quieto
 // 01 arriba
 // 11 abajo
 // 10 boton derecho
 //Send 4 bits Mouse R1, R2, L1, L2   R,T,W,E bit data ePSXe compatible  
 uint8_t auxData = 0;
 uint8_t MouseBit0,MouseBit1,MouseBit2,MouseBit3;

 if (xpos < 0)
 {//Izquierda
  MouseBit0 = 1;
  MouseBit1 = 1;
 }
 else
 {
  if (xpos > 0)
  { //Derecha
   MouseBit0 = 1;
   MouseBit1 = 0;
  }
  else
  {//Quieto
   MouseBit0 = 0;
   MouseBit1 = 0;  
  }
 }
 
 if (ypos < 0)
 {
  MouseBit2 = 1;
  MouseBit3 = 1;
 }
 else
 {
  if (ypos > 0)
  {
   MouseBit2 = 1;
   MouseBit3 = 0;
  }
  else
  {
   MouseBit2 = 0;
   MouseBit3 = 0; 
  }
 } 

 //Botones    
 if (mLeft == 1) 
 {
  MouseBit0 = 0;
  MouseBit1 = 1;
 }
 
 if (mRight == 1)
 {
  MouseBit2 = 0;
  MouseBit3 = 1;
 }
  
 auxData = MouseBit0 | (MouseBit1<<1) | (MouseBit2<<2) | (MouseBit3<<3);

 if (auxData == 0)
  return;

 digitalWrite(pinR1,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinL2,LOW);

 //Send 4 bits
 //Bit 0
 if ((auxData & 1) == 1) 
  digitalWrite(pinR1,HIGH);
 else 
  digitalWrite(pinR1,LOW);
  
 if ((auxData & 2) == 2) 
  digitalWrite(pinR2,HIGH);
 else 
  digitalWrite(pinR2,LOW);

 //Bit 1
 if ((auxData & 4) == 4) 
  digitalWrite(pinL1,HIGH);
 else 
  digitalWrite(pinL1,LOW);

 //Bit 2
 if ((auxData & 8) == 8) 
  digitalWrite(pinL2,HIGH);
 else 
  digitalWrite(pinL2,LOW);
    
 delay(aDelay);
 digitalWrite(pinR1,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinL2,LOW); 
}


//**************************************************************************
/*
void SendDataMouseFourBtn(int xpos, int ypos, uint8_t mLeft, uint8_t mRight, uint8_t aDelay, uint8_t syncDelay)
{
 //PSX  ePSXe   Function 
 //R1 -   R   - sync
 //R2 -   T   - Data Bit 0,3
 //L1 -   W   - Data Bit 1,4
 //L2 -   E   - Data Bit 2,5
 //2 bits X 
 // 00 quieto
 // 01 derecha
 // 11 izquierda
 // 10 movimiento rapido
 //2 bits Y
 //2 bits buttons
 //Send 6 bits Mouse R1, R2, L1, L2   R sync T,W,E bit data ePSXe compatible  
 uint8_t auxData = 0;
 uint8_t MouseBit0,MouseBit1,MouseBit2,MouseBit3,MouseBit4,MouseBit5;

 if (xpos < 0)
 {//Izquierda
  MouseBit0 = 1;
  MouseBit1 = 1;
 }
 else
 {
  if (xpos > 0)
  { //Derecha
   MouseBit0 = 1;
   MouseBit1 = 0;
  }
  else
  {//Quieto
   MouseBit0 = 0;
   MouseBit1 = 0;  
  }
 }
 
 if (ypos < 0)
 {
  MouseBit2 = 1;
  MouseBit3 = 1;
 }
 else
 {
  if (ypos > 0)
  {
   MouseBit2 = 1;
   MouseBit3 = 0;
  }
  else
  {
   MouseBit2 = 0;
   MouseBit3 = 0; 
  }
 } 
   
 MouseBit4 = mLeft;
 MouseBit5 = mRight;  
  
 auxData = MouseBit0 | (MouseBit1<<1) | (MouseBit2<<2) | (MouseBit3<<3) | (MouseBit4<<4) | (MouseBit5<<5);

 if (auxData == 0)
  return;

 digitalWrite(pinR1,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinL2,LOW);
 delay(syncDelay);

 //Bit 0 First Send 3 bits
 if ((auxData & 1) == 1) 
  digitalWrite(pinR2,HIGH);
 else 
  digitalWrite(pinR2,LOW);

 //Bit 1
 if ((auxData & 2) == 2) 
  digitalWrite(pinL1,HIGH);
 else 
  digitalWrite(pinL1,LOW);

 //Bit 2
 if ((auxData & 4) == 4) 
  digitalWrite(pinL2,HIGH);
 else 
  digitalWrite(pinL2,LOW);
    
 digitalWrite(pinR1,HIGH);
 delay(aDelay);
 digitalWrite(pinR1,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinL2,LOW);
 delay(syncDelay);


 //Bit 3 Second Send 3 bits
 if ((auxData & 8) == 8) 
  digitalWrite(pinR2,HIGH);
 else 
  digitalWrite(pinR2,LOW);

 //Bit 4
 if ((auxData & 16) == 16) 
  digitalWrite(pinL1,HIGH);
 else 
  digitalWrite(pinL1,LOW);

 //Bit 5
 if ((auxData & 32) == 32) 
  digitalWrite(pinL2,HIGH);
 else 
  digitalWrite(pinL2,LOW);
    
 digitalWrite(pinR1,HIGH);
 delay(aDelay);
 digitalWrite(pinR1,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinL2,LOW);
 delay(syncDelay); 
}
*/
