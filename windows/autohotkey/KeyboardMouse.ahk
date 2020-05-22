;***************************
;* PSX MOUSE KEYBOARD      *
;* Autor: ackerman         *
;* Jaime Jose Gavin Sierra *
;* Keys compatible ePSXe   *
;***************************
;Button    - Key - Bit
;L1        - W   - (sync)
;L2        - E   - Q7
;R1        - R   - Q6
;R2        - T   - Q5
;Triangulo - D   - Q4
;Cuadrado  - S   - Q3
;Circulo   - X   - Q2
;X         - Z   - Q1
;Start     - V   - Q0


;************************************************
SendDataMouseFourBtnNoStrobe(xpos,xposAntes, ypos, yposAntes, mLeft, mRight, aDelay)
{
 ;2 bits X 
 ; 00 quieto
 ; 01 derecha
 ; 11 izquierda
 ; 10 Btn 0
 ;2 bits Y
 ; 00 quieto
 ; 01 derecha
 ; 11 izquierda
 ; 10 Btn 1  
 ;Send 4 bits Mouse R1, R2, L1, L2   R,T,W,E bit data ePSXe compatible
 auxData:= 0 
 auxData2:= 0
 
 if (xpos < xposAntes)
 {
  ;Izquierda
  MouseBit0:= 1
  MouseBit1:= 1
 }
 else
 {
  if (xpos > xposAntes)
  {
   ;Derecha
   MouseBit0:= 1
   MouseBit1:= 0
  }
  else
  {
   ;Quieto
   MouseBit0:= 0
   MouseBit1:= 0  
  }
 }
 
 if (ypos < yposAntes)
 {
  MouseBit2:= 1
  MouseBit3:= 0
 }
 else
 {
  if (ypos > yposAntes)
  {
   MouseBit2:= 1
   MouseBit3:= 1
  }
  else
  {
   MouseBit2:= 0
   MouseBit3:= 0  
  }
 } 
 ;Botones
 if (mLeft = true) 
 {
  if (MouseBit0 = 0 && MouseBit1 = 0)
  {
   MouseBit0:= 0
   MouseBit1:= 1
  }
  else
  {
   auxData2:= 0 | (1<<1)
  }
 }
 
 if (mRight = true)
 {
  if (MouseBit2 = 0 && MouseBit3 = 0)
  {
   MouseBit2:= 0
   MouseBit3:= 1
  }
  else
  {
   auxData2:= (0<<2) | (1<<3)
  }
 }
        
 auxData:= MouseBit0 | (MouseBit1<<1) | (MouseBit2<<2) | (MouseBit3<<3)
 
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up}
 ;Bit 0
 if ((auxData & 1)=1) 
  SendInput {R down}
 else 
  SendInput {R up}
  
 ;Bit 1
 if ((auxData & 2)=2) 
  SendInput {T down}
 else 
  SendInput {T up}  
  
 ;Bit 2
 if ((auxData & 4)=4) 
  SendInput {W down}
 else 
  SendInput {W up}    

 ;Bit 3
 if ((auxData & 8)=8) 
  SendInput {E down}
 else 
  SendInput {E up}      
      
 Sleep, aDelay
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up}

 ;Si botones y movimiento al mismo tiempo, se envia 2 nibbles
 if (auxData2 !=0) 
 {
  auxData = auxData2;
  ;Bit 0
  if ((auxData & 1)=1) 
   SendInput {R down}
  else 
   SendInput {R up}
   
  ;Bit 1
  if ((auxData & 2)=2) 
   SendInput {T down}
  else 
   SendInput {T up}  
  
  ;Bit 2
  if ((auxData & 4)=4) 
   SendInput {W down}
  else 
   SendInput {W up}    

  ;Bit 3
  if ((auxData & 8)=8) 
   SendInput {E down}
  else 
   SendInput {E up}      
      
  Sleep, aDelay
  SendInput {R up}
  SendInput {T up}
  SendInput {W up}
  SendInput {E up} 
 }
}

