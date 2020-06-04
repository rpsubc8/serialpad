#SingleInstance, force
#include <CvJoyInterface>
;***************************
;* Upload PsExe            *
;* Autor: ackerman         *
;* Jaime Jose Gavin Sierra *
;* Keys compatible ePSXe   *
;***************************
;Boton     - PAD - Key
;Triangulo -  1  -  D
;Circulo   -  2  -  X
;X         -  3  -  Z
;Cuadrado  -  4  -  S
;L2        -  5  -  E
;R2        -  6  -  T
;L1        -  7  -  W
;R1        -  8  -  R
;Start     -  9  -  V
;Select    - 10  -  C
;Arriba    - 13  -  Up
;Derecha   - 14  -  Right
;Abajo     - 15  -  Down
;Izquierda - 16  -  Left

;Pad1L2          (1<< 0)  0
;Pad1R2          (1<< 1)  1
;Pad1L1          (1<< 2)  2
;Pad1R1          (1<< 3)  3
;Pad1tri         (1<< 4)  4
;Pad1crc         (1<< 5)  5
;Pad1x           (1<< 6)  6
;Pad1sqr         (1<< 7)  7
;Pad1Select      (1<< 8)  8
;Pad1Start       (1<<11)  9
;Pad1Up          (1<<12) 10
;Pad1Right       (1<<13) 11
;Pad1Down        (1<<14) 12
;Pad1Left        (1<<15) 13 


; Create an object from vJoy Interface Class.
global  vJoyInterface := new CvJoyInterface()

; Was vJoy installed and the DLL Loaded?
if (!vJoyInterface.vJoyEnabled()){
	; Show log of what happened
	Msgbox % vJoyInterface.LoadLibraryLog
	ExitApp
}

global myStick := vJoyInterface.Devices[1]

;****************************************************
SendByte4BtnJoy(auxData, aDelay, syncDelay)
{
 ;Envia 1 byte 4 botones con Sync 
 ;R1 L1 R2 L2  botones
 ; 3  2  1  0  bit
 ;PSX  ePSXe   Function 
 ;L2 -  btn1  - Data Bit 0,3,6
 ;R2 -  btn2  - Data Bit 1,4,7
 ;L1 -  btn3  - Data Bit 2,5,x 
 ;R1 -  btn4  - sync 
 myStick.SetBtn(0, 1)
 myStick.SetBtn(0, 2)
 myStick.SetBtn(0, 3)
 myStick.SetBtn(0, 4) 
 ;First Send 3 bits 
 myStick.SetBtn((auxData & 1),1) 
 myStick.SetBtn(((auxData & 2)>>1),2) 
 myStick.SetBtn(((auxData & 4)>>2),3)
 myStick.SetBtn(1, 4)
 Sleep, aDelay 
 myStick.SetBtn(0, 1)
 myStick.SetBtn(0, 2)
 myStick.SetBtn(0, 3)
 myStick.SetBtn(0, 4)
 Sleep, syncDelay
 
 ;Second 3 bits  
 myStick.SetBtn(((auxData & 8)>>3),1)
 myStick.SetBtn(((auxData & 16)>>4),2)
 myStick.SetBtn(((auxData & 32)>>5),3)
 myStick.SetBtn(1, 4)
 Sleep, aDelay 
 myStick.SetBtn(0, 1)
 myStick.SetBtn(0, 2)
 myStick.SetBtn(0, 3)
 myStick.SetBtn(0, 4)
 Sleep, syncDelay 
 
 ;Tercer 3 bits  
 myStick.SetBtn(((auxData & 64)>>6),1)
 myStick.SetBtn(((auxData & 128)>>7),2)
 auxCRC:= (auxData & 1)
 myStick.SetBtn((auxCRC & 1),3) ;CRC
 myStick.SetBtn(1, 4)
 Sleep, aDelay 
 myStick.SetBtn(0, 1)
 myStick.SetBtn(0, 2)
 myStick.SetBtn(0, 3)
 myStick.SetBtn(0, 4)
 Sleep, syncDelay 
}

