//**************************
//* Author: Ackerman       *
//* Controla 4 botones PAD *
//* Recibe datos por usb   *
//**************************
//9600 baudios 5000 delay en linea
//En PSX
//Pad1L2 (1<< 0)
//Pad1R2 (1<< 1)
//Pad1L1 (1<< 2)
//Pad1R1 (1<< 3)

#define pinL1 5
#define pinL2 6
#define pinR1 7
#define pinR2 8
#define gbDelay 25

#define max_buf 256
uint8_t gb_aux=0;
uint8_t gb_cont_buf=0;
uint8_t gb_buf[max_buf];
uint8_t gbUseFlag=0;


void HexTo4Btn(unsigned char a);
uint8_t CharHexToDec(char a);
//void PollPad(void);

//*************************************
uint8_t CharHexToDec(char a)
{
 uint8_t aReturn=0;
 if (a>=97) aReturn= a-87; //ABCDEF
 else if (a>=65) aReturn= a-55; //abcdef
  else if (a>=48) aReturn=a-48; //0123456789
 return aReturn;
}

//*************************************
void PADHexTo4Btn(unsigned char a)
{ 
 digitalWrite(pinL2,(a&0x01));
 digitalWrite(pinR2,((a>>1)&0x01));
 digitalWrite(pinL1,((a>>2)&0x01));
 digitalWrite(pinR1,((a>>3)&0x01));
}


//***************************************
void setup()
{
 pinMode(pinL2, OUTPUT);
 pinMode(pinR2, OUTPUT);
 pinMode(pinL1, OUTPUT);
 pinMode(pinR1, OUTPUT);

 //Serial.begin(38400);
 //Serial.begin(9600);
 //Serial.begin(115200);
 Serial.begin (9600);
 
 digitalWrite(pinL2,LOW);
 digitalWrite(pinR2,LOW);
 digitalWrite(pinL1,LOW);
 digitalWrite(pinR1,LOW);  
}

void loop() 
{ 
 if(Serial.available()>=1)
 {  
  gb_aux = Serial.read();
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13 && gb_aux!=42 && gb_aux!=35)
  {
   gb_buf[gb_cont_buf++] = (CharHexToDec(gb_aux)&0x0F);   
  }
 } 
 if ((gb_aux==42) || (gb_aux==35))
 {//El asterisco 42 almohadilla 35
  gb_cont_buf=0; //Vaciamos buffer serie
  PADHexTo4Btn(0); //Resetemos botones mando
  Serial.end(); 
  Serial.begin(9600);
  switch (gb_aux)
  {
   case 42: gbUseFlag=1; break;
   case 35: gbUseFlag=0; break;
   default: gbUseFlag=1; break;
  } 
  gb_aux=0;
 }
 else
 {
  if (gb_aux==10)
  {   
   gb_aux=0;
   if (gbUseFlag == 1)
   {//Hya flag 50 ms
    for (uint8_t i=0;i<gb_cont_buf;i++)
    {     
     PADHexTo4Btn(gb_buf[i]);
     for (uint8_t j=0;j<gbDelay;j++) delayMicroseconds(1000);     
     PADHexTo4Btn(0);
     for (uint8_t j=0;j<gbDelay;j++) delayMicroseconds(1000);     
    }    
   }
   else
   {//No hay flag 25 ms
    for (uint8_t i=0;i<gb_cont_buf;i++)
    {          
     PADHexTo4Btn(gb_buf[i]);
     for (uint8_t j=0;j<gbDelay;j++) delayMicroseconds(1000);
    }        
   }
   gb_cont_buf = 0;
  }
 }
}

/*void loop() 
{ 
 while(Serial.available()>=1)
 {
  gb_aux = Serial.read();
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13)
  {
   PADHexTo4Btn(CharHexToDec(gb_aux));
   delay(aDelay);
   //PADHexTo4Btn(0);
   //delay(aDelay);   
  }
 }
}*/