;************************************************
SendDataMouseFourBtn(xpos,xposAntes, ypos, yposAntes, mLeft, mRight, aDelay, syncDelay)
{ 
 ;PSX  ePSXe   Function 
 ;R1 -   R   - sync
 ;R2 -   T   - Data Bit 0,3
 ;L1 -   W   - Data Bit 1,4
 ;L2 -   E   - Data Bit 2,5
 ;2 bits X 
 ; 00 quieto
 ; 01 derecha
 ; 11 izquierda
 ; 10 movimiento rapido
 ;2 bits Y
 ;2 bits buttons
 ;Send 6 bits Mouse R1, R2, L1, L2   R sync T,W,E bit data ePSXe compatible
 auxData:= 0 
 
 if (xpos < xposAntes)
 {
  ;Izquierda
  MouseBit0:= 1
  MouseBit1:= 1
 }
 else
 {
  if (xpos > xposAntes)
  {
   ;Derecha
   MouseBit0:= 1
   MouseBit1:= 0
  }
  else
  {
   ;Quieto
   MouseBit0:= 0
   MouseBit1:= 0  
  }
 }
 
 if (ypos < yposAntes)
 {
  MouseBit2:= 1
  MouseBit3:= 1
 }
 else
 {
  if (ypos > yposAntes)
  {
   MouseBit2:= 1
   MouseBit3:= 0
  }
  else
  {
   MouseBit2:= 0
   MouseBit3:= 0  
  }
 } 
  
 if (mLeft = true)
 {
  MouseBit4:= 1
 }
 else
 {
  MouseBit4:= 0
 }
 
 if (mRight = true)
 {
  MouseBit5:= 1
 }
 else
 {
  MouseBit5:= 0
 } 
    
  
 auxData:= MouseBit0 | (MouseBit1<<1) | (MouseBit2<<2) | (MouseBit3<<3) | (MouseBit4<<4) | (MouseBit5<<5)
   
 ;MsgBox %auxData%
 
 if (auxData = 0)
  return
    
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay

 ;Bit 0 First Send 3 bits
 if ((auxData & 1)=1) 
  SendInput {T down}
 else 
  SendInput {T up}
  
 ;Bit 1
 if ((auxData & 2)=2) 
  SendInput {W down}
 else 
  SendInput {W up}  
  
 ;Bit 2
 if ((auxData & 4)=4) 
  SendInput {E down}
 else 
  SendInput {E up}    
     
 SendInput {R down} 
 Sleep, aDelay  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay
 

 ;Bit 3 Second Send 3 bits 
 if ((auxData & 8)=8) 
  SendInput {T down}
 else 
  SendInput {T up} 

 ;Bit 4
 if ((auxData & 16)=16) 
  SendInput {W down}
 else 
  SendInput {W up}    
  
 ;Bit 5
 if ((auxData & 32)=32) 
  SendInput {E down}
 else 
  SendInput {E up}

 SendInput {R down} 
 Sleep, aDelay  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay    
}


