PROGRAM PADREMOT;
USES Crt;
TYPE
 enumMove=(moveUp,moveDown,moveLeft,moveRight,moveSelect,moveStart,moveX,moveO);
CONST
 offsetX=20;
 offsetY=4;
 COM1_Base = $3F8;
 COM2_Base = $2F8;
 LCR_Offset = $03;
 Latch_Low = $00;
 Latch_High = $01;
VAR
 gbBaud: longint;
 salir:boolean;
 c:char;
 gbMute: boolean;

{************************************************}
PROCEDURE Prepara;
BEGIN
 ASM
  mov dx,03FBh {Line Control Register}
  in al,dx
  and al,$7F   {0111_1111b   Set DLAB=0}
  out dx,al
 END;
END;

{************************************************}
PROCEDURE Set9600baudios;
BEGIN
 ASM  {9600 baudios}
  mov ah,0
  {mov al,11000111b}
  mov al,11100011b {9600 111 NoParity 00 StopBits 0 CharSize8bits 11}
  mov dx,0 {COM1}
  int 14h
 END;
END;
{************************************************}
PROCEDURE SendSerial(ch: char);
VAR
 contTimeOut,LSR_Reg: word;
BEGIN
 contTimeOut:= 0;
 WHILE (contTimeOut< 100) DO
  BEGIN
   LSR_Reg:= port[$3FD];
   IF (LSR_Reg = $60) THEN
    contTimeOut:= 1000;
  contTimeOut:= contTimeOut+1;
 END;
 port[$3F8]:= byte(Ch);
END;
{************************************************}
procedure SetBaudRate(NewRate: longint);
var
  DivisorLatch: Word;
begin
  DivisorLatch := 115200 div NewRate;
  Port[COM1_Base + LCR_Offset] := Port[COM1_Base + LCR_Offset] or $80; {Set DLAB}
  Port[COM1_Base + Latch_High] := DivisorLatch shr 8;
  Port[COM1_Base + Latch_Low] := DivisorLatch and $FF;
  Port[COM1_Base + LCR_Offset] := Port[COM1_Base + LCR_Offset] and $7F; {Clear DLAB}
end;
{*****************************************************}
PROCEDURE ShowPAD;
BEGIN
 gotoxy(offsetX,offsetY);  WRITE(' ÚÄÄÄÄÄ¿              ÚÄÄÄÄÄ¿ ');
 gotoxy(offsetX,offsetY+1);WRITE('ÚÙ     ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ     À¿');
 gotoxy(offsetX,offsetY+2);WRITE('³   ß                    ß   ³');
 gotoxy(offsetX,offsetY+3);WRITE('³ ß   ß    ÜÜ   ÜÜ     ß   ß ³');
 gotoxy(offsetX,offsetY+4);WRITE('À¿  ß  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿  ß  ÚÙ');
 gotoxy(offsetX,offsetY+5);WRITE(' À¿  ÚÄÙ              ÀÄ¿  ÚÙ ');
 gotoxy(offsetX,offsetY+6);WRITE('  ÀÄÄÙ                  ÀÄÄÙ  ');

 gotoxy(2,14);WRITE('Q - (ESC) - EXIT');
 gotoxy(2,15);WRITE('- -  (-)  - MUTE ARDUINO');
 gotoxy(2,16);WRITE('+ -  (+)  - UNMUTE ARDUINO');
 gotoxy(2,17);WRITE('W -  (^)  - UP');
 gotoxy(2,18);WRITE('S -  (v)  - DOWN');
 gotoxy(2,19);WRITE('A -  (<)  - LEFT');
 gotoxy(2,20);WRITE('D -  (>)  - RIGHT');
 gotoxy(2,21);WRITE('X -  (X)  - X');
 gotoxy(2,22);WRITE('Z -  (O)  - O');
 gotoxy(2,23);WRITE('C -  (C)  - SEL');
 gotoxy(2,24);WRITE('V -  (V)  - START');
END;

{*****************************************************}
PROCEDURE ShowUp(a:boolean);
BEGIN gotoxy(offsetX+4,offsetY+2); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ShowDown(a:boolean);
BEGIN gotoxy(offsetX+4,offsetY+4); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ShowLeft(a:boolean);
BEGIN gotoxy(offsetX+2,offsetY+3); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ShowRight(a:boolean);
BEGIN gotoxy(offsetX+6,offsetY+3); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ShowSelect(a:boolean);
BEGIN gotoxy(offsetX+11,offsetY+3); IF (a=true) THEN WRITE('ßß') ELSE WRITE('ÜÜ'); END;
{*****************************************************}
PROCEDURE ShowStart(a:boolean);
BEGIN gotoxy(offsetX+16,offsetY+3); IF (a=true) THEN WRITE('ßß') ELSE WRITE('ÜÜ'); END;
{*****************************************************}
PROCEDURE ShowX(a:boolean);
BEGIN gotoxy(offsetX+25,offsetY+4); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ShowO(a:boolean);
BEGIN gotoxy(offsetX+27,offsetY+3); IF (a=true) THEN WRITE('Ü') ELSE WRITE('ß'); END;
{*****************************************************}
PROCEDURE ClearPAD;
BEGIN
 ShowUp(false);
 ShowDown(false);
 ShowLeft(false);
 ShowRight(false);
 ShowSelect(false);
 ShowStart(false);
 ShowX(false);
 ShowO(false);
