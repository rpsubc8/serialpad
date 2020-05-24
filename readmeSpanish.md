# Serial PAD
Permite usar un teclado o ratón en una PSX real conectada al puerto de mandos. Soporta:
<ul>
 <li><b>Ratón PS/2 (usb)</b> - 100% funcional y se incluye pantalla de test</li>
 <li><b>Teclado PS/2 (usb)</b> - código funcional, pero falta pantalla de test</li> 
 <li><b>Comunicación lenta serie</b> - código funcional, aunque falta pantalla de test</ul>
</ul>
<center><img src="preview/captureEpsxMouse.gif"></center>
De esta forma se puede introducir datos por teclado o ratón en cualquier modelo de PSX o PsOne conectando un teclado a un puerto de mandos de la PSX.<br>
<center><img src="preview/ps1mouse.jpg"></center>
El ratón y teclado que van a usarse son de PC, así que este proyecto, no va a hacer uso de ratones de PSX, es decir, que no es compatible con el ratón de PSX original.
<br><br>

<h2>Simulación</h2>
Para todo aquel que no quiera modificar un mando real de PSX, se puede realizar pruebas mediante emulación:
<ul>
 <li><a href="https://www.epsxe.com/">Emulador de PSX (ePSX)</a></li>
 <li><a href="https://www.autohotkey.com/">Auto Hot Key (Script teclado y ratón)</a></li>
</ul>
Una vez instalado el Auto Hot Key, podremos lanzar el KeyboardMouse.ahk, que traducirá los movimiento y pulsación de botones del ratón en pulsaciones de teclado, que el emulador ePSX interpretará como pulsación de botones del mando.<br>
<center><img src="preview/ePSXconfigPAD.gif"></center>
Debemos de configurar los gatillos del mando con las teclas:
<ul>
 <li><b>R1</b> - R</li>
 <li><b>R2</b> - T</li>
 <li><b>L1</b> - W</li>
 <li><b>L2</b> - E</li> 
</ul>
Si lanzamos el script KeyboardMouse.ahk en una ventana de comandos, veremos como al mover al ratón, se generan pulsaciones de teclas:
<center><img src="preview/autohotkey.gif"></center>
Debemos de tener cuidado, y asegurarnos de estar en la ventana del emulador ePSX, ya que cada vez que hagamos algo con el ratón, estará pulsando teclas.<br>
En el emulador, si lanzamos el main.exe, nos saldrá el menú con los botones:
<ul>
 <li><b>Fast</b> - Velocidad rápida del ratón</li>
 <li><b>Normal</b> - Velocidad normal</li>
 <li><b>Slow</b> - Velocidad lenta</li>
 <li><b>X</b> - Invierte el movimiento eje X</li>
 <li><b>Y</b> - Invierte el movimiento eje Y</li>
 <li><b>Left</b> - Zurdos</li>
</ul>
En la parte superior, nos aparecerá información de las coordenadas y botones del ratón, así como el estado del mando de la PSX.
Si pasamos por encima de los botones, se iluminarán, y si se hace click se activará.<br>
Debemos tener en cuenta, que la emulación no va bien, dado que el script de teclado y el emulador tiene unas demoras, que al final se traduce en unos movimientos no fluidos del ratón y fallos esporádicos. Sin embargo no es muy útil para ver que al menos funciona.
<br><br>

<h2>Modificar mando</h2>
Si queremos una respuesta fluida, debemos modificar el mando real.
<center><img src="preview/mandoCableBotones.jpg"></center>
Se van a usar los 4 botones (gatillos laterales), es decir, R1, R2, L1 y L2. El motivo se debe a que son los más fáciles de modificar sin falta de usar soldador, es decir, que cualquiera puede hacerlo. Estos botones estan unidos a la placa del mando con un hilo para cada uno de los botones, y otro para la masa. La forma de detectar un botón pulsada, es muy sencilla, tna sólo con unirlo con la masa.<br>
No todos los mandos usan el sistema de masa, así que tendremos que tener en cuenta si nuestro mando sirve para este proyecto.
<center><img src="preview/mandoR1R2.jpg"></center>
Debemos mirar con el multitester la masa, que se corresponde con la masa real del mando.<br>
Luego usaremos un sistema de buffer de transistores que activaran los botones. Dichos transistores seran controlados por un ARDUINO, de manera, que se leeran los datos del ratón PS/2 y se activarán los botones del mando.
<br><br>


<h2>Mando físico en Emulador</h2>
Una vez actualizado el ARDUINO con el código del ratón, y conectado el ratón al PS/2, podemos hacer pruebas sobre el emulador ePSX, antes de probar con una PSX real, gracias a un conversor de mandos PSX a usb.
<center><img src="preview/PSXusb.jpg"></center>
Debemos de configurar el mando en el emulador de PSX, de manera que usemos los botones del mando, en lugar de las letras.
<center><img src="preview/"></center>
Y si todo es correcto, al ejecutar el main.exe, podremos usar el ratón.
<br><br>


<h2>Mando físico en consola real</h2>
Se requiere los siguiente materiales
<ul>
 <li>Mando PSX botones R1,R2,L1,L2</li>
 <li>Adaptador usb a PS/2 o ratón PS/2</li>
 <li>Teclado PS/2</li>
 <li>Ratón PS/2</li>
 </li>Arduino Nano o UNO</li>
</ul>
<br><br>