;************************************************
SendDataKEYBOARDFourBtnNoStrobe(dato, aDelay)
{
 ;PSX  ePSXe   Function 
 ;R1 -   R   - Data Bit 0,4
 ;R2 -   T   - Data Bit 1,5
 ;L1 -   W   - Data Bit 2,6
 ;L2 -   E   - Data Bit 3,7
 ;Send 8 bits R1, R2, L1, L2   R,T,W,E bit data ePSXe compatible fast NO STROBE
 ;Todos los nibbles tienen un 1
 ; Bit7 a 1 Parte alta que es el Bit3
 ; Bit7 a 0 Parte baja
 auxData:= -1

 Switch dato
 {
  Case "A": auxData:= 129 ;1000 0001 0x81 129 A
  Case "B": auxData:= 130 ;1000 0010 0x82 130 B
  Case "C": auxData:= 131 ;1000 0011 0x83 131 C
  Case "D": auxData:= 132 ;1000 0100 0x84 132 D
  Case "E": auxData:= 133 ;1000 0101 0x85 133 E
  Case "F": auxData:= 134 ;1000 0110 0x86 134 F
  Case "G": auxData:= 135 ;1000 0111 0x87 135 G
  Case "H": auxData:= 145 ;1001 0001 0x91 145 H
  Case "I": auxData:= 146 ;1001 0010 0x92 146 I
  Case "J": auxData:= 147 ;1001 0011 0x93 147 J
  Case "K": auxData:= 148 ;1001 0100 0x94 148 K
  Case "L": auxData:= 149 ;1001 0101 0x95 149 L
  Case "M": auxData:= 150 ;1001 0110 0x96 150 M
  Case "N": auxData:= 151 ;1001 0111 0x97 151 N
  Case "Ñ": auxData:= 161 ;1010 0001 0xA1 161 Ñ
  Case "O": auxData:= 162 ;1010 0010 0xA2 162 O
  Case "P": auxData:= 163 ;1010 0011 0xA3 163 P
  Case "Q": auxData:= 164 ;1010 0100 0xA4 164 Q
  Case "R": auxData:= 165 ;1010 0101 0xA5 165 R
  Case "S": auxData:= 166 ;1010 0110 0xA6 166 S
  Case "T": auxData:= 167 ;1010 0111 0xA7 167 T
  Case "U": auxData:= 177 ;1011 0001 0xB1 177 U
  Case "V": auxData:= 178 ;1011 0010 0xB2 178 V
  Case "W": auxData:= 179 ;1011 0011 0xB3 179 W
  Case "X": auxData:= 180 ;1011 0100 0xB4 180 X
  Case "Y": auxData:= 181 ;1011 0101 0xB5 181 Y
  Case "Z": auxData:= 182 ;1011 0110 0xB6 182 Z  
  Case " ": auxData:= 183 ;1011 0111 0xB7 183  
  Case "0": auxData:= 193 ;1100 0001 0xC1 193 0 
  Case "1": auxData:= 194 ;1100 0010 0xC2 194 1 
  Case "2": auxData:= 195 ;1100 0011 0xC3 195 2  
  Case "3": auxData:= 196 ;1100 0100 0xC4 196 3
  Case "4": auxData:= 197 ;1100 0101 0xC5 197 4
  Case "5": auxData:= 198 ;1100 0110 0xC6 198 5 
  Case "6": auxData:= 199 ;1100 0111 0xC7 199 6  
  Case "7": auxData:= 209 ;1101 0001 0xD1 209 7 
  Case "8": auxData:= 210 ;1101 0010 0xD2 210 8 
  Case "9": auxData:= 211 ;1101 0011 0xD3 211 9
  Case ".": auxData:= 212 ;1101 0100 0xD4 212 .
  Case ",": auxData:= 213 ;1101 0101 0xD5 213 ,
  Case "-": auxData:= 214 ;1101 0110 0xD6 214 -
  Case "+": auxData:= 215 ;1101 0111 0xD7 215 +      
  
  Case "*": auxData:= 225 ;1110 0001 0xE1 225 *
  Case "/": auxData:= 226 ;1110 0010 0xE2 226 /  
 ;Enter Borrar Ctrl Alt Mayusculas Tab Shift Ins Supr ESC Break F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12
 ;Left Right Up Down []()&%$·"!^¨Ç:;_{}^ 
  
  Default: auxData:= -1
 }
   
 if (auxData = -1)
  return
  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up}

 ;Bit 0 First Send 4 bits
 if ((auxData & 1)=1) 
  SendInput {R down}
 else 
  SendInput {R up}
  
 ;Bit 1
 if ((auxData & 2)=2) 
  SendInput {T down}
 else 
  SendInput {T up}  
  
 ;Bit 2
 if ((auxData & 4)=4) 
  SendInput {W down}
 else 
  SendInput {W up}    
  
 ;Bit 3
 if ((auxData & 8)=8) 
  SendInput {E down}
 else 
  SendInput {E up}      
         
 Sleep, aDelay  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up}  
 
 
 ;Bit 4 Second Send 4 bits
 if ((auxData & 16)=16) 
  SendInput {R down}
 else 
  SendInput {R up}
  
 ;Bit 5
 if ((auxData & 32)=32) 
  SendInput {T down}
 else 
  SendInput {T up}  
  
 ;Bit 6
 if ((auxData & 64)=64) 
  SendInput {W down}
 else 
  SendInput {W up}    
  
 ;Bit 7
 if ((auxData & 128)=128) 
  SendInput {E down}
 else 
  SendInput {E up}       
 
 Sleep, aDelay
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
}