;****************************************************
SendByte4Btn(auxData, aDelay, syncDelay)
{
 ;Envia 1 byte 4 botones con Sync 
 ;R1 L1 R2 L2
 ;PSX  ePSXe   Function 
 ;L2 -   E  - Data Bit 0,3,6
 ;R2 -   T  - Data Bit 1,4,7
 ;L1 -   W  - Data Bit 2,5,x 
 ;R1 -   R  - sync
 SendInput {R up}{E up}{T up}{W up}
 ;First Send 3 bits  
 if ((auxData & 1)=1)   
  SendInput {E down}
 else
  SendInput {E up} 
 if ((auxData & 2)=2) 
  SendInput {T down}
 else
  SendInput {T up}
 if ((auxData & 4)=4)
  SendInput {W down}
 else
  SendInput {W up}
 SendInput {R down}
 Sleep, aDelay 
 SendInput {R up}{E up}{T up}{W up}
 Sleep, syncDelay  
 
 ;Second 3 bits  
 if ((auxData & 8)=8) 
  SendInput {E down}
 else
  SendInput {E up}
 if ((auxData & 16)=16) 
  SendInput {T down}
 else
  SendInput {T up}
 if ((auxData & 32)=32)
  SendInput {W down}
 else
  SendInput {W up} 
 SendInput {R down}
 Sleep, aDelay 
 SendInput {R up}{E up}{T up}{W up}
 Sleep, syncDelay    

 ;Tercer 3 bits  
 if ((auxData & 64)=64) 
  SendInput {E down}
 else
  SendInput {E up}
 if ((auxData & 128)=128) 
  SendInput {T down}
 else
  SendInput {T up}
 auxCRC:= (auxData & 1)
 if ((auxCRC & 1)=1)
  SendInput {W down}
 else
  SendInput {W up}
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}{E up}{T up}{W up}
 Sleep, syncDelay 
}

;****************************************************
SendHeadLink4Btn(auxSize, useKeyboard, auxAddress, aDelay, aSync)
{
 ;Envia cabecera Sincronia cable 4 botones (7 bytes)
 ; 3 bytes - Size
 ; 1 byte
 ;  6 bites - Speed
 ;  2 bits  - Type compression
 ; 3 bytes - address Empieza en 0x80 
 dataSize0:= (auxSize & 0x000000FF)
 dataSize1:= (auxSize>>8) & 0x000000FF
 dataSize2:= (auxSize>>16) & 0x000000FF
 dataSpeed:= 0x0001 ;Speed 13 botones fast
 dataType:= 0x0000 ;Type no compresion 
 dataSpeedType:= dataSpeed|(dataType<<6) 
 address0:= (auxAddress & 0x000000FF)
 address1:= (auxAddress>>8) & 0x000000FF
 address2:= (auxAddress>>16) & 0x000000FF
  
 if (useKeyboard = 1)
 {
  SendByte4Btn(dataSize0, aDelay,aSync)
  SendByte4Btn(dataSize1, aDelay,aSync)
  SendByte4Btn(dataSize2, aDelay,aSync)
  SendByte4Btn(dataSpeedType, aDelay,aSync)
  SendByte4Btn(address0, aDelay,aSync)
  SendByte4Btn(address1, aDelay,aSync)
  SendByte4Btn(address2, aDelay,aSync)
 }
 else
 {
  SendByte4BtnJoy(dataSize0, aDelay,aSync)
  SendByte4BtnJoy(dataSize1, aDelay,aSync)
  SendByte4BtnJoy(dataSize2, aDelay,aSync)
  SendByte4BtnJoy(dataSpeedType, aDelay,aSync)
  SendByte4BtnJoy(address0, aDelay,aSync)
  SendByte4BtnJoy(address1, aDelay,aSync)
  SendByte4BtnJoy(address2, aDelay,aSync) 
 }
}

;****************************************************
SendBuffer4Btn(auxBuffer, useKeyboard, auxSize, aDelay, aSync)
{
 ;Envia un buffer a un mando modificado 4 botones
 auxArray := StrSplit(auxBuffer, ",")
 cont:=2
 Loop,%auxSize%
 {  
  c := auxArray[cont]+0 ;Convierto a numerico  
  if (useKeyboard = 1)  
   SendByte4Btn(c,aDelay,aSync)
  else
   SendByte4BtnJoy(c,aDelay,aSync)
  cont:= cont + 1
 }
}

;****************************************************
UpJoy14Btn()
{
 myStick.SetBtn(0, 1)
 myStick.SetBtn(0, 2)
 myStick.SetBtn(0, 3)
 myStick.SetBtn(0, 4)
 myStick.SetBtn(0, 5)
 myStick.SetBtn(0, 6)
 myStick.SetBtn(0, 7)
 myStick.SetBtn(0, 8)
 myStick.SetBtn(0, 9)
 myStick.SetBtn(0, 10)
 myStick.SetBtn(0, 11)
 myStick.SetBtn(0, 12)
 myStick.SetBtn(0, 13)
 myStick.SetBtn(0, 14)
}