<h2>Protocolo Ratón</h2>
Se ha usado un protocolo muy simple similar a como se usaban los ratones en los primeros COMMODORE y MSX por puerto de juegos, pero con varias mejoras de velocidad.<br>
Se envian cada 25 milisegundos 4 bits, que contienen:
<ul>
 <li>2 bits movimiento X</li>
 <li>2 bits movimiento Y</li>
</ul>
O bien:
<ul>
 <li>2 bits botón izquierdo</li>
 <li>2 bits botón derecho</li> 
</ul>

Los bits del movimiento pueden ser:
<ul>
 <li><b>00</a> - No hay movimiento</li>
 <li><b>01</a> - Movimiento positivo</li>
 <li><b>11</a> - Movimiento negativo</li>
</ul>
Para los botones, si se da la opción <b>10</b> es que el botón está activo.
Recordemos, que la PSX en cada retrazo vertical, se leen los datos del PAD, así que se leen entre 16 y 20 milisegundos.
<br><br>
 

# Otras pruebas
Permite usar un mando de psx <b>(conectado a la psx)</b> como si fuera un modem <b>(lento)</b> serie, pudiendo enviar ficheros o usar el teclado, en cualquier consola psx, incluida la psone.
Y lo más importante, se permite realizar un upload de un ps-exe directamente  a memoria o a la memory card.
<center><img src="preview/capturaepsx.gif"></center>

<h2>Simulacion</h2>
Para pruebas rápidas estoy usando un emulador de psx (epsx) y un script (autohotkey) que envia las pulsaciones de teclas para el emulador.
Se usan 8 botones del mando, junto con 1 más (L1) para el pulso (sincronia) del dato.
En siguientes versiones se usara un arduino que con transistores activará o no los botones.

<center><table>
 <tr>
  <td>Button</td><td>Key</td><td>Bit</td>
 </tr>
 <tr>
  <td>L1</td><td>W</td><td>(sync)</td>
 </tr>
 <tr>  
  <td>L2</td><td>E</td><td>Q7</td>
 </tr>   
 <tr>  
  <td>R1</td><td>R</td><td>Q6</td>
 </tr>   
 <tr>     
  <td>R2</td><td>T</td><td>Q5</td>
 </tr>  
 <tr>  
  <td>Triangulo</td><td>D</td><td></td>
 </tr>  
 <tr>  
  <td>Cuadrado</td><td>S</td><td>Q3</td>
 <tr> 
 </tr>  
  <td>Circulo</td><td>X</td><td>Q2</td>
 <tr> 
 </tr>  
  <td>X</td><td>Z</td><td>Q1</td>
 <tr>      
  <td>Start</td><td>V</td><td>Q0</td>
 </tr>  
</table></center>

<h2>Fases de desarrollo</h2>
Se realiza un ataque en varios frentes, desde el m&aacute;s sencillo, al m&aacute;s complejo:
<ul>
 <li><b>Control de teclado remoto y real</b> (puerto PC PS2) con arduino activando 9 botones del Gamepad, por medio
   de un protocolo muy simple de flanco ascendente y transistores, pero lento en velocidad. Se realizará un programa que envia los datosen msdos x86 100% compatible DOSBOX para el remoto via emulaci&oacute;n serie, así como la posibilidad de leer directamente de arduino el teclado con la libreria PS2keyboard.
  <center><img src="preview/mandoAnalizar.jpg"></center>
 </li>
 <li><b>Envio de PS-exe</b> a memoria de PSOne o a tarjeta de memoria de PSOne con arduino activando 9 botones del Gamepad,
  , por medio de un protocolo muy simple de flanco ascendente y transistores, pero lento en velocidad. Se realizará un programa que envia datos en msdos x86 100% compatible DOSBOX para el remoto via emulaci&oacute;n serie
 </li>
 <li><b>Carga de PS-exe</b> desde memorias de PSX (128 KB), con posibilidad de trozear el exe en varias tarjetas. Se realizará un programa en msdos x86 100% compatible DOSBOX para trocear el exe en múltiples .MCR de 128 KB. Dichos .MCR se puede guardar en tarjeta real desde PS2 o mediante el sakura de puerto paralelo (vmware con windows 95 y 98 con soporte puerto paralelo).
  <center><img src="preview/memecardAnalizar.jpg"></center>
 </li>
 <li>
  <b>Recepción de datos por medio de sonido</b> de la PSX, los 2 canales de audio. Se realizará un programa en psx que genera tonos con pulsos que pueden ser decodificados como datos binarios. La psx recibirá comandos remotos y enviará los datos como sonido.
 </li>
 <li>
  <b>Simulación de mando y memoria</b> psx por medio de arduino. He capturado los comandos del mando de la psx, así como los de la memoria, al usar un protocolo SPI a 250 Khz. Estoy realizando un programa en arduino que al recibir los comandos, envie las respuestas a un programa en la psone que interprete para cargar datos.  
  <center><img src="preview/mandoAnalizadorDigital.jpg"></center>
 </li>
</ul>


<h2>Conclusion</h2>
Se usa esta opción, porque mientras que la psx si dispone de puerto serie, la psone blanca, no. Se tiene por tanto que realizar modificaciones. Además se logra una alternativa al SIOCONS

<h2>Uso de MSDOS</h2>
El motivo de usar MSDOS, es para lograr máxima compatibilidad con DOSBOX, de forma que se puede usar todo el kit de desarrollo psyq de 16bits, ya que así se puede usar en casi cualquier máquina actual que no sea PC. También se puede usar el msdosplayer para Windows 7 64 bits en adelante.

<h2>Historial</h2>
Aún no es funcional de cara al usuario.
Permite recibir comando de recepcion de fichero, y se reciben 100 bytes.

