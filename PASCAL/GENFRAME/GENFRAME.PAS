{*************************************************************}
{* Author: ackerman                                          *}
{* Jaime Jose Gavin Sierra                                   *}
{* Convierte Binario PsEXE en nibbles para envio por ARDUINO *}
{* Se puede enviar con el RealTerm. Se tiene que poner una   *}
{* espera en milisegundos en el retorno de linea             *}
{* Numero de bytes x 50 milisegundos = 21 x 50 = 1050 milis  *}
{* Soporte para 4 botones buffer transistores                *}
{* Soporte para Fake SPI 14 botones y analogico              *}
{* Version PASCAL estandar MSDOS x86, DOSBOX y Win32         *}
{*************************************************************}
{En binario SPI Comunicaciones (2 bytes)}
{Bit}
{0  Select Button 0 Pres 1 Release}
{1  L3/Joy-button 0 Pres 1 Release/None/Disabled) ;analog mode only}
{2  R3/Joy-button 0 Pres 1 Release/None/Disabled) ;analog mode only}
{3  Start Button  0 Pres 1 Release}
{4  Joypad Up     0 Pres 1 Release}
{5  Joypad Right  0 Pres 1 Release}
{6  Joypad Down   0 Pres 1 Release}
{7  Joypad Left   0 Pres 1 Release}
{8  L2 Button     0 Pres 1 Release (Lower-left shoulder)}
{9  R2 Button     0 Pres 1 Release (Lower-right shoulder)}
{10 L1 Button     0 Pres 1 Release (Upper-left shoulder)}
{11 R1 Button     0 Pres 1 Release (Upper-right shoulder)}
{12 /\ Button     0 Pres 1 Release (Triangle, upper button)}
{13 () Button     0 Pres 1 Release (Circle, right button)}
{14 >< Button     0 Pres 1 Release (Cross, lower button)}
{15 [] Button     0 Pres 1 Release (Square, left button)}

{Botones en la PSX real}
{Pad1L2          (1<< 0)  0}
{Pad1R2          (1<< 1)  1}
{Pad1L1          (1<< 2)  2}
{Pad1R1          (1<< 3)  3}
{Pad1tri         (1<< 4)  4}
{Pad1crc         (1<< 5)  5}
{Pad1x           (1<< 6)  6}
{Pad1sqr         (1<< 7)  7}
{Pad1Select      (1<< 8)  8}
{Pad1Start       (1<<11)  9}
{Pad1Up          (1<<12) 10}
{Pad1Right       (1<<13) 11}
{Pad1Down        (1<<14) 12}
{Pad1Left        (1<<15) 13}

{Botones conversor USB PSX botones controlador en Windows}
{triangulo - 1}
{Circulo   - 2}
{X         - 3}
{Cuadrado  - 4}
{L2        - 5}
{R2        - 6}
{L1        - 7}
{R1        - 8}
{Start     - 9}
{Select    - 10}
{Arriba    - 13}
{Derecha   - 14}
{Abajo     - 15}
{Izquierda - 16}

{Analogico}
{Stick izquierdo - 11}
{Stick derecho   - 12}
PROGRAM GENFRAME;
VAR
 gbFileRead: FILE;
 gbFileWrite: TEXT;
 gbContLinea, gbContContLinea,gbNumRead,i: Word;
 gbSpeed, gbCompress, gbCode: integer;
 Buf: array[1..32000] of byte; {32000 bytes}
 gbCadAddress,gbCadSal: string;
 gbAddress: array[1..4] of byte;
 gbModeFlag: boolean; {TRUE flanco btn. FALSE, no flanco}
 gbTopeLinea: word;
 isHead: boolean;

{******************************************************}
FUNCTION HexToNibbleDec(aData: char): byte;
VAR
 aReturn: byte;
BEGIN
 aReturn:= 0;
 CASE (aData) OF
  '0': aReturn:= 0;
  '1': aReturn:= 1;
  '2': aReturn:= 2;
  '3': aReturn:= 3;
  '4': aReturn:= 4;
  '5': aReturn:= 5;
  '6': aReturn:= 6;
  '7': aReturn:= 7;
  '8': aReturn:= 8;
  '9': aReturn:= 9;
  'A':aReturn:= 10;
  'B':aReturn:= 11;
  'C':aReturn:= 12;
  'D':aReturn:= 13;
  'E':aReturn:= 14;
  'F':aReturn:= 15;
  else
   aReturn:=0;
 END;
 HexToNibbleDec:= aReturn;
