{*************************************************************}
{* Author: ackerman                                          *}
{* Jaime Jose Gavin Sierra                                   *}
{* Convierte Binario PsEXE en nibbles para envio por ARDUINO *}
{* Se puede enviar con el RealTerm. Se tiene que poner una   *}
{* espera en milisegundos en el retorno de linea             *}
{* Numero de bytes x 50 milisegundos = 21 x 50 = 1050 milis  *}
{* Version PASCAL estandar MSDOS x86, DOSBOX y Win32         *}
{*************************************************************}
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
  else
   gbTopeLinea:= 6;
 END;
 SaveHead;
 {Writeln(gbFileWrite);}
 gbCadSal:= '';
 gbContLinea:=0;
 gbContContLinea:=0;
 REPEAT
  BlockRead(gbFileRead, Buf, SizeOf(Buf), gbNumRead);
  IF ((gbSpeed=1) AND (isHead = FALSE)) THEN
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
  END;
 UNTIL (gbNumRead = 0);
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
 {Writeln(gbFileWrite);}
 Close(gbFileWrite);
 Close(gbFileRead);
END.