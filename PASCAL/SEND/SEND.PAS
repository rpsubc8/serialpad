PROGRAM Serie;
USES Crt;
(*11100011b 9600 baudios 1 bit stop*)
CONST
 COM1_Base = $3F8;
 COM2_Base = $2F8;
 LCR_Offset = $03;
 Latch_Low = $00;
 Latch_High = $01;
VAR
 gbNoWait: boolean;
 gbStopArduino: boolean;
 gbBaud: longint;
 gbCadFile: string;
 gbDelayLine: word;
 gbCurrentTime: LONGINT ABSOLUTE $0040:$006C;
 gbBeforeTime: LONGINT;
 gbFile: Text;
 Ch: Char;
 auxByte: byte;
 contador: integer;
 gbTotalLineas: word;
 gbCurrentLinea: word;
 gbBuffer: ARRAY[1..16000] OF char;
 gbCurrentBuffer: word;
 i:word;

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

{************************************************}
(*function GetBaudRate: Integer;
var
  DivisorLatch: Word;
begin
  Port[COM1_Base + LCR_Offset] := Port[COM1_Base + LCR_Offset] or $80; {Set DLAB}
  DivisorLatch := (Port[COM1_Base + Latch_High] shl 8) + Port[COM1_Base + Latch_Low];
  Port[COM1_Base + LCR_Offset] := Port[COM1_Base + LCR_Offset] and $7F; {Clear DLAB}
  GetBaudRate := 115200 div DivisorLatch;
end;*)

{************************************************}
(*FUNCTION CharHexToDec(c: Char): byte;
VAR
 aReturn: byte;
 a: integer;
BEGIN
 contador:= 0;
 a:= ord(c);
 IF (a>=97) THEN
  aReturn:= a-87
 ELSE
  IF (a>=65) THEN aReturn:= a-55
  ELSE
   IF (a>=48) THEN aReturn:= a-48;
 CharHexToDec:= aReturn;
END;*)

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
FUNCTION GetLinesFile:word;
VAR aReturn: word;
BEGIN
 aReturn:= 0;
 Reset(gbFile);
 WHILE NOT EOF(gbFile) DO BEGIN Readln(gbFile); aReturn:= aReturn+1; END;
 Reset(gbFile); GetLinesFile:= aReturn;
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
PROCEDURE GetParams;
VAR
 code: integer;
 aux: string;
BEGIN
 gbCadFile:= ParamStr(1);
 Val(ParamStr(2), gbDelayLine, code);
 gbDelayLine:= gbDelayLine DIV 55; {18.2 ticks}
 aux:= ParamStr(3);
 IF (aux='115200') THEN gbBaud:= 115200
 ELSE gbBaud:= 9600;
 
 gbNoWait:= false;  
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
  END;
END;

{************************************************}
PROCEDURE ShowHelp;
BEGIN
 writeln('');
 writeln('SEND PAD PSX ARDUINO   Author: Ackerman');
 writeln('----------------------------------------');
 writeln('SEND frame.txt delay baud nowait stop');
 writeln('');
 writeln(' delay milliseconds');
 writeln(' baud:');
 writeln('  9600   - 9600 bauds');
 writeln('  115200 - 115200 bauds');
 writeln(' nowait: Not wait key press');
 writeln(' stop: Start Stop arduino');
 writeln('');
 writeln('Example:');
 writeln(' SEND frame.txt 1050 9600 stop');
END;

{************************************************}
PROCEDURE EnviaFichero;
BEGIN
 gotoxy(1,1); Write('Delay ',ParamStr(2),' Baud ',gbBaud);
 IF (gbNoWait = false) THEN
  BEGIN
   IF (gbStopArduino = true) THEN
    BEGIN
     gotoxy(1,2);Write('ARDUINO STOP');
     SendSerial('-'); {Paro ARDUINO}
    END;
   gotoxy(1,3);Write('Press key to begin');
   WHILE NOT(keypressed) DO;
   readkey;
   IF (gbStopArduino = true) THEN
    BEGIN
     SendSerial('+'); {Arranco ARDUINO}
     gotoxy(1,2);Write('ARDUINO START');
     gbBeforeTime:= gbCurrentTime;
     WHILE ((gbCurrentTime - gbBeforeTime) <= gbDelayLine) DO;
    END;
  END;
 Reset(gbFile);
 while not Eof(gbFile) do
  begin
   if keypressed then exit;
   IF (EOLN(gbFile)=TRUE) THEN
    BEGIN
     FOR i:= 1 TO gbCurrentBuffer DO
      SendSerial(gbBuffer[i]);
     SendSerial(chr($0D));
     SendSerial(chr($0A));
     gbCurrentLinea:= gbCurrentLinea+1;
     gotoxy(1,4); Write('Line ',gbTotalLineas,'/',gbCurrentLinea);
     gbBeforeTime:= gbCurrentTime;
     WHILE (((gbCurrentTime - gbBeforeTime) <= gbDelayLine) AND NOT Keypressed) DO;
     {Delay(1150);}
     gbCurrentBuffer:= 1;
    END;
   Read(gbFile, Ch);
   gbBuffer[gbCurrentBuffer]:= Ch;
   gbCurrentBuffer:= gbCurrentBuffer+1;
  end;
END;

{************************************************}
BEGIN
 {Delay(2000);}
 IF (paramCount<3) THEN
  BEGIN
   ShowHelp;
   Exit
  END;
 GetParams;

 gbCurrentBuffer:=1;
 gbCurrentLinea:=0;
 Clrscr;
 IF (gbBaud=9600) THEN
  Set9600baudios
 ELSE
  SetBaudRate(gbBaud);
 Prepara;

 {Assign(F,'c:\tp\bin\serie\frame.txt');}
 Assign(gbFile,gbCadFile);
 gbTotalLineas:= GetLinesFile;
 EnviaFichero;
 Close(gbFile);
END.



{Sobra}
   {Write(Ch);}
   {port[$3F8]:= CharHexToDec(Ch);}
   {port[$3F8]:= byte(Ch);}
   {al,0010_0000b  ;Use bit 5 to see if THR is empty}
   {WHILE ((port[$3FD] AND $20) = 0) DO;}

(*   auxByte:= byte(Ch);
   asm
    mov dx,0
    mov al,'F'
    mov ah,1
    int 14h
   end;*)
   {contador:= contador+1;}
   {if (contador>=60) then}
   (*Write(Ch);*)
   {Delay(1);}

(* asm
  mov dx,03F8h
  mov ah,0
  mov al,'F'
  out dx,al
 end;*)

{   Delay(50);}
(*   Write(Ch);  { Descargar ficheros tipo texto }*)

 (*asm
  mov dx,03F8h
  mov ah,0
  mov al,'F'
  out dx,al
 end;*)