;************************************************
SendDataKEYBOARDFourBtn(dato, aDelay, syncDelay)
{ 
 ;PSX  ePSXe   Function 
 ;R1 -   R   - sync
 ;R2 -   T   - Data Bit 0,3
 ;L1 -   W   - Data Bit 1,4
 ;L2 -   E   - Data Bit 2,5
 ;Send 6 bits R1, R2, L1, L2   R sync T,W,E bit data ePSXe compatible
 auxData:= -1
 Switch dato
 {
  Case "A": auxData:= 0
  Case "B": auxData:= 1
  Case "C": auxData:= 2
  Case "D": auxData:= 3
  Case "E": auxData:= 4
  Case "F": auxData:= 5
  Case "G": auxData:= 6
  Case "H": auxData:= 7
  Case "I": auxData:= 8
  Case "J": auxData:= 9
  Case "K": auxData:= 10
  Case "L": auxData:= 11
  Case "M": auxData:= 12
  Case "N": auxData:= 13
  Case "Ñ": auxData:= 14
  Case "O": auxData:= 15
  Case "P": auxData:= 16
  Case "Q": auxData:= 17
  Case "R": auxData:= 18
  Case "S": auxData:= 19
  Case "T": auxData:= 20
  Case "U": auxData:= 21
  Case "V": auxData:= 22
  Case "W": auxData:= 23
  Case "X": auxData:= 24
  Case "Y": auxData:= 25
  Case "Z": auxData:= 26
  Case ".": auxData:= 27
  Case ",": auxData:= 28
  Case ":": auxData:= 29
  Case "[": auxData:= 30
  Case " ": auxData:= 31 
  Default: auxData:= -1
 }
 if (auxData = -1)
  return
    
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay

 ;Bit 0 First Send 3 bits
 if ((auxData & 1)=1) 
  SendInput {T down}
 else 
  SendInput {T up}
  
 ;Bit 1
 if ((auxData & 2)=2) 
  SendInput {W down}
 else 
  SendInput {W up}  
  
 ;Bit 2
 if ((auxData & 4)=4) 
  SendInput {E down}
 else 
  SendInput {E up}    
     
 SendInput {R down} 
 Sleep, aDelay  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay
 

 ;Bit 3 Second Send 3 bits 
 if ((auxData & 8)=8) 
  SendInput {T down}
 else 
  SendInput {T up} 

 ;Bit 4
 if ((auxData & 16)=16) 
  SendInput {W down}
 else 
  SendInput {W up}    
  
 ;Bit 5
 if ((auxData & 32)=32) 
  SendInput {E down}
 else 
  SendInput {E up}

 SendInput {R down} 
 Sleep, aDelay  
 SendInput {R up}
 SendInput {T up}
 SendInput {W up}
 SendInput {E up} 
 Sleep, syncDelay    
}


;************************************************
SendDataKEYBOARDTwoBtn(dato, aDelay, syncDelay)
{
 ;PSX  ePSXe   Function 
 ;R1 -   R   - sync
 ;R2 -   T   - Data
 ;Send 5 bits BAUDOT R1, R2, R sync T bit data ePSXe compatible
 auxData:= -1
 Switch dato
 {
  Case "A": auxData:= 0
  Case "B": auxData:= 1
  Case "C": auxData:= 2
  Case "D": auxData:= 3
  Case "E": auxData:= 4
  Case "F": auxData:= 5
  Case "G": auxData:= 6
  Case "H": auxData:= 7
  Case "I": auxData:= 8
  Case "J": auxData := 9
  Case "K": auxData := 10
  Case "L": auxData:= 11
  Case "M": auxData := 12
  Case "N": auxData := 13
  Case "Ñ": auxData := 14
  Case "O": auxData := 15
  Case "P": auxData := 16
  Case "Q": auxData := 17
  Case "R": auxData := 18
  Case "S": auxData := 19
  Case "T": auxData := 20
  Case "U": auxData := 21
  Case "V": auxData := 22
  Case "W": auxData := 23
  Case "X": auxData := 24
  Case "Y": auxData := 25
  Case "Z": auxData := 26
  Case ".": auxData := 27
  Case ",": auxData := 28
  Case ":": auxData := 29
  Case "[": auxData := 30
  Case " ": auxData := 31
  Default: auxData := -1
 }
 if (auxData = -1)
  return
  
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay
 
 ;Bit 0 
 if ((auxData & 1)=1) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay 
 
 ;Bit 1
 if ((auxData & 2)=2)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay 

 ;Bit 2
 if ((auxData & 4)=4)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay  

 ;Bit 3
 if ((auxData & 8)=8)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay  
 
 ;Bit 4
 if ((auxData & 16)=16)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay  
}