END;

{******************************************************}
FUNCTION NibbleToHexString(aData: byte):char;
VAR
 aReturn: char;
BEGIN
 aReturn:='0';
 CASE (aData) OF
  0: aReturn:= '0';
  1: aReturn:= '1';
  2: aReturn:= '2';
  3: aReturn:= '3';
  4: aReturn:= '4';
  5: aReturn:= '5';
  6: aReturn:= '6';
  7: aReturn:= '7';
  8: aReturn:= '8';
  9: aReturn:= '9';
  10:aReturn:= 'A';
  11:aReturn:= 'B';
  12:aReturn:= 'C';
  13:aReturn:= 'D';
  14:aReturn:= 'E';
  15:aReturn:= 'F';
  else
   aReturn:='0';
 END;
 NibbleToHexString:= aReturn;
END;

{******************************************************}
PROCEDURE SaveByteTextNotFlag(d0,d1: byte);
VAR
 aux: byte;
 {0xxx Primer byte}
 {1xxx}
 {0xxx}
 {1xxx Segundo byte}
 {0xxx}
 {1xxx}
BEGIN  {Sin Flag Sincronia, modo rapido}
 aux:= (d0 AND $07); {Primer byte}
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= (((d0 SHR 3) AND $07) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= ((d0 SHR 6) AND $03);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);

 aux:= ((d1 AND $07)OR $08); {Segundo byte}
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= ((d1 SHR 3) AND $07);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= (((d1 SHR 6) AND $03) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
   gbContContLinea:= gbContContLinea+1;
   IF (gbContContLinea>4) THEN
    BEGIN
     gbContContLinea:= 0;
     {Writeln(gbFileWrite);}
    END;
  END;
END;

{******************************************************}
PROCEDURE SaveByteText(data: byte; putCero: boolean);
VAR
 aux: byte;
BEGIN
 aux:= ((data AND $07) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 IF (putCero = TRUE) THEN gbCadSal:= gbCadSal + '0';

 aux:= (((data SHR 3) AND $07) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 IF (putCero = TRUE) THEN gbCadSal:= gbCadSal + '0';

 aux:= (((data SHR 6) AND $03) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 IF (putCero = TRUE) THEN gbCadSal:= gbCadSal + '0';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{***************************************************}
PROCEDURE SaveHead;
VAR
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 Writeln(gbFileWrite);
 Writeln(gbFileWrite);
 Writeln(gbFileWrite,'*'); {Sincronismo}
 IF (gbModeFlag = TRUE) THEN
  Writeln(gbFileWrite,'*') {Vaciar buffer serial ARDUINO}
 else
  Writeln(gbFileWrite,'#'); {Vaciar buffer serial ARDUINO}

 cad:= '';
 SaveByteText((auxSize AND $FF),NOT(gbModeFlag));
 SaveByteText(((auxSize SHR 8) AND $FF),NOT(gbModeFlag));
 SaveByteText(((auxSize SHR 16) AND $FF),NOT(gbModeFlag));
 SaveByteText((gbSpeed OR (gbCompress SHL 6)),NOT(gbModeFlag));
 FOR i:=1 TO 3 DO
  SaveByteText(gbAddress[i],NOT(gbModeFlag));
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveByteHeadSPI(data: byte);
{Bit, que seria el 3 nibble}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= ((data AND $07) OR $08);
 gbCadSal:= gbCadSal + '00'+NibbleToHexString(aux)+'0';

 aux:= (((data SHR 3) AND $07) OR $08);
 gbCadSal:= gbCadSal + '00'+NibbleToHexString(aux)+'0';

 aux:= (((data SHR 6) AND $03) OR $08);
 gbCadSal:= gbCadSal + '00'+NibbleToHexString(aux)+'0';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save3BytesSPI(a,b,c: byte);
VAR
 aux: word; {16 bits 2 bytes mando}
BEGIN
 {Envia 3 bytes en 2 pasadas Joystick}
 { 1 byte 4 bits Low}
 { 1 byte 4 bits High}
 {8 bits 4 bits 1 bit CRC 13 bits  14 botones}
 {8 bits 4 bits 1 bit CRC}
 {Izquierda Abajo Derecha Arriba Start Select Cuadrado X Circulo Triangulo R1 L1 R2 L2}
 {Left      Down  Right   Up     V     C         S     Z  X        D       R  W  T  E}
 {Pos Bit en SPI 2 bytes controlador}
 {7         6     5       4      3     0        15     14 13       12      11 10 9  8}
 {Primer byte}
 aux:= 0;{%0000000000000000;}
 IF ((a AND 1) = 1)  THEN aux:=aux OR 256;  {0000000100000000 bit 8}
 IF ((a AND 2) = 2)  THEN aux:=aux OR 512;  {0000001000000000 bit 9}
 IF ((a AND 4) = 4)  THEN aux:=aux OR 1024; {0000010000000000 bit 10}
 IF ((a AND 8) = 8)  THEN aux:=aux OR 2048; {0000100000000000 bit 11}
 IF ((a AND 16)=16)  THEN aux:=aux OR 4096; {0001000000000000 bit 12}
 IF ((a AND 32)=32)  THEN aux:=aux OR 8192; {0010000000000000 bit 13}
 IF ((a AND 64)=64)  THEN aux:=aux OR 16384;{0100000000000000 bit 14}
 IF ((a AND 128)=128)THEN aux:=aux OR 32768;{1000000000000000 bit 15}
 {Primer nibble Segundo byte}
 IF ((b AND 1) = 1)  THEN aux:=aux OR 1; {0000000000000001 bit 0}
 IF ((b AND 2) = 2)  THEN aux:=aux OR 8; {0000000000001000 bit 3}
 IF ((b AND 4) = 4)  THEN aux:=aux OR 16;{0000000000010000 bit 4}
 IF ((b AND 8) = 8)  THEN aux:=aux OR 32;{0000000000100000 bit 5}
 aux:= aux OR 128; {0000000010000000 7}
 gbCadSal:= gbCadSal + NibbleToHexString((aux AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 4)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 8) AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 12)AND $0F));

 {Tercer byte}
 aux:= 0; {0000000000000000}
 IF ((c AND 1) = 1)  THEN aux:=aux OR 256;  {0000000100000000 bit 8}
 IF ((c AND 2) = 2)  THEN aux:=aux OR 512;  {0000001000000000 bit 9}
 IF ((c AND 4) = 4)  THEN aux:=aux OR 1024; {0000010000000000 bit 10}
 IF ((c AND 8) = 8)  THEN aux:=aux OR 2048; {0000100000000000 bit 11}
 IF ((c AND 16)=16)  THEN aux:=aux OR 4096; {0001000000000000 bit 12}
 IF ((c AND 32)=32)  THEN aux:=aux OR 8192; {0010000000000000 bit 13}
 IF ((c AND 64)=64)  THEN aux:=aux OR 16384;{0100000000000000 bit 14}
 IF ((c AND 128)=128)THEN aux:=aux OR 32768;{1000000000000000 bit 15}
 {Segundo nibble Segundo byte}
 IF ((b AND 16) = 16)  THEN aux:=aux OR 1; {0000000000000001 bit 0}
 IF ((b AND 32) = 32)  THEN aux:=aux OR 8; {0000000000001000 bit 3}
 IF ((b AND 64) = 64)  THEN aux:=aux OR 16;{0000000000010000 bit 4}
 IF ((b AND 128)=128)  THEN aux:=aux OR 32;{0000000000100000 bit 5}
 aux:= aux OR 128; {0000000010000000 7}
 gbCadSal:= gbCadSal + NibbleToHexString((aux AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 4)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 8) AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString(((aux SHR 12)AND $0F));

 {gbCadSal:= gbCadSal+'JJ';}

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveHeadSPI;
VAR
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 Writeln(gbFileWrite);
 Writeln(gbFileWrite);
 Writeln(gbFileWrite,'*'); {Sincronismo}
 Writeln(gbFileWrite,'*');  {Vaciar buffer serial ARDUINO}

 cad:= '';
 SaveByteHeadSPI((auxSize AND $FF));
 SaveByteHeadSPI(((auxSize SHR 8) AND $FF));
 SaveByteHeadSPI(((auxSize SHR 16) AND $FF));
 SaveByteHeadSPI((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  SaveByteHeadSPI(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE ShowHelp;
BEGIN
 writeln('GENFRAME psexe frame memaddress speed compress');
 writeln('');
 writeln(' speed: 0,1,2,3,4');
 writeln(' compress: 0,1,2,3');
 writeln('');
 writeln('Example:');
 writeln(' GENFRAME demo1.exe frame.txt 80010000 0 0');
END;

{******************************************}
PROCEDURE GetParams;
VAR
 aux: byte;
BEGIN
 Assign(gbFileRead, ParamStr(1));
 Assign(gbFileWrite, ParamStr(2));
 gbCadAddress:= ParamStr(3);
 gbAddress[1]:= HexToNibbleDec(gbCadAddress[8]) OR (HexToNibbleDec(gbCadAddress[7])SHL 4);
 gbAddress[2]:= HexToNibbleDec(gbCadAddress[6]) OR (HexToNibbleDec(gbCadAddress[5])SHL 4);
 gbAddress[3]:= HexToNibbleDec(gbCadAddress[4]) OR (HexToNibbleDec(gbCadAddress[3])SHL 4);
 gbAddress[4]:= HexToNibbleDec(gbCadAddress[2]) OR (HexToNibbleDec(gbCadAddress[1])SHL 4);
 Val(ParamStr(4), gbSpeed, gbCode);
 Val(ParamStr(5), gbCompress, gbCode);
 CASE (gbSpeed) OF
  0: gbModeFlag:= TRUE;
  1: gbModeFlag:= FALSE;
  else
   gbModeFlag:= TRUE;
 END;
END;

{********}
{* MAIN *}
{********}
BEGIN
 IF (paramCount<5) THEN
  BEGIN
   ShowHelp;
   Exit
  END;
 GetParams;

 Reset(gbFileRead, 1);  { Tamanio registro 1}
 Rewrite(gbFileWrite);

 isHead:= FALSE;
 gbCadSal:= '';
 gbContLinea:=0;
 gbContContLinea:=0;
 CASE (gbSpeed) OF
  0: gbTopeLinea:= 6;
  1: gbTopeLinea:= 6;
  2: gbTopeLinea:= 1;
  else
   gbTopeLinea:= 6;
 END;

 CASE (gbSpeed) OF
  0: SaveHead;
  1: SaveHead;
  2: BEGIN
      SaveHeadSPI;
      gbTopeLinea:= 2; {2 Para igualar linea ancho archivo txt}
     END;
  else
   SaveHead;
 END;

 {Writeln(gbFileWrite);}
 gbCadSal:= '';
 gbContLinea:=0;
 gbContContLinea:=0;
 REPEAT
  BlockRead(gbFileRead, Buf, SizeOf(Buf), gbNumRead);
  IF ((gbCompress=1) AND (isHead = FALSE)) THEN
   BEGIN {Primera vez es la cabecera}
    isHead:= TRUE;
    FOR i:=2049 TO gbNumRead DO
     Buf[i-1920]:= Buf[i]; {Empieza en 129}
    gbNumRead:= gbNumRead-1920;
   END;

  CASE (gbSpeed) OF
   0: FOR i:=1 TO gbNumRead DO
       SaveByteText(Buf[i],FALSE); {No se pone 0}
   1: BEGIN
       i:= 1;
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveByteTextNotFlag(Buf[i],Buf[i+1]); {Modo 25 milis}
         i:= i+2;
        END;
      END;
   2: BEGIN
       i:= 1;
       WHILE (i<gbNumRead) DO
        BEGIN
         Save3BytesSPI(Buf[i],Buf[i+1],Buf[i+2]);
         i:= i+3;
        END;
      END;
  END;
 UNTIL (gbNumRead = 0);
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
 {Writeln(gbFileWrite);}
 Close(gbFileWrite);
 Close(gbFileRead);
END.