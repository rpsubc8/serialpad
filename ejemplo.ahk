;Autor: ackerman
;EA1HLX
;Jaime Jose Gavin Sierra
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
 Sleep, 25

 SendInput {W up}
 Sleep, 25
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
 SendData(200)
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

Sleep,2000

SendFile("prueba.txt",buffer)




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
 