;************************************************
SendDataPADTwoBtn(dato, aDelay, syncDelay)
{
 ;PSX  ePSXe   Function 
 ;R1 -   R   - sync
 ;R2 -   T   - Data
 ;Send 14 buttons state 4 bits R1, R2, R sync T bit data ePSXe compatible
 ;Button   Key     Dec
 ;Select    C       0
 ;Start     V       1 
 ;L2        E       2
 ;L1        W       3
 ;UP        Up      4
 ;LEFT      Left    5
 ;DOWN      Down    6
 ;RIGHT     Right   7
 ;R2        T       8
 ;R1        R       9
 ;Triangle  D      10
 ;Square    S      11
 ;Circle    X      12
 ;X         Z      13
 auxData:= -1
 Switch dato
 { 
  Case "C": auxData := 0
  Case "V": auxData := 1
  Case "E": auxData := 2
  Case "W": auxData := 3
  Case "UP": auxData := 4
  Case "LEFT": auxData := 5
  Case "DOWN": auxData := 6
  Case "RIGHT": auxData := 7
  Case "T": auxData := 8
  Case "R": auxData := 9
  Case "D": auxData := 10
  Case "S": auxData := 11
  Case "X": auxData := 12
  Case "Z": auxData := 13  
  Default: auxData := -1
 }
 
 if (auxData = -1)
  return
 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay

 ;Bit 0 
 if ((auxData & 1)=1) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay 
 
 ;Bit 1
 if ((auxData & 2)=2)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay 
 
 ;Bit 2
 if ((auxData & 4)=4) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay

 ;Bit 3
 if ((auxData & 8)=8) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, syncDelay  
}


;************************************************
SendDataTwoBtn(dato, aDelay)
{
 ;Only 2 buttons R1 and R2, R (sync) T (data) Send 8 bits
 ;Send bit 0
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay
 
 ;Bit 0 
 if ((dato & 1)=1) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay 
 
 ;Bit 1
 if ((dato & 2)=2)
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay 
 
 ;Bit 2
 if ((dato & 4)=4) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay

 ;Bit 3
 if ((dato & 8)=8) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay 

 ;Bit 4
 if ((dato & 16)=16) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay
 
 ;Bit 5
 if ((dato & 32)=32) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay

 ;Bit 6
 if ((dato & 64)=64) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay 

 ;Bit 3
 if ((dato & 128)=128) 
  SendInput {T down}
 else 
  SendInput {T up} 
 SendInput {R down} 
 Sleep, aDelay 
 SendInput {R up}
 SendInput {T up}
 Sleep, aDelay  
}

;************************************************
SendData(dato)
{
 ;Send data push buttons gamepad
 if ((dato & 1)=1) 
  SendInput {V down}
 else
  SendInput {V up}
 if ((dato & 2)=2) 
  SendInput {Z down}
 else
  SendInput {Z up} 
 if ((dato & 4)=4) 
  SendInput {X down}
 else
  SendInput {X up} 
 if ((dato & 8)=8) 
  SendInput {S down}
 else
  SendInput {S up}  
 if ((dato & 16)=16) 
  SendInput {D down}
 else
  SendInput {D up}  
 if ((dato & 32)=32) 
  SendInput {T down}
 else
  SendInput {T up}  
 if ((dato & 64)=64) 
  SendInput {R down}
 else
  SendInput {R up}  
 if ((dato & 128)=128) 
  SendInput {E down}
 else
  SendInput {E up}  
 SendInput {W down}
 Sleep, 50

 SendInput {W up}
 Sleep, 50
}

