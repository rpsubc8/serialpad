PROGRAM Serie;
USES Crt;
(*11100011b 9600 baudios 1 bit stop*)
VAR
 F: Text;
 Ch: Char;
 auxByte: byte;
 contador: integer;

FUNCTION CharHexToDec(c: Char): byte;
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
END;

BEGIN
 Delay(2000);
 Clrscr;
 asm
  mov ah,0
{  mov al,11000111b}
  mov al,11100011b {9600 111 NoParity 00 StopBits 0 CharSize8bits 11}
  mov dx,0 {COM1}
  int 14h
 end;

 {port[$3F8]:= $FF;}

 Assign(F, 'c:\tp\bin\serie\salida.txt');
 Reset(F);
  while not Eof(F) do
  begin
   if keypressed then exit;
   IF (EOLN(F)=TRUE) THEN
    BEGIN
     Write ('*');
     Delay(1050);
    END;
   Read(F, Ch);
   {port[$3F8]:= CharHexToDec(Ch);}
   port[$3F8]:= byte(Ch);
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
   Delay(1);

(* asm
  mov dx,03F8h
  mov ah,0
  mov al,'F'
  out dx,al
 end;*)

{   Delay(50);}
(*   Write(Ch);  { Descargar ficheros tipo texto }*)
  end;
end.


 (*asm
  mov dx,03F8h
  mov ah,0
  mov al,'F'
  out dx,al
 end;*)
END.