/*void loop()
{
 while(Serial.available()>=1)
 {
  gb_aux = Serial.read();   
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13 && gb_aux!=42)  
   gb_buf[gb_cont_buf++]=gb_aux;
  else
   break; 
 }
 
 if (gb_aux==42)
 {
  gb_cont_buf=0;  
  gb_aux=0;
 }
 else
 {
  if (gb_aux==10)
  {
   gb_aux = 0;
   //Serial.print(gb_cont_buf);
   //Serial.print(" ");
   for (uint8_t i=0;i<gb_cont_buf;i++)
   {    
    PADHexTo4Btn(CharHexToDec(gb_buf[i]));
    delay(aDelay);
    PADHexTo4Btn(0);
    delay(aDelay);    
    //Serial.print(auxChr);
    //Serial.print("|");
   }
   gb_cont_buf=0;
  }
 }
}
*/

/*Funciona
void loop() 
{ 
 while(Serial.available()>=1)
 {
  gb_aux = Serial.read();
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13)
  {
   PADHexTo4Btn(CharHexToDec(gb_aux));
   delay(aDelay);
   PADHexTo4Btn(0);
   delay(aDelay);
  }
 }
}*/


/*void loop()
{
 while(Serial.available()>=1)
 {
  gb_aux = Serial.read();   
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13 && gb_aux!=42)  
   gb_buf[gb_cont_buf++]=gb_aux;
 }
 if (gb_aux==42)
 {
  gb_cont_buf=0;
 }
 else
 {
  if (gb_aux==10)
  {
   gb_aux = 0;
   //Serial.print(gb_cont_buf);
   //Serial.print(" ");
   for (uint8_t i=0;i<(gb_cont_buf>>1);i++)
   {
    uint8_t auxLow = CharHexToDec(gb_buf[(i<<1)+1]);
    uint8_t auxHigh= CharHexToDec(gb_buf[(i<<1)]);
    unsigned char auxByte= ((auxHigh<<4)|auxLow);   
    PADHexTo4Btn((auxByte&0x7)|0x08);
    delay(aDelay);
    PADHexTo4Btn(0);
    delay(aDelay);
    PADHexTo4Btn(((auxByte>>3)&0x7)|0x08);
    delay(aDelay);
    PADHexTo4Btn(0);
    delay(aDelay);
    PADHexTo4Btn(((auxByte>>6)&0x3)|0x08);
    delay(aDelay);
    PADHexTo4Btn(0);
    delay(aDelay);
    //Serial.print(auxByte,HEX);
    //Serial.print("|");
   }
   gb_cont_buf=0;
  }
 }

   //Serial.print(gb_aux);
  //Serial.print("|");
 //PADHexTo4Btn(CharHexToDec(gb_aux));
}
*/
//***************************************
/*
//Funciona poniendo delay en el realterm
void loop() 
{ 
 while(Serial.available()>=1)
 {
  gb_aux = Serial.read();
  if (gb_aux!=0 && gb_aux!=10 && gb_aux!=32 && gb_aux!=13)
  {
   PADHexTo4Btn(CharHexToDec(gb_aux));
   delay(aDelay);
   PADHexTo4Btn(0);
   delay(aDelay);
  }
 }
}
*/





//Sobra
   //gb_buf[gb_cont_buf] = gb_aux;   
   //gb_cont_buf ++;
   //char kk=gb_aux;
   //Serial.print(kk);
   //Serial.print("|");
 /*if (gb_cont_buf>=60)
 {
  Serial.print(gb_cont_buf);
  Serial.print(" ");
  for (uint8_t i=0;i<gb_cont_buf;i++)
  {    
    uint8_t c = CharHexToDec(gb_buf[i]);
    //PADHexTo4Btn(CharHexToDec(gb_buf[i]));
    delay(aDelay);
    //PADHexTo4Btn(0);
    delay(aDelay);
    char kk=gb_buf[i];
    Serial.print(kk);
    //Serial.print(gb_buf[i]);
    //Serial.print("|");
    //PADHexTo4Btn(gb_buf[i]);
    //delay(aDelay);
    //PADHexTo4Btn(0);
    //delay(aDelay);      
  }
  gb_cont_buf=0;
 }*/
    //Serial.print(incoming);
    //Serial.print("|");
    //Serial.print(decimal);
    //Serial.print("|");
    
