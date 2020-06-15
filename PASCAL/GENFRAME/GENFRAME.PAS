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
 gbContLinea, gbNumRead,i: Word;
 gbSpeed, gbCompress, gbCode: integer;
 Buf: array[1..32000] of byte; {32000 bytes}
 gbCadAddress,gbCadSal: string;
 gbAddress: array[1..4] of byte;

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
PROCEDURE SaveByteText(data: byte);
VAR
 aux: byte;
BEGIN
 aux:= ((data AND $07) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= (((data SHR 3) AND $07) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 aux:= (((data SHR 6) AND $03) OR $08);
 gbCadSal:= gbCadSal + NibbleToHexString(aux);
 {Write(fileWrite,NibbleToHexString(aux));}
 {Write(fileWrite,NibbleToHexString(aux));}
 {Write(fileWrite,NibbleToHexString(aux));}
 gbContLinea:= gbContLinea+1;
 IF (gbContLinea>6) THEN
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
 Writeln(gbFileWrite,'*'); {Vaciar buffer serial ARDUINO}

 cad:= '';
 SaveByteText((auxSize AND $FF));
 SaveByteText(((auxSize SHR 8) AND $FF));
 SaveByteText(((auxSize SHR 16) AND $FF));
 SaveByteText((gbSpeed OR (gbCompress SHL 6)));
 FOR i:=1 TO 3 DO
  SaveByteText(gbAddress[i]);
 Write(gbFileWrite,cad); {Ya ha metido retorno linea}
END;

{******************************************************}
PROCEDURE ShowHelp;
BEGIN
 writeln('GENFRAME psexe frame memaddres speed compress');
 writeln('');
 writeln('speed 0,1,2,3,4');
 writeln('compress 0,1,2,3');
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

 Reset(gbFileRead, 1);  { Tamabioo registro 1}
 Rewrite(gbFileWrite);

 gbCadSal:= '';
 gbContLinea:=0;
 SaveHead;
 REPEAT
  BlockRead(gbFileRead, Buf, SizeOf(Buf), gbNumRead);
  FOR i:=1 TO gbNumRead DO
   SaveByteText(Buf[i]);
 UNTIL (gbNumRead = 0);
 IF (gbContLinea>0) THEN
  Writeln(gbFileWrite,gbCadSal); {Ultima linea}
 {Writeln(gbFileWrite);}
 Close(gbFileWrite);
 Close(gbFileRead);
END.