END;

{************************************************}
PROCEDURE GetParams;
VAR
 code: integer;
 aux: string;
BEGIN
 {gbCadFile:= ParamStr(1);}
 {Val(ParamStr(2), gbDelayLine, code);}
 {gbDelayLine:= gbDelayLine DIV 55;} {18.2 ticks}
 aux:= ParamStr(1);
 IF (aux='115200') THEN gbBaud:= 115200
 ELSE gbBaud:= 9600;

 (*gbNoWait:= false;
 gbStopArduino:= false;
 IF (ParamCount>=4) THEN
  BEGIN
   IF ParamStr(4)='nowait' THEN gbNoWait:= true;
   IF (ParamStr(4)='stop') THEN gbStopArduino:= true;
  END;
 IF (ParamCount>=5) THEN
  BEGIN
   IF ParamStr(5)='nowait' THEN gbNoWait:= true;
   IF (ParamStr(5)='stop') THEN gbStopArduino:= true;
  END;*)
END;

{************************************************}
PROCEDURE ShowInfo;
BEGIN
 Writeln;
 Writeln('PADREMOT baud');
 Writeln;
 Writeln('baud: 9600, 115200');
 Writeln;
 Writeln('Example: PADREMOT 115200');
END;

{*****************************************************}
PROCEDURE Send(a:enumMove);
BEGIN
 CASE a OF
  moveUp:    BEGIN SendSerial('F');SendSerial('E');SendSerial('F');SendSerial('F'); END;
  moveDown:  BEGIN SendSerial('F');SendSerial('B');SendSerial('F');SendSerial('F'); END;
  moveLeft:  BEGIN SendSerial('F');SendSerial('7');SendSerial('F');SendSerial('F'); END;
  moveRight: BEGIN SendSerial('F');SendSerial('D');SendSerial('F');SendSerial('F'); END;
  moveSelect:BEGIN SendSerial('E');SendSerial('F');SendSerial('F');SendSerial('F'); END;
  moveStart: BEGIN SendSerial('7');SendSerial('F');SendSerial('F');SendSerial('F'); END;
  moveX:     BEGIN SendSerial('F');SendSerial('F');SendSerial('F');SendSerial('B'); END;
  moveO:     BEGIN SendSerial('F');SendSerial('F');SendSerial('F');SendSerial('D'); END;
 END;
 SendSerial(chr($0D));
 SendSerial(chr($0A));
END;

{*****************************************************}
PROCEDURE ResetPAD;
BEGIN
 SendSerial('F');
 SendSerial('F');
 SendSerial('F');
 SendSerial('F');
 SendSerial(chr($0D));
 SendSerial(chr($0A));
END;

{*****************************************************}
PROCEDURE ShowStateARDUINO;
BEGIN
 gotoxy(28,1);
 IF (gbMute=true) THEN
  Write(' MUTE ARDUINO ')
 ELSE
  Write('UNMUTE ARDUINO');
END;

{*****************************************************}
BEGIN
 gbMute:= false;
 IF (ParamCount<1) THEN
  BEGIN
   ShowInfo;
   exit;
  END;

 GetParams;
 Clrscr;
 ShowPAD;
 IF (gbBaud=9600) THEN
  Set9600baudios
 ELSE
  SetBaudRate(gbBaud);
 gotoxy(30,12);WRITE('BAUD ',gbBaud);
 ShowStateARDUINO;
 Prepara;
 delay(100);
 SendSerial(chr(34)); {Modo PAD}
 delay(100);
 SendSerial('+'); {UNMUTE}
 ResetPAD;
 REPEAT
  IF (KeyPressed) THEN
   BEGIN
    c:= Upcase(readkey);
    CASE ord(c) OF
     87: BEGIN ShowUp(true); Send(moveUp); END;{W}
     83: BEGIN ShowDown(true); Send(moveDown); END;{S}
     65: BEGIN ShowLeft(true); Send(moveLeft); END;{A}
     68: BEGIN ShowRight(true); Send(moveRight); END;{D}
     67: BEGIN ShowSelect(true); Send(moveSelect); END;{C}
     86: BEGIN ShowStart(true); Send(moveStart); END;{V}
     88: BEGIN ShowX(true); Send(moveX); END;{X}
     90: BEGIN ShowO(true); Send(moveO); END;{Z}
     43: BEGIN SendSerial('+'); gbMute:= false; END;{+}
     45: BEGIN SendSerial('-'); gbMute:= true; END;{-}
     81: salir:= true;{Q}
     72: BEGIN ShowUp(true); Send(moveUp); END;
     80: BEGIN ShowDown(true); Send(moveDown); END;
     75: BEGIN ShowLeft(true); Send(moveLeft); END;
     77: BEGIN ShowRight(true); Send(moveRight); END;
     27: salir:= true;
    END;
    ShowStateARDUINO;
    gotoxy(offsetx+14,offsety+6);WRITE('',c);
    delay(50);
    ClearPAD;
    gotoxy(offsetx+14,offsety+6);WRITE(' ');
    ResetPAD;
   END
 UNTIL (salir=true);
 ResetPAD;
 SendSerial(chr(33)); {Modo PADSIO}
 IF (gbMute=true) THEN
  SendSerial('-'); {Obligo mute}
 clrscr;
END.