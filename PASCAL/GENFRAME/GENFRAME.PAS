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
 gbMaxSizeReadBuf: word; {32000 por defecto}
 gbCadAddress,gbCadSal: string;
 gbAddress: array[1..4] of byte;
 gbModeFlag: boolean; {TRUE flanco btn. FALSE, no flanco}
 gbTopeLinea: word;
 isHead: boolean;
 gbFlipFlop: byte;

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
PROCEDURE SaveByteTextNotFlag(d0: byte; VAR refFlipFlop: byte);
VAR{Paso del bit 3, se encarga ARDUINO}
 aux: byte;
 {1xxx Primer byte}
 {0xxx}
 {1xxx}
BEGIN  {Sin Flag Sincronia, modo rapido}
 IF (refFlipFlop = $08) THEN
  BEGIN
   aux:= (d0 AND $07) OR $08; {Primer byte}
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   aux:= ((d0 SHR 3) AND $07);
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   aux:= ((d0 SHR 6) AND $03) OR $08;
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   refFlipFlop:= 0;
  END
 ELSE
  BEGIN
   aux:= (d0 AND $07); {Primer byte}
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   aux:= ((d0 SHR 3) AND $07) OR $08;
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   aux:= ((d0 SHR 6) AND $03);
   gbCadSal:= gbCadSal + NibbleToHexString(aux);
   refFlipFlop:= $08;
  END;

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
 {Writeln(gbFileWrite);}
 {Writeln(gbFileWrite);}
 {Writeln(gbFileWrite,'*');} {Sincronismo}
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
PROCEDURE Save3ByteHeadSPI(data: byte);
{Bit, que seria el 3 nibble. 3 nibbles Arduino 50 ms 12 botones}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save5BytesHeadAnalogSPI(data: byte);
{Bit, que seria el 3 nibble. 3 nibbles Arduino 50 ms 5 bytes 10 caracteres}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F00000000';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F00000000';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F00000000';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save2BytesHeadAnalogSPI(data: byte);
{Bit, que seria el 3 nibble. 5 nibbles Arduino 50 ms Modo 8}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveHeadAnalogSPISpeed10Flag(data: byte);
{Bit, que seria el 3 nibble. 5 nibbles Arduino 50 ms Modo 10 Modo}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveHeadAnalogSPISpeed11NoFlag(data: byte);
{Bit, que seria el 3 nibble. 10 nibbles Arduino 25 ms Speed 11}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';
 gbCadSal:= gbCadSal + 'F000000000';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';
 gbCadSal:= gbCadSal + 'F000000000';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'000000000';
 gbCadSal:= gbCadSal + 'F000000000';

 Write(gbFileWrite,gbCadSal);
 gbCadSal:='';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save2BytesHeadAnalogSPINoFlag(data: byte);
{Bit, que seria el 3 nibble. 3 nibbles Arduino 25 ms 5 nibbles 5 caracteres}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';
 gbCadSal:= gbCadSal + 'F0000';

 aux:= (not(((data SHR 3) AND $07) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';
 gbCadSal:= gbCadSal + 'F0000';

 aux:= (not(((data SHR 6) AND $03) OR $08)AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'0000';
 gbCadSal:= gbCadSal + 'F0000';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save1ByteHeadSPI(data: byte);
{Bit, que seria el 3 nibble. Logica negada envio arduino}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08)) AND $0F;
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F';

 aux:= (not(((data SHR 3) AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F';

 aux:= (not(((data SHR 6) AND $03) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'F';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save1ByteHeadSPINoFlag(data: byte);
{Bit, que seria el 3 nibble. Logica negada envio arduino 25 ms}
{8   L2 Button}
{9   R2 Button}
{10  L1 Button}
{11  R1 Button}
VAR
 aux: byte;
BEGIN
 aux:= (not((data AND $07) OR $08)) AND $0F;
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

 aux:= (not(((data SHR 3) AND $07) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

 aux:= (not(((data SHR 6) AND $03) OR $08) AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(aux)+'FFF';

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
 aux: byte; {50 ms 12 botones 3 caracteres ARDUINO}
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
 {Pos Bit en SPI 2 bytes controlador}
 {  15     14 13       12      11 10 9  8}
 {   7      6  5        4       3  2 1  0 Equivale}
 {Primer byte}
 gbCadSal:= gbCadSal + NibbleToHexString((not(a AND $0F)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((a SHR 4)AND $0F) AND $0F));
 {Segundo byte parte baja}
 aux:=0;
 IF ((b AND 1)=1) THEN aux:= aux OR 1;
 IF ((b AND 2)=2) THEN aux:= aux OR 8;
 IF ((b AND 4)=4) THEN aux:= aux OR 16;
 IF ((b AND 8)=8) THEN aux:= aux OR 32;
 gbCadSal:= gbCadSal + NibbleToHexString((not(aux AND $0F)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((aux SHR 4)AND $0F)AND $0F));
 {Tercer byte}
 gbCadSal:= gbCadSal + NibbleToHexString((not(c AND $0F)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((c SHR 4)AND $0F) AND $0F));
 {Segundo byte parte alta}
 aux:=0;
 IF ((b AND 16)=16) THEN aux:= aux OR 1;
 IF ((b AND 32)=32) THEN aux:= aux OR 8;
 IF ((b AND 64)=64) THEN aux:= aux OR 16;
 IF ((b AND 128)=128) THEN aux:= aux OR 32;
 gbCadSal:= gbCadSal + NibbleToHexString((not(aux AND $0F) AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((aux SHR 4)AND $0F)AND $0F));

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveByteSPI(a: byte);
BEGIN
 {1 byte 8 botones Envio 1 byte Gatillos y botones.Logica negada Arduino}
 {Cuadrado X Circulo Triangulo R1 L1 R2 L2}
 {   S     Z  X        D       R  W  T  E}
 {Pos Bit en SPI 2 bytes controlador}
 {  15     14 13       12      11 10 9  8}
 {   7      6  5        4       3  2 1  0 Equivale}
 gbCadSal:= gbCadSal + NibbleToHexString((not(a AND $0F)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((a SHR 4)AND $0F) AND $0F));

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save5BytesAnalogSPI(a,b,c,d,e: byte);
BEGIN
 {1 byte 8 botones Envio 1 byte Gatillos y botones.Logica negada Arduino}
 {Cuadrado X Circulo Triangulo R1 L1 R2 L2}
 {   S     Z  X        D       R  W  T  E}
 {Pos Bit en SPI 2 bytes controlador}
 {  15     14 13       12      11 10 9  8}
 {   7      6  5        4       3  2 1  0 Equivale}
 {Analogico Right X,Y, Left X,Y}
 gbCadSal:= gbCadSal + NibbleToHexString((not(a AND $0F)AND $0F));
 gbCadSal:= gbCadSal + NibbleToHexString((not((a SHR 4)AND $0F) AND $0F));
 {Right Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(b AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(((b SHR 4)AND $0F));
 {Right Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(c AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(((c SHR 4)AND $0F));
 {Left Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(d AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(((d SHR 4)AND $0F));
 {Left Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(e AND $0F);
 gbCadSal:= gbCadSal + NibbleToHexString(((e SHR 4)AND $0F));

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE Save3BytesAnalogSPI(a,b,c: byte);
BEGIN
 {1 byte 8 botones Envio 1 byte Gatillos y botones.Logica negada Arduino}
 {Cuadrado X Circulo Triangulo R1 L1 R2 L2}
 {   S     Z  X        D       R  W  T  E}
 {Pos Bit en SPI 2 bytes controlador}
 {  15     14 13       12      11 10 9  8}
 {   7      6  5        4       3  2 1  0 Equivale}
 {Analogico Right X,Y, Left X,Y}
 gbCadSal:= gbCadSal + '0'; {Forzar fallo analogico, no envia nada}
 {Right Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(a AND $0F); {Baja a}
 {gbCadSal:= gbCadSal + '0';}
 {gbCadSal:= gbCadSal + NibbleToHexString(c AND $03);} {2 bits de c}
 {Right Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(((a SHR 4)AND $0F)); {Alta a}
 {gbCadSal:= gbCadSal + NibbleToHexString((c SHR 2)AND $03);} {2 bits de c}
 {gbCadSal:= gbCadSal + '0';}
 {Left Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(b AND $0F); {Baja b}
 {gbCadSal:= gbCadSal + NibbleToHexString((c SHR 4)AND $03);} {2 bits de c}
 {gbCadSal:= gbCadSal + '0';}
 {Left Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(((b SHR 4)AND $0F)); {Alta b}
 {gbCadSal:= gbCadSal + NibbleToHexString((c SHR 6)AND $03);} {2 bits de c}
 {gbCadSal:= gbCadSal + '0';}

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveAnalogSPISpeed1011(a,b,c,d: byte);
VAR aux: byte;
BEGIN
 {gbCadSal:= gbCadSal + '00';} {Forzar fallo analogico, no envia nada}
 aux:= ((NOT(d)AND $0F)) AND $0F; {Baja d}
 gbCadSal:= gbCadSal + NibbleToHexString(aux); {Alta d}
 gbCadSal:= gbCadSal + '0';
 {Right Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(a AND $0F); {Baja a}
 aux:= (((d SHR 4)AND $01) SHL 2)AND $04;
 gbCadSal:= gbCadSal + NibbleToHexString((c AND $03)OR aux); {2 bits de c}
 {Right Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(((a SHR 4)AND $0F)); {Alta a}
 aux:= (((d SHR 5)AND $01) SHL 2)AND $04;
 gbCadSal:= gbCadSal + NibbleToHexString(((c SHR 2)AND $03)OR aux); {2 bits de c}
 {Left Joy X}
 gbCadSal:= gbCadSal + NibbleToHexString(b AND $0F); {Baja b}
 aux:= (((d SHR 6)AND $01) SHL 2)AND $04;
 gbCadSal:= gbCadSal + NibbleToHexString(((c SHR 4)AND $03)OR aux); {2 bits de c}
 {Left Joy Y}
 gbCadSal:= gbCadSal + NibbleToHexString(((b SHR 4)AND $0F)); {Alta b}
 aux:= (((d SHR 7)AND $01) SHL 2)AND $04;
 gbCadSal:= gbCadSal + NibbleToHexString(((c SHR 6)AND $03)OR aux); {2 bits de c}

 Write(gbFileWrite,gbCadSal);
 gbCadSal:='';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;


{******************************************************}
PROCEDURE SaveCustomSPISpeed1213(VAR a: array OF byte);
VAR aux,i: byte;{Graba 16 nibbles 8 bytes}
BEGIN
 FOR i:=0 TO 7 DO
  BEGIN
   gbCadSal:= gbCadSal + NibbleToHexString(a[i] AND $0F); {Baja}
   gbCadSal:= gbCadSal + NibbleToHexString(((a[i] SHR 4)AND $0F)); {Alta}
  END;

 Write(gbFileWrite,gbCadSal);
 gbCadSal:='';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveCustomSPISpeed1415(VAR a: array OF byte);
VAR aux,i: byte;{Graba 32 nibbles 16 bytes 14 bytes}
BEGIN
 FOR i:=0 TO 13 DO
  BEGIN
   gbCadSal:= gbCadSal+NibbleToHexString(a[i] AND $0F);{Baja}
   gbCadSal:= gbCadSal+NibbleToHexString(((a[i] SHR 4)AND $7));{Alta 7 bits}
  END;
    
 aux:= ((a[0] AND $80)SHR 7)OR((a[1] AND $80) SHR 6)OR((a[2] AND $80) SHR 5)OR((a[3] AND $80) SHR 4);
 gbCadSal:= gbCadSal+NibbleToHexString(aux AND $0F);
 aux:= ((a[4] AND $80)SHR 7)OR((a[5] AND $80) SHR 6)OR((a[6] AND $80) SHR 5); 
 gbCadSal:= gbCadSal+NibbleToHexString(aux AND $07);

 aux:= ((a[7] AND $80)SHR 7)OR((a[8] AND $80) SHR 6)OR((a[9] AND $80) SHR 5)OR((a[10] AND $80) SHR 4);
 gbCadSal:= gbCadSal+NibbleToHexString(aux AND $0F);
 aux:= ((a[11] AND $80)SHR 7)OR((a[12] AND $80) SHR 6)OR((a[13] AND $80) SHR 5); 
 gbCadSal:= gbCadSal+NibbleToHexString(aux AND $07);

 Write(gbFileWrite,gbCadSal);
 gbCadSal:='';

 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>gbTopeLinea) THEN
  BEGIN
   gbContLinea:= 0;
   Writeln(gbFileWrite,gbCadSal);
   gbCadSal:= '';
  END;
END;

{******************************************************}
PROCEDURE SaveHead3ByteSPI;
VAR{Graba 3 bytes 12 bits 50 ms Head}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save3ByteHeadSPI((auxSize AND $FF));
 Save3ByteHeadSPI(((auxSize SHR 8) AND $FF));
 Save3ByteHeadSPI(((auxSize SHR 16) AND $FF));
 Save3ByteHeadSPI((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save3ByteHeadSPI(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHead5BytesAnalogSPI;
VAR{5 bytes 40 bits 50 ms Head 10 caracteres Arduino}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save5BytesHeadAnalogSPI((auxSize AND $FF));
 Save5BytesHeadAnalogSPI(((auxSize SHR 8) AND $FF));
 Save5BytesHeadAnalogSPI(((auxSize SHR 16) AND $FF));
 Save5BytesHeadAnalogSPI((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save5BytesHeadAnalogSPI(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHead2BytesAnalogSPI;
VAR{16 bits 50 ms Head 5 caracteres Arduino Modo 8}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save2BytesHeadAnalogSPI((auxSize AND $FF));
 Save2BytesHeadAnalogSPI(((auxSize SHR 8) AND $FF));
 Save2BytesHeadAnalogSPI(((auxSize SHR 16) AND $FF));
 Save2BytesHeadAnalogSPI((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save2BytesHeadAnalogSPI(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHeadSpeed10Flag;
VAR{32 bits 50 ms Head 10 caracteres Arduino Modo 10}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 SaveHeadAnalogSPISpeed10Flag((auxSize AND $FF));
 SaveHeadAnalogSPISpeed10Flag(((auxSize SHR 8) AND $FF));
 SaveHeadAnalogSPISpeed10Flag(((auxSize SHR 16) AND $FF));
 SaveHeadAnalogSPISpeed10Flag((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  SaveHeadAnalogSPISpeed10Flag(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHeadSpeed12Flag;
VAR{64 bits 50 ms Head 16 caracteres Arduino Modo 12}
 aux: byte;
 i:integer;
 auxSize: longint;
 a: ARRAY[0..7] OF byte;
BEGIN
 auxSize:= FileSize(gbFileRead);
 a[0]:=(auxSize AND $FF);
 a[1]:=((auxSize SHR 8) AND $FF);
 a[2]:=((auxSize SHR 16) AND $FF);
 a[3]:=(gbSpeed OR (gbCompress SHL 6));
 a[4]:=gbAddress[1];
 a[5]:=gbAddress[2];
 a[6]:=gbAddress[3];
 a[7]:=0;
 SaveCustomSPISpeed1213(a);
 FOR i:=1 TO 20 DO
  Write(gbFileWrite,'0000000000000000');
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHeadSpeed14Flag;
VAR{112 bits 50 ms Head 32 char 16 bytes 14 bytes Arduino Modo 14}
 aux: byte;
 i:integer;
 auxSize: longint;
 a: ARRAY[0..13] OF byte; {14 bytes enviar}
BEGIN
 auxSize:= FileSize(gbFileRead);
 a[0]:=(auxSize AND $FF);
 a[1]:=((auxSize SHR 8) AND $FF);
 a[2]:=((auxSize SHR 16) AND $FF);
 a[3]:=(gbSpeed OR (gbCompress SHL 6));
 a[4]:=gbAddress[1];
 a[5]:=gbAddress[2];
 a[6]:=gbAddress[3];
 FOR i:=7 TO 13 DO
  a[i]:=0;
 SaveCustomSPISpeed1415(a);
 FOR i:=1 TO 10 DO
  Write(gbFileWrite,'00000000000000000000000000000000');
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHeadSpeed13NoFlag;
VAR{64 bits 25 ms Head 16 caracteres Arduino Modo 13}
 aux: byte;
 i:integer;
 auxSize: longint;
 a: ARRAY[0..7] OF byte;
BEGIN
 auxSize:= FileSize(gbFileRead);
 a[0]:=(auxSize AND $FF);
 a[1]:=((auxSize SHR 8) AND $FF);
 a[2]:=((auxSize SHR 16) AND $FF);
 a[3]:=(gbSpeed OR (gbCompress SHL 6));
 a[4]:=gbAddress[1];
 a[5]:=gbAddress[2];
 a[6]:=gbAddress[3];
 a[7]:=0;
 SaveCustomSPISpeed1213(a);
 FOR i:=1 TO 41 DO
  Write(gbFileWrite,'0000000000000000');
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;


{******************************************************}
PROCEDURE SaveHeadSpeed15NoFlag;
VAR{112 bits 25 ms Head 32 char 16 bytes 14 bytes Arduino Modo 15}
 aux: byte;
 i:integer;
 auxSize: longint;
 a: ARRAY[0..13] OF byte;
BEGIN
 auxSize:= FileSize(gbFileRead);
 a[0]:=(auxSize AND $FF);
 a[1]:=((auxSize SHR 8) AND $FF);
 a[2]:=((auxSize SHR 16) AND $FF);
 a[3]:=(gbSpeed OR (gbCompress SHL 6));
 a[4]:=gbAddress[1];
 a[5]:=gbAddress[2];
 a[6]:=gbAddress[3]; 
 FOR i:=7 TO 13 DO
  a[i]:=0; 
 SaveCustomSPISpeed1415(a);
 FOR i:=1 TO 41 DO
  Write(gbFileWrite,'00000000000000000000000000000000');
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHeadSpeed11NoFlag;
VAR{32 bits 25 ms Head 10 caracteres Arduino Modo 11}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 SaveHeadAnalogSPISpeed11NoFlag((auxSize AND $FF));
 SaveHeadAnalogSPISpeed11NoFlag(((auxSize SHR 8) AND $FF));
 SaveHeadAnalogSPISpeed11NoFlag(((auxSize SHR 16) AND $FF));
 SaveHeadAnalogSPISpeed11NoFlag((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  SaveHeadAnalogSPISpeed11NoFlag(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHead2BytesAnalogSPINoFlag;
VAR{16 bits 25 ms Head 5 caracteres Arduino}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save2BytesHeadAnalogSPINoFlag((auxSize AND $FF));
 Save2BytesHeadAnalogSPINoFlag(((auxSize SHR 8) AND $FF));
 Save2BytesHeadAnalogSPINoFlag(((auxSize SHR 16) AND $FF));
 Save2BytesHeadAnalogSPINoFlag((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save2BytesHeadAnalogSPINoFlag(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;


{******************************************************}
PROCEDURE SaveHead1ByteSPI;
VAR
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save1ByteHeadSPI((auxSize AND $FF));
 Save1ByteHeadSPI(((auxSize SHR 8) AND $FF));
 Save1ByteHeadSPI(((auxSize SHR 16) AND $FF));
 Save1ByteHeadSPI((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save1ByteHeadSPI(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE SaveHead1ByteSPINoFlag;
VAR {25 milis}
 aux: byte;
 cad: string;
 i:integer;
 auxSize: longint;
BEGIN
 auxSize:= FileSize(gbFileRead);
 cad:= '';
 Save1ByteHeadSPINoFlag((auxSize AND $FF));
 Save1ByteHeadSPINoFlag(((auxSize SHR 8) AND $FF));
 Save1ByteHeadSPINoFlag(((auxSize SHR 16) AND $FF));
 Save1ByteHeadSPINoFlag((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  Save1ByteHeadSPINoFlag(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
END;

{******************************************************}
PROCEDURE ShowHelp;
BEGIN
 writeln('GENFRAME psexe frame memaddress speed compress ms');
 writeln('');
 writeln(' speed:');
 writeln('   0 - 4 btns 50 ms (3 bits) 60 bauds');
 writeln('   1 - 4 btns 25 ms (3 bits) 120 bauds');
 writeln('   2 - SPI 50 ms (8 bits) 160 bauds');
 writeln('   3 - SPI 25 ms (8 bits) 320 bauds');
 writeln('   4 - SPI 50 ms (12 bits) 240 bauds (ERROR)');
 writeln('   5 - SPI 25 ms (12 bits) 480 bauds (ERROR)');
 writeln('   6 - SPI 50 ms (40 bits) 800 bauds (ERROR)');
 writeln('   7 - SPI 25 ms (40 bits) 1600 bauds (ERROR)');
 writeln('   8 - SPI 50 ms (16 bits) 320 bauds');
 writeln('   9 - SPI 25 ms (16 bits) 640 bauds');
 writeln('  10 - SPI 50 ms (32 bits) 640 bauds');
 writeln('  11 - SPI 25 ms (32 bits) 1280 bauds');
 writeln('  12 - SPI 50 ms (64 bits custom) 1280 bauds');
 writeln('  13 - SPI 25 ms (64 bits custom) 2560 bauds');
 writeln('  14 - SPI 50 ms (112 bits custom) 2240 bauds');
 writeln('  15 - SPI 25 ms (112 bits custom) 4480 bauds');
 writeln(' compress:');
 writeln('  0 - No compression');
 writeln('  1 - Head remove 1920 bytes');
 writeln('  2 - ');
 writeln('  3 - ');
 writeln(' ms:');
 writeln('  25 - 25 ms');
 writeln('  20 - 20 ms');
 writeln('  18 - 18 ms');
 writeln('  17 - 17 ms');
 writeln('  16 - 16 ms');
 writeln('');
 writeln('Example:');
 writeln(' GENFRAME demo1.exe frame.txt 80010000 0 0');
END;

{******************************************}
PROCEDURE GetParams;
VAR
 aux: byte;
BEGIN
 gbFlipFlop:= $08;
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
 gbMaxSizeReadBuf:= 32000;
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
  0: gbTopeLinea:= 6; {4 btns 50 ms 60 bauds}
  1: gbTopeLinea:= 6; {4 btns 25 mss120 bauds}
  2: gbTopeLinea:= 6; {spi 9 btns 1 byte 50 ms 160 bauds}
  3: gbTopeLinea:= 6; {spi 9 btns 1 byte 25 ms 320 bauds}
  4: gbTopeLinea:= 6; {spi 12 btns 12 bits 50 ms 240 bauds}
  6: gbTopeLinea:= 6; {spi 50 ms 800 bauds Mando analogico}

  8: gbTopeLinea:= 6; {spi 50 ms 320 bauds Mando analogico}
  9: gbTopeLinea:= 6; {spi 25 ms 640 bauds Mando analogico}
  10:gbTopeLinea:= 6; {spi 50 ms 640 bauds Mando analogico}
  11:gbTopeLinea:= 6; {spi 25 ms 1290 bauds Mando analogico}
  12:gbTopeLinea:= 6; {spi 50 ms 1280 bauds PSXSDK}
  13:gbTopeLinea:= 6; {spi 25 ms 2560 bauds PSXSDK}
  14:gbTopeLinea:= 6; {spi 50 ms 2560 bauds PSXSDK}
  15:gbTopeLinea:= 6; {spi 25 ms 5120 bauds PSXSDK}
  else
   gbTopeLinea:= 6;
 END;

 Writeln(gbFileWrite);

 CASE (gbSpeed) OF
  0: SaveHead;
  1: BEGIN
      SaveHead;
      gbTopeLinea:= 13;
     END;
  2: BEGIN
      Writeln(gbFileWrite,'+*'); {Sincronismo Vaciar serial ARDUINO}
      SaveHead1ByteSPI;
      gbTopeLinea:= 20;
     END;
  3: BEGIN
      Writeln(gbFileWrite,'+#'); {Sincronismo 25 ms}
      SaveHead1ByteSPINoFlag;
      gbTopeLinea:= 41;
     END;
  4: BEGIN
      Writeln(gbFileWrite,'$');{3 caracteres arduino}
      SaveHead3ByteSPI;
      gbTopeLinea:= 9;
     END;
  6: BEGIN
      Writeln(gbFileWrite,'$,*');{10 caracteres arduino 50 ms}
      SaveHead5BytesAnalogSPI; {Mando analogico 5 bytes 40 bits}
      gbTopeLinea:= 20;
     END;
  8: BEGIN
      Writeln(gbFileWrite,'$.*');{5 caracteres arduino 50 ms}
      SaveHead2BytesAnalogSPI; {Mando analogico 2 bytes 16 bits}
      gbTopeLinea:= 20;
     END;
  9: BEGIN
      Writeln(gbFileWrite,'$.#');{5 caracteres arduino 25 ms no flag}
      SaveHead2BytesAnalogSPINoFlag; {Mando analogico 2 bytes 16 bits}
      gbTopeLinea:= 41;
     END;
  10:BEGIN
      {No pongo codigo numero de nibbles}
      Writeln(gbFileWrite,'$*');{10 nibbles arduino 50 ms flag}
      SaveHeadSpeed10Flag; {Mando analogico 4 bytes}
      gbTopeLinea:= 20;
     END;
  11:BEGIN
      {No pongo codigo numero de nibbles}
      Writeln(gbFileWrite,'$#');{10 nibbles arduino 50 ms no flag}
      SaveHeadSpeed11NoFlag; {Mando analogico 4 bytes}
      gbTopeLinea:= 41;
     END;
  12:BEGIN
      Writeln(gbFileWrite,'$*');{16 nibbles arduino 50 ms no flag}
      SaveHeadSpeed12Flag; {custom 8 bytes}
      gbTopeLinea:= 20;
     END;
  13:BEGIN
      Writeln(gbFileWrite,'$#');{16 nibbles arduino 25 ms no flag}
      SaveHeadSpeed13NoFlag; {custom 8 bytes}
      gbTopeLinea:= 41;
     END;
  14:BEGIN
      gbMaxSizeReadBuf:= 31976; {Multiplo 14}
      Writeln(gbFileWrite,'$*');{32 char 16 bytes arduino 50 ms flag}
      SaveHeadSpeed14Flag; {custom 14 bytes}
      gbTopeLinea:= 20;
     END;
  15:BEGIN
      gbMaxSizeReadBuf:= 31976; {Multiplo 14}
      Writeln(gbFileWrite,'$#');{32 char 16 bytes arduino 25 ms no flag}
      SaveHeadSpeed15NoFlag; {custom 14 bytes}
      gbTopeLinea:= 41;  
     END;
  else
   SaveHead;
 END;

 {Writeln(gbFileWrite);}
 gbCadSal:= '';
 gbContLinea:=0;
 gbContContLinea:=0;
 REPEAT
  BlockRead(gbFileRead, Buf, gbMaxSizeReadBuf, gbNumRead);
  IF ((gbCompress=1) AND (isHead = FALSE)) THEN
   BEGIN {Primera vez es la cabecera}
    isHead:= TRUE;
    FOR i:=2049 TO gbNumRead DO
     Buf[i-1920]:= Buf[i]; {Empieza en 129}
    gbNumRead:= gbNumRead-1920;
   END;

  CASE (gbSpeed) OF
   0: FOR i:=1 TO gbNumRead DO
       SaveByteText(Buf[i],FALSE); {No se pone 0 4 btns 60 bauds}
   1: FOR i:= 1 TO gbNumRead DO
       SaveByteTextNotFlag(Buf[i], gbFlipFlop); {Modo 25 milis 4 btns}
   2: FOR i:=1 TO gbNumRead DO
       SaveByteSPI(Buf[i]); {SPI 9 btns 1 byte 50 ms 160 bauds}
   3: FOR i:=1 TO gbNumRead DO
       SaveByteSPI(Buf[i]); {SPI 9 btns 1 byte 25 ms 320 bauds}
   4: BEGIN
       i:= 1;
       WHILE (i<gbNumRead) DO
        BEGIN
         Save3BytesSPI(Buf[i],Buf[i+1],Buf[i+2]);
         i:= i+3;
        END;
      END;
   6: BEGIN
       i:= 1; {10 caracteres 5 bytes 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         Save5BytesAnalogSPI(Buf[i],Buf[i+1],Buf[i+2],Buf[i+3],Buf[i+4]);
         i:= i+5;
        END;
      END;
   8: BEGIN
       i:= 1; {5 caracteres 2 bytes 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         Save3BytesAnalogSPI(Buf[i],Buf[i+1],Buf[i+2]);
         i:= i+2;
        END;
      END;
   9: BEGIN
       i:= 1; {5 caracteres 3 bytes 25 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         Save3BytesAnalogSPI(Buf[i],Buf[i+1],Buf[i+2]);
         i:= i+2;
        END;
      END;
   10:BEGIN
       i:= 1; {10 caracteres 4 bytes 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveAnalogSPISpeed1011(Buf[i],Buf[i+1],Buf[i+2],Buf[i+3]);
         i:= i+4;
        END;
      END;
   11:BEGIN
       i:= 1; {10 caracteres 4 bytes 25 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveAnalogSPISpeed1011(Buf[i],Buf[i+1],Buf[i+2],Buf[i+3]);
         i:= i+4;
        END;
      END;
   12:BEGIN
       i:= 1;{16 nibbles 8 bytes 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveCustomSPISpeed1213(Buf[i]);
         i:= i+8;
        END;
      END;
   13:BEGIN
       i:= 1;{16 nibbles 8 bytes 25 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveCustomSPISpeed1213(Buf[i]);
         i:= i+8;
        END;
      END;
   14:BEGIN
       i:= 1;{32 nibbles 16 bytes 14 bytes envio 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveCustomSPISpeed1415(Buf[i]);
         i:= i+14;
        END;
      END;
   15:BEGIN
       i:= 1;{32 nibbles 16 bytes 14 bytes envio 50 ms}
       WHILE (i<gbNumRead) DO
        BEGIN
         SaveCustomSPISpeed1415(Buf[i]);
         i:= i+14;
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