;****************************************************
Send3Bytes14BtnJoy(a,b,c, aDelay, syncDelay)
{
 ;Envia 3 bytes en 2 pasadas Joystick
 ; 1 byte 4 bits Low
 ; 1 byte 4 bits High
 ;8 bits 4 bits 1 bit CRC 13 bits  14 botones
 ;8 bits 4 bits 1 bit CRC
 ;Izquierda Abajo Derecha Arriba Start Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 ;Left      Down  Right   Up     V     C         S     Z  X        D       R  W  T  E  
 UpJoy14Btn()
 ;First Send 1 byte
 myStick.SetBtn((a & 1), 1) 
 myStick.SetBtn(((a & 2)>>1), 2)
 myStick.SetBtn(((a & 4)>>2), 3)
 myStick.SetBtn(((a & 8)>>3), 4)
 myStick.SetBtn(((a & 16)>>4), 5)  
 myStick.SetBtn(((a & 32)>>5), 6)
 myStick.SetBtn(((a & 64)>>6), 7)
 myStick.SetBtn(((a & 128)>>7), 8)  
 ;First nibble Second byte
 myStick.SetBtn((b & 1), 9)
 myStick.SetBtn(((b & 2)>>1), 10)
 myStick.SetBtn(((b & 4)>>2), 11) 
 myStick.SetBtn(((b & 8)>>3), 12) 
 myStick.SetBtn(1, 14)
 Sleep, aDelay
 UpJoy14Btn() 
 Sleep, syncDelay  
 
 ;Envio tercer byte
 myStick.SetBtn((c & 1), 1) 
 myStick.SetBtn(((c & 2)>>1), 2)
 myStick.SetBtn(((c & 4)>>2), 3)
 myStick.SetBtn(((c & 8)>>3), 4)
 myStick.SetBtn(((c & 16)>>4), 5)  
 myStick.SetBtn(((c & 32)>>5), 6)
 myStick.SetBtn(((c & 64)>>6), 7)
 myStick.SetBtn(((c & 128)>>7), 8)   
 ;Segundo nibble Segundo byte 
 myStick.SetBtn(((b & 16)>>4), 9)
 myStick.SetBtn(((b & 32)>>5), 10)
 myStick.SetBtn(((b & 64)>>6), 11) 
 myStick.SetBtn(((b & 128)>>7), 12) 
 myStick.SetBtn(1, 14)
 Sleep, aDelay
 UpJoy14Btn() 
 Sleep, syncDelay   
}