;************************************************
SendFileTwoBtn(fileName,buffer)
{
 ;Send file with only 2 btn  R1 R2
 ;FF - CMD
 ;02 - Send file
 ;xx - Size file
 ;bytes
 
 SendDataTwoBtn(255,50)
 SendDataTwoBtn(2,50)
 SendDataTwoBtn(100,50)
 SendDataTwoBtn(0,50) 
 cont=1
 ;Loop,100
 {
  dato:= buffer[cont]
  cont:=cont+1
  if (cont>10)
  cont:=1
  SendDataTwoBtn(dato, 50)
 }  
}

;************************************************
SendFile(fileName,buffer)
{
 ;FF - CMD
 ;02 - Send file
 ;xx - Size file
 ;bytes
    
 SendData(255)
 SendData(2)
 ;0x0064 100
 SendData(100)
 SendData(0)
 cont=1 
 Loop,100
 {
  dato:= buffer[cont]
  cont:=cont+1
  if (cont>10)
  cont:=1
  SendData(dato)
 }
}

;**********************
;* Programa principal *
;**********************
buffer:= Object()
buffer:= [48,49,50,51,52,53,54,55,56,57]


;Loop,20
;SendData(4)

;Sleep,2000

;SendFile("prueba.txt",buffer)
;SendFileTwoBtn("prueba.txt",buffer)
OutputVar =""

;Teclado
/*
Loop
{ 
 OutputVar =""
 Input, OutputVar, L1, {Left}{Right}{Up}{Down}
 ;MsgBox, %OutputVar%  
 StringUpper, OutputVar, OutputVar
 if (OutputVar = "Q")
  ExitApp
 ;SendDataPADTwoBtn(OutputVar,50, 50)
 ;SendDataKEYBOARDTwoBtn(OutputVar,50, 50)
 ;SendDataKEYBOARDFourBtn(OutputVar,50, 50)
 SendDataKEYBOARDFourBtnNoStrobe(OutputVar,50)
 ;Vale hasta 25 10 ms
}

*/


;Raton
MouseGetPos, xposAntes, yposAntes
mouseLeftAntes := GetKeyState("LButton")
mouseRightAntes := GetKeyState("RButton")
Loop
{
 MouseGetPos, xpos, ypos 
 mouseLeft := GetKeyState("LButton")
 mouseRight := GetKeyState("RButton") 
 ;SendDataMouseFourBtn(xpos,xposAntes,ypos,yposAntes,mouseLeft,mouseRight,50, 50) 
 if ((xpos != xposAntes) || (ypos != yposAntes) || (mouseLeft != mouseLeftAntes) || (mouseRight != mouseRightAntes))
 {
  SendDataMouseFourBtnNoStrobe(xpos,xposAntes,ypos,yposAntes,mouseLeft,mouseRight,50) 
 }
 if (xpos != xposAntes)
 {
  xposAntes:= xpos
 }
 if (ypos != yposAntes)
 {
  yposAntes:= ypos
 }
 if (mouseLeft != mouseLeftAntes)
 {
   mouseLeftAntes:= mouseLeft
 }
 if (mouseRight != mouseRightAntes)
 {
   mouseRightAntes:= mouseRight
 }
}


ExitApp


 ;MsgBox, %OutputVar% 
 ;SendDataPADTwoBtn("V",50)
 ;MsgBox, "Fin"

 ;SendInput {E up}
 ;SendInput {R up}  
 ;SendInput {T up}  
 ;SendInput {D up}  
 ;SendInput {S up}  
 ;SendInput {X up}  
 ;SendInput {Z up}  
 ;SendInput {V up}   
; ;01000001 
; SendInput {V down}
; SendInput {R down} 
; SendInput {W down} 
; Sleep, 50
; SendInput {W up} 
; SendInput {R up}
; SendInput {V up}
; Sleep, 50 
 