;****************************************************
Send3Bytes14Btn(a,b,c, aDelay, syncDelay)
{
 ;Envia 3 bytes en 2 pasadas
 ; 1 byte 4 bits Low
 ; 1 byte 4 bits High
 ;8 bits 4 bits 1 bit CRC 13 bits  14 botones
 ;8 bits 4 bits 1 bit CRC
 ;Izquierda Abajo Derecha Arriba Start Select Cuadrado X Circulo Triangulo R1 L1 R2 L2 
 ;Left      Down  Right   Up     V     C         S     Z  X        D       R  W  T  E  
 
 ;Pad1L2          (1<< 0)  0
 ;Pad1R2          (1<< 1)  1
 ;Pad1L1          (1<< 2)  2
 ;Pad1R1          (1<< 3)  3
 ;Pad1tri         (1<< 4)  4
 ;Pad1crc         (1<< 5)  5
 ;Pad1x           (1<< 6)  6
 ;Pad1sqr         (1<< 7)  7
 ;Pad1Select      (1<< 8)  8
 ;Pad1Start       (1<<11)  9
 ;Pad1Up          (1<<12) 10
 ;Pad1Right       (1<<13) 11
 ;Pad1Down        (1<<14) 12
 ;Pad1Left        (1<<15) 13
 SendInput {Left up}{Down up}{Right up}{Up up}{V up}{C up}{S up}{Z up}{X up}{D up}{R up}{W up}{T up}{E up}
 
 ;First Send 1 byte
 if ((a & 1)=1)
  SendInput {E down}
 else
  SendInput {E up}
 if ((a & 2)=2)
  SendInput {T down}
 else
  SendInput {T up}  
 if ((a & 4)=4)
  SendInput {W down}
 else
  SendInput {W up}  
 if ((a & 8)=8)
  SendInput {R down}
 else
  SendInput {R up}  
 if ((a & 16)=16)
  SendInput {D down}
 else
  SendInput {D up}    
 if ((a & 32)=32)
  SendInput {X down}
 else
  SendInput {X up}    
 if ((a & 64)=64)
  SendInput {Z down}
 else
  SendInput {Z  up}    
 if ((a & 128)=128)
  SendInput {S down}
 else
  SendInput {S up}  
  
 ;First nibble Second byte
 if ((b & 1)=1)
  SendInput {C down}
 else
  SendInput {C up} 
 if ((b & 2)=2)
  SendInput {V down}
 else
  SendInput {V up}  
 if ((b & 4)=4)
  SendInput {Up down}
 else
  SendInput {Up up}    
 if ((b & 8)=8)
  SendInput {Right down}
 else
  SendInput {Right up}    
 
 SendInput {Left down}
 Sleep, aDelay 
 SendInput {Left up}{Down up}{Right up}{Up up}{V up}{C up}{S up}{Z up}{X up}{D up}{R up}{W up}{T up}{E up}
 Sleep, syncDelay  
 
 ;Envio tercer byte
 if ((c & 1)=1)
  SendInput {E down}
 else
  SendInput {E up}
 if ((c & 2)=2)
  SendInput {T down}
 else
  SendInput {T up}  
 if ((c & 4)=4)
  SendInput {W down}
 else
  SendInput {W up}  
 if ((c & 8)=8)
  SendInput {R down}
 else
  SendInput {R up}  
 if ((c & 16)=16)
  SendInput {D down}
 else
  SendInput {D up}    
 if ((c & 32)=32)
  SendInput {X down}
 else
  SendInput {X up}    
 if ((c & 64)=64)
  SendInput {Z down}
 else
  SendInput {Z  up}    
 if ((c & 128)=128)
  SendInput {S down}
 else
  SendInput {S up}
  
 ;Segundo nibble Segundo byte
 if ((b & 16)=16)
  SendInput {C down}
 else
  SendInput {C up} 
 if ((b & 32)=32)
  SendInput {V down}
 else
  SendInput {V up}  
 if ((b & 64)=64)
  SendInput {Up down}
 else
  SendInput {Up up}    
 if ((b & 128)=128)
  SendInput {Right down}
 else
  SendInput {Right up}   

 SendInput {Left down}
 Sleep, aDelay 
 SendInput {Left up}{Down up}{Right up}{Up up}{V up}{C up}{S up}{Z up}{X up}{D up}{R up}{W up}{T up}{E up}
 Sleep, syncDelay    
}

;****************************************************
SendBuffer14Btn(auxBuffer, useKeyboard, auxSize, aDelay, aSync)
{
 ;Envia un buffer a un mando Fake spi con 14 botones
 auxArray := StrSplit(auxBuffer, ",") 
 resto:= Mod(auxSize,3)
 if (resto != 0)
 {
  resto:= 3-resto
  auxSize:= auxSize + resto
  Loop,%resto%
  {
   auxArray.Push(0x00)
  }     
 }
 auxSize := auxSize/3
 cont:= 2
 Loop,%auxSize%
 {  
  c := auxArray[cont]+0 ;Convierto a numerico  
  d := auxArray[cont+1]+0
  e := auxArray[cont+2]+0
  if (useKeyboard = 1)  
   Send3Bytes14Btn(c,d,e,aDelay,aSync)
  else
   Send3Bytes14BtnJoy(c,d,e,aDelay,aSync)
  cont:= cont + 3
 }
}

;**********************
;* Programa principal *
;**********************

;FileRead, Contents, blade.txt
FileRead, Contents, demo1.txt
if not ErrorLevel  ; Successfully loaded.
{
 Sleep ,4000
 StartTime := A_TickCount
 

 
 ;Envio 4 botones 1 mando
 ;SendHeadLink4Btn(20428,0,0x80010000,25,25) ;No usar teclado
 ;SendBuffer4Btn(Contents,0,20428,25,25) ;No usar teclado
 
 ;Envio 14 botones 1 mando
 SendHeadLink4Btn(20428,0,0x80010000,25,25) ;No usar teclado
 SendBuffer14Btn(Contents,0,20428,25,25) ;No usar teclado
 ElapsedTime := A_TickCount - StartTime
 sec := Round(ElapsedTime/1000)
 ;MsgBox,  %ElapsedTime% milis %sec% sec 
 Contents := ""  ; Free the memory. 